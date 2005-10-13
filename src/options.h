/*
 * options.h -- functions for dealing with program options
 *
 * Copyright (C)1999-2005 Mark Simpson <damned@theworld.com>
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
extern int8 g_flags;     /* program options */
/* extern char *g_directory;*/ /* output directory */
/* extern FILE *g_file; */     /* input file */

/* macros for dealing with program flags */
#define DEBUG_ON ((g_flags)&DBG_OUT)
#define VERBOSE_ON ((g_flags)&VERBOSE)
#define LIST_ONLY ((g_flags)&LIST)
#define USE_PATHS ((g_flags)&PATHS)
#define INTERACTIVE ((g_flags)&CONFIRM)
#define OVERWRITE_FILES ((g_flags)&OVERWRITE)
#define NUMBER_FILES ((g_flags)&NUMBERED)

/* flags to modify behaviour of file parsing */
enum { NONE    	= 0x00,
       VERBOSE 	= 0x01,
       DBG_OUT 	= 0x02,
       LIST    	= 0x04,
       PATHS   	= 0x08,
       OVERWRITE= 0x10,
       CONFIRM 	= 0x20,
       NUMBERED = 0x40,
       SAVEBODY  = 0x80
};

#endif /* OPTIONS_H */
