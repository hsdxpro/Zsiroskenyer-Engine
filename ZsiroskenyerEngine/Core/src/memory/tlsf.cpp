////////////////////////////////////////////////////////////////////////////////
//	file: tlsf.cpp
//
//	content: implements the TLSF class' methods
//			 see header for more
////////////////////////////////////////////////////////////////////////////////

#include "tlsf.h"
#include <stdexcept>
#include <exception>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// constructor
TLSF::TLSF(size_t size, unsigned SLI) {
	growthFactor = (float)TLSF_GROWTHFACTOR_DEFAULT;
	// initialize and allocate memory
	if (!init(SLI)) {
		throw runtime_error("TLSF failed to initialize");
	}
	if (!extend(size)) {
		clear();
		throw runtime_error("TLSF failed to initialize");
	}
}



////////////////////////////////////////////////////////////////////////////////
// initialization
bool TLSF::init(unsigned SLI) {
	// truncates SLI to 5 if it's more
	this->SLI = (SLI>5u ? 5u : SLI);
	// calculates FLI
#ifdef TLSF_X64
	FLI = 63;
#else
	FLI = 31;
#endif
	nFLLists = (FLI-LOG_MBS+1);

	// calculates header size and allocates it
	headerSize = sizeof(void*) * (nFLLists * exp2(this->SLI)) + nFLLists*sizeof(unsigned);
	try {
		header = new unsigned char[headerSize];
	}
	catch (exception&) {
		return false;
	}

	// initialize the header
	bitmap_FL = 0u;
	memset(header, 0, headerSize);

	// set additional variables
	totalSize = 0u;
	baseSize = 0u;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// deinitializes the pool, releases all memory
void TLSF::clear() {
	// delete the header
	if (header)
		delete[] header;

	// delete pools
	for (unsigned i=0; i<pMemBlocks.size(); i++)
		delete[] pMemBlocks[i];
	pMemBlocks.clear();
}

////////////////////////////////////////////////////////////////////////////////
// re-initializes the pool
void TLSF::reset(size_t size, unsigned SLI) {
	clear();
	if (!init(SLI)) {
		throw runtime_error("TLSF failed to initialize");
	}
	if (!extend(size)) {
		clear();
		throw runtime_error("TLSF failed to initialize");
	}
}


////////////////////////////////////////////////////////////////////////////////
// allocating more system memory
bool TLSF::extend(size_t size) {
	// DWORD-align size
	set_bit(&size, 0, 0); set_bit(&size, 1, 0);
	size = size>64 ? size:64;

	// allocate new memory
	void* newMemBlock = (void*)new char[size];
	if (!newMemBlock)
		return false;

	// set base size
	if (baseSize==0u)
		baseSize = size;

	// add block to the list of blocks
	pMemBlocks.push_back(newMemBlock);
	totalSize+=size;

	// set the whole memory piece as a free block
	SHeaderFree* block =  (SHeaderFree*)newMemBlock;

	block->_size(size);	block->is_free(true); block->is_last(true);
	block->previousPhysBlock = nullptr;
	block->nextFreeBlock = nullptr;
	block->previousFreeBlock = nullptr;

	register_free_block(block);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// seamlessly grow the pool (not a client function); used by allocate()
bool TLSF::grow(size_t smin=0) {
	// no size specified
	if (baseSize==0u)
		return extend(TLSF_BASESIZE_DEFAULT);
	
	// calculate size of extension
	size_t extSize;
		// get max of minsize*2, totalSize*growthFactor, baseSize
	extSize = size_t(double(totalSize)*double(growthFactor-1.f));
	extSize = smin*2u > extSize ? smin*2u : extSize;
	extSize = baseSize > extSize ? baseSize : extSize;

	return extend(extSize);
}




////////////////////////////////////////////////////////////////////////////////
// manage free blocks

// registers a free block
void TLSF::register_free_block(SHeaderFree* block) {
	// calculate indices
	unsigned fl, sl;
	mapping(block->_size()-TLSF_BLOCK_OVERHEAD, fl, sl);
	// set bitmaps to 'true'
	set_bit(&bitmap_FL, fl, 1);
	set_bit(_bitmap(fl), sl, 1);

	// insert this block to the list
	SHeaderFree** ppFirst = get_sublist(fl, sl); // address of the pointer that points to the first item's header
	SHeaderFree* pHeaderFirst = *ppFirst; // pointer to the first's header

	// if the list is not empty
	if (pHeaderFirst) {
		// link it to the list
		pHeaderFirst->previousFreeBlock = block;
		block->nextFreeBlock = pHeaderFirst;
		block->previousFreeBlock = nullptr;
		// set the first item as 'block'
		*ppFirst = block;
	}
	// if there is no first, so the list is empty
	else {
		*ppFirst = block;
		// also set block's linkage
		block->nextFreeBlock = nullptr;
		block->previousFreeBlock = nullptr;
	}

	block->is_free(true);
}
// unregisters a free block
void TLSF::unregister_free_block(SHeaderFree* block) {
	// simply remove it from its list if it is not the first of a list
	if (block->previousFreeBlock) {
		//cout << "unregister: not first";
		block->previousFreeBlock->nextFreeBlock = block->nextFreeBlock;
		if (block->nextFreeBlock) {
			block->nextFreeBlock->previousFreeBlock = block->previousFreeBlock;
		}
	}
	// if it is the first, but there are other in the list, replace this with the other
	else if (block->nextFreeBlock) {
		// locate block
		unsigned fl, sl;
		mapping(block->_size()-TLSF_BLOCK_OVERHEAD, fl, sl);

		// bitmaps are left as they are

		// set the first item of the list
		*get_sublist(fl, sl) = block->nextFreeBlock;
		block->nextFreeBlock->previousFreeBlock = nullptr;
	}
	// if it was the only block in this list, set bitmaps and ptr-to-first to nullptr
	else {
		// locate block
		unsigned fl, sl;
		mapping(block->_size()-TLSF_BLOCK_OVERHEAD, fl, sl);
		// set bitmaps to 'false'
		unsigned *bitmap_sl = _bitmap(fl);
		set_bit(bitmap_sl, sl, 0);
		if (*bitmap_sl==0)
			set_bit(&bitmap_FL, fl, 0);
		// set sublist to nullptr
		*get_sublist(fl, sl) = nullptr;
		// set the blocks ptr to nullptr
		block->nextFreeBlock = block->previousFreeBlock = nullptr;
	}
	block->is_free(false);
}



////////////////////////////////////////////////////////////////////////////////
// concatenate free block to larger free block
void TLSF::concat_blocks(SHeaderFree* block1, SHeaderFree* block2) {
	// find and set the prevPhysBlock of the block after block2
	if (block2->is_last() == false) {
		SHeaderUsed* block_after = (SHeaderUsed*)(size_t(block2)+block2->_size()); // we don't care if it's a used block or not
		block_after->previousPhysBlock = (SHeaderUsed*)block1;
	}

	// unregister the first block
	if (block1->is_free()) {
		unregister_free_block(block1);
	}

	// refill the data of block1
	block1->_size(block1->_size() + block2->_size());
	block1->is_last(block2->is_last());
	block1->is_free(true);
		// prevPhysBlock stays the same, linking is managed correctly by register/unregister
	
	// unregister block2, too
	if (block2->is_free()) {
		unregister_free_block(block2);
	}

	// register block1 again as the new, bigger concatenated block
	register_free_block(block1);
}
void TLSF::concat_blocks(SHeaderFree* block1, SHeaderFree* block2, SHeaderFree* block3) {
	// find and set the prevPhysBlock of the block after block3
	if (block3->is_last() == false) {
		SHeaderUsed* block_after = (SHeaderUsed*)(size_t(block3)+block3->_size()); // we don't care if it's a used block or not
		block_after->previousPhysBlock = (SHeaderUsed*)block1;
	}

	// unregister the first block
	if (block1->is_free()){
		unregister_free_block(block1);
	}

	// refill data of block1
	block1->_size(block1->_size() + block2->_size() + block3->_size());
	block1->is_last(block3->is_last());
	block1->is_free(true);
		// prevPhysBlock stays the same, linking is managed correctly by register/unregister

	// unregister both block2 and block3
	if (block2->is_free()) {
		unregister_free_block(block2);
	}
	if (block3->is_free()) {
		unregister_free_block(block3);
	}

	// register block1 as the concatenated block
	register_free_block(block1);
}




////////////////////////////////////////////////////////////////////////////////
// truncate block: creates a used block of the requested size, and a free block from the remaining
//					if not enough space for the free block, it marks the whole as used
TLSF::SHeaderUsed* TLSF::truncate_block(SHeaderFree* block, size_t reqSize) {
	size_t remainingSize = block->_size() - (reqSize+TLSF_BLOCK_OVERHEAD);
	// truncation not possible, just return this block as is
	if (remainingSize < TLSF_BLOCK_OVERHEAD+MBS) {
		// unregister this block
		unregister_free_block(block);
		/* note: these parameters of this block remain the same: is_last, prevPhysBlock */
		block->is_free(false);
		return reinterpret_cast<SHeaderUsed*>(block);
	}
	// else, truncate the block
	else {
		// find the place of the new free block
		SHeaderFree* newFreeBlock = (SHeaderFree*)(size_t(block)+reqSize+TLSF_BLOCK_OVERHEAD);
		//set its parameters
		newFreeBlock->_size(remainingSize);
		newFreeBlock->previousPhysBlock = (SHeaderUsed*)block;
		newFreeBlock->nextFreeBlock = newFreeBlock->previousFreeBlock = nullptr;
		newFreeBlock->is_last(block->is_last());
		newFreeBlock->is_free(true);

		// find and set the prevPhysBlock of the block after 'block'
		if (block->is_last() == false) {
			SHeaderUsed* block_after = (SHeaderUsed*)(size_t(block)+block->_size()); // we don't care if it's a used block or not
			block_after->previousPhysBlock = (SHeaderUsed*)newFreeBlock;
		}

		// ungregister 'block' and register 'newFreeBlock'
		unregister_free_block(block);
		register_free_block(newFreeBlock);

		// set params of block
		block->is_free(false);
		block->is_last(false);
		block->_size(reqSize+TLSF_BLOCK_OVERHEAD);

		return reinterpret_cast<SHeaderUsed*>(block);
	}
	return nullptr;
}



 



////////////////////////////////////////////////////////////////////////////////
// malloc and free


void* TLSF::allocate(size_t size) {
	// if size is too small, set it to bigger
	if (size < MBS)
		size = MBS;
	// dword align size (very important)
	if (size & 0x3) {
		set_bit(&size, 0, 0);
		set_bit(&size, 1, 0);
		size+=4u;
	}

	// find a good block
	SHeaderFree* block;

	// compute fl and sl
	unsigned fl_min, sl_min, fl, sl, bm_sl;
	mapping(size, fl_min, sl_min); fl=fl_min; sl=sl_min;
	size_t bm_fl = bitmap_FL;

	bm_fl >>= fl_min;
	if (!bm_fl) {
		if (grow(size))
			return allocate(size);
		return nullptr;
	}

	fl = fl_min+ffs(bm_fl);

	if (fl>fl_min) {
		bm_sl = *_bitmap(fl);
		sl = ffs(bm_sl);
	}
	else {
		bm_sl = *_bitmap(fl);
		bm_sl >>= sl_min;
		// increase fl
		if (!bm_sl) {
			bm_fl = bitmap_FL;
			fl = fl_min+1;
			bm_fl >>= fl;
			if (!bm_fl) {
				if (grow(size))
					return allocate(size);
				return nullptr;
			}
			bm_fl <<= fl;
			fl = ffs(bm_fl);
			bm_sl = *_bitmap(fl);
			sl = ffs(bm_sl);
		}
		// check block size
		else {
			bm_sl <<= sl_min;
			sl = ffs(bm_sl);
			// increase fl
			if (!bm_sl) {
				bm_fl = bitmap_FL;
				fl = fl_min+1;
				bm_fl >>= fl;
				if (!bm_fl) {
					if (grow(size))
						return allocate(size);
					return nullptr;
				}
				bm_fl <<= fl;
				fl = ffs(bm_fl);
				bm_sl = *_bitmap(fl);
				sl = ffs(bm_sl);
			}
			else {
				// fl & sl is already set
				block = *get_sublist(fl, sl);
				// check the size of this block
				if (block->_size() < size+TLSF_BLOCK_OVERHEAD) {
					if (sl<(exp2(SLI)-1))
						sl+=1;
					else {
						fl++; sl=0;
					}
					if (!bitmap_FL>>fl) {
						if (grow(size))
							return allocate(size);
						return nullptr;
					}
					bm_sl = *_bitmap(fl);
					bm_sl >>= sl;
					// increase fl
					if (!bm_sl) {
						fl+=1;
						bm_fl = bitmap_FL;
						bm_fl >>= fl;
						if (!bm_fl) {
							if (grow(size))
								return allocate(size);
							return nullptr;
						}
						fl += ffs(bm_fl);
						bm_sl = *_bitmap(fl);
						sl = ffs(bm_sl);
					}
					else {
						sl += ffs(bm_sl);
					}
				}
			}
		}
	}	

	// find and truncate this block
	block = *get_sublist(fl, sl);

	if (block->_size() < size+TLSF_BLOCK_OVERHEAD) {
		return nullptr;
	}
	SHeaderUsed* allocatedBlock = truncate_block(block, size);

	// return a pointer to the memory location after this block's header	
	return reinterpret_cast<void*>(allocatedBlock+1);
}




////////////////////////////////////////////////////////////////////////////////
// free
// note: the user might free a "wrong" location, but this case cannot be checked
//		 valid header could be generated anywhere in the allocated memory, and then freed, which causes an error
void TLSF::deallocate(void* ptr) {

	if (ptr == nullptr) {
		return;
	}
	// this must be a used block, get the pointer to the block structure
	SHeaderUsed *block = ((SHeaderUsed*)ptr)-1;
	SHeaderUsed *nextBlock, *prevBlock;

	// this prevents the program from crashing when bad pointer is passed
#ifdef TLSF_DEBUG
	signal(SIGSEGV, SegFaultThrow);
	try {
#endif
		// get neighbouring blocks		
		prevBlock = block->previousPhysBlock;
		if (!block->is_last())
			nextBlock = (SHeaderUsed*)(size_t(block)+block->_size());
		else
			nextBlock = nullptr; 
#ifdef TLSF_DEBUG
		// a little validation check
		if (nextBlock && nextBlock->previousPhysBlock != block) {
			throw 1;
		}
	}
	catch (...) {
		if (error_handler)
			error_handler(6);
		return;
	}
	signal(SIGSEGV, SIG_DFL);
#endif

	// register block as free: 	

	// concat this block with neighbours as needed
	block->is_free(false);
	// if all three are free
	if (prevBlock && prevBlock->is_free() && nextBlock && nextBlock->is_free()) {
		concat_blocks((SHeaderFree*)prevBlock, (SHeaderFree*)block, (SHeaderFree*)nextBlock);
		return;
	}
	// if prev is free
	else if (prevBlock && prevBlock->is_free()) {
		concat_blocks((SHeaderFree*)prevBlock, (SHeaderFree*)block);
		return;
	}
	// if next is free
	else if (nextBlock && nextBlock->is_free()) {
		concat_blocks((SHeaderFree*)block, (SHeaderFree*)nextBlock);
		return;
	}
	// if none is free, then just register this as free
	else {
		block->is_free(true);
		register_free_block((SHeaderFree*)block);	
		return;
	}
}




