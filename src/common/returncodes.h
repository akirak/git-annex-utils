/* Copyright 2012 Steve Seltzer

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3 of the License */

#ifndef __RETURNCODES_H__
#define __RETURNCODES_H__

#define RTRN_OK 0
#define RTRN_ERR_CMDLINE 1
/* an internal error, should no be possible */
#define RTRN_ERR_INTERNAL 2
#define RTRN_ERR_PATHTOOLONG 4
#define RTRN_ERR_BADCWD 8
#define RTRN_ERR_FILE 16
#define RTRN_ERR_NOTGITREPO 32

#endif /* __RETURNCODES_H__ */
