//-----------------------------------------------------------------------------
// Assignment Copyright 2021, Ed Keenan, all rights reserved.
// Implementation done by Kevin Endres
//----------------------------------------------------------------------------- 

#include "Free.h"
#include "Used.h"

// Add code here
UsedNode::UsedNode(unsigned int blockSize)
    : mBlockSize((uint16_t) blockSize),
    mAboveBlockFree(false),
    mType(Block::Used),
    pUsedNext(nullptr),         //Need to Revisit
    pUsedPrev(nullptr)          //Need to Revisit
{

}

UsedNode& UsedNode::operator= (const UsedNode& R)
{
    this->mAboveBlockFree = R.mAboveBlockFree;
    this->mBlockSize = R.mBlockSize;
    this->mType = R.mType;
    this->pUsedNext = R.pUsedNext;
    this->pUsedPrev = R.pUsedPrev;
    return *this;
}

UsedNode::~UsedNode()
{
    //do nothing
}

// ---  End of File ---

