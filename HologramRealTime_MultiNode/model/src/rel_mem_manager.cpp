#include "model/rel_mem_manager.h"
#include <model/sync_gpu_memory.h>
#include <graphics/sys.h>


RelativeMemoryManager::RelativeMemoryManager()
:data(0)
{
}

RelativeMemoryManager::~RelativeMemoryManager()
{
	if (!data) return;

	Clear();
}


void RelativeMemoryManager::Reset()
{
	Clear();
	if (!data)
		data = new RelativeAllocator();

	FreeElement* f = new FreeElement();
	f->size = size_;
	f->pointer_ = 0;
	data->Insert(f);
}

void RelativeMemoryManager::Clear()
{
	FreeElement* f = data->sFreeElementHead;
	if (!f) return;
	FreeElement* s = f;
	f = f->next;
	delete s;
	while (f) {
		s = f;
		f = f->next;
		delete s;
	}
	delete data;
}
std::vector<graphics::ivec2> RelativeMemoryManager::getRenderInfo()
{
	std::vector<graphics::ivec2> free_start;
	FreeElement* f = data->sFreeElementHead;
	if (!f) return free_start;
	if (f && f->pointer_) { free_start.push_back(graphics::ivec2(0,f->pointer_-1)); }
	int s = f->pointer_ + f->size;
	f = f->next;
	while(f) {
		free_start.push_back(graphics::ivec2(s,f->pointer_-1));
		f = f->next;
	}
	return free_start;
}

int RelativeMemoryManager::getRange()
{
	int ret = 0;
	FreeElement* f = data->sFreeElementHead;
	if (!f) return (int)size_;
	if (f && f->pointer_) { ret = f->pointer_; }
	f = f->next;
	while(f) {
		ret = f->pointer_;
		f = f->next;
	}
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
// memory allocation implementation (very simple FreeElement list)

int RelativeMemoryManager::Allocate(size_t size,  size_t& allocated_memory_size)
{
	// find the first chunk at least the size requested
	FreeElement* prev = 0;
	FreeElement* f = data->sFreeElementHead;

	// first fit

	while (f && (f->size < size)) {
		prev = f;
		f = f->next;
	}
	
	// if we found one, disconnect it
	if (f) {
		data->locTree.remove((size_t)f->pointer_);

		if (prev) prev->next = f->next;
		else data->sFreeElementHead = f->next;
		
		f->next = 0;
		memset(&f->loc, 0, sizeof(f->loc));
	} else {
		allocated_memory_size = 0; // enlarge
		return -1;
	}

	// f is disconnected from the FreeElement list at this point

	// if the FreeElement chunk is too(?) big, carve a peice off and return
	// the rest to the FreeElement list
	if (f->size > (size)) {
		FreeElement* tmp = data->Slice(f, size); // slice size byte off 'f'
		data->Insert(f); // return the remainder to the FreeElement list
		f = tmp;
		allocated_memory_size = size;
	}
	else {
		allocated_memory_size = f->size;
	}

	int p = f->pointer_;
	delete f;

	return p;
}


void RelativeMemoryManager::Deallocate(int p, size_t size)
{
	if (!size) return;

	FreeElement* f = new FreeElement();
	f->pointer_ = p;
	f->size = size;

	data->Insert(f);
	//FreeElement* fr = data->sFreeElementHead;
	//LOG("free :");
	//if (fr) LOG("(%d, %d) ", fr->pointer_, fr->size);
	//while (fr->next) 
	//{
		//fr = fr->next;
		//if (fr) LOG("(%d, %d) ", fr->pointer_, fr->size);
	//}
	//LOG("\n");
	
}

void RelativeMemoryManager::setNewSize(size_t s)
{
	FreeElement* f = data->sFreeElementHead;

	if (!f) {
		FreeElement* nf = new FreeElement();
		nf->pointer_ = size_;
		nf->size = s - nf->pointer_;
		data->Insert(nf);
		size_ = s;
		return;
	}

	while (f->next) {
		f = f->next;
	}
	
	if (f->pointer_ + f->size >= s-1) return;

	FreeElement* nf = new FreeElement();
	f->next = nf;
	nf->pointer_ = f->pointer_ + f->size;
	nf->size = s - nf->pointer_;
	data->Insert(nf);
	size_ = s;
}

size_t RelativeMemoryManager::FreeElementSpace()
{
	return (data ? data->FreeElementSpace() : 0);
}

size_t RelativeMemoryManager::FreeElementCount()
{
	return (data ? data->FreeElementCount() : 0);
}