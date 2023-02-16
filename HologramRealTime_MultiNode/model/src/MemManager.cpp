
#include "model/MemManager.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <map>
#include <vector>
#include <deque>
#include <algorithm>
#ifdef _WIN32
#include <conio.h>
#endif

#include <graphics/sys.h>
#include <math.h>

#include "model/avl_tree.h"

using namespace std;

#include "model/sync_gpu_memory.h"

int kMaxUndoStackSize = 10000;
SyncGpuMemory* kSyncGpuMemory;
static bool kAllocChunkMode = false;

////////////////////////////////////////////////////////////////////////////////
// result codes

const int E_OK					= 1;
const int E_TRANSACTION_OPEN	= -1;
const int E_NO_TRANSACTION		= -3;
const int E_NO_DATA				= -6;
const int E_INVALID_ARGUMENT	= -7;

////////////////////////////////////////////////////////////////////////////////
// system invariants

static const int PAGE_SIZE = 4096;
static const int BLOCK_SIZE = PAGE_SIZE * 16;

////////////////////////////////////////////////////////////////////////////////
// freelist node

struct Free {
	Free() :  size(0), next(0) {}; // use only with placement new

private:
	Free(const Free&) {}
	~Free() {};

public:
	typedef unsigned __int64	SIZE_TYPE;
	SIZE_TYPE	size; // length of the availble buffer starting at '&next'
	Free*		next; // TODO: remove

	struct _bylocation {
		_bylocation() : lt(0), gt(0), bf(0) {}
		Free*		lt;
		Free*		gt;
		int			bf;
	} loc;

	static Free* FromPtr(void* p) 
	{ 
		return reinterpret_cast<Free*>((char*)p - sizeof(SIZE_TYPE)); 
	}

	void* UserPtr() const
	{ 
		return static_cast<void*>(const_cast<Free**>(&next));
	}

	void* Begin() const
	{
		return static_cast<void*>(const_cast<Free*>(this));
	}

	void* End() const
	{
		return reinterpret_cast<void*>((char*)UserPtr() + size);
	}

	bool Adjacent(const Free* f) const
	{
		return (f && (this->End() == f->Begin()));
	}

	bool Overlap(Free* f) const 
	{
		return (this->End() > f->Begin());
	}
};

////////////////////////////////////////////////////////////////////////////////
//  

struct AddressAbstractor { // for AVL tree
	typedef Free* handle;

	typedef unsigned __int64 key;

	typedef size_t size;

	handle get_less(handle h, bool access) { return h->loc.lt; }
	void set_less(handle h, handle lh) { h->loc.lt = lh; }

	handle get_greater(handle h, bool access) { return h->loc.gt; }
	void set_greater(handle h, handle gh) { h->loc.gt = gh; }

	int get_balance_factor(handle h) { return h->loc.bf; }
	void set_balance_factor(handle h, int bf) { h->loc.bf = bf; }

	int compare_key_key(key k1, key k2) 
	{ 
		if (k1 == k2) return(0);
		if (k1 > k2) return(1);
		return(-1);
	}

	int compare_key_node(key k, handle h) { return(compare_key_key(k, (key)h)); }
	int compare_node_node(handle h1, handle h2) { return(compare_key_key((key)h1, (key)h2)); }

	handle null(void) { return(0); }
	static bool read_error(void) { return(false); }
};

////////////////////////////////////////////////////////////////////////////////
// 

struct AllocationManager
{
	Free*	sFreeHead;

	abstract_container::avl_tree<AddressAbstractor, 31> locTree;

	AllocationManager() : sFreeHead(0), locTree() {}

	Free* Slice(Free* from, size_t bytes)
	{
		const size_t true_bytes = bytes + sizeof(Free::SIZE_TYPE);
		if ((from->size - true_bytes) < sizeof(from->size)) 
			return 0;

		Free* slice = reinterpret_cast<Free*>((char*)from->UserPtr() + (from->size - true_bytes));
		memset(slice, 0, sizeof(Free));
		slice->size = bytes;
		from->size -= true_bytes;

		return slice;
	}

