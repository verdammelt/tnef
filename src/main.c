/*
 * main.c -- extract files from microsoft TNEF format
 *
 * Copyright (C)1999, 2000, 2001, 2002 Mark Simpson <damned@world.std.com>
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
 * Commentary:
 * 	main function and command line parsing.
 */
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#if HAVE_GETOPT_LONG
#  include <getopt.h>
#else
#  include "getopt_long.h"
#endif /* HAVE_GETOPT_LONG */

#include "tnef.h"

/* COPYRIGHTS & NO_WARRANTY -- defined to make code below a little nicer to
   read */
static const char* COPYRIGHTS = \
"Copyright (C) 1999 by Mark Simpson\n"
"Copyright (C) 1997 by Thomas Boll (original code)";
static const char* NO_WARRANTY = \
"%s comes with ABSOLUTELY NO WARRANTY.\n"
"You may redistribute copies of %s under the terms of the GNU General\n"
"Public License.  For more information about these matters, see the file\n"
"named COPYING.";
static const char* USAGE = \
"-f FILE, --file=FILE\tuse FILE as input ('-' == stdin)\n"
"-C DIR, --directory=DIR\tunpack files into DIR\n"
"-t,     --list      \tlist files, do not extract\n"
"-w,     --interactive\task for confirmation for every action\n"
"        --confirmation\tsame as -w\n"
"        --overwrite \tOverwrite existing files\n"
"        --number-backups\tIf need to overwrite file FOO, make FOO.1 instead\n"
"        --use-paths \tUse pathnames for files if found in the TNEF file\n"
"                    \t(for security reasons paths to included files are\n"
"                    \t ignored by default)\n"
"-h,     --help      \tshow this message\n"
"-V,     --version   \tdisplay version and copyright\n"
"-v,     --verbose   \tproduce verbose output\n"
"        --debug     \tproduce a lot of output\n"
"\n"
"-d                  \t[same as -C; deprecated]\n"
"-l                  \t[same as -t; deprecated]\n"
"-n, --dry_run       \t[same as -t; deprecated]\n"
"\nIf FILE is not specified standard input is used\n"
"\nReport bugs to <%s>\n";


static void
usage (char* prog)
{
    fprintf (stdout, "%s: [options] [FILE]\n", prog);
    fprintf (stdout, USAGE, PACKAGE_BUGREPORT);
}


static void
parse_cmdline (int argc, char **argv,
               char **in_file,
               char **out_dir,
               int *flags)
{
    int i = 0;
    int option_index = 0;
    static struct option long_options[] = 
    { 
        {"file", required_argument, 0, 'f' },
        {"directory", required_argument, 0, 'C' },
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"dry_run", no_argument, 0, 'l'},
        {"list", no_argument, 0, 'l'}, /* for now same as -n */
        {"confirmation", no_argument, 0, 'w' },
        {"interactive", no_argument, 0, 'w' },
        {"version", no_argument, 0, 'V'},
        {"use-paths", no_argument, 0, 0},
        {"debug", no_argument, 0, 0},
        {"overwrite", no_argument, 0, 0 },
        {"number-backups", no_argument, 0, 0 },
        { 0, 0, 0, 0 }
    };

    while ((i = getopt_long (argc, argv, "f:C:d:vVwhtnl", 
                             long_options, &option_index)) != -1)
    {
        switch (i) 
        {
        case 0:               /* long options with no val field */
            if (strcmp (long_options[option_index].name,
                        "debug") == 0)
            {
                *flags |= DBG_OUT;
            }
            else if (strcmp (long_options[option_index].name,
                             "use-paths") == 0)
            {
                *flags |= PATHS;
            }
            else if (strcmp (long_options[option_index].name,
                             "overwrite") == 0)
            {
                *flags |= OVERWRITE;
            }
            else if (strcmp (long_options[option_index].name,
                             "number-backups") == 0)
            {
                *flags |= NUMBERED;
            }
            else
            {
                abort ();       /* impossible! */
            }
            break;

        case 'V':
            fprintf (stderr, "%s\n", PACKAGE_STRING);
            fprintf (stderr, COPYRIGHTS);
            fprintf (stderr, "\n");
            fprintf (stderr, NO_WARRANTY, PACKAGE, PACKAGE);
            fprintf (stderr, "\n");
            exit (0);
            break;

        case 'v': 
            *flags |= VERBOSE;
            break;

        case 'f':
            if (strcmp (optarg, "-") == 0) (*in_file) = NULL;
            else (*in_file) = optarg;
            break;
            
        case 'C':
        case 'd':
            (*out_dir) = optarg;
            break;

        case 'h':
            usage(argv[0]);
            exit (0);
            break;

                                /* for now -t -l -n --list and --dry-run
                                   are the same.  -l,-n,--dry-run are all
                                   deprecated.  */
        case 't':
        case 'l':
        case 'n':
            *flags |= LIST;
            break;

        case 'w':
            *flags |= CONFIRM;
            break;

        case '?':
            fprintf (stderr, "Try '%s --help' for more info.\n", argv[0]);
            exit (1);
            break;

        default:
            abort();            /* this is a problem */
        }
    }
    if (optind < argc)
    {
        (*in_file) = argv[optind++];
        if (optind < argc)
        {
            fprintf (stderr, "%s: Extra parameters -- '", argv[0]);
            while (optind < argc)
                fprintf (stderr, "%s ", argv[optind++]);
            fprintf (stderr, "'\n");
        }
    }
}


int
main (int argc, char *argv[]) 
{
    FILE *fp = NULL;
    char *in_file = NULL;
    char *out_dir = NULL;
    int flags = NONE;
    
    parse_cmdline (argc, argv, &in_file, &out_dir, &flags);

    /* open the file */
    if (in_file)
    {
        fp = fopen (in_file, "rb");
        if (fp == NULL) 
        {
            perror (in_file);
            exit (1);
        }
    }
    else
    {
        fp = stdin;
    }

    if (fp == stdin && flags & CONFIRM)
    {
        fprintf (stderr, 
                 "Cannot read file from STDIN and use "
                 "interactive mode at the same time.\n");
        exit (1);
    }
    
    return parse_file (fp, out_dir, flags);
}
