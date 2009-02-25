      SUBROUTINE ana_perturb (ng, tile, model)
!
!! svn $Id: ana_perturb.h 34 2007-04-27 04:40:21Z arango $
!!======================================================================
!! Copyright (c) 2002-2007 The ROMS/TOMS Group                         !
!!   Licensed under a MIT/X style license                              !
!!   See License_ROMS.txt                                              !
!!                                                                     !
!=======================================================================
!                                                                      !
!  This routine perturbs initial conditions for momentum and tracers   !
!  type variables using analytical expressions.                        !
!                                                                      !
!  It is also used to perturb  the tangent linear and adjoint models   !
!  at specified state variable and spatial  (i,j,k)  point to verify   !
!  the correctness of these algorithms.  This is  activated with the   !
!  SANITY_CHECK CPP switch.                                            !
!                                                                      !
!  If each interior point is  perturbed at one time,  the  resulting   !
!  tangent linear (T) and adjoint (A) M-by-N matrices yield:           !
!                                                                      !
!                T - tranpose(A) = 0    within round off               !
!                                                                      !
!  That is, their inner product give a symmetric matrix.  Here, M is   !
!  the number of state  points and N is the number of perturbations.   !
!  In realistic applications,  it is awkward to perturb all interior   !
!  points for each state variable.  Alternatively, random check at a   !
!  specified points is inexpensive.  The standard input "User" array   !
!  is used to specify such point:                                      !
!                                                                      !
!     INT(user(1)) => state variable to perturb                        !
!     INT(user(2)) => I-index to perturb                               !
!     INT(user(3)) => J-index to perturb                               !
!     INT(user(4)) => K-index to perturb (3D state fields)             !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_ncparam
      USE mod_ocean
      USE mod_stepping
!
! Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model

#include "tile.h"
!
      CALL ana_perturb_tile (ng, model, Istr, Iend, Jstr, Jend,         &
     &                       LBi, UBi, LBj, UBj,                        &
     &                       kstp(ng), krhs(ng), knew(ng),              &
#ifdef SOLVE3D
     &                       nstp(ng), nrhs(ng), nnew(ng),              &
     &                       OCEAN(ng) % ad_u,                          &
     &                       OCEAN(ng) % ad_v,                          &
     &                       OCEAN(ng) % ad_t,                          &
#endif
     &                       OCEAN(ng) % ad_ubar,                       &
     &                       OCEAN(ng) % ad_vbar,                       &
     &                       OCEAN(ng) % ad_zeta,                       &
#ifdef SOLVE3D
     &                       OCEAN(ng) % tl_u,                          &
     &                       OCEAN(ng) % tl_v,                          &
     &                       OCEAN(ng) % tl_t,                          &
#endif
     &                       OCEAN(ng) % tl_ubar,                       &
     &                       OCEAN(ng) % tl_vbar,                       &
     &                       OCEAN(ng) % tl_zeta)
!
! Set analytical header file name used.
!
      IF (Lanafile) THEN
        WRITE (ANANAME(19),'(a,a)') TRIM(Adir), '/ana_perturb.h'
      END IF

      RETURN
      END SUBROUTINE ana_perturb
!
!***********************************************************************
      SUBROUTINE ana_perturb_tile (ng, model, Istr, Iend, Jstr, Jend,   &
     &                             LBi, UBi, LBj, UBj,                  &
     &                             kstp, krhs, knew,                    &
#ifdef SOLVE3D
     &                             nstp, nrhs, nnew,                    &
     &                             ad_u, ad_v, ad_t,                    &
#endif
     &                             ad_ubar, ad_vbar, ad_zeta,           &
#ifdef SOLVE3D
     &                             tl_u, tl_v, tl_t,                    &
#endif
     &                             tl_ubar, tl_vbar, tl_zeta)
!***********************************************************************
!
      USE mod_param
      USE mod_parallel
      USE mod_iounits
      USE mod_ncparam
      USE mod_scalars
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, model, Iend, Istr, Jend, Jstr
      integer, intent(in) :: LBi, UBi, LBj, UBj
      integer, intent(in) :: kstp, krhs, knew
#ifdef SOLVE3D
      integer, intent(in) :: nstp, nrhs, nnew
