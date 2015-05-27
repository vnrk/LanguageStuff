/*
memmove offers guaranteed behavior if the memory regions pointed to by the source and destination arguments overlap. memcpy makes no such guarantee, and may therefore be more efficiently implementable. When in doubt, it's safer to use memmove.
The problem with this code is in that additional test: the comparison (dp < sp) is not quite portable (it compares two pointers which do not necessarily point within the same object) and may not be as cheap as it looks. On some machines (particularly segmented architectures), it may be tricky and significantly less efficient
*/

void *memmove(void *dest, void const *src, size_t n)
{
	register char *dp = dest;
	register char const *sp = src;
	if(dp < sp) {
		while(n-- > 0)
			*dp++ = *sp++;
	} else {
		dp += n;
		sp += n;
		while(n-- > 0)
			*--dp = *--sp;
	}

	return dest;
}