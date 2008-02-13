#ifndef ATLAS_MVT_H
#define ATLAS_MVT_H

#include "atlas_misc.h"

#define ATL_mvTMU 2
#define ATL_mvTNU 8
#ifndef ATL_L1mvelts
   #define ATL_L1mvelts ((3*ATL_L1elts)>>2)
#endif
#ifndef ATL_mvNNU
   #include "atlas_zmvN.h"
#endif

#define ATL_GetPartMVT(A_, lda_, mb_, nb_) \
{ \
   *(mb_) = (ATL_L1mvelts - (ATL_mvTMU<<1)) / ((ATL_mvTMU<<1)+1); \
   if (*(mb_) > ATL_mvTNU) *(mb_) = (*(mb_)/ATL_mvTNU)*ATL_mvTNU; \
   else (*mb_) = ATL_mvTNU; \
   *(nb_) = ATL_mvTMU; \
}

#endif
