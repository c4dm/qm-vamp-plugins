#ifndef ATLAS_MVS_H
#define ATLAS_MVS_H

#include "atlas_misc.h"

#define ATL_mvSMU 2
#define ATL_mvSNU 32
#ifndef ATL_L1mvelts
   #define ATL_L1mvelts ((3*ATL_L1elts)>>2)
#endif
#ifndef ATL_mvNNU
   #include "atlas_zmvN.h"
#endif
#ifndef ATL_mvTNU
   #include "atlas_zmvT.h"
#endif
#define ATL_GetPartSYMV(A_, lda_, mb_, nb_) \
{ \
   *(nb_) = ATL_mvSMU; \
   *(mb_) = 160; \
}

#endif
