#ifndef UMCF_BLUECFD_COMPAT_TABLE_H
#define UMCF_BLUECFD_COMPAT_TABLE_H
#ifdef mingw_w64
#include "Table.T.H"
#else
#include_next "Table.H"
#endif
#endif
