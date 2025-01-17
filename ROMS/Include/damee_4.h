/*
** svn $Id: damee_4.h 889 2018-02-10 03:32:52Z arango $
*******************************************************************************
** Copyright (c) 2002-2019 The ROMS/TOMS Group                               **
**   Licensed under a MIT/X style license                                    **
**   See License_ROMS.txt                                                    **
*******************************************************************************
**
** Options for North Atlantic DAMEE Application, 3/4 degree resolution
**
** Application flag:   DAMEE_4
** Input script:       roms_damee_4.in
*/

#define ROMS_MODEL
#define UV_ADV
#define UV_COR
#define UV_QDRAG
#define DJ_GRADPS
#define SPLINES_VDIFF
#define SPLINES_VVISC
#define TS_U3HADVECTION
#define TS_C4VADVECTION
#define NONLIN_EOS
#define SALINITY
#define SOLVE3D
#define MASKING
#define QCORRECTION
#define SRELAXATION
#define CURVGRID
#define AVERAGES
#define LMD_MIXING
#ifdef LMD_MIXING
# define LMD_RIMIX
# define LMD_CONVEC
# define LMD_SKPP
# define LMD_NONLOCAL
# define RI_SPLINES
#endif
#define ANA_BSFLUX
#define ANA_BTFLUX
