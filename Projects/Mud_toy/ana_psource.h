      SUBROUTINE ana_psource (ng, tile, model)
!
!! svn $Id: ana_psource.h 1162 2007-10-22 02:18:14Z jcwarner $
!!======================================================================
!! Copyright (c) 2002-2007 The ROMS/TOMS Group                         !
!!   Licensed under a MIT/X style license                              !
!!   See License_ROMS.txt                                              !
!!                                                                     !
!=======================================================================
!                                                                      !
!  This subroutine sets analytical tracer and mass point Sources       !
!  and/or Sinks.  River runoff can be consider as a point source.      !
!                                                                      !
!=======================================================================
!
      USE mod_param
      USE mod_grid
      USE mod_ncparam
      USE mod_ocean
      USE mod_sources
      USE mod_stepping
!
      integer, intent(in) :: ng, tile, model

      integer :: LBi, UBi, LBj, UBj
!
      LBi=LBOUND(GRID(ng)%h,DIM=1)
      UBi=UBOUND(GRID(ng)%h,DIM=1)
      LBj=LBOUND(GRID(ng)%h,DIM=2)
      UBj=UBOUND(GRID(ng)%h,DIM=2)
!
      CALL ana_psource_grid (ng, model, LBi, UBi, LBj, UBj,             &
     &                       nnew(ng), knew(ng), Nsrc(ng),              &
     &                       OCEAN(ng) % zeta,                          &
     &                       OCEAN(ng) % ubar,                          &
     &                       OCEAN(ng) % vbar,                          &
#ifdef SOLVE3D
     &                       OCEAN(ng) % u,                             &
     &                       OCEAN(ng) % v,                             &
     &                       GRID(ng) % z_w,                            &
#endif
     &                       GRID(ng) % h,                              &
     &                       GRID(ng) % on_u,                           &
     &                       GRID(ng) % om_v,                           &
     &                       SOURCES(ng) % Isrc,                        &
     &                       SOURCES(ng) % Jsrc,                        &
     &                       SOURCES(ng) % Lsrc,                        &
     &                       SOURCES(ng) % Dsrc,                        &
#ifdef SOLVE3D
# if defined UV_PSOURCE || defined Q_PSOURCE
     &                       SOURCES(ng) % Qshape,                      &
     &                       SOURCES(ng) % Qsrc,                        &
# endif
# ifdef TS_PSOURCE
     &                       SOURCES(ng) % Tsrc,                        &
# endif
#endif
     &                       SOURCES(ng) % Qbar)
!
! Set analytical header file name used.
!
      IF (Lanafile) THEN
        WRITE (ANANAME(20),'(a,a)') TRIM(Adir), '/ana_psource.h'
      END IF

      RETURN
      END SUBROUTINE ana_psource
!
!***********************************************************************
      SUBROUTINE ana_psource_grid (ng, model, LBi, UBi, LBj, UBj,       &
     &                             nnew, knew, Nsrc,                    &
     &                             zeta, ubar, vbar,                    &
#ifdef SOLVE3D
     &                             u, v, z_w,                           &
#endif
     &                             h, on_u, om_v,                       &
     &                             Isrc, Jsrc, Lsrc, Dsrc,              &
#ifdef SOLVE3D
# if defined UV_PSOURCE || defined Q_PSOURCE
     &                             Qshape, Qsrc,                        &
# endif
# ifdef TS_PSOURCE
     &                             Tsrc,                                &
# endif
#endif
     &                             Qbar)
!***********************************************************************
!
      USE mod_param
      USE mod_scalars
#ifdef SEDIMENT
      USE mod_sediment
#endif
#ifndef ASSUMED_SHAPE
      USE mod_sources, ONLY : Msrc
#endif
!
!  Imported variable declarations.
!
      integer, intent(in) :: ng, model, LBi, UBi, LBj, UBj
      integer, intent(in) :: nnew, knew

      integer, intent(out) :: Nsrc
!
#ifdef ASSUMED_SHAPE
      logical, intent(out) :: Lsrc(:,:)

      integer, intent(out) :: Isrc(:)
      integer, intent(out) :: Jsrc(:)

      real(r8), intent(in) :: zeta(LBi:,LBj:,:)
      real(r8), intent(in) :: ubar(LBi:,LBj:,:)
      real(r8), intent(in) :: vbar(LBi:,LBj:,:)
# ifdef SOLVE3D
      real(r8), intent(in) :: u(LBi:,LBj:,:,:)
      real(r8), intent(in) :: v(LBi:,LBj:,:,:)
      real(r8), intent(in) :: z_w(LBi:,LBj:,0:)
# endif
      real(r8), intent(in) :: h(LBi:,LBj:)
      real(r8), intent(in) :: on_u(LBi:,LBj:)
      real(r8), intent(in) :: om_v(LBi:,LBj:)

      real(r8), intent(out) :: Dsrc(:)
      real(r8), intent(out) :: Qbar(:)
