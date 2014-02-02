////////////////////////////////////////////////////////////////////////////////
// file: fixed_size_pool.h
//
// content: a memory pool class that can allocate memory blocks of fixed size
// note: allocator should be tlsf or some good, fragmentationless shit
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <iostream>




////////////////////////////////////////////////////////////////////////////////
// crappy default allocator
class allocator_dfl {
	public:
		void* malloc(size_t s) {
			return ::malloc(s);
		}
		void free(void* ptr) {
			::free(ptr);
		}
};
static allocator_dfl default_allocator_obj;
//typedef allocator_dfl allocator;


////////////////////////////////////////////////////////////////////////////////
// memory pool for different sizes

template <class T, class allocator=allocator_dfl> 
class CFixedSizePool {
	public:
		// ctors, w/ and w/o specified allocator object, w/ and w/o starting size
		CFixedSizePool();				   // uses default static allocator
		CFixedSizePool(allocator& alloc);  // uses specified object
		CFixedSizePool(size_t init_size);					 // uses default static allocator
		CFixedSizePool(size_t init_size, allocator& alloc);  // uses specified object
		// dtor
		~CFixedSizePool();

		// allocate objects
		T* alloc();
		void free(void* ptr);

		// manipulate storage
		bool extend(size_t n);  // allocate space for n-more objects
		void reset();			// deallocates every memblock allocated by extend, reset values

		// miscellaneous
		void set_growth_rate(size_t static_growth, float growth_factor) {nInitObjects=static_growth; growthFactor=growth_factor;}
		size_t size() {return nTotalObjects;} // return the number of allocated objects (not bytes!)

	private:
		allocator* allocator_obj;	  // the allocator object's reference
		static const size_t obj_size; // size of objects		
		size_t nTotalObjects;		  // total number of objects pre-allocated
		size_t nInitObjects;		  // size of the block that was allocated first
		float growthFactor;

		void* pFirstObj;	// list of free objects (singly linked list)
		void* pFirstBlock;	// list of memory blocks (singly linked list)
};








////////////////////////////////////////////////////////////////////////////////
// function definitions
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// statics
template <class T, class allocator>
const size_t CFixedSizePool<T,allocator>::obj_size = sizeof(void*) > sizeof(T) ? sizeof(void*) : sizeof(T);

////////////////////////////////////////////////////////////////////////////////
// contructors/destructor
// NOTE: init_size is not bytes, it's the number of objects

// uses static default allocator, without init size
template <class T, class allocator>
CFixedSizePool<T,allocator>::CFixedSizePool() {
	pFirstBlock = nullptr;
	pFirstObj = nullptr;
	nTotalObjects = 0;
	nInitObjects = 0;
	growthFactor = 1.2f;
	// create allocator
	allocator_obj = &default_allocator_obj;
}
// uses specified allocator object, without init size
template <class T, class allocator>
CFixedSizePool<T,allocator>::CFixedSizePool(allocator& alloc) { 
	pFirstBlock = nullptr;
	pFirstObj = nullptr;
	nTotalObjects = 0;
	nInitObjects = 0;
	growthFactor = 1.2f;
	// assign allocator
	allocator_obj = &alloc;
}
// uses static default allocator
template <class T, class allocator>
CFixedSizePool<T,allocator>::CFixedSizePool(size_t init_size) {
	pFirstBlock = nullptr;
	pFirstObj = nullptr;
	nTotalObjects = 0;
	nInitObjects = 0;
	growthFactor = 1.2f;
	// create allocator
	allocator_obj = &default_allocator_obj;
	// allocate objects
	extend(init_size);
}
// uses specified allocator object
template <class T, class allocator>
CFixedSizePool<T,allocator>::CFixedSizePool(size_t init_size, allocator& alloc) { 
	pFirstBlock = nullptr;
	pFirstObj = nullptr;
	nTotalObjects = 0;
	nInitObjects = 0;
	growthFactor = 1.2f;
	// assign allocator
	allocator_obj = &alloc;
	// allocate object
	extend(init_size);
}

// dtor
template <class T, class allocator>
CFixedSizePool<T,allocator>::~CFixedSizePool() {
	reset();
}


////////////////////////////////////////////////////////////////////////////////
// extend: adds a new block containing n items to the block-list
template <class T, class allocator>
bool CFixedSizePool<T,allocator>::extend(size_t n) {
	// allocate block
	size_t blockSize = n*obj_size+sizeof(void*);
	void* block = allocator_obj->malloc(blockSize);
	if (!block)
		return false; // return false if allocation of block failed

	// add block to list
	*(reinterpret_cast<void**>(block)) = pFirstBlock;
	pFirstBlock = block;

	// fill block
	size_t s;
	for (s=sizeof(void*); s<blockSize; s+=obj_size) {
		*reinterpret_cast<void**>(s+(size_t)block) = reinterpret_cast<void*>(s+obj_size+(size_t)block);
	};
	s-=obj_size; // loop leaves s pointing to the item past the end of block

	// set list of free block
	*reinterpret_cast<void**>(s+(size_t)block) = pFirstObj;
	pFirstObj = reinterpret_cast<void*>((size_t)block + sizeof(void*));

	// set nTotalObjects if it's not set yet
	if (nInitObjects==0)
		nInitObjects=n;
	nTotalObjects+=n;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// reset: see header
template <class T, class allocator>
void CFixedSizePool<T,allocator>::reset() {
	// free all the blocks
	void* pBlock = pFirstBlock;
	void* tmp;
	while (pBlock) {
		tmp = *reinterpret_cast<void**>(pBlock);
		allocator_obj->free(pBlock);
		pBlock = tmp;
	}
	// reset variables
	pFirstBlock = nullptr;
	pFirstObj = nullptr;
	nTotalObjects = 0;
	nInitObjects = 0;
}


////////////////////////////////////////////////////////////////////////////////
// alloc: simply gives a pointer, does not initialize
template <class T, class allocator>
T* CFixedSizePool<T,allocator>::alloc() {
	void* ptr = pFirstObj;
	if (ptr==nullptr) {
		// extend
		size_t extStatic = 16 > nInitObjects ? 16 : nInitObjects;
		size_t extNum = extStatic > size_t(double(nTotalObjects)*double(growthFactor-1.f)) ? extStatic : size_t(double(nTotalObjects)*double(growthFactor-1.f));
		// run this stuff again
		if (extend(extNum)) {
			void* ptr = pFirstObj;
			pFirstObj = *((void**)ptr);
			return reinterpret_cast<T*>(ptr);
		} 
		else {
			return nullptr;
		}
	}
	pFirstObj = *((void**)ptr);
	return reinterpret_cast<T*>(ptr);
}

////////////////////////////////////////////////////////////////////////////////
// free: only marks block as free, does not call destructor
template <class T, class allocator>
void CFixedSizePool<T,allocator>::free(void* ptr) {
	if (ptr != nullptr) {
		*(void**)ptr = pFirstObj;
		pFirstObj = ptr;
	}
}
