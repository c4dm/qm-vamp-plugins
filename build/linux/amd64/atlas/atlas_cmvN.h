#ifndef ATLAS_MVN_H
#define ATLAS_MVN_H

#include "atlas_misc.h"

#define ATL_mvNMU 32
#define ATL_mvNNU 1
#ifndef ATL_L1mvelts
   #define ATL_L1mvelts ((3*ATL_L1elts)>>2)
#endif
#define ATL_AXPYMV

#define ATL_GetPartMVN(A_, lda_, mb_, nb_) \
{ \
   *(mb_) = (ATL_L1mvelts - (ATL_mvNNU<<1)) / ((ATL_mvNNU<<1)+1); \
   if (*(mb_) > ATL_mvNMU) *(mb_) = ATL_mvNMU*( *(mb_)/ATL_mvNMU ); \
   else *(mb_) = ATL_mvNMU; \
   *(nb_) = ATL_mvNNU; \
}

#endif
