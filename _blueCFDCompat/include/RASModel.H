#ifndef UMCF_BLUECFD_COMPAT_RASMODEL_H
#define UMCF_BLUECFD_COMPAT_RASMODEL_H
#ifdef mingw_w64
#include "RASModel.T.H"
#else
#include_next "RASModel.H"
#endif
#endif
