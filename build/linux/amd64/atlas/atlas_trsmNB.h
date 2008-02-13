#ifndef ATLAS_TRSMNB_H
   #define ATLAS_TRSMNB_H

   #ifdef SREAL
      #define TRSM_NB 60
   #elif defined(DREAL)
      #define TRSM_NB 12
   #else
      #define TRSM_NB 4
   #endif

#endif
