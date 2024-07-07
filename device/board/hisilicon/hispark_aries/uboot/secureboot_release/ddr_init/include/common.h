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

#ifndef __COMMON_H_
#define __COMMON_H_
#include "types.h"
/*-----------------------------------------------------------------
 * set cpu mode interface
------------------------------------------------------------------*/ 
void set_mod_normal();
void set_mod_slow();

/*-----------------------------------------------------------------
 * serial interface
------------------------------------------------------------------*/ 
int serial_init ();
int serial_deinit();
void serial_putc (const char c);
void serial_putchar (const char c);
void serial_puts (const char *s);
void serial_flush();
int serial_getc (void);
int serial_tstc (void);

/*-----------------------------------------------------------------
 * mmc interface
------------------------------------------------------------------*/ 
int mmc_init();
void mmc_deinit();
size_t mmc_read(void *ptr, size_t size);


/*-----------------------------------------------------------------
 * timer interface
------------------------------------------------------------------*/ 
int timer_init();
void timer_deinit();
void timer_start();
unsigned long timer_get_val();
unsigned long timer_get_divider();

/*-----------------------------------------------------------------
 * I cache interface
------------------------------------------------------------------*/ 
void open_I_cache();
void close_I_cache();

/*-----------------------------------------------------------------
 * mem operation interface
------------------------------------------------------------------*/ 
void*  memset(void * s,int c,size_t count);
void*  memcpy(void * dest,const void *src,size_t count);

#endif /*__COMMON_H_*/
