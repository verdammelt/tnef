/* -*- mode: c++ -*- */
/* 
 * alloc.h
 * 
 * Copyright (C) 2000 by Person or Persons Unknown <damned@world.std.com>
 * 
 * Commentary: 
 */
#ifndef ALLOC_H
#define ALLOC_H

#if STDC_HEADERS
#  include <stdlib.h>
#else
extern void free (void*);
#endif /* STDC_HEADERS */

extern void*
MALLOC (size_t size);
#define FREE(_x) free(_x);_x=NULL

#endif /* ALLOC_H */
