#include "cppdefs.h"

      MODULE ad_t3dmix_mod

#ifdef ADJOINT
# if !defined TS_FIXED && defined SOLVE3D &&  \
     (defined TS_DIF2  || defined TS_DIF4)

!
!svn $Id: ad_t3dmix.F 889 2018-02-10 03:32:52Z arango $
!================================================== Hernan G. Arango ===
!  Copyright (c) 2002-2019 The ROMS/TOMS Group                         !
!    Licensed under a MIT/X style license                              !
!    See License_ROMS.txt                                              !
!=======================================================================
!                                                                      !
!  This routine computes adjoint horizontal mixing of tracers.         !
!                                                                      !
!=======================================================================
!
      implicit none

      PRIVATE
#  ifdef TS_DIF2
      PUBLIC ad_t3dmix2
#  endif
#  ifdef TS_DIF4
      PUBLIC ad_t3dmix4
#  endif

      CONTAINS

#  ifdef TS_DIF4
#   if defined MIX_S_TS
#    include "ad_t3dmix4_s.h"
#   elif defined MIX_GEO_TS
#    include "ad_t3dmix4_geo.h"
#   elif defined MIX_ISO_TS
#    include "ad_t3dmix4_iso.h"
#   endif
#  endif

#  ifdef TS_DIF2
#   if defined MIX_S_TS
#    include "ad_t3dmix2_s.h"
#   elif defined MIX_GEO_TS
#    include "ad_t3dmix2_geo.h"
#   elif defined MIX_ISO_TS
#    include "ad_t3dmix2_iso.h"
#   endif
#  endif

# endif
#endif

      END MODULE ad_t3dmix_mod
