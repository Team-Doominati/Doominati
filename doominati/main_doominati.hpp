//-----------------------------------------------------------------------------
//
// Copyright (C) 2016 Team Doominati
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Entry point definitions.
//
//-----------------------------------------------------------------------------

#ifndef DGE__doominati__main_doominati_H__
#define DGE__doominati__main_doominati_H__

#if DGE_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#define DGE_doominati_InitMain() SDL_SetMainReady()
#else
#define DGE_doominati_InitMain()
#endif

#endif//DGE__doominati__main_doominati_H__

