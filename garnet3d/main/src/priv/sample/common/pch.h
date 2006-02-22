#ifndef __GN_PCH_H__
#define __GN_PCH_H__
// *****************************************************************************
// \file    common/pch.h
// \brief   PCH header
// \author  chenlee (2006.1.7)
// *****************************************************************************

#include "garnet/GNbase.h"
#include "garnet/GNcore.h"
#include "garnet/GNgfx.h"
#include "garnet/GNinput.h"
#include "garnet/base/pragma.h"

#if GN_XENON
#include <xtl.h>
#else
#define NOMINMAX
#include <windows.h>
#endif

// *****************************************************************************
//                           End of pch.h
// *****************************************************************************
#endif // __GN_PCH_H__
