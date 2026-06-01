#ifndef UMCF_BLUECFD_COMPAT_NAMEDENUM_H
#define UMCF_BLUECFD_COMPAT_NAMEDENUM_H
#ifdef mingw_w64
#include "NamedEnum.T.H"
#else
#include_next "NamedEnum.H"
#endif
#endif
