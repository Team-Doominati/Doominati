//-----------------------------------------------------------------------------
//
// Copyright (C) 2017 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Horrible art of hacks was passed down for 2000 years.
//
//-----------------------------------------------------------------------------

#ifndef DGE__Core__CompilerIntrinsics_H__
#define DGE__Core__CompilerIntrinsics_H__

//
// DGE_ForceInline
//
#if defined(_MSC_VER)
# define DGE_ForceInline __forceinline
#elif defined(__GNUC__)
# define DGE_ForceInline __attribute__((always_inline))
#endif

//
// DGE_ForceInlineMSVC
//
// Defined only if compiling under Microsoft Visual C++.
// For optimizations that only benefit it.
//
#if defined(_MSC_VER)
# define DGE_ForceInlineMSVC DGE_ForceInline
#else
# define DGE_ForceInlineMSVC
#endif

#endif//DGE__Core__CompilerIntrinsics_H__

