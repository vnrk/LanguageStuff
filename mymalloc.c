/*
The struct "Header" is, as you might have guessed, the declaration of a node in a linked list. Each such node contains a pointer to the next one. I don't quite understand the morecore function, nor the "wrap-around", I have never used this function, nor sbrk. But I assume that it allocates a header as specified in this struct, and also some chunk of raw data following that header. If so, that explains why there is no actual data pointer: the data is assumed to follow the header, adjacently in memory. So for each node, we get the header, and we get a chunk of raw data following the header.

The iteration itself is pretty straight-forward, they are going through a single-linked list, one node at a time.

At the end of the loop, they set the pointer to point one past the end of the "chunk", then store that in a static variable, so that the program will remember where it previously allocated memory, next time the function is called.

They are using a trick to make their header end up on an aligned memory address: they store all the overhead info in a union together with a variable large enough to correspond to the platform's alignment requirement. So if the size of "ptr" plus the size of "size" are too small to give the exact alignment, the union guarantees that at least sizeof(Align) bytes are allocated. I believe that this whole trick is obsolete today, since the C standard mandates automatic struct/union padding.
*/
#include <stdbool.h>

typedef long Align;                      /* for alignment to long boundary */

typedef union header                     /* block header */
{
  struct
  {
    union header *ptr;                   /* next block if on free list */
    size_t size;                         /* size of this block */
  } s;

  Align x;                               /* force alignment of blocks */

} Header;


static Header base = {0};                /* empty list to get started */
static Header* freeptr = NULL;           /* start of free list */


/* malloc: general-purpose storage allocator */
void* kr_malloc (size_t nbytes)
{
  Header*  p;
  Header*  prevptr;
  size_t   nunits;
  void*    result;
  bool     is_allocating;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;

  prevptr = freeptr;
  if (prevptr == NULL)                   /* no free list yet */
  {
    base.s.ptr  = &base;
    freeptr     = &base;
    prevptr     = &base;
    base.s.size = 0;
  }

  is_allocating = true;
  for (p = prevptr->s.ptr; is_allocating; p = p->s.ptr)
  {
    if (p->s.size >= nunits)             /* big enough */
    {
      if (p->s.size == nunits)           /* exactly */
      {
        prevptr->s.ptr = p->s.ptr;
      }
      else                               /* allocate tail end */
      {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }

      freeptr = prevptr;
      result = p+1;
      is_allocating = false;             /* we are done */
    }

    if (p == freeptr)                    /* wrapped around free list */
    {
      p = morecore(nunits);
      if (p == NULL)
      {
        result = NULL;                   /* none left */
        is_allocating = false;
      }
    }
    prevptr = p;
  } /* for */

  return result;
}