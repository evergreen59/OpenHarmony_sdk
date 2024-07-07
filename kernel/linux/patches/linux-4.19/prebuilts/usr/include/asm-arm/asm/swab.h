/*
 * This header was generated from the Linux kernel headers by update_headers.py,
 * to provide necessary information from kernel to userspace, such as constants,
 * structures, and macros, and thus, contains no copyrightable information.
 */
#ifndef __ASM_ARM_SWAB_H
#define __ASM_ARM_SWAB_H
#include <linux/types.h>
#if !defined(__STRICT_ANSI__) || defined(__KERNEL__)
#  define __SWAB_64_THRU_32__
#endif
static __inline__  __u32 __arch_swab32(__u32 x)
{
	__u32 t;
#ifndef __thumb__
	if (!__builtin_constant_p(x)) {

		__asm__ ("eor\t%0, %1, %1, ror #16" : "=r" (t) : "r" (x));
	} else
#endif
		t = x ^ ((x << 16) | (x >> 16));
	x = (x << 24) | (x >> 8);
	t &= ~0x00FF0000;
	x ^= (t >> 8);
	return x;
}
#define __arch_swab32 __arch_swab32
#endif
