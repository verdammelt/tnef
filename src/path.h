/* -*- mode: c++ -*- */
/* 
 * path.h
 * 
 * Copyright (C) 2000 by Person or Persons Unknown <damned@world.std.com>
 * 
 * Commentary: 
 */
#ifndef PATH_H
#define PATH_H

extern char *
concat_fname (const char* fname1, const char* fname2);
#if !HAVE_BASENAME
extern char *
basename (const char* path);
#endif /* !HAVE_BASENAME */

#endif /* !PATH_H */