	bool IsValidPointer(void* p, size_t _size) 
	{
		_size = ((_size / 8) + 1) * 8;
		if (!p) return false;

		Free* node = Free::FromPtr(p);

		Free* cursor = locTree.search((AddressAbstractor::key)node, abstract_container::EQUAL);
		if (cursor) {
			return false;
		}
		cursor = locTree.search((AddressAbstractor::key)node, abstract_container::LESS);
		if (!cursor) {
			if (node->size >= _size) return true;
			else return false;
		}

		//cprintf("input %p, begin %p, end %p\n", node, cursor->Begin(), cursor->End());
		if ( node >= cursor->Begin() && node < cursor->End()) 
			return false;

		cursor = locTree.search((AddressAbstractor::key)node, abstract_container::GREATER);	
		if (!cursor) {
			if (node->size >= _size) return true;
			else return false;
		}
		unsigned char* pp = (unsigned char*)node;
		pp = (unsigned char*)pp + node->size;
		if ( pp >= cursor->Begin()) 
			return false;

		if (node->size >= _size) return true;
		return false;
	}

	void Insert(Free* node)
	{
		// precautionary
		node->next = 0;
		memset(&node->loc, 0, sizeof(node->loc));

		if (sFreeHead == 0) {
			sFreeHead = node;
			locTree.insert(node);
		} else if (sFreeHead->Begin() > node->Begin()) {
			node->next = sFreeHead;
			sFreeHead = node;
			locTree.insert(node);

			Compact(sFreeHead);
		} else {
			Free* cursor = locTree.search((AddressAbstractor::key)node, abstract_container::LESS);
			node->next = cursor->next;
			cursor->next = node;
			locTree.insert(node);

			Compact(node);
			Compact(cursor);
		}
	}

	void Compact(Free* node)
	{
		if (node->Adjacent(node->next)) {
			locTree.remove((AddressAbstractor::key)node->next);

			node->size += (node->next->size + sizeof(node->size));
			node->next = node->next->next;
		}
	}



};

////////////////////////////////////////////////////////////////////////////////
// typedef and definition of global page to space map

typedef map<void*, size_t>	Page2UndoContainerMap;
Page2UndoContainerMap* pages_;

////////////////////////////////////////////////////////////////////////////////
// representations of transactions and spaces


typedef map<void*, size_t>	BlockMap;

struct DiskBackupRecord {
	void* page;

	DiskBackupRecord(): page(0) {}
	DiskBackupRecord(const DiskBackupRecord& cp): page(cp.page){}
	DiskBackupRecord(void* p) { page = p; }

};

typedef std::vector<DiskBackupRecord> DiskBackup; 
typedef std::map<void*, int> MemoryImage;

struct Transaction { 
	Transaction() : id(++nextId), diskbackups(),  added(), command() {}
	Transaction(const Transaction& c): id(c.id), diskbackups(c.diskbackups), added(c.added), command(c.command) {}
	DiskBackup diskbackups;
	BlockMap added; 

	size_t	id;
	std::string command;
	static size_t nextId; 

	bool isEmpty() const {
		if (!diskbackups.size() && !added.size()) return true;
		return false;
	}
};
size_t Transaction::nextId = 0; 

typedef deque<Transaction> Transactions;


struct UndoContainer { 
	UndoContainer() : done(), undone(), cleanup(), transacting(false), id(nextId), data(0) 
	{
		char buffer[100];
		itoa(id, buffer, 10);
		strcat(buffer, "_backup.kbk");
		std::string name(buffer);
		file_name = name;
		fp = fopen(buffer, "wb");
		fclose(fp);
		fp = fopen(buffer, "rb+");
		itoa(id, buffer, 10);
		strcat(buffer, "_image.kbk");
		file_name2 = std::string(buffer);
		fp_image = fopen(buffer, "wb");
		fclose(fp_image);
		fp_image = fopen(buffer, "rb+");
		image_offset_ = 0;
	}

