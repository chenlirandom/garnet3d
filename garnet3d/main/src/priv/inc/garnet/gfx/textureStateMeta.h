/*
 * This file use to define all texture stage states
 *
 * NOTE : internelly used by render module, do NOT include it directly in your
 *        own header or source files.
 *
 * syntax : GNGFX_DEFINE_TS(
 *               ts,
 *               defvalue-for-stage-0,
 *               d3dname,
 *               glname1, glname2 )
 *
 */

#ifndef GNGFX_DEFINE_TS
#error "GNGFX_DEFINE_TS not defined!"
#endif

//!
//! color operation
//!
GNGFX_DEFINE_TS( COLOROP      , TSV_MODULATE       , D3DTSS_COLOROP   , GL_COMBINE_RGB_ARB   , GL_RGB_SCALE_ARB      )

//!
//! color argument 0
//!
GNGFX_DEFINE_TS( COLORARG0    , TSV_TEXTURE_COLOR  , D3DTSS_COLORARG1 , GL_SOURCE0_RGB_ARB   , GL_OPERAND0_RGB_ARB   )

//!
//!  color argument 1
//!
GNGFX_DEFINE_TS( COLORARG1    , TSV_CURRENT_COLOR  , D3DTSS_COLORARG2 , GL_SOURCE1_RGB_ARB   , GL_OPERAND1_RGB_ARB   )

//!
//! alpha operation
//!
GNGFX_DEFINE_TS( ALPHAOP      , TSV_ARG0           , D3DTSS_ALPHAOP   , GL_COMBINE_ALPHA_ARB , GL_ALPHA_SCALE        )

//!
//! alpha argument 0
//!
GNGFX_DEFINE_TS( ALPHAARG0    , TSV_TEXTURE_ALPHA  , D3DTSS_ALPHAARG1 , GL_SOURCE0_ALPHA_ARB , GL_OPERAND0_ALPHA_ARB )

//!
//! alpha argument 1
//!
GNGFX_DEFINE_TS( ALPHAARG1    , TSV_CURRENT_ALPHA  , D3DTSS_ALPHAARG2 , GL_SOURCE1_ALPHA_ARB , GL_OPERAND1_ALPHA_ARB )
