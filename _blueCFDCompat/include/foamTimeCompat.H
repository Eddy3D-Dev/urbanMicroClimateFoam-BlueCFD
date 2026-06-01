#ifndef UMCF_BLUECFD_COMPAT_FOAM_TIME_COMPAT_H
#define UMCF_BLUECFD_COMPAT_FOAM_TIME_COMPAT_H

/*
    blueCFD/OpenFOAM on Windows renames Time.H to Time.T.H.
    Do not create a wrapper named Time.H because Windows treats Time.H
    and the system header time.h as the same name.
*/

#ifdef mingw_w64
#include "Time.T.H"
#else
#include "Time.H"
#endif

#endif
