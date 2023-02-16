#ifndef ALLOCATOR_H_INCLUDED
#define ALLOCATOR_H_INCLUDED

#include "model/MemManager.h"
#include <memory>

extern unsigned int kCurrentUndoContainerId;

template <class T>
class Allocator 
{
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
		
	typedef T* pointer;
	typedef const T* const_pointer;
		
	typedef T& reference;
	typedef const T& const_reference;
		
	pointer address(reference r) const { return &r; }
	const_pointer address(const_reference r) const { return &r; }
		
	Allocator() throw() {}
	
	template <class U> 
	Allocator(const Allocator<U>&) throw() {}
		
	~Allocator() throw() {}
		
		
	pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
	{
		return static_cast<pointer>(malloc(n*sizeof(value_type))/*Allocate(n * sizeof(value_type), kCurrentUndoContainerId)*/);
	}
		
	void deallocate(void* p, size_type n)
	{
		free(p);/*Deallocate(p)*/;
	}
		
	void construct(pointer p, const T& val) { new(p) T(val); }
	void destroy(pointer p) { p->~T(); }
		
	size_type max_size() const throw() { return (size_type)(-1); }
		
	template <class U>
	bool operator==(const Allocator<U>& other) const
	{ return (true); }

	template <class U>
	bool operator!=(const Allocator<U>& other)  const
	{ return (false); }

	template <class U>
	struct rebind { typedef Allocator<U> other; };
};




#endif // ALLOCATOR_H_INCLUDED
