////////////////////////////////////////////////////////////////////////////////
// file: tlsf.h
//
// Two-Level Segregated Fit (TLSF)
//	This memory allocator groups allocations to size-classes. Size-classes
//	are determined in a two level process, first on a logarithmic scale 
//	(first level classes), and then first level classes are linearly split
//	into second level classes. This method provides 'good fit' (not best),
//	and extremely fast (160-200 cycle) allocations, with minimized internal
//	fragmentation.	
//
// Applications:
//	Use this allocater everywhere you can.
//	The only drawback is the fact that it cannot shrink in any ways.
//
// Known issues:
//	It is not scalable with multithreading.
////////////////////////////////////////////////////////////////////////////////


// include
#pragma once

#include <vector>
#include <cstdint>

#ifdef _M_X64
#define TLSF_X64  // define if compiling for x64
#endif



////////////////////////////////////////////////////////////////////////////////
// helper functions:

#ifdef WIN32 // WIN32 instrinsics

#include <intrin.h>

// log2 and 2^n 
inline unsigned log2(uint32_t n) {
	unsigned long idx;
	_BitScanReverse(&idx, n);
	return idx;
}
#ifdef TLSF_X64
inline unsigned log2(uint64_t n) {
	unsigned long idx;
	_BitScanReverse64(&idx, n);
	return idx;
}
#endif
#ifndef TLSF_X64
inline size_t exp2(unsigned y) {
	return (1u<<y);
}
#else
inline size_t exp2(unsigned y) {
	return (1ull<<y);
}
#endif

// find first set
inline unsigned ffs(uint32_t arg) {
	unsigned long idx;
	_BitScanForward(&idx, arg);
	return idx;
}
#ifdef TLSF_X64
inline unsigned ffs(uint64_t arg) {
	unsigned long idx;
	_BitScanForward64(&idx, arg);
	return idx;
}
#endif

// set bits
inline void set_bit(uint32_t* arg, unsigned char bit, bool val) {
	if (val) {
		_bittestandset(reinterpret_cast<long*>(arg), bit);
	}
	else {
		_bittestandreset(reinterpret_cast<long*>(arg), bit);
	}
}
#ifdef TLSF_X64
inline void set_bit(uint64_t* arg, unsigned char bit, bool val) {
	if (val) {
		_bittestandset64(reinterpret_cast<long long*>(arg), bit);
	}
	else {
		_bittestandreset64(reinterpret_cast<long long*>(arg), bit);
	}
}
#endif
inline bool get_bit(uint32_t* arg, unsigned char bit) {
	return _bittest(reinterpret_cast<long*>(arg), bit) != 0;
}
#ifdef TLSF_X64
inline bool get_bit(uint64_t* arg, unsigned char bit) {
	return _bittest64(reinterpret_cast<long long*>(arg), bit) != 0;
}
#endif

#else // WIN32 not def
static_assert(false, "TLSF bit functions not implemented for this compiler!");
#endif // WIN32



////////////////////////////////////////////////////////////////////////////////
// defines for class TLSF
#define MBS (sizeof(size_t)*2u)		// min block size, bytes - set to 8/16 byte (x86/x64)
#define LOG_MBS (log2(MBS))

#define TLSF_BLOCK_OVERHEAD (sizeof(size_t)*2u)

// mask for reading sizes
#ifndef TLSF_X64
#define SIZE_MASK 0xFFFFFFFCu
#else
#define SIZE_MASK 0xFFFFFFFFFFFFFFFCull
#endif

#define TLSF_BIT_ISLAST 1
#define TLSF_BIT_ISFREE 0

// growth properties
#define TLSF_GROWTHFACTOR_DEFAULT 1.2f	// float/double
#define TLSF_BASESIZE_DEFAULT 16384		// 16k


////////////////////////////////////////////////////////////////////////////////
// class: TLSF
// this version can grow dynamically, as more memory is needed
// note that it can NOT shrink, even if it's mostly empty
////////////////////////////////////////////////////////////////////////////////
class TLSF {
	public:
		TLSF(size_t size=256, unsigned SLI=5);  // initializing constructor		
		~TLSF() {clear();}				  // destructor