#endif
!
#ifdef ASSUMED_SHAPE
# ifdef SOLVE3D
      real(r8), intent(inout) :: ad_u(LBi:,LBj:,:,:)
      real(r8), intent(inout) :: ad_v(LBi:,LBj:,:,:)
      real(r8), intent(inout) :: ad_t(LBi:,LBj:,:,:,:)
# endif
      real(r8), intent(inout) :: ad_ubar(LBi:,LBj:,:)
      real(r8), intent(inout) :: ad_vbar(LBi:,LBj:,:)
      real(r8), intent(inout) :: ad_zeta(LBi:,LBj:,:)
# ifdef SOLVE3D
      real(r8), intent(inout) :: tl_u(LBi:,LBj:,:,:)
      real(r8), intent(inout) :: tl_v(LBi:,LBj:,:,:)
      real(r8), intent(inout) :: tl_t(LBi:,LBj:,:,:,:)
# endif
      real(r8), intent(inout) :: tl_ubar(LBi:,LBj:,:)
      real(r8), intent(inout) :: tl_vbar(LBi:,LBj:,:)
      real(r8), intent(inout) :: tl_zeta(LBi:,LBj:,:)
#else
# ifdef SOLVE3D
      real(r8), intent(inout) :: ad_u(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(inout) :: ad_v(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(inout) :: ad_t(LBi:UBI,LBj:UBj,N(ng),2,NT(ng))
# endif
      real(r8), intent(inout) :: ad_ubar(LBi:UBi,LBj:UBj,3)
      real(r8), intent(inout) :: ad_vbar(LBi:UBi,LBj:UBj,3)
      real(r8), intent(inout) :: ad_zeta(LBi:UBi,LBj:UBj,3)
# ifdef SOLVE3D
      real(r8), intent(inout) :: tl_u(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(inout) :: tl_v(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(inout) :: tl_t(LBi:UBi,LBj:UBj,N(ng),3,NT(ng))
# endif
      real(r8), intent(inout) :: tl_ubar(LBi:UBi,LBj:UBj,3)
      real(r8), intent(inout) :: tl_vbar(LBi:UBi,LBj:UBj,3)
      real(r8), intent(inout) :: tl_zeta(LBi:UBi,LBj:UBj,3)
#endif
!
!  Local variable declarations.
!
      integer :: IstrR, IendR, JstrR, JendR, IstrU, JstrV
      integer :: IperAD, JperAD, KperAD, ivarAD
      integer :: IperTL, JperTL, KperTL, ivarTL
      integer :: i, itrc, j, k
!
#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Set tangent and adjoint variable and random point to perturb.
!-----------------------------------------------------------------------
!
      ivarTL=INT(user(1))
      ivarAD=INT(user(2))
      IperTL=INT(user(3))
      IperAD=INT(user(4))
      JperTL=INT(user(5))
      JperAD=INT(user(6))
#ifdef SOLVE3D
      KperTL=INT(user(7))
      KperAD=INT(user(8))
#endif
      IF (Master) THEN
        IF (TLmodel) THEN
          IF (ivarTL.eq.isUbar) THEN
            WRITE (stdout,10) 'tl_ubar perturbed at (i,j) = ',          &
     &                        IperTL, JperTL
          ELSE IF (ivarTL.eq.isVbar) THEN
            WRITE (stdout,10) 'tl_vbar perturbed at (i,j) = ',          &
     &                        IperTL, JperTL
          ELSE IF (ivarTL.eq.isFsur) THEN
            WRITE (stdout,10) 'tl_zeta perturbed at (i,j) = ',          &
     &                        IperTL, JperTL
#ifdef SOLVE3D
          ELSE IF (ivarTL.eq.isUvel) THEN
            WRITE (stdout,20) 'tl_u perturbed at (i,j,k) = ',           &
     &                        IperTL, JperTL, KperTL
          ELSE IF (ivarTL.eq.isVvel) THEN
            WRITE (stdout,20) 'tl_v perturbed at (i,j,k) = ',           &
     &                        IperTL, JperTL, KperTL
#endif
          END IF
#ifdef SOLVE3D
          DO itrc=1,NT(ng)
            IF (ivarTL.eq.isTvar(itrc)) THEN
              WRITE (stdout,30) 'ad_t perturbed at (i,j,k,itrc) = ',   &
     &                          IperTL, JperTL, KperTL, itrc
            END IF
          END DO
#endif
        END IF
        IF (ADmodel) THEN
          IF (ivarAD.eq.isUbar) THEN
            WRITE (stdout,40) 'ad_ubar perturbed at (i,j) = ',          &
     &                        IperAD, JperAD
          ELSE IF (ivarAD.eq.isVbar) THEN
            WRITE (stdout,40) 'ad_vbar perturbed at (i,j) = ',          &
     &                        IperAD, JperAD
          ELSE IF (ivarAD.eq.isFsur) THEN
            WRITE (stdout,40) 'ad_zeta perturbed at (i,j) = ',          &
     &                        IperAD, JperAD
#ifdef SOLVE3D
          ELSE IF (ivarAD.eq.isUvel) THEN
            WRITE (stdout,50) 'ad_u perturbed at (i,j,k) = ',           &
     &                        IperAD, JperAD, KperAD
          ELSE IF (ivarAD.eq.isVvel) THEN
            WRITE (stdout,50) 'ad_v perturbed at (i,j,k) = ',           &
     &                        IperAD, JperAD, KperAD
#endif
          END IF
#ifdef SOLVE3D
          DO itrc=1,NT(ng)
            IF (ivarAD.eq.isTvar(itrc)) THEN
              WRITE (stdout,60) 'ad_t perturbed at (i,j,k,itrc) = ',    &
     &                          IperAD, JperAD, KperAD, itrc
            END IF
          END DO
#endif
        END IF
      END IF
!
!-----------------------------------------------------------------------
!  Peturb initial conditions for 2D momentum (m/s) components.
!-----------------------------------------------------------------------
!
      IF (TLmodel) THEN
        DO j=JstrR,JendR
          DO i=Istr,IendR
            IF ((ivarTL.eq.isUbar).and.                                 &
     &          (i.eq.IperTL).and.(j.eq.JperTL)) THEN
              tl_ubar(i,j,kstp)=1.0_r8
            ELSE
              tl_ubar(i,j,kstp)=0.0_r8
            END IF
          END DO
        END DO
        DO j=Jstr,JendR
          DO i=IstrR,IendR
            IF ((ivarTL.eq.isVbar).and.                                 &
     &          (i.eq.IperTL).and.(j.eq.JperTL)) THEN
              tl_vbar(i,j,kstp)=1.0_r8
            ELSE
              tl_vbar(i,j,kstp)=0.0_r8
            END IF
          END DO
        END DO
      END IF
!
      IF (ADmodel) THEN
        DO j=JstrR,JendR
          DO i=Istr,IendR
            IF ((ivarAD.eq.isUbar).and.                                 &
     &          (i.eq.IperAD).and.(j.eq.JperAD)) THEN
              ad_ubar(i,j,knew)=1.0_r8
            ELSE
              ad_ubar(i,j,knew)=0.0_r8
            END IF
          END DO
        END DO
        DO j=Jstr,JendR
          DO i=IstrR,IendR
            IF ((ivarAD.eq.isVbar).and.                                 &
     &          (i.eq.IperAD).and.(j.eq.JperAD)) THEN
              ad_vbar(i,j,knew)=1.0_r8
            ELSE
              ad_vbar(i,j,knew)=0.0_r8
            END IF
          END DO
        END DO
      END IF
!
!-----------------------------------------------------------------------
!  Perturb initial conditions for free-surface (m).
!-----------------------------------------------------------------------
!
      IF (TLmodel) THEN
        DO j=JstrR,JendR
          DO i=IstrR,IendR
            IF ((ivarTL.eq.isFsur).and.                                 &
     &          (i.eq.IperTL).and.(j.eq.JperTL)) THEN
              tl_zeta(i,j,kstp)=1.0_r8
            ELSE
              tl_zeta(i,j,kstp)=0.0_r8
            END IF
          END DO
        END DO
      END IF
!
      IF (ADmodel) THEN
        DO j=JstrR,JendR
          DO i=IstrR,IendR
            IF ((ivarAD.eq.isFsur).and.                                 &
     &          (i.eq.IperAD).and.(j.eq.JperAD)) THEN
              ad_zeta(i,j,knew)=1.0_r8
            ELSE
              ad_zeta(i,j,knew)=0.0_r8
            END IF
          END DO
        END DO
      END IF

#ifdef SOLVE3D
!
!-----------------------------------------------------------------------
!  Initial conditions for 3D momentum components (m/s).
!-----------------------------------------------------------------------
!
      IF (TLmodel) THEN
        DO k=1,N(ng)
          DO j=JstrR,JendR
            DO i=Istr,IendR
              IF ((ivarTL.eq.isUvel).and.                               &
     &            (i.eq.IperTL).and.(j.eq.JperTL).and.                  &
     &            (k.eq.KperTL)) THEN
                tl_u(i,j,k,nstp)=1.0_r8
              ELSE
                tl_u(i,j,k,nstp)=0.0_r8
              END IF
            END DO
          END DO
          DO j=Jstr,JendR
            DO i=IstrR,IendR
              IF ((ivarTL.eq.isVvel).and.                               &
     &            (i.eq.IperTL).and.(j.eq.JperTL).and.                  &
     &            (k.eq.KperTL)) THEN
                tl_v(i,j,k,nstp)=1.0_r8
              ELSE
                tl_v(i,j,k,nstp)=0.0_r8
              END IF
            END DO
          END DO
        END DO
      END IF
!
      IF (ADmodel) THEN
        DO k=1,N(ng)
          DO j=JstrR,JendR
            DO i=Istr,IendR
              IF ((ivarAD.eq.isUvel).and.                               &
     &            (i.eq.IperAD).and.(j.eq.JperAD).and.                  &
     &            (k.eq.KperAD)) THEN
                ad_u(i,j,k,nstp)=1.0_r8
              ELSE
                ad_u(i,j,k,nstp)=0.0_r8
              END IF
            END DO
          END DO
          DO j=Jstr,JendR
            DO i=IstrR,IendR
              IF ((ivarAD.eq.isVvel).and.                               &
     &            (i.eq.IperAD).and.(j.eq.JperAD).and.                  &
     &            (k.eq.KperAD)) THEN
                ad_v(i,j,k,nstp)=1.0_r8
              ELSE
                ad_v(i,j,k,nstp)=0.0_r8
              END IF
            END DO
          END DO
        END DO
      END IF
!
!-----------------------------------------------------------------------
!  Perturb initial conditions for tracer type variables.
!-----------------------------------------------------------------------
!
      IF (TLmodel) THEN
        DO itrc=1,NT(ng)
          DO k=1,N(ng)
            DO j=JstrR,JendR
              DO i=IstrR,IendR
                IF ((ivarTL.eq.isTvar(itrc)).and.                       &
     &              (i.eq.IperTL).and.(j.eq.JperTL).and.                &
     &              (k.eq.KperTL)) THEN
                  tl_t(i,j,k,nstp,itrc)=1.0_r8
                ELSE
                  tl_t(i,j,k,nstp,itrc)=0.0_r8
                END IF
              END DO
            END DO
          END DO
        END DO
      END IF
!
      IF (ADmodel) THEN
        DO itrc=1,NT(ng)
          DO k=1,N(ng)
            DO j=JstrR,JendR
              DO i=IstrR,IendR
                IF ((ivarAD.eq.isTvar(itrc)).and.                       &
     &              (i.eq.IperAD).and.(j.eq.JperAD).and.                &
     &              (k.eq.KperAD)) THEN
                  ad_t(i,j,k,nstp,itrc)=1.0_r8
                ELSE
                  ad_t(i,j,k,nstp,itrc)=0.0_r8
                END IF
              END DO
            END DO
          END DO
        END DO
      END IF
#endif
!
 10   FORMAT (/,' ANA_PERTURB - Tangent ', a, 2i4,/)
#ifdef SOLVE3D
 20   FORMAT (/,' ANA_PERTURB - Tangent ', a, 3i4,/)
 30   FORMAT (/,' ANA_PERTURB - Tangent ', a, 4i4,/)
#endif
 40   FORMAT (/,' ANA_PERTURB - Adjoint ', a, 2i4,/)
#ifdef SOLVE3D
 50   FORMAT (/,' ANA_PERTURB - Adjoint ', a, 3i4,/)
 60   FORMAT (/,' ANA_PERTURB - Adjoint ', a, 4i4,/)
#endif

      RETURN
      END SUBROUTINE ana_perturb_tile