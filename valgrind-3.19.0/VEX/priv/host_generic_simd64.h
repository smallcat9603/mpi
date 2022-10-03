
/*---------------------------------------------------------------*/
/*--- begin                             host_generic_simd64.h ---*/
/*---------------------------------------------------------------*/

/*
   This file is part of Valgrind, a dynamic binary instrumentation
   framework.

   Copyright (C) 2004-2017 OpenWorks LLP
      info@open-works.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.

   The GNU General Public License is contained in the file COPYING.

   Neither the names of the U.S. Department of Energy nor the
   University of California nor the names of its contributors may be
   used to endorse or promote products derived from this software
   without prior written permission.
*/

/* Generic helper functions for doing 64-bit SIMD arithmetic in cases
   where the instruction selectors cannot generate code in-line.
   These are purely back-end entities and cannot be seen/referenced
   as clean helper functions from IR.

   These will get called from generated code and therefore should be
   well behaved -- no floating point or mmx insns, just straight
   integer code.

   Each function implements the correspondingly-named IR primop.
*/

#ifndef __VEX_HOST_GENERIC_SIMD64_H
#define __VEX_HOST_GENERIC_SIMD64_H

#include "libvex_basictypes.h"

/* DO NOT MAKE THESE INTO REGPARM FNS!  THIS WILL BREAK CALLING
   SEQUENCES GENERATED BY host-x86/isel.c. */

extern ULong h_generic_calc_Add32x2 ( ULong, ULong );
extern ULong h_generic_calc_Add16x4 ( ULong, ULong );
extern ULong h_generic_calc_Add8x8  ( ULong, ULong );

extern ULong h_generic_calc_QAdd16Sx4 ( ULong, ULong );
extern ULong h_generic_calc_QAdd8Sx8  ( ULong, ULong );
extern ULong h_generic_calc_QAdd16Ux4 ( ULong, ULong );
extern ULong h_generic_calc_QAdd8Ux8  ( ULong, ULong );

extern ULong h_generic_calc_Sub32x2 ( ULong, ULong );
extern ULong h_generic_calc_Sub16x4 ( ULong, ULong );
extern ULong h_generic_calc_Sub8x8  ( ULong, ULong );

extern ULong h_generic_calc_QSub16Sx4 ( ULong, ULong );
extern ULong h_generic_calc_QSub8Sx8  ( ULong, ULong );
extern ULong h_generic_calc_QSub16Ux4 ( ULong, ULong );
extern ULong h_generic_calc_QSub8Ux8  ( ULong, ULong );

extern ULong h_generic_calc_Mul16x4    ( ULong, ULong );
extern ULong h_generic_calc_Mul32x2    ( ULong, ULong );
extern ULong h_generic_calc_MulHi16Sx4 ( ULong, ULong );
extern ULong h_generic_calc_MulHi16Ux4 ( ULong, ULong );

extern ULong h_generic_calc_CmpEQ32x2  ( ULong, ULong );
extern ULong h_generic_calc_CmpEQ16x4  ( ULong, ULong );
extern ULong h_generic_calc_CmpEQ8x8   ( ULong, ULong );
extern ULong h_generic_calc_CmpGT32Sx2 ( ULong, ULong );
extern ULong h_generic_calc_CmpGT16Sx4 ( ULong, ULong );
extern ULong h_generic_calc_CmpGT8Sx8  ( ULong, ULong );

extern ULong h_generic_calc_CmpNEZ32x2 ( ULong );
extern ULong h_generic_calc_CmpNEZ16x4 ( ULong );
extern ULong h_generic_calc_CmpNEZ8x8  ( ULong );

extern ULong h_generic_calc_QNarrowBin32Sto16Sx4 ( ULong, ULong );
extern ULong h_generic_calc_QNarrowBin16Sto8Sx8  ( ULong, ULong );
extern ULong h_generic_calc_QNarrowBin16Sto8Ux8  ( ULong, ULong );
extern ULong h_generic_calc_NarrowBin32to16x4    ( ULong, ULong );
extern ULong h_generic_calc_NarrowBin16to8x8     ( ULong, ULong );

extern ULong h_generic_calc_InterleaveHI8x8 ( ULong, ULong );
extern ULong h_generic_calc_InterleaveLO8x8 ( ULong, ULong );
extern ULong h_generic_calc_InterleaveHI16x4 ( ULong, ULong );
extern ULong h_generic_calc_InterleaveLO16x4 ( ULong, ULong );
extern ULong h_generic_calc_InterleaveHI32x2 ( ULong, ULong );
extern ULong h_generic_calc_InterleaveLO32x2 ( ULong, ULong );

