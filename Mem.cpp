//-----------------------------------------------------------------------------
// Assignment Copyright 2021, Ed Keenan, all rights reserved.
// Implementation done by Kevin Endres
//----------------------------------------------------------------------------- 

#include "Mem.h"
#include "Heap.h"
#include "Block.h"

// To help with coalescing... not required
struct SecretPtr
{
	FreeNode *pFree;
};

// ----------------------------------------------------
//  Initialized the Memory block:
//  Format the Heap
//  Create a Free block
//  Heap structure links to Free hdr
//-----------------------------------------------------
void Mem::Initialize()
{
	Heap* pHeap = this->poHeap;
	FreeNode* pFreeHdrStart = (FreeNode*)this->pTopAddr;
	FreeNode* pFreeHdrEnd = pFreeHdrStart + 1;
	unsigned int blockSize = (unsigned int)this->pBottomAddr - (unsigned int)pFreeHdrEnd;
	FreeNode* pFreeNode = new(pFreeHdrStart) FreeNode(blockSize);

	pHeap->pUsedHead = nullptr;
	pHeap->pFreeHead = pFreeNode;
	pHeap->pNextFit = pFreeNode;
	pHeap->currFreeMem = pFreeNode->mBlockSize;
	++pHeap->currNumFreeBlocks;
}

// ----------------------------------------------------
//  Do your Magic here:
//  Find a Free block that fits
//  Change it to used (may require subdivision)
//  Correct the heap Links (used,free) headers 
//  Update stats
//  Return pointer to block
//-----------------------------------------------------
void *Mem::Malloc( const uint32_t _size )
{
	Heap* pHeap = this->poHeap;
	UsedNode* mallocBlock = nullptr;
	UsedNode* mallocHdr = nullptr;
	FreeNode* freeBlock;

	//find the header of the block to allocate
	freeBlock = FindFreeBlock(_size);

	//check to see if suitable free block was found
	if (freeBlock != nullptr) {
		//update counters/lists for Free Nodes/Memory
		AdvanceNextFit(freeBlock);
		pHeap->RemoveFree(freeBlock);
		pHeap->DecrementCounters(freeBlock);
		//allocate the block
		mallocHdr = new(freeBlock) UsedNode(_size);
		mallocBlock = mallocHdr + 1;
		//update counters/lists for Used Nodes/Memory
		pHeap->AddUsed(mallocHdr);
		pHeap->IncrementCounters(mallocHdr);
		SetAboveUsed(mallocHdr);
	}
	else { 
		//do nothing
	}
	return mallocBlock;
}

// ----------------------------------------------------
//  Do your Magic here:
//  Return the Free block to the system
//  May require Coalescing
//  Correct the heap Links (used,free) headers 
//  Update stats
//-----------------------------------------------------
void Mem::Free( void * const data )
{
	Heap* pHeap = this->poHeap;
	bool aboveFree, belowFree;
	FreeNode* dataHdr = (FreeNode*)((unsigned int)data - (unsigned int)sizeof(FreeNode));
	pHeap->RemoveUsed((UsedNode *)dataHdr);	
	pHeap->DecrementCounters((UsedNode*)dataHdr);
	dataHdr->mType = Block::Free;

	aboveFree = dataHdr->mAboveBlockFree;
	belowFree = isBelowFree(dataHdr);

	//coalesce if there's a free neighbor
	if (aboveFree || belowFree) {
		dataHdr = Coalesce(dataHdr);	//Coalesce handles counters
	}
	else {
		//free current node
		pHeap->AddFree(dataHdr);
		pHeap->IncrementCounters(dataHdr);
		SetSecretPointer(dataHdr);
	}

	SetAboveFree(dataHdr);
} 
 
/*
 Takes header pointer and size of requested block; Returns the higher address new block
 */
FreeNode* Mem::Split(FreeNode* split, unsigned int size)
{
	Heap* pHeap = this->poHeap;
	//create new FreeNode
	FreeNode* newFreeLoc = (FreeNode*)((unsigned int)split + size + sizeof(FreeNode));
	FreeNode* newFreeNode = new(newFreeLoc) FreeNode(*split);
	//update pointers
	if (split->pFreeNext != nullptr) {
		split->pFreeNext->pFreePrev = newFreeNode;
	}
	split->pFreeNext = newFreeNode;
	newFreeNode->pFreePrev = split;
	//update sizes
	split->mBlockSize = (uint16_t)size;
	newFreeNode->mBlockSize -= ((uint16_t)size + sizeof(FreeNode));
	//update counter
	++pHeap->currNumFreeBlocks;
	pHeap->currFreeMem -= sizeof(FreeNode);
	//ensure that new node has valid secret pointer
	SetSecretPointer(newFreeNode);
	return split;
} 

FreeNode* Mem::Coalesce(FreeNode* coalesce)
{
	FreeNode* result = nullptr;
	bool aboveFree = coalesce->mAboveBlockFree;
	bool belowFree = isBelowFree(coalesce);

	//update counter
	this->poHeap->currFreeMem += coalesce->mBlockSize + sizeof(FreeNode);

	if (aboveFree && belowFree) {
		result = CoalesceBothSides(coalesce);
	}
	else if (aboveFree) {
		result = CoalesceAbove(coalesce);
	}
	else {
		result = CoalesceBelow(coalesce);
	}
	SetSecretPointer(result);
	return result;
}

