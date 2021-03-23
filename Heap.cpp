//-----------------------------------------------------------------------------
// Assignment Copyright 2021, Ed Keenan, all rights reserved.
// Implementation done by Kevin Endres
//----------------------------------------------------------------------------- 

#include "Heap.h"
#include "Mem.h"

Heap::Heap()
:	pUsedHead(nullptr),
	pFreeHead(nullptr),
	pNextFit(nullptr)
{
    peakNumUsed = 0;        // number of peak used allocations
    peakUsedMemory  = 0;    // peak size of used memory

    currNumUsedBlocks = 0;  // number of current used allocations
    currUsedMem = 0;        // current size of the total used memory

    currNumFreeBlocks = 0;  // number of current free blocks
    currFreeMem = 0;        // current size of the total free memory

    pad0 = 0;                // pad

}

/*
Takes header as argument; Adds FreeNode to Heap list
*/
void Heap::AddFree(FreeNode* pFreeAdd)
{
    //update free list, maintain address order
    FreeNode* iterator = this->pFreeHead;
    FreeNode* pAddNext = iterator;
    FreeNode* pAddPrev = iterator;

    //insert into empty list
    if (iterator == nullptr) {
        pFreeAdd->pFreeNext = nullptr;
        pFreeAdd->pFreePrev = nullptr;
        this->pFreeHead = pFreeAdd;
        this->pNextFit = pFreeAdd;
    }
    //insert before existing head
    else if (pFreeAdd < iterator) {
        iterator->pFreePrev = pFreeAdd;
        pFreeAdd->pFreeNext = iterator;
        pFreeAdd->pFreePrev = nullptr;
        this->pFreeHead = pFreeAdd;
    }
    else {
        iterator = iterator->pFreeNext;
        while (iterator != nullptr) {
            //find node before which to insert
            if ((unsigned int)pFreeAdd < (unsigned int)iterator || iterator->pFreeNext == nullptr) {
                pAddNext = iterator;
                pAddPrev = iterator->pFreePrev;
                break;
            }
            else {
                iterator = iterator->pFreeNext;
            }
        }
        //insert insert before iterator, middle of list
        if (iterator) {
            pFreeAdd->pFreeNext = pAddNext;
            pFreeAdd->pFreePrev = pAddPrev;
            pFreeAdd->pFreePrev->pFreeNext = pFreeAdd;
            iterator->pFreePrev = pFreeAdd;
        }
        //insert as last item in list
        else {
            pFreeAdd->pFreeNext = nullptr;
            pFreeAdd->pFreePrev = pAddNext;
            pAddNext->pFreeNext = pFreeAdd;
        }
    }
}

/*
Takes header as argument; Add UsedNode to Heap list
*/
void Heap::AddUsed(UsedNode* pUsedAdd)
{
    //Add to front of used list
    pUsedAdd->pUsedNext = this->pUsedHead;
    if (this->pUsedHead != nullptr) {
        this->pUsedHead->pUsedPrev = pUsedAdd;
    }
    this->pUsedHead = pUsedAdd;
}

/*
Takes header as argument; Removes FreeNode to Heap list
*/
void Heap::RemoveFree(FreeNode* pFreeRemove)
{
    if (pFreeRemove->pFreeNext) {
        pFreeRemove->pFreeNext->pFreePrev = pFreeRemove->pFreePrev;
    }
    if (pFreeRemove->pFreePrev) {
        pFreeRemove->pFreePrev->pFreeNext = pFreeRemove->pFreeNext;
    }
    else {
        this->pFreeHead = pFreeRemove->pFreeNext;
    }
    //update NextFit if that's the node being removed
    if (pFreeRemove == this->pNextFit) {
        this->pNextFit = pFreeRemove->pFreeNext;
        if (this->pNextFit == nullptr) {
            this->pNextFit = this->pFreeHead;
        }
    }
}

void Heap::RemoveUsed(UsedNode* pUsedRemove)
{
    if (pUsedRemove->pUsedNext) {
        pUsedRemove->pUsedNext->pUsedPrev = pUsedRemove->pUsedPrev;
    }
    if (pUsedRemove->pUsedPrev) {
        pUsedRemove->pUsedPrev->pUsedNext = pUsedRemove->pUsedNext;
    }
    else {
        this->pUsedHead = pUsedRemove->pUsedNext;
    }
}

void Heap::IncrementCounters(const FreeNode* const pNode)
{
    ++this->currNumFreeBlocks;
    this->currFreeMem += pNode->mBlockSize;
}

void Heap::IncrementCounters(const UsedNode* const pNode)
{
    ++this->currNumUsedBlocks;
    this->currUsedMem += pNode->mBlockSize;
    if (this->currNumUsedBlocks > this->peakNumUsed) {
        this->peakNumUsed = this->currNumUsedBlocks;
    }
    if (this->currUsedMem > this->peakUsedMemory) {
        this->peakUsedMemory = this->currUsedMem;
    }
}

void Heap::DecrementCounters(const FreeNode* const pNode)
{
    --this->currNumFreeBlocks;
    this->currFreeMem -= pNode->mBlockSize;
}


void Heap::DecrementCounters(const UsedNode* const pNode)
{
    --this->currNumUsedBlocks;
    this->currUsedMem -= pNode->mBlockSize;
}


// ---  End of File ---
