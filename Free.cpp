//-----------------------------------------------------------------------------
// Assignment Copyright 2021, Ed Keenan, all rights reserved.
// Implementation done by Kevin Endres
//----------------------------------------------------------------------------- 

#include "Used.h"
#include "Free.h"
#include "Block.h"

// Add magic here
FreeNode::FreeNode(unsigned int blockSize)
    : mBlockSize((uint16_t)blockSize),
    mAboveBlockFree(false),
    mType(Block::Free),
    pFreeNext(nullptr),
    pFreePrev(nullptr)
{
}

FreeNode::FreeNode(const FreeNode &R)
    : mBlockSize((uint16_t)R.mBlockSize),
    mAboveBlockFree(R.mAboveBlockFree),
    mType(Block::Free),
    pFreeNext(R.pFreeNext),
    pFreePrev(R.pFreePrev)
{
}

FreeNode& FreeNode::operator= (const FreeNode& R)
{
    this->mAboveBlockFree = R.mAboveBlockFree;
    this->mBlockSize = R.mBlockSize;
    this->mType = R.mType;
    this->pFreeNext = R.pFreeNext;
    this->pFreePrev = R.pFreePrev;
    return *this;
}

FreeNode::~FreeNode()
{
    //do nothing
}
// ---  End of File ---