		void* allocate(size_t size);	// allocates memory from the pool (min size: 8/16 byte, overhead: 8/16 byte (x86/x64))
		void deallocate(void* ptr);		// frees memory: some error handling for bad pointers, debug mode only

		void reset(size_t size=256, unsigned SLI=5);

		void set_growth_rate(size_t static_growth, float growth_factor) {baseSize=static_growth; growthFactor=growth_factor;}
	private:
		// manage pool size and initialization
		bool init(unsigned SLI);	// initialization
		bool extend(size_t size);	// both implicit and explicit call to allocate more memory from heap
		void clear();				// de-initialization -> frees everything! pool won't work
		bool grow(size_t smin);		// internally handles pool growing
		
		// data that describes the memory pool
		unsigned FLI;		// first level index = log2(max_size), set to 31 or 63
		unsigned nFLLists;	// number of first level lists = FLI-log2(minBlockSize)
		unsigned SLI;		// second level index: set to 4 or 5
		size_t headerSize;	// size of the bookkeeping data header
		size_t totalSize;	// total size of memory allocated by the pool
		size_t baseSize;	// this gives a hint to the pool on how to scale
		float growthFactor;	// scaling becomes exponential after a while

		void* header;		// pointer to the header structure: bitmaps for second-level + linked list start point ptrs
		size_t bitmap_FL;	// bitmap lookup table for the 32/64 first-level class
		std::vector<void*> pMemBlocks;  // pointers to the blocks allocated from heap (may change type for efficiency)


		// helper structs for headers of free and occupied blocks
		struct SHeaderUsed {
			size_t size;
			SHeaderUsed* previousPhysBlock;

			inline size_t _size() {return (size&SIZE_MASK);}
			inline void _size(size_t s) {size&=0x3; size |= (s&SIZE_MASK);}

			inline bool is_free() {return get_bit(&size, TLSF_BIT_ISFREE);}
			inline void is_free(bool val) {set_bit(&size, TLSF_BIT_ISFREE, val);}
			inline bool is_last() {return get_bit(&size, TLSF_BIT_ISLAST);}
			inline void is_last(bool val) {set_bit(&size, TLSF_BIT_ISLAST, val);}
		};
		struct SHeaderFree {
			size_t size;
			SHeaderUsed* previousPhysBlock;
			SHeaderFree* previousFreeBlock;
			SHeaderFree* nextFreeBlock;

			inline size_t _size() {return (size&SIZE_MASK);}
			inline void _size(size_t s) {size&=0x3; size |= (s&SIZE_MASK);}

			inline bool is_free() {return get_bit(&size, TLSF_BIT_ISFREE);}
			inline void is_free(bool val) {set_bit(&size, TLSF_BIT_ISFREE, val);}
			inline bool is_last() {return get_bit(&size, TLSF_BIT_ISLAST);}
			inline void is_last(bool val) {set_bit(&size, TLSF_BIT_ISLAST, val);}
		};

		// short functions
		// this maps a block of reqSize to the first and second indices
		inline void TLSF::mapping(size_t size, unsigned& fl, unsigned& sl) {
			fl = log2(size);
			sl = unsigned(((size^exp2(fl)) << SLI) >> fl);
		}
		// return pointer to the bitmap that is used for fl's second-level lists
		inline unsigned* _bitmap(unsigned fl) {
			return (unsigned*)(size_t(header) + sizeof(unsigned)*(fl-LOG_MBS));
		}
		// return the address of the pointer to the first block of sublist defined by fl & sl
		inline SHeaderFree** get_sublist(unsigned fl, unsigned sl) {
			return (SHeaderFree**)(size_t(header) + ((fl-LOG_MBS)*(exp2(SLI)) + sl)*sizeof(size_t) + nFLLists*sizeof(unsigned));
		}

		// registers a free block
		void register_free_block(SHeaderFree* block);
		// unregisters a free block
		void unregister_free_block(SHeaderFree* block);
		// concatenate two or three neighbouring free blocks
		void concat_blocks(SHeaderFree* block1, SHeaderFree* block2);
		void concat_blocks(SHeaderFree* block1, SHeaderFree* block2, SHeaderFree* block3);
		// truncates a free block to specified size
		SHeaderUsed* truncate_block(SHeaderFree* block, size_t reqSize);
};










