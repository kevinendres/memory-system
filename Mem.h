//-----------------------------------------------------------------------------
// Assignment Copyright 2021, Ed Keenan, all rights reserved.
// Implementation done by Kevin Endres
//----------------------------------------------------------------------------- 

#ifndef MEM_H
#define MEM_H

#include "Heap.h"

class Mem
{
public:
	enum class MemHeaderGuard
	{
		Type_A,
		Type_5
	};

public:

	Mem( MemHeaderGuard type );	

	Mem() = delete;
	Mem(const Mem &) = delete;
	Mem & operator = (const Mem &) = delete;
	~Mem();

	Heap *getHeap();
	void Print(int count);

	//sanity methods
	FreeNode* FindFreeBlock(uint32_t);
	FreeNode* Split(FreeNode* split, unsigned int size);
	FreeNode* Coalesce(FreeNode* coalesce);	
	FreeNode* CoalesceAbove(FreeNode*);
	FreeNode* CoalesceBelow(FreeNode*);
	FreeNode* CoalesceBothSides(FreeNode*);
	bool isBelowFree(const FreeNode* const) const;
	void SetSecretPointer(const FreeNode* const) const;
	void SetAboveFree(const FreeNode* const) const;
	void SetAboveUsed(const UsedNode* const) const;
	void AdvanceNextFit(const FreeNode* const);

	// implement these functions
	void Free( void * const data );
	void *Malloc( const uint32_t size );
	void Initialize( );


private:
	void *poRawMem;		//testing/maintenance only; do not use
	Heap *poHeap;
	void *pTopAddr;
	void *pBottomAddr;
	MemHeaderGuard type;
};

#endif 

// ---  End of File ---

