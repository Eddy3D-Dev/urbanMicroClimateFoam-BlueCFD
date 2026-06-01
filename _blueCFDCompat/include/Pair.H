#ifndef UMCF_BLUECFD_COMPAT_PAIR_H
#define UMCF_BLUECFD_COMPAT_PAIR_H
#ifdef mingw_w64
#include "Pair.T.H"
#else
#include_next "Pair.H"
#endif
#endif
