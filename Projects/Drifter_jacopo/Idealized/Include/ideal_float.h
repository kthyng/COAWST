/*
** svn $Id: flt_test.h 65 2007-05-17 05:21:22Z arango $
*******************************************************************************
** Copyright (c) 2002-2007 The ROMS/TOMS Group                               **
**   Licensed under a MIT/X style license                                    **
**   See License_ROMS.txt                                                    **
*******************************************************************************
**
** Options for Floats Tracking Test.
**
** Application flag:   FLT_TEST
** Input script:       ocean_flt_test2d.in,  ocean_flt_test3d.in
**                     floats_flt_test2d.in, floats_flt_test3d.in
*/

#define UV_ADV
#define UV_LOGDRAG
#define UV_VIS2
#define MIX_S_UV
#define FLOATS
#define MASKING
#define EW_PERIODIC
#define NORTHERN_WALL
#define SOUTHERN_WALL
#define ANA_GRID
#define ANA_INITIAL
#define ANA_SMFLUX

# define SOLVE3D
#ifdef SOLVE3D
# define DJ_GRADPS
# define TS_A4HADVECTION
# define TS_A4VADVECTION
# define SPLINES
# define ANA_BTFLUX
# define ANA_STFLUX
#endif

#define GLS_MIXING
#if defined GLS_MIXING
# define KANTHA_CLAYSON
# define N2S2_HORAVG
#endif

#ifdef TEST1

/* Test 1 */
# define CHARNOK
# define CRAIG_BANNER

#elif defined TEST0

/* Test 0 */
# define CHARNOK
# undef CRAIG_BANNER

#endif