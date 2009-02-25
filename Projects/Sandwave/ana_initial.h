      SUBROUTINE ana_initial (ng, tile, model)
!
!! svn $Id: ana_initial.h 735 2007-04-27 14:00:46Z jcwarner $
!!======================================================================
!! Copyright (c) 2002-2007 The ROMS/TOMS Group                         !
!!   Licensed under a MIT/X style license                              !
!!   See License_ROMS.txt                                              !
!!                                                                     !
!=======================================================================
!                                                                      !
!  This subroutine sets initial conditions for momentum and tracer     !
!  type variables using analytical expressions.                        !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_grid
      USE mod_ncparam
      USE mod_ocean
      USE mod_stepping
!
! Imported variable declarations.
!
      integer, intent(in) :: ng, tile, model

#include "tile.h"
!
      CALL ana_initial_tile (ng, model, Istr, Iend, Jstr, Jend,         &
     &                       LBi, UBi, LBj, UBj,                        &
     &                       GRID(ng) % h,                              &
#ifdef SPHERICAL
     &                       GRID(ng) % lonr,                           &
     &                       GRID(ng) % latr,                           &
#else
     &                       GRID(ng) % xr,                             &
     &                       GRID(ng) % yr,                             &
#endif
#ifdef SOLVE3D
     &                       GRID(ng) % z_r,                            &
     &                       OCEAN(ng) % u,                             &
     &                       OCEAN(ng) % v,                             &
     &                       OCEAN(ng) % t,                             &
#endif
     &                       OCEAN(ng) % ubar,                          &
     &                       OCEAN(ng) % vbar,                          &
     &                       OCEAN(ng) % zeta)
!
! Set analytical header file name used.
!
      IF (Lanafile) THEN
        WRITE (ANANAME(10),'(a,a)') TRIM(Adir), '/ana_initial.h'
      END IF

      RETURN
      END SUBROUTINE ana_initial
!
!***********************************************************************
      SUBROUTINE ana_initial_tile (ng, model, Istr, Iend, Jstr, Jend,   &
     &                             LBi, UBi, LBj, UBj,                  &
     &                             h,                                   &
#ifdef SPHERICAL
     &                             lonr, latr,                          &
#else
     &                             xr, yr,                              &
#endif
#ifdef SOLVE3D
     &                             z_r,                                 &
     &                             u, v, t,                             &
#endif
     &                             ubar, vbar, zeta)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, model, Iend, Istr, Jend, Jstr
      integer, intent(in) :: LBi, UBi, LBj, UBj
!
#ifdef ASSUMED_SHAPE
      real(r8), intent(in) :: h(LBi:,LBj:)
# ifdef SPHERICAL
      real(r8), intent(in) :: lonr(LBi:,LBj:)
      real(r8), intent(in) :: latr(LBi:,LBj:)
# else
      real(r8), intent(in) :: xr(LBi:,LBj:)
      real(r8), intent(in) :: yr(LBi:,LBj:)
# endif
# ifdef SOLVE3D
      real(r8), intent(in) :: z_r(LBi:,LBj:,:)

      real(r8), intent(out) :: u(LBi:,LBj:,:,:)
      real(r8), intent(out) :: v(LBi:,LBj:,:,:)
      real(r8), intent(out) :: t(LBi:,LBj:,:,:,:)
# endif
      real(r8), intent(out) :: ubar(LBi:,LBj:,:)
      real(r8), intent(out) :: vbar(LBi:,LBj:,:)
      real(r8), intent(out) :: zeta(LBi:,LBj:,:)
#else
# ifdef SPHERICAL
      real(r8), intent(in) :: lonr(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: latr(LBi:UBi,LBj:UBj)
# else
      real(r8), intent(in) :: xr(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: yr(LBi:UBi,LBj:UBj)
# endif
      real(r8), intent(in) :: h(LBi:UBi,LBj:UBj)
# ifdef SOLVE3D
      real(r8), intent(in) :: z_r(LBi:UBi,LBj:UBj,N(ng))

      real(r8), intent(out) :: u(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(out) :: v(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(out) :: t(LBi:UBi,LBj:UBj,N(ng),3,NT(ng))
# endif
      real(r8), intent(out) :: ubar(LBi:UBi,LBj:UBj,3)
      real(r8), intent(out) :: vbar(LBi:UBi,LBj:UBj,3)
      real(r8), intent(out) :: zeta(LBi:UBi,LBj:UBj,3)
#endif
!
!  Local variable declarations.
!
      integer :: IstrR, IendR, JstrR, JendR, IstrU, JstrV
      integer :: Iless, Iplus, i, itrc, j, k
      real(r8) :: depth, dx, val1, val2, val3, val4, x, x0, y, y0

#include "set_bounds.h"
!
!-----------------------------------------------------------------------
!  Initial conditions for 2D momentum (m/s) components.
!-----------------------------------------------------------------------
!
#if defined MY_APPLICATION
      DO j=JstrR,JendR
        DO i=Istr,IendR
          ubar(i,j,1)=???
        END DO
      END DO
      DO j=Jstr,JendR
        DO i=IstrR,IendR
          vbar(i,j,1)=???
        END DO
      END DO
#else
      DO j=JstrR,JendR
        DO i=Istr,IendR
          ubar(i,j,1)=0.0_r8
        END DO
      END DO
      DO j=Jstr,JendR
        DO i=IstrR,IendR
          vbar(i,j,1)=0.0_r8
        END DO
      END DO
#endif
!
!-----------------------------------------------------------------------
!  Initial conditions for free-surface (m).
!-----------------------------------------------------------------------
!
#if defined MY_APPLICATION
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          zeta(i,j,1)=???
        END DO
      END DO
#elif defined SANDWAVE
      val1=100.0_r8
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          zeta(i,j,1)=9.0E-06_r8*REAL(i,r8)*val1
        END DO
      END DO
#else
      DO j=JstrR,JendR
        DO i=IstrR,IendR
          zeta(i,j,1)=0.0_r8
        END DO
      END DO
#endif

#ifdef SOLVE3D
!
!-----------------------------------------------------------------------
!  Initial conditions for 3D momentum components (m/s).
!-----------------------------------------------------------------------
!
# if defined MY_APPLICATION
      DO k=1,N(ng)
       DO j=JstrR,JendR
         DO i=Istr,IendR
            u(i,j,k,1)=???
          END DO
        END DO
        DO j=Jstr,JendR
          DO i=IstrR,IendR
            v(i,j,k,1)=???
          END DO
        END DO
      END DO
# elif defined SANDWAVE
      DO k=1,N(ng)
       DO j=JstrR,JendR
         DO i=Istr,IendR
            u(i,j,k,1)=-1.0_r8*LOG((h(i,j)+z_r(i,j,k))/Zob(ng))/        &
     &                 (LOG(h(i,j)/Zob(ng))-1.0_r8+Zob(ng)/h(i,j))
          END DO
        END DO
        DO j=Jstr,JendR
          DO i=IstrR,IendR
            v(i,j,k,1)=0.0_r8
          END DO
        END DO
      END DO
# else
      DO k=1,N(ng)
       DO j=JstrR,JendR
         DO i=Istr,IendR
            u(i,j,k,1)=0.0_r8
          END DO
        END DO
        DO j=Jstr,JendR
          DO i=IstrR,IendR
            v(i,j,k,1)=0.0_r8
          END DO
        END DO
      END DO
# endif
!
!-----------------------------------------------------------------------
!  Initial conditions for tracer type variables.
!-----------------------------------------------------------------------
!
!  Set initial conditions for potential temperature (Celsius) and
!  salinity (PSU).
!
# if defined MY_APPLICATION
      DO k=1,N(ng)
        DO j=JstrR,JendR
          DO i=IstrR,IendR
            t(i,j,k,1,itemp)=???
#  ifdef SALINITY
            t(i,j,k,1,isalt)=???
#  endif
          END DO
        END DO
      END DO
# elif defined SANDWAVE
      DO k=1,N(ng)
        DO j=JstrR,JendR
          DO i=IstrR,IendR
            t(i,j,k,1,itemp)=20.0_r8
#  ifdef SALINITY
            t(i,j,k,1,isalt)=0.0_r8
#  endif
          END DO
        END DO
      END DO
# else
      DO k=1,N(ng)
        DO j=JstrR,JendR
          DO i=IstrR,IendR
            t(i,j,k,1,itemp)=T0(ng)
#  ifdef SALINITY
            t(i,j,k,1,isalt)=S0(ng)
#  endif
          END DO
        END DO
      END DO
# endif
#endif

      RETURN
      END SUBROUTINE ana_initial_tile