#ifndef SMM_H
   #define SMM_H

   #define ATL_mmMULADD
   #define ATL_mmLAT 1
   #define ATL_mmMU  12
   #define ATL_mmNU  1
   #define ATL_mmKU  80
   #define MB 80
   #define NB 80
   #define KB 80
   #define NBNB 6400
   #define MBNB 6400
   #define MBKB 6400
   #define NBKB 6400
   #define NB2 160
   #define NBNB2 12800

   #define ATL_MulByNB(N_) ((N_) * 80)
   #define ATL_DivByNB(N_) ((N_) / 80)
   #define ATL_MulByNBNB(N_) ((N_) * 6400)
   #define NBmm ATL_sJIK80x80x80TN80x80x0_a1_b1
   #define NBmm_b1 ATL_sJIK80x80x80TN80x80x0_a1_b1
   #define NBmm_b0 ATL_sJIK80x80x80TN80x80x0_a1_b0
   #define NBmm_bX ATL_sJIK80x80x80TN80x80x0_a1_bX

#endif
