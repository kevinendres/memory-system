//-----------------------------------------------------------------------------
// Assignment Copyright 2021, Ed Keenan, all rights reserved.
// Implementation done by Kevin Endres
//----------------------------------------------------------------------------- 

#ifndef FREE_H
#define FREE_H

#include "Types.h"
#include "Block.h"

class UsedNode;

class FreeNode
{
public:
    // ---------------------------------------------------------
    // Do not reorder, change or add data fields
    //     --> any changes to the data... is a 0 for assignment
    // You can add methods if you wish
    // ---------------------------------------------------------
    FreeNode() = delete;
    FreeNode(unsigned int blocksize);
    FreeNode(const FreeNode&);
    FreeNode& operator = (const FreeNode&);
    ~FreeNode();

    uint16_t mBlockSize;        // size of block
    bool     mAboveBlockFree;   // AboveBlock flag:
                                // if(AboveBlock is type free) -> true 
                                // if(AboveBlock is type used) -> false
    Block    mType;             // block type 
    FreeNode *pFreeNext;        // next free block
    FreeNode *pFreePrev;        // prev free block

};

#endif 

// ---  End of File ---
