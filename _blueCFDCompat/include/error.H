#ifndef UMCF_BLUECFD_COMPAT_ERROR_H
#define UMCF_BLUECFD_COMPAT_ERROR_H
#ifdef mingw_w64
#include "error.T.H"
#else
#include_next "error.H"
#endif
#endif
