#ifndef UMCF_BLUECFD_COMPAT_PATCHTOOLS_H
#define UMCF_BLUECFD_COMPAT_PATCHTOOLS_H
#ifdef mingw_w64
#include "PatchTools.T.H"
#else
#include_next "PatchTools.H"
#endif
#endif