extern ULong h_generic_calc_CatOddLanes16x4 ( ULong, ULong );
extern ULong h_generic_calc_CatEvenLanes16x4 ( ULong, ULong );
extern ULong h_generic_calc_Perm8x8       ( ULong, ULong );
extern ULong h_generic_calc_PermOrZero8x8 ( ULong, ULong );

extern ULong h_generic_calc_ShlN8x8  ( ULong, UInt );
extern ULong h_generic_calc_ShlN16x4 ( ULong, UInt );
extern ULong h_generic_calc_ShlN32x2 ( ULong, UInt );

extern ULong h_generic_calc_ShrN16x4 ( ULong, UInt );
extern ULong h_generic_calc_ShrN32x2 ( ULong, UInt );

extern ULong h_generic_calc_SarN8x8  ( ULong, UInt );
extern ULong h_generic_calc_SarN16x4 ( ULong, UInt );
extern ULong h_generic_calc_SarN32x2 ( ULong, UInt );

extern ULong h_generic_calc_Avg8Ux8  ( ULong, ULong );
extern ULong h_generic_calc_Avg16Ux4 ( ULong, ULong );

extern ULong h_generic_calc_Max16Sx4 ( ULong, ULong );
extern ULong h_generic_calc_Max8Ux8  ( ULong, ULong );
extern ULong h_generic_calc_Min16Sx4 ( ULong, ULong );
extern ULong h_generic_calc_Min8Ux8  ( ULong, ULong );

extern UInt  h_generic_calc_GetMSBs8x8 ( ULong );

/* 32-bit SIMD HELPERS */

extern UInt h_generic_calc_Add16x2   ( UInt, UInt );
extern UInt h_generic_calc_Sub16x2   ( UInt, UInt );

extern UInt h_generic_calc_HAdd16Ux2 ( UInt, UInt );
extern UInt h_generic_calc_HAdd16Sx2 ( UInt, UInt );
extern UInt h_generic_calc_HSub16Ux2 ( UInt, UInt );
extern UInt h_generic_calc_HSub16Sx2 ( UInt, UInt );

extern UInt h_generic_calc_QAdd16Ux2 ( UInt, UInt );
extern UInt h_generic_calc_QAdd16Sx2 ( UInt, UInt );
extern UInt h_generic_calc_QSub16Ux2 ( UInt, UInt );
extern UInt h_generic_calc_QSub16Sx2 ( UInt, UInt );

extern UInt h_generic_calc_Add8x4   ( UInt, UInt );
extern UInt h_generic_calc_Sub8x4   ( UInt, UInt );

extern UInt h_generic_calc_HAdd8Ux4 ( UInt, UInt );
extern UInt h_generic_calc_HAdd8Sx4 ( UInt, UInt );
extern UInt h_generic_calc_HSub8Ux4 ( UInt, UInt );
extern UInt h_generic_calc_HSub8Sx4 ( UInt, UInt );

extern UInt h_generic_calc_QAdd8Ux4 ( UInt, UInt );
extern UInt h_generic_calc_QAdd8Sx4 ( UInt, UInt );
extern UInt h_generic_calc_QSub8Ux4 ( UInt, UInt );
extern UInt h_generic_calc_QSub8Sx4 ( UInt, UInt );

extern UInt h_generic_calc_Sad8Ux4  ( UInt, UInt );

extern UInt h_generic_calc_QAdd32S  ( UInt, UInt );
extern UInt h_generic_calc_QSub32S  ( UInt, UInt );

extern UInt h_generic_calc_CmpNEZ16x2 ( UInt );
extern UInt h_generic_calc_CmpNEZ8x4  ( UInt );

extern ULong h_calc_DPBtoBCD ( ULong dpb );
extern ULong h_calc_BCDtoDPB ( ULong bcd );

// Signed and unsigned integer division, that behave like
// the ARMv7 UDIV and SDIV instructions.
extern UInt  h_calc_udiv32_w_arm_semantics ( UInt,  UInt  );
extern ULong h_calc_udiv64_w_arm_semantics ( ULong, ULong );
extern Int   h_calc_sdiv32_w_arm_semantics ( Int,   Int   );
extern Long  h_calc_sdiv64_w_arm_semantics ( Long,  Long  );


#endif /* ndef __VEX_HOST_GENERIC_SIMD64_H */

/*---------------------------------------------------------------*/
/*--- end                               host_generic_simd64.h ---*/
/*---------------------------------------------------------------*/
