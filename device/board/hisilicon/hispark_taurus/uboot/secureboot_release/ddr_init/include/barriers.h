/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __BARRIERS_H__
#define __BARRIERS_H__

#ifndef __ASSEMBLY__

#ifndef CONFIG_ARM64
/*
 * CP15 Barrier instructions
 * Please note that we have separate barrier instructions in ARMv7
 * However, we use the CP15 based instructtions because we use
 * -march=armv5 in U-Boot
 */
#define CP15ISB	asm volatile ("mcr     p15, 0, %0, c7, c5, 4" : : "r" (0))
#define CP15DSB	asm volatile ("mcr     p15, 0, %0, c7, c10, 4" : : "r" (0))
#define CP15DMB	asm volatile ("mcr     p15, 0, %0, c7, c10, 5" : : "r" (0))

#endif /* !CONFIG_ARM64 */

#if __LINUX_ARM_ARCH__ >= 7
#define ISB	asm volatile ("isb sy" : : : "memory")
#define DSB	asm volatile ("dsb sy" : : : "memory")
#define DMB	asm volatile ("dmb sy" : : : "memory")
#elif __LINUX_ARM_ARCH__ == 6
#define ISB	CP15ISB
#define DSB	CP15DSB
#define DMB	CP15DMB
#else
#define ISB	asm volatile ("" : : : "memory")
#define DSB	CP15DSB
#define DMB	asm volatile ("" : : : "memory")
#endif

#define isb()	ISB
#define dsb()	DSB
#define dmb()	DMB
#endif	/* __ASSEMBLY__ */
#endif	/* __BARRIERS_H__ */
