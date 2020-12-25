/**
 * File: dyh_alloc.h
 * Author: DYH
 */

#ifndef _MY_ALLOC_H_
#define _MY_ALLOC_H_

#include <stdio.h>
#include <unistd.h>

#define BATCH (1 << 20)

/**
 * Function: dyh_malloc
 * Use: Allocate SIZE bytes of memory.
 */
void *dyh_malloc(size_t size);

/**
 * Function: dyh_free
 * Use: Free a block of memory allocated.
 */
void dyh_free(void *p_block);

typedef double Align;

/* Head of the block */
typedef union _block {
    struct {
        union _block *ptr; /* Pointer to next block. */
        /**
         * The lowest bit: if it is available (0 for not, 1 for yes).
         * Others: size of the block.
         *
         * size = size_of_the_block + if_available
         * size_of_the_block = size >> 1
         */
        unsigned size;
    } s;
    Align a; /* Force to align. */
} Block;

static Block base = {{&base, 0}}; /* The head node. */
/**
 * Pointer to available memory block,
 * initially pointed to base.
 */
static Block *cur = &base;

/**
 * Allpy to the system for more memory.
 */
static Block *morecore(unsigned nu);
/**
 * Merge neiboring available blocks.
 */
static Block *defrag(Block *pre);

void *dyh_malloc(size_t nbytes) {
    Block *p;

    /**
     * Convert nbytes to number of the cells,
     * each cell for size of Block.
     * Minus 1 to avoid additional allocate.
     */
    unsigned nunits = (nbytes + sizeof(Block) - 1) / sizeof(Block) + 1;

    /**
     * Travel the chain.
     */
    for (p = cur;; p = p->s.ptr) {
        if (p->s.size & 1 && (p->s.size >> 1) >= nunits) {
            /**
             * Found enough available block.
             */
            if ((p->s.size >> 1) == nunits) {
                /**
                 * Available block equals to the need.
                 */
                /* Change the Block to unavailable. */
                p->s.size = nunits << 1;
            } else {
                /**
                 * Available > Needed.
                 */
                /* Divide it into two. */
                (p + nunits)->s.size = p->s.size - (nunits << 1);
                (p + nunits)->s.ptr = p->s.ptr;
                p->s.ptr = p + nunits;

                /* Allocate the first block. */
                p->s.size = nunits << 1;
            }
            /* Move current pointer to the next block. */
            cur = p->s.ptr;

            /* Return the start pointer. */
            return (void *)(p + 1);
        }
        if (p->s.ptr == cur) {
            /**
             * Cannot find enough free blocks.
             * Then allocate from the system.
             */
            if ((p = morecore(nunits)) == NULL) {
                /* Failed. */
                return NULL;
            }
        }
    }
}

/**
 * Allpy to the system for more memory.
 */
static Block *morecore(unsigned nu) {
    char *cp;
    Block *up, *p;
    if (nu < BATCH) {
        /* Set to the minimum. */
        nu = BATCH;
    }

    /* Call sbrk() for more memory. */
    cp = (char *)sbrk(nu * sizeof(Block));
    if (cp == (char *)-1) {
        /* Failed */
        return NULL;
    }
    /**
     * Convert to Block.
     */
    up = (Block *)cp;
    up->s.size = (nu << 1) + 1;

    /* Find a place to insert to the chain. */
    for (p = &base; p->s.ptr != &base && p->s.ptr < up; p = p->s.ptr)
        ;
    /* Insert to the chain. */
    up->s.ptr = p->s.ptr;
    p->s.ptr = up;

    /* Merge neighboring available blocks. */
    return defrag(p);
}

/**
 * Merge neiboring avaible blocks.
 */
static Block *defrag(Block *pre) {
    Block *mid, *pst;

    /* pre->mid->pst */
    mid = pre->s.ptr;
    pst = mid->s.ptr;

    if ((pre + (pre->s.size >> 1) > mid) ||
        (pst != &base && mid + (mid->s.size >> 1) > pst)) {
        return NULL;
    }

    if ((pst->s.size & 1) && (mid + (mid->s.size >> 1) == pst)) {
        /**
         * mid and pst are neighboring.
         * Merge them.
         */
        mid->s.ptr = pst->s.ptr;
        mid->s.size += pst->s.size - 1;
    }

    if ((pre->s.size & 1) && (pre + (pre->s.size >> 1) == mid)) {
        /**
         * pre and mid are neighboring.
         * Merge them.
         */
        pre->s.ptr = mid->s.ptr;
        pre->s.size += mid->s.size - 1;
    }

    return pre;
}

#endif