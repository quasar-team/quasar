/******************************************************************************
** Copyright (C) 2006-2011 Unified Automation GmbH. All Rights Reserved.
** Web: http://www.unifiedautomation.com
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
** 
** Project: Utility functions.
** Description: Portable shutdown flag using CTRL-C (SIGINT)
** Notes: Visual Studio catches CTRL-C when it's thrown by default,
**        which is not wanted normally. You can disable this by opening the
          exceptions dialog via the menu Debug->Exceptions. Then deactivate
          the "CTRL-C" exception in the "Win32 Exceptions" category.
@author Unified Automation
@author Piotr Nikiel
******************************************************************************/
#ifndef __SHUTDOWN_H__
#define __SHUTDOWN_H__

#include <uavariant.h>

#ifdef _WIN32
#  ifdef _WIN32_WCE
/* Windows CE */
#    define SHUTDOWN_SEQUENCE "Escape"
#  else /* _WIN32_WCE */
/* Windows 2000/XP/Vista */
/* Uncomment the following line to use CTRL-C on Windows to shutdown. */
//#define USE_CTRLC_ON_WINDOWS
#    ifdef USE_CTRLC_ON_WINDOWS
/* Use CTRL-C signal */
#      define SHUTDOWN_SEQUENCE "CTRL-C"
#    else /* USE_CTRLC_ON_WINDOWS */
/* Use 'x' to shutdown using DOS conio.h */
#      define SHUTDOWN_SEQUENCE "x"
#    endif /* USE_CTRLC_ON_WINDOWS */
#  endif /* _WIN32_WCE */
#else /* _WIN32 */
/* Linux */
#  define SHUTDOWN_SEQUENCE "CTRL-C"
#endif /* _WIN32 */

/* activate some logging */
#define SHUTDOWN_TRACE_ACTIVE
#ifdef SHUTDOWN_TRACE_ACTIVE
#  include <stdio.h> /* for printf */
#  define SHUTDOWN_TRACE printf
#else
#  define SHUTDOWN_TRACE
#endif

/* Call this on startup. */
void RegisterSignalHandler();

/* Use this to check if the shutdown flag is set (i.e. the server should exit its 'main' loop and quit). */
unsigned int ShutDownFlag();

/* Call to make server shut down. Not relevant for 'ordinary' servers but required when a server is embedded
 * into a Python process or so.
 */
void ShutDown();

//! Need to keep it extern for usgae in opcserver_open62541.cpp, for instance.
extern volatile OpcUa_Boolean g_RunningFlag;

#endif
