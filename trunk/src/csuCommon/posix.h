#pragma warning(disable : 4996; disable : 4244; disable : 4028; disable : 4305; disable : 4018) 

#include <string.h>
#define basename(A) strrchr(A, '\\')

#define popen _popen

#define _CRT_SECURE_NO_WARNINGS
#define	F_OK	0
#define	R_OK	4
#define	W_OK	2
#define	X_OK	1

/* check for missing definition for S_ISDIR */
#ifndef S_ISDIR
#  ifdef _S_ISDIR
#    define S_ISDIR(x) _S_ISDIR(x)
#  else
#    ifdef S_IFDIR
#      ifndef S_IFMT
#        ifdef _S_IFMT
#          define S_IFMT _S_IFMT
#        endif
#      endif
#       ifdef S_IFMT
#         define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#       endif
#    endif
#  endif
#endif

/* check for missing definition for S_ISREG */
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# define S_ISREG(m) (((m) & _S_IFMT) == _S_IFREG)
#endif 