# ifdef SOLVE3D
#  if defined UV_PSOURCE || defined Q_PSOURCE
      real(r8), intent(out) :: Qshape(:,:)
      real(r8), intent(out) :: Qsrc(:,:)
#  endif
#  ifdef TS_PSOURCE
      real(r8), intent(out) :: Tsrc(:,:,:)
#  endif
# endif
#else
      logical, intent(out) :: Lsrc(Msrc,NT(ng))

      integer, intent(out) :: Isrc(Msrc)
      integer, intent(out) :: Jsrc(Msrc)

      real(r8), intent(in) :: zeta(LBi:UBi,LBj:UBj,3)
      real(r8), intent(in) :: ubar(LBi:UBi,LBj:UBj,3)
      real(r8), intent(in) :: vbar(LBi:UBi,LBj:UBj,3)
# ifdef SOLVE3D
      real(r8), intent(in) :: u(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(in) :: v(LBi:UBi,LBj:UBj,N(ng),2)
      real(r8), intent(in) :: z_w(LBi:UBi,LBj:UBj,0:N(ng))
# endif
      real(r8), intent(in) :: h(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: on_u(LBi:UBi,LBj:UBj)
      real(r8), intent(in) :: om_v(LBi:UBi,LBj:UBj)

      real(r8), intent(out) :: Dsrc(Msrc)
      real(r8), intent(out) :: Qbar(Msrc)
# ifdef SOLVE3D
#  if defined UV_PSOURCE || defined Q_PSOURCE
      real(r8), intent(out) :: Qshape(Msrc,N(ng))
      real(r8), intent(out) :: Qsrc(Msrc,N(ng))
#  endif
#  ifdef TS_PSOURCE
      real(r8), intent(out) :: Tsrc(Msrc,N(ng),NT(ng))
#  endif
# endif
#endif
!
!  Local variable declarations.
!
	integer :: is, i, j, k, ised, itrc
      real(r8) :: fac, my_area, ramp
!
!-----------------------------------------------------------------------
!  Set tracer and/or mass point sources and/or sink.
!-----------------------------------------------------------------------
!
      IF (iic(ng).eq.ntstart(ng)) THEN
!
!  Set-up point Sources/Sink number (Nsrc), direction (Dsrc), I- and
!  J-grid locations (Isrc,Jsrc), and logical switch for type of tracer
!  to apply (Lsrc). Currently, the direction can be along XI-direction
!  (Dsrc = 0) or along ETA-direction (Dsrc > 0).  The mass sources are
!  located at U- or V-points so the grid locations should range from
!  1 =< Isrc =< L  and  1 =< Jsrc =< M.
!
#if defined MUD_TOY
        Lsrc=.TRUE.
        Nsrc=1
        Dsrc=-1
        Isrc=5
        Jsrc=6
#else
        ana_psource.h: No values provided for Lsrc, Nsrc, Dsrc,
                                              Isrc, Jsrc.
#endif
      END IF
#if defined UV_PSOURCE || defined Q_PSOURCE
# ifdef SOLVE3D
!
!  If appropriate, set-up nondimensional shape function to distribute
!  mass point sources/sinks vertically.  It must add to unity!!.
!
#  if defined MUD_TOY
        DO is=1,Nsrc
          DO k=1,N(ng)-1
            Qshape(is,k)=0.0
          END DO
	Qshape(is,N(ng)-1)=1.0
        END DO
#  else
        ana_psource.h: No values provided for Qshape.
#  endif
# endif
!
!  Set-up vertically integrated mass transport (m3/s) of point
!  Sources/Sinks (positive in the positive U- or V-direction and
!  viceversa).
!
# if defined MUD_TOY
      DO is=1,Nsrc
        Qbar(is)=0.001_r8
      END DO
# else
      ana_psource.h: No values provided for Qbar.
# endif

# ifdef SOLVE3D
!
!  Set-up mass transport profile (m3/s) of point Sources/Sinks.
!
      DO k=1,N(ng)
        DO is=1,Nsrc
          Qsrc(is,k)=Qbar(is)*Qshape(is,k)
        END DO
      END DO
# endif
#endif

#if defined TS_PSOURCE && defined SOLVE3D
!
!  Set-up tracer (tracer units) point Sources/Sinks.
!
# if defined MUD_TOY
      DO itrc=1,NT(ng)
        DO k=1,N(ng)
          DO is=1,Nsrc
            IF (itrc.eq.itemp) THEN
              Tsrc(is,k,itrc)=T0(ng)      !8.0
            END IF
            IF (itrc.eq.isalt) THEN
              Tsrc(is,k,itrc)=S0(ng)      !30.0
            END IF
          END DO
        END DO
      END DO
# else
      ana_psource.h: No values provided for Tsrc.
# endif
#endif

      RETURN
      END SUBROUTINE ana_psource_grid