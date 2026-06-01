#ifndef UMCF_BLUECFD_COMPAT_TYPEINFO_H
#define UMCF_BLUECFD_COMPAT_TYPEINFO_H
#ifdef mingw_w64
#include "typeInfo.T.H"
#else
#include_next "typeInfo.H"
#endif
#endif
