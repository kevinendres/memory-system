//-----------------------------------------------------------------------------
// Assignment Copyright 2021, Ed Keenan, all rights reserved.
// Implementation done by Kevin Endres
//----------------------------------------------------------------------------- 

#ifndef USED_H
#define USED_H

#include "Types.h"
#include "Block.h"

class FreeNode;

class UsedNode
{
public:
   // ---------------------------------------------------------
   // Do not reorder, change or add data fields
   //     --> any changes to the data... is a 0 for assignment
   // You can add methods if you wish
   // ---------------------------------------------------------
    UsedNode() = delete;
    UsedNode(const UsedNode&) = delete;
    UsedNode& operator = (const UsedNode&);
    UsedNode(unsigned int blockSize);
    ~UsedNode();

    uint16_t   mBlockSize;       // size of block (16 bit)
    bool       mAboveBlockFree;  // AboveBlock flag:
                                 // if(AboveBlock is type free) -> true 
                                 // if(AboveBlock is type used) -> false
    Block      mType;            // block type 
    UsedNode   *pUsedNext;       // next used block
    UsedNode   *pUsedPrev;       // prev used block


};

#endif 

// ---  End of File ---

