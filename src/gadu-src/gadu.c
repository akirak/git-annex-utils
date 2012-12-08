/* Copyright 2012 Steve Seltzer, see COPYING for license */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */

/* local headers */
#include <opts.h>
#include <common/returncodes.h>
#include <common/normalizepath.h>
#include <common/findgitdir.h>

int main(int argc, char *argv[]){
  int idx_paths; /* index to the first path in argv[] */
  int retval=RTRN_OK;

  idx_paths=procopts(argc, argv);

  while(idx_paths<argc){
    size_t size;
    char path[PATH_MAX+1]={};
    int depth;

    /* first normalize the path */
    switch(normalizepath(argv[idx_paths],path,PATH_MAX+1)){
    case 1:
      fprintf(stderr,"%s: path too long error while normalizing '%s'\n",opt_progname,argv[idx_paths]);
      retval|=RTRN_ERR_PATHTOOLONG;
      break;
    case 2:
      fprintf(stderr,"%s: unable to obtain current directory while normalizing '%s'\n",opt_progname,argv[idx_paths]);
      retval|=RTRN_ERR_BADCWD;
      break;
    case 3:
      fprintf(stderr,"%s: couldn't stat '%s' while normalizing '%s'\n",opt_progname,path,argv[idx_paths]);
      retval|=RTRN_ERR_FILE;
      break;
    case 0:
      depth=findgitdir(path);
      dothedu(argv[idx_paths],&size);
      break;
    default:
      fprintf(stderr,"%s: unknown error while normalizing '%s'\n",opt_progname,argv[idx_paths]);
      retval|=RTRN_ERR_INTERNAL;
    }
    idx_paths++;
  }

  return retval;
}
