#ifndef __rel_mem_manager_h
#define __rel_mem_manager_h

#include	<windows.h>

#include	<map>
#include	<vector>
#include	<deque>
#include	<algorithm>

#include	<conio.h>

#include	<math.h>
#include	"model/avl_tree.h"

#include	<graphics/ivec.h>



struct FreeElement {
	FreeElement() :  size(0), next(0) {}; // use only with placement new

	FreeElement(const FreeElement&) {}
	~FreeElement() {};

	typedef unsigned long	SIZE_TYPE;
	SIZE_TYPE	size; 
	FreeElement*		next;

	struct _bylocation {
		_bylocation() : lt(0), gt(0), bf(0) {}
		FreeElement*		lt;
		FreeElement*		gt;
		int			bf;
	} loc;

	int	   pointer_;


	int UserPtr()
	{ 
		return pointer_;
	}

	int Begin()
	{
		return pointer_;
	}

	int End()
	{
		return pointer_ + size;
	}

	bool Adjacent(FreeElement* f)
	{
		return (f && (this->End() == f->Begin()));
	}

	bool Overlap(FreeElement* f) 
	{
		return (this->End() > f->Begin());
	}


};

////////////////////////////////////////////////////////////////////////////////
//  

struct Addressing { // for AVL tree
	typedef FreeElement* handle;
	typedef int key;
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

	int compare_key_node(key k, handle h) { return(compare_key_key(k, (key)h->pointer_)); }
	int compare_node_node(handle h1, handle h2) { return(compare_key_key((key)h1->pointer_, (key)h2->pointer_)); }

	handle null(void) { return(0); }
	static bool read_error(void) { return(false); }
};

////////////////////////////////////////////////////////////////////////////////
// 

struct RelativeAllocator
{
	FreeElement*			sFreeElementHead;
	abstract_container::avl_tree<Addressing, 31> locTree;


	RelativeAllocator() : sFreeElementHead(0) {}

	FreeElement* Slice(FreeElement* from, size_t bytes)
	{
		const size_t true_bytes = bytes;
		if (from->size < true_bytes) 
			return 0;

		FreeElement* slice = new FreeElement();
		slice->pointer_ = from->UserPtr();
		from->pointer_ = (slice->pointer_ + bytes);
		slice->size = bytes;
		from->size -= true_bytes;

		return slice;
	}


	void Insert(FreeElement* node)
	{
		// precautionary
		node->next = 0;
		memset(&node->loc, 0, sizeof(node->loc));

		if (sFreeElementHead == 0) {
			sFreeElementHead = node;
			locTree.insert(node);
		} else if (sFreeElementHead->Begin() > node->Begin()) {
			node->next = sFreeElementHead;
			sFreeElementHead = node;
			locTree.insert(node);

			Compact(sFreeElementHead);
		} else {
			FreeElement* cursor = locTree.search((size_t)node->pointer_, abstract_container::LESS);
			node->next = cursor->next;
			cursor->next = node;
			locTree.insert(node);

			Compact(node);
			Compact(cursor);
		}
	}

	void Compact(FreeElement* node)
	{
		if (node->Adjacent(node->next)) {
			//printf("compact\n");
			locTree.remove((size_t)node->next->pointer_);

			node->size += (node->next->size);
			FreeElement* save = node->next;
			node->next = node->next->next;
			delete save;
		}
	}

	size_t FreeElementSpace()
	{
		size_t unused = 0;
		FreeElement* f = sFreeElementHead;
		while (f) { 
			unused += f->size; 
			f = f->next; 
		}
		return unused;
	}

	size_t FreeElementCount()
	{
		size_t blocks = 0;
		FreeElement* f = sFreeElementHead;
		while (f) { ++blocks; f = f->next; }
		return blocks;
	}

};


class 	RelativeMemoryManager {

public:

	RelativeMemoryManager();

	~RelativeMemoryManager();

	void Clear();
	// Clear and Create new space
	void Reset();

	// allocation: return position of the allocated
	int Allocate(size_t size, size_t& allocated_memory_size);

	void  Deallocate(int ptr, size_t size);

	// debug
	size_t FreeElementSpace();
	size_t FreeElementCount();
	
	void setBaseAddr(unsigned __int64 b) { base_addr_ = b; }
	unsigned __int64 baseAddr() const { return base_addr_; }
	void CreateSpace(int data_block, size_t size)
	{
		size_ = size;
		if (!data)
			data = new RelativeAllocator();

		FreeElement* f = new FreeElement();
		f->size = size;
		f->pointer_ = data_block;
		data->Insert(f);
	}

	size_t size() const { return size_; }
	void setNewSize(size_t s);

	int address(int index) const { return base_addr_ + index; }

	std::vector<graphics::ivec2> getRenderInfo();

	int getRange();
public:



private:
	RelativeAllocator* data;

	unsigned __int64 base_addr_;
	size_t			 size_;
};
#endif