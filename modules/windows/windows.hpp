#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#define NOSERVICE
#define NOMCX
#define NOIME
#define NOSOUND
#define NOCOMM
#define NOKANJI
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE
#define NOMINMAX
#define NODRAWTEXT
#define NOBITMAP
#define NOHELP

#define UNICODE
#define _UNICODE

#include <winsdkver.h>
#include <sdkddkver.h>
#include <windows.h>
#include <windowsx.h>
#include <winerror.h>

#include <tchar.h>

//	#include <dbghelp.h>
//	#include <sal.h>

#include <wrl.h>

#include <memoryapi.h>

#include <synchapi.h>

#include <combaseapi.h>
#include <comdef.h>
#include <commdlg.h>

#include <handleapi.h>

