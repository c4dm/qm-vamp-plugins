#ifndef ATLAS_CR1_H
#define ATLAS_CR1_H

#define ATL_L1r1elts 3809
#define ATL_r1MU 16
#define ATL_r1NU 1

#define ATL_GetPartR1(A_, lda_, mb_, nb_) \
{ \
   (mb_) = (ATL_L1r1elts - (ATL_r1NU+ATL_r1NU)) / (ATL_r1NU+ATL_r1NU+1); \
   if ((mb_) > ATL_r1MU) (mb_) = ATL_r1MU*((mb_)/ATL_r1MU); \
   else (mb_) = ATL_r1MU; \
   (nb_) = ATL_r1NU; \
}

#endif