	void Clear() { 
		fclose(fp);
		::remove(file_name.c_str());
		fclose(fp_image);
		::remove(file_name2.c_str());
	}

	Transactions done; 
	Transactions undone; 
	Transactions cleanup; // TODO: cleanup backups at the time of truncations?

	MemoryImage  memory_backup;

	bool transacting; 

	size_t id;
	static size_t nextId; 
	std::string file_name;
	std::string file_name2;
	FILE* fp;
	FILE* fp_image;
	int  image_offset_;

	AllocationManager* data;


	AllocationManager* AssertData() 
	{

		if (!transacting) {
			return NULL; // throw?
		}

		if (data == NULL) {
#ifdef _WIN32
			void* p = malloc(PAGE_SIZE);//::VirtualAlloc(0, PAGE_SIZE, MEM_COMMIT, PAGE_READONLY);
#else
			void* p = memalign(PAGE_SIZE, PAGE_SIZE);
			mprotect(p, PAGE_SIZE, PROT_READ);
#endif

			(*pages_)[p] = id;
			done[0].added[p] = PAGE_SIZE;
			data = new (p) AllocationManager(); 

		} 
		return data;
	}

	Free* More(size_t size)
	{
		if (!transacting)
			return NULL; // throw?

		const size_t actual_size = ((size / BLOCK_SIZE) + 1) * BLOCK_SIZE;

#ifdef _WIN32
		void* p = malloc(actual_size); //::VirtualAlloc(0, actual_size, MEM_COMMIT, PAGE_READONLY);
#else
		void* p = memalign(PAGE_SIZE, actual_size);
		mprotect(p, actual_size, PROT_READ);
#endif
		if (!p) {
			throw "alloc failed";
		}

		for (unsigned int i = 0; i < actual_size; i += PAGE_SIZE) {
			void* q = (char*)p + i;
			(*pages_)[q] = id;

			if (kSyncGpuMemory && kAllocChunkMode) {
				kSyncGpuMemory->addPage(q);
			}
		}

		Free* f = new (p) Free();
		f->size = actual_size - sizeof(f->size);

		done[0].added[p] = actual_size;
		return f;
	}

};
size_t UndoContainer::nextId = 0; 

typedef map<size_t, UndoContainer> UndoContainers;
UndoContainers* spaces_;
bool    kMemManagerInitialized = false;


