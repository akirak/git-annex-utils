/* Copyright 2012 Steve Seltzer, see COPYING for license */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_REGEX_H
# include <regex.h>
#endif /* HAVE_REGEX_H */
#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif /* HAVE_DIRENT_H */
#include <stdio.h>
#include <stdlib.h>

/* local headers */
#include <opts.h>
#include <common/returncodes.h>
#include <dothedu.h>

/* Walk through the directories and tally up all annexed file sizes
 *
 * To accomplish this, we walk through the directories looking for symlinks
 * that point to the .git/annex/... directory and then tally up the size
 * from them.
 *
 * path - the path inside the git tree
 * top - (output) the top of the git tree
 * n - the size of the output var
 * returns:
 *  0 - success
 *  1 - couldn't stat something
 *  2 - couldn't open a dir in the path
 *  3 - path size exceeds PATH_MAX
 */
int dothedu(const char *path, size_t *size, unsigned int depth){
  return dothepath(path,size,1,depth);
}

/* handle a path
 *
 * path - the path to handle
 * size - place to report back the file/dir size
 * cmdline - this path came from the command line
 * depth - the depth of the .git dir from the path
 *
 * returns:
 * 0 - success
 * 1 - couldn't stat something
 */
int dothepath(const char *path, size_t *size, int cmdline, unsigned int depth){
  struct stat st;
  regex_t regex;
  regmatch_t regmatch;
  char *p;

  if(lstat(path,&st)){
    fprintf(stderr,"%s: warning - couldn't stat %s\n",opt_progname,path);
    return 1; /* couldn't stat a path */
  }

  if(S_ISDIR(st.st_mode))
    return dothedir(path,size,cmdline|(!opt_summarize),depth+(cmdline?0:1));

  *size=0; /* we're handling it, start with a zero size */

  do{
    if(S_ISLNK(st.st_mode)){
      char linkbuf[PATH_MAX+1];
      ssize_t rslt;
      unsigned int i;

      /* read the link */
      rslt=readlink(path,linkbuf,PATH_MAX);
      if(rslt>=0)
	linkbuf[rslt]=0;
      else
	linkbuf[0]=0; /* zero len string on err */

      /* look for the annexed link structure
       * must be "../" * depth followed by ".git/annex/objects/" */
      for(i=0;i<depth;i++){
	if(strncmp("../",linkbuf+(3*i),3))
	  break; /* not an annexed link */
      }
      if(strncmp(".git/annex/objects/",linkbuf+(3*depth),sizeof(".git/annex/objects/")-1))
	break; /* not an annexed link */

      /* find the last '/' character */
      p=linkbuf+strlen(linkbuf);
      while( (p>linkbuf) && (*p)!='/' ) p--;

      if(regcomp(&regex,"[-]s([0-9]+)[-]",REG_EXTENDED)){
	fprintf(stderr,"%s: An error occured compiling the regex, this shouldn't be possible!\n",opt_progname);
	exit(RTRN_ERR_INTERNAL);
      }

      rslt=regexec(&regex,p,1,&regmatch,0);

      regfree(&regex);

      if(rslt || regmatch.rm_so<0)
	break; /* regex didn't match, not an annexed link?  Not an error? */

      *size=strtoll(p+regmatch.rm_so+2,p+regmatch.rm_eo-1,10);
    }
  }while(0);

  if(cmdline) printpath(*size,path); /* output if this was a cmdline arg */

  return 0;
}

int dothedir(const char *path,size_t *size, int output, unsigned int depth){
  DIR *d;
  struct dirent *de;
  size_t cursize;
  char tmppath[PATH_MAX+1];
  size_t pathsize;

  *size=0;

  /* try to open the dir */
  d=opendir(path);
  if(!d)
    return 2;

  /* calculate the pathsize and return an error if > PATH_MAX */
  pathsize=strlen(path);
  if(path[pathsize-1]!='/') pathsize+=1;
  if(pathsize>PATH_MAX)
    return 3;

  /* copy the path to tmppath and add a '/' if needed */
  strcpy(tmppath,path);
  if(path[pathsize-1]!='/') strcat(tmppath,"/");

  /* for each directory entry... */
  while(de=readdir(d))
    /* if the entry is not "." or ".."... */
    if( strcmp(de->d_name,".") && strcmp(de->d_name,"..") ){
      size_t tmpsize;
      /* if the resultant path will be larger than PATH_MAX return an error */
      if(strlen(de->d_name)+pathsize > PATH_MAX)
	return 3;
      strcat(tmppath,de->d_name); /* add the entry to the path */
      if(!dothepath(tmppath,&tmpsize,0,depth)) /* handle the new path */
	cursize+=tmpsize; /* add to tally if successful */
      tmppath[pathsize]=0; /* remove the entry from the path */
    }

  closedir(d); /* close the directory */

  if(output)
    printpath(cursize,path); /* output the result */
  *size=cursize; /* return the result */

  return 0;
}

void printpath(size_t size, const char *path){
  if(opt_bytes) /* if we output bytes rather than 512 byte blocks */
    printf("%zu\t%s\n",size,path);
  else
    printf("%zu\t%s\n",size/512+(size%512?1:0),path);
}
