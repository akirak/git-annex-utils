/* Copyright 2012 Steve Seltzer

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3 of the License
 
   Display the usage */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>

/* local headers */
#include <opts.h>

void usage(){
  printf("usage: %s [options] [<path>...]\n",opt_progname);
  printf(
"  -0, --null            end lines with a null character rather than a newline character\n"
"  -B, --block-size      set the block size\n"
"  -L, --local-only      only count files that are available locally\n"
"  -R, --remote-only     only count files that are *not* available locally\n"
"  -a, --all             print counts for all files, not just directories\n"
"  -b, --bytes           output size in bytes rather than 1024 byte blocks\n"
"  -c, --total           print the total of all the command line arguments\n"
"  -h, --human-readable  print sizes in human readable format (e.g, 1K 234M 2G)\n"
"  -k                    like --block-size=1K\n"
"  -m                    like --block-size=1M\n"
"  -s, --summarize       display only a total for each argument\n"
"  --si                  human-readable format, but use powers of 1000\n"
"  -V, --version         print version\n"
"  --help                display this message\n"
"\n");
}