#ifdef _WIN32
int MemAccessViolationSignalHandler(void* exp)
{
	return EXCEPTION_CONTINUE_EXECUTION;

	LPEXCEPTION_POINTERS e = static_cast<LPEXCEPTION_POINTERS>(exp);

	if (e->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
		return EXCEPTION_CONTINUE_SEARCH; // return (*previous)(e);

	bool writing = (e->ExceptionRecord->ExceptionInformation[0] != 0);
	void* addr = (void*)e->ExceptionRecord->ExceptionInformation[1];

	void* page = (void*)((ULONG_PTR)addr & ~(PAGE_SIZE - 1)); 


	if (writing == false)
		return EXCEPTION_CONTINUE_SEARCH; // return (*previous)(e);

	Page2UndoContainerMap::iterator i = (*pages_).find(page);
	if (i == (*pages_).end())
		return EXCEPTION_CONTINUE_SEARCH; // return (*previous)(e);

	UndoContainers::iterator s = (*spaces_).find(i->second);
	if (s == (*spaces_).end())
		return EXCEPTION_CONTINUE_SEARCH; // my error

	UndoContainer& space = s->second;
	if (!space.transacting) 
		return EXCEPTION_CONTINUE_SEARCH; // user error

	void* backup = 0;

	unsigned long* ptr = (unsigned long*) page;


	if (kSyncGpuMemory) {
		kSyncGpuMemory->addNeedSyncPage(page);
	}
	
	Transaction& t = space.done[0];

	t.diskbackups.push_back(DiskBackupRecord(page));
	unsigned long old = 0;

	bool ok = (bool)::VirtualProtect(page, PAGE_SIZE, PAGE_READWRITE, &old);


	if (ok == false) {
		return EXCEPTION_CONTINUE_SEARCH; // return (*previous)(e);
	}

	return EXCEPTION_CONTINUE_EXECUTION;
}
#else
void MemAccessViolationSignalHandler(int sig, siginfo_t *si, void *context)
{

	if (!((sig == SIGSEGV) || (sig == SIGBUS)))
		return;

	void* addr = (void*)si->si_addr;

	void* page = (void*)((ULONG_PTR)addr & ~(PAGE_SIZE - 1)); 


	PageSpaceMap::iterator i = (*pages_).find(page);
	if (i == (*pages_).end())
		return; 

	Spaces::iterator s = (*spaces_).find(i->second);
	if (s == (*spaces_).end())
		return; // my error

	Space& space = s->second;
	if (!space.transacting) 
		return; // user error

	void* backup = 0;

	unsigned long* ptr = (unsigned long*) page;

	if (kSyncGpuMemory) {
		kSyncGpuMemory->addNeedSyncPage(page);
	}


	Transaction& t = space.done[0];
	t.diskbackups.push_back(DiskBackupRecord(page));

	unsigned long old = 0;

	bool ok = mprotect(page, PAGE_SIZE, PROT_READ|PROT_WRITE) == -1? false:true;

	if (ok == false)
		return ;

	return;
}
#endif
////////////////////////////////////////////////////////////////////////////////
//  API helper functions and implementation

struct MarkBlockFree
{
	MarkBlockFree(UndoContainer& s) : space(s) {}

	void operator ()(const BlockMap::value_type& b)
	{
		#ifdef _WIN32
			::VirtualFree(b.first, 0, MEM_RELEASE);
		#else
			free(b.first);
		#endif

	}
	UndoContainer& space;
};

static void ReleaseBlock(const BlockMap::value_type& b)
{
#ifdef _WIN32
	::VirtualFree(b.first, 0, MEM_RELEASE);
#else
	free(b.first);
#endif
}


struct RecycleTransaction
{
	RecycleTransaction(UndoContainer& s) : space(s) {}
	void operator() (const Transactions::value_type& t)
	{
		for_each(t.added.begin(), t.added.end(), MarkBlockFree(space));
	}
	UndoContainer& space;
};

static void ReleaseTransaction(const Transactions::value_type& t)
{
	for_each(t.added.begin(), t.added.end(), ReleaseBlock);
}

bool CommitBackupNew(UndoContainer& s, DiskBackupRecord& rec)
{

	unsigned long* after = (unsigned long*)rec.page;  // page

	{ // scope
#ifdef _WIN32
		unsigned long old = 0;
		bool ok = ::VirtualProtect(after, PAGE_SIZE, PAGE_READONLY, &old);
		if (ok == false)
			throw "virtual protect failed";
#else 
		mprotect(after, PAGE_SIZE, PROT_READ);
#endif
	}

	return true;
}



static void ApplyDeltaFromDisk(UndoContainer& space, DiskBackupRecord& v)
{
	/*
	unsigned long old = 0;
#ifdef _WIN32
	bool ok = ::VirtualProtect(v.page, PAGE_SIZE, PAGE_READWRITE, &old);
	if (ok == false) 
		throw;
#else
	mprotect(v.page, PAGE_SIZE, PROT_READ|PROT_WRITE);
#endif
	if (kSyncGpuMemory) {
		kSyncGpuMemory->addNeedSyncPage(v.page);
	}

	// now apply delta: 1. read the delta from disk of the input space, and apply this 
	void* delta = malloc(v.end-v.start);
	memset(delta, 0, v.end-v.start);
	fseek(space.fp, v.start, SEEK_SET);
	fread(delta, v.end-v.start, 1, space.fp);
	ApplyCompressedDelta(v.page, delta);

	free(delta);
#ifdef _WIN32
	ok = ::VirtualProtect(v.page, PAGE_SIZE, PAGE_READONLY, &old);
	if (ok == false) 
		throw;
#else
	mprotect(v.page, PAGE_SIZE, PROT_READ);
#endif
	*/
}

bool IsTransacting(const UndoContainers::value_type& s)
{
	return s.second.transacting;
}



void ClearContainer(UndoContainers::value_type& s)
{
	UndoContainerId sid = s.first;
	UndoContainer& space = s.second;
	
	for_each(space.undone.begin(), space.undone.end(), ReleaseTransaction);
	space.undone.clear();

	for_each(space.done.begin(), space.done.end(), ReleaseTransaction);
	space.done.clear();

	for_each(space.cleanup.begin(), space.cleanup.end(), ReleaseTransaction);
	space.cleanup.clear();

	std::vector<void*> pages;
	Page2UndoContainerMap::iterator b = (*pages_).begin();
	while(b != (*pages_).end()) {
		if (b->second == sid) {
			pages.push_back(b->first);
			b++;
		}
		else {
			++b;
		}
	}
	for (size_t i = 0 ; i < pages.size() ; i++)
		pages_->erase(pages[i]);

	space.data = 0;
	space.Clear();
}

////////////////////////////////////////////////////////////////////////////////
// Mm API methods



UndoContainerId CreateUndoContainer(size_t size)
{
	if (!kMemManagerInitialized) {
		if (!spaces_)	spaces_ = new UndoContainers();
		if (!pages_)	pages_ = new Page2UndoContainerMap();
		kMemManagerInitialized = true;
	}

	UndoContainerId sid = ++UndoContainer::nextId;
	UndoContainer& space = (*spaces_)[sid];
	OpenTransaction(space.id);
	if (size) {
		space.AssertData();
		space.More(size);
	}
	return space.id;
}

int	Destroy(UndoContainerId sid)
{
	int clear = Clear(sid);
	if (clear < 1)
		return clear;

	(*spaces_).erase(sid);

	return E_OK;
}

int Clear(UndoContainerId sid)
{
	if (!kMemManagerInitialized) {
		if (!spaces_)	spaces_ = new UndoContainers();
		if (!pages_)	pages_ = new Page2UndoContainerMap();
		kMemManagerInitialized = true;
	}

	if (!spaces_) return E_OK;
	UndoContainers::iterator s = (*spaces_).find(sid);
	if (s == (*spaces_).end())
		return E_INVALID_ARGUMENT; // user error

	UndoContainer& space = s->second;
	/*if (space.transacting)
		return E_TRANSACTION_OPEN;*/

	ClearContainer(*s);

	return E_OK;
}

int ClearAll()
{
	if (!kMemManagerInitialized) {
		if (!spaces_)	spaces_ = new UndoContainers();
		if (!pages_)	pages_ = new Page2UndoContainerMap();
		kMemManagerInitialized = true;
	}

	/*if (std::find_if((*spaces_).begin(), (*spaces_).end(), IsTransacting) != (*spaces_).end())
		return E_TRANSACTION_OPEN;*/

	std::for_each((*spaces_).begin(), (*spaces_).end(), ClearContainer);
	spaces_->clear();
	return E_OK;
}


int OpenTransaction(UndoContainerId sid)
{

	UndoContainers::iterator s = (*spaces_).find(sid);
	if (s == (*spaces_).end())
		return E_INVALID_ARGUMENT; // user error

	UndoContainer& space = s->second;

	if (space.transacting)
		return E_TRANSACTION_OPEN;

	Transaction empty;
	space.done.push_front(empty);

	space.transacting = true;

	return E_OK;
}


int CommitTransaction(UndoContainerId sid)
{
	return E_OK;
	UndoContainers::iterator s = (*spaces_).find(sid);
	if (s == (*spaces_).end())
		return E_INVALID_ARGUMENT; // user error

	UndoContainer& space = s->second;

	if (!space.transacting)
		return E_NO_TRANSACTION;


	while (space.done.size() >= kMaxUndoStackSize) 
	{
		Transaction& tt = space.done[space.done.size()-1];

		space.cleanup.push_back(tt);
		space.done.pop_back();
	}

	if (space.done.size() < 1) return E_NO_TRANSACTION;

	Transaction& t = space.done[0];

	bool empty_action = true;

	DiskBackup backups = t.diskbackups;
	t.diskbackups.clear();

	for (int i = 0 ; i < backups.size() ; i++) {
		bool ret = CommitBackupNew(space, backups[i]);
		if (ret) empty_action = false;
	}

	space.transacting = false;

	if (empty_action) {
		for_each(t.added.begin(), t.added.end(), MarkBlockFree(space));
		space.done.pop_front();
	} else if (space.undone.size() > 0) {
		for_each(space.undone.begin(), space.undone.end(), RecycleTransaction(space));
		space.undone.clear();
	}

	return E_OK;
}

////////////////////////////////////////////////////////////////////////////////
// memory allocation implementation (very simple free list)

void* Allocate(size_t size, UndoContainerId sid)
{
	//size += 4;
	size = ((size / 8) + 1) * 8; //alignment
								 //size += 4;

	UndoContainers::iterator s = (*spaces_).find(sid);
	if (s == (*spaces_).end())
		return NULL;

	UndoContainer& space = s->second;
	if (!space.transacting)
		return NULL;

	size = max(size, sizeof(Free));

	// TODO: assert that "data" is allocated in space
	space.AssertData();

	// are there any more free chunks?
	if (!space.data->sFreeHead) {
		space.data->Insert(space.More(size));
	}


	// find the first chunk at least the size requested
	Free* prev = 0;
	Free* f = space.data->sFreeHead;
	while (f && (f->size < size)) {
		prev = f;
		f = f->next;
	}

	// if we found one, disconnect it
	if (f) {
		space.data->locTree.remove((AddressAbstractor::key)f);

		if (prev) prev->next = f->next;
		else space.data->sFreeHead = f->next;

		f->next = 0;
		memset(&f->loc, 0, sizeof(f->loc));
	}
	else {
		f = space.More(size);
	}

	// f is disconnected from the free list at this point

	// if the free chunk is too(?) big, carve a peice off and return
	// the rest to the free list
	if (f->size > (2 * (size + sizeof(Free)))) {
		Free* tmp = space.data->Slice(f, size); // slice size byte off 'f'
		space.data->Insert(f); // return the remainder to the free list
		f = tmp;
	}


	void* p = reinterpret_cast<void*>((char*)f + sizeof(Free::SIZE_TYPE));

	return p;

}

void* AllocateChunk(size_t size, UndoContainerId sid)
{
	kAllocChunkMode = true;

	UndoContainers::iterator s = (*spaces_).find(sid);
	if (s == (*spaces_).end()) {
		kAllocChunkMode = false;
		return NULL;
	}

	UndoContainer& space = s->second;
	if (!space.transacting) {
		kAllocChunkMode = false;
		return NULL;
	}

	size = max(size, sizeof(Free));

	// TODO: assert that "data" is allocated in space
	space.AssertData();
	Free* f = space.More(size);
	void* p = reinterpret_cast<void*>((char*)f + sizeof(Free::SIZE_TYPE));

	kAllocChunkMode = false;
	return p;
	return malloc(size);
}


void Deallocate(void* p, UndoContainerId sid)
{
	if (!p) return;

	UndoContainers::iterator s = (*spaces_).find(sid);
	if (s == (*spaces_).end())
		throw "attempt to reallocate unmanaged memory";

	UndoContainer& space = s->second;
	Free* f = Free::FromPtr(p);

	space.data->Insert(f);
	/*if (!p) return;
	free(p);*/
}

void Deallocate(void* p)
{
	free(p);
}


