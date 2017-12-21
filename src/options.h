/*
 * options.h -- functions for dealing with program options
 *
 * Copyright (C)1999-2018 Mark Simpson <damned@theworld.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can either send email to this
 * program's maintainer or write to: The Free Software Foundation,
 * Inc.; 59 Temple Place, Suite 330; Boston, MA 02111-1307, USA.
 *
 */
#ifndef OPTIONS_H
#define OPTIONS_H

#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "common.h"

/* Global variables, used by all (or nearly all) functions */
extern int g_flags;     /* program options */

/* macros for dealing with program flags */
#define DEBUG_ON ((g_flags)&DBG_OUT)
#define VERBOSE_ON ((g_flags)&VERBOSE)
#define LIST_ONLY ((g_flags)&LIST)
#define SHOW_MIME ((g_flags)&LISTMIME)
#define USE_PATHS ((g_flags)&PATHS)
#define INTERACTIVE ((g_flags)&CONFIRM)
#define OVERWRITE_FILES ((g_flags)&OVERWRITE)
#define NUMBER_FILES ((g_flags)&NUMBERED)
#define CHECKSUM_SKIP ((g_flags)&CHECKSUM_OK)
#define ENCODE_SKIP ((g_flags)&ENCODE_OK)
#define CRUFT_SKIP ((g_flags)&CRUFT_OK)
#define UNIX_FS ((g_flags)&UNIX_PATHS)
#define ABSOLUTE_OK ((g_flags)&ABSOLUTE_PATHS)

/* flags to modify behaviour of file parsing */
enum { NONE    	= 0x00,
       VERBOSE 	= 0x01,
       DBG_OUT 	= 0x02,
       LIST    	= 0x04,
       PATHS   	= 0x08,
       OVERWRITE= 0x10,
       CONFIRM 	= 0x20,
       NUMBERED = 0x40,
       SAVEBODY = 0x80,
       LISTMIME = 0x100,
       CHECKSUM_OK = 0x200,
       ENCODE_OK = 0x400,
       CRUFT_OK = 0x800,
       UNIX_PATHS = 0x1000,
       ABSOLUTE_PATHS = 0x2000
};

#endif /* OPTIONS_H */