FreeNode* Mem::CoalesceAbove(FreeNode* coalesce)
{
	FreeNode* result;
	Heap* pHeap = this->poHeap;
	//coalesce above only
	FreeNode* pPrev = ((SecretPtr *)((unsigned int)coalesce - sizeof(SecretPtr)))->pFree;
	//update size
	pPrev->mBlockSize += sizeof(FreeNode) + coalesce->mBlockSize;

	result = pPrev;
	//update NextFit if needed
	if (pHeap->pNextFit == coalesce) {
		AdvanceNextFit(result);
	}
	if (pHeap->pFreeHead == coalesce) {
		pHeap->pFreeHead = result;
	}
	return result;
}

FreeNode* Mem::CoalesceBelow(FreeNode* coalesce)
{
	FreeNode* result;
	Heap* pHeap = this->poHeap;
	//coalesce below only
	FreeNode* pNext = (FreeNode*)((unsigned int)coalesce + coalesce->mBlockSize + sizeof(FreeNode));
	//update size
	coalesce->mBlockSize += sizeof(FreeNode) + pNext->mBlockSize;
	//update pointers
	coalesce->pFreeNext = pNext->pFreeNext;
	if (pNext->pFreeNext != nullptr) {
		pNext->pFreeNext->pFreePrev = coalesce;
	}
	coalesce->pFreePrev = pNext->pFreePrev;
	if (pNext->pFreePrev != nullptr) {
		pNext->pFreePrev->pFreeNext = coalesce;
	}
	result = coalesce;
	//update NextFit if needed
	if (pHeap->pNextFit == pNext) {
		AdvanceNextFit(result);
	}
	if (pHeap->pFreeHead == pNext) {
		pHeap->pFreeHead = result;
	}
	return result;
}

FreeNode* Mem::CoalesceBothSides(FreeNode* coalesce)
{
	FreeNode* result;
	Heap* pHeap = this->poHeap;
	//coalesce on both sides
	FreeNode* pNext = (FreeNode*)((unsigned int)coalesce + coalesce->mBlockSize + sizeof(FreeNode));
	FreeNode* pPrev = ((SecretPtr *)((unsigned int)coalesce - sizeof(SecretPtr)))->pFree;
	//update size
	pPrev->mBlockSize += (2 * sizeof(FreeNode)) + coalesce->mBlockSize + pNext->mBlockSize;
	//update pointers
	pPrev->pFreeNext = pNext->pFreeNext;
	if (pNext->pFreeNext != nullptr) {
		pNext->pFreeNext->pFreePrev = pPrev;
	}
	result = pPrev;
	//update special case counters
	--pHeap->currNumFreeBlocks;
	pHeap->currFreeMem += sizeof(FreeNode);	//compounds with memory addition before if/else branch
	//update NextFit if needed
	if (pHeap->pNextFit == coalesce || pHeap->pNextFit == pNext) {
		AdvanceNextFit(result);
	}
	if (pHeap->pFreeHead == coalesce || pHeap->pFreeHead == pNext) {
		pHeap->pFreeHead = result;
	}
	return result;
}

/*
Takes const pointer to header
*/
bool Mem::isBelowFree(const FreeNode* const pNode) const
{
	bool result = false;
    const FreeNode* probe = (FreeNode*)((unsigned int)pNode + (unsigned int)pNode->mBlockSize + (unsigned int)sizeof(FreeNode));
	if (probe < this->pBottomAddr) {
		result = (probe->mType == Block::Free);
	}
	return result;
}

void Mem::SetAboveFree(const FreeNode* const pNode) const
{
    UsedNode* probe = (UsedNode*)((unsigned int)pNode + (unsigned int)pNode->mBlockSize + (unsigned int)sizeof(FreeNode));
	if (probe < this->pBottomAddr) {
		probe->mAboveBlockFree = true;
	}
}

void Mem::SetAboveUsed(const UsedNode* pNode) const
{
    UsedNode* probe = (UsedNode*)((unsigned int)pNode + (unsigned int)pNode->mBlockSize + (unsigned int)sizeof(FreeNode));
	if (probe < this->pBottomAddr) {
		probe->mAboveBlockFree = false;
	}
}

void Mem::SetSecretPointer(const FreeNode* const pNode) const
{
	FreeNode* ptrLocation = (FreeNode *)((unsigned int)pNode + (unsigned int)pNode->mBlockSize 
		+ (unsigned int)sizeof(FreeNode) - (unsigned int)sizeof(FreeNode*));
	SecretPtr* secretPtr = new(ptrLocation) SecretPtr;
	secretPtr->pFree = (FreeNode *)pNode;
}

void Mem::AdvanceNextFit(const FreeNode* const newNext)
{
	Heap* pHeap = this->poHeap;
	pHeap->pNextFit = (FreeNode *)newNext;
	if (pHeap->pNextFit == nullptr) {
		pHeap->pNextFit = pHeap->pFreeHead;
	}
}

FreeNode* Mem::FindFreeBlock(uint32_t _size)
{
	FreeNode* iterator = this->poHeap->pNextFit;
	Heap* pHeap = this->poHeap;
	//find the header of the block to allocate
	do {
		if (iterator->mBlockSize == _size) {
			//found perfect block 
			break;
		}
		else if (iterator->mBlockSize > _size) {
			//found a splittable block
			iterator = Split(iterator, _size);
			break;
		}
		//keep walking the list
		else {
			if (iterator->pFreeNext != nullptr) {
				iterator = iterator->pFreeNext;
			}
			else {
				iterator = pHeap->pFreeHead;
			}
		}
	} while (iterator != pHeap->pNextFit);
	return iterator;
}

// ---  End of File ---
