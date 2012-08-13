/*
 * platform_windows_runtime_os.h - Windows runtime version discovery.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef VICE_PLATFORM_WINDOWS_RUNTIME_OS_H
#define VICE_PLATFORM_WINDOWS_RUNTIME_OS_H

#ifndef VER_NT_WORKSTATION
#define VER_NT_WORKSTATION 0x00000001
#endif

#ifndef VER_NT_SERVER
#define VER_NT_SERVER 0x00000003
#endif

#ifndef VER_SUITE_TERMINAL
#define VER_SUITE_TERMINAL 0x00000010
#endif

#ifndef VER_SUITE_ENTERPRISE
#define VER_SUITE_ENTERPRISE 0x00000002
#endif

#ifndef VER_SUITE_SMALLBUSINESS
#define VER_SUITE_SMALLBUSINESS 0x00000001
#endif

#ifndef VER_SUITE_EMBEDDEDNT
#define VER_SUITE_EMBEDDEDNT 0x00000040
#endif

#ifndef VER_SUITE_DATACENTER
#define VER_SUITE_DATACENTER 0x00000080
#endif

#ifndef VER_SUITE_PERSONAL
#define VER_SUITE_PERSONAL 0x00000200
#endif

#ifndef VER_SUITE_BLADE
#define VER_SUITE_BLADE 0x00000400
#endif

#ifndef VER_SUITE_COMPUTE_SERVER
#define VER_SUITE_COMPUTE_SERVER 0x00004000
#endif

#ifndef VER_SUITE_STORAGE_SERVER
#define VER_SUITE_STORAGE_SERVER 0x00002000
#endif

#ifndef VER_SUITE_WH_SERVER
#define VER_SUITE_WH_SERVER 0x00008000
#endif

#ifndef PRODUCT_STARTER
#define PRODUCT_STARTER 0x0000000B
#endif

#ifndef PRODUCT_HOME_BASIC
#define PRODUCT_HOME_BASIC 0x00000002
#endif

#ifndef PRODUCT_HOME_PREMIUM
#define PRODUCT_HOME_PREMIUM 0x00000003
#endif

#ifndef PRODUCT_BUSINESS
#define PRODUCT_BUSINESS 0x00000006
#endif

#ifndef PRODUCT_ENTERPRISE
#define PRODUCT_ENTERPRISE 0x00000004
#endif

#ifndef PRODUCT_ULTIMATE
#define PRODUCT_ULTIMATE 0x00000001
#endif

#ifndef PRODUCT_WEB_SERVER
#define PRODUCT_WEB_SERVER 0x00000011
#endif

#ifndef PRODUCT_ENTERPRISE_SERVER
#define PRODUCT_ENTERPRISE_SERVER 0x0000000A
#endif

#ifndef PRODUCT_DATACENTER_SERVER
#define PRODUCT_DATACENTER_SERVER 0x00000008
#endif

#ifndef PRODUCT_STANDARD_SERVER
#define PRODUCT_STANDARD_SERVER 0x00000007
#endif

#ifndef PRODUCT_SMALLBUSINESS_SERVER
#define PRODUCT_SMALLBUSINESS_SERVER 0x00000009
#endif

#ifndef PRODUCT_ENTERPRISE_SERVER_IA64
#define PRODUCT_ENTERPRISE_SERVER_IA64 0x0000000F
#endif

#ifndef PRODUCT_CLUSTER_SERVER
#define PRODUCT_CLUSTER_SERVER 0x00000012
#endif

#ifndef PRODUCT_SERVER_FOR_SMALLBUSINESS
#define PRODUCT_SERVER_FOR_SMALLBUSINESS 0x00000018
#endif

#ifndef PRODUCT_SERVER_FOUNDATION
#define PRODUCT_SERVER_FOUNDATION 0x00000021
#endif

#ifndef SM_TABLETPC
#define SM_TABLETPC 86
#endif

#ifndef SM_MEDIACENTER
#define SM_MEDIACENTER 87
#endif

#ifndef SM_STARTER
#define SM_STARTER 88
#endif

#ifndef SM_SERVERR2
#define SM_SERVERR2 89
#endif

#ifndef PROCESSOR_ARCHITECTURE_AMD64
#define PROCESSOR_ARCHITECTURE_AMD64 9
#endif

#ifndef PROCESSOR_ARCHITECTURE_IA64
#define PROCESSOR_ARCHITECTURE_IA64 6
#endif

#define VICE_SM_SERVERR2		8
#define VICE_SM_MEDIACENTER	4
#define VICE_SM_STARTER		2
#define VICE_SM_TABLETPC		1

typedef BOOL (WINAPI *VGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
typedef void (WINAPI *VGNSI)(LPSYSTEM_INFO);

typedef struct winver_s {
    char *name;
    int platformid;
    int majorver;
    int minorver;
    int realos;
    int producttype;
    int suite;
    int pt6;
    int metrics;
} winver_t;

static winver_t windows_versions[] = {
    { NULL, 0,
      0, 0, 0, 0, 0, 0, 0 },	 /* place holder for what has been detected */
    { "Windows 95", VER_PLATFORM_WIN32_WINDOWS,
      4, 0, 3, -1, -1, -1, -1 },
    { "Windows 98", VER_PLATFORM_WIN32_WINDOWS,
      4, 10, 4, -1, -1, -1, -1 },
    { "Windows ME", VER_PLATFORM_WIN32_WINDOWS,
      4, 90, 5, -1, -1, -1, -1 },
    { "Windows NT 3.10 Workstation", VER_PLATFORM_WIN32_NT,
      3, 10, 0, VER_NT_WORKSTATION, -1, -1, -1 },
    { "Windows NT 3.10 Advanced Server", VER_PLATFORM_WIN32_NT,
      3, 10, 0, VER_NT_SERVER, -1, -1, -1 },
    { "Windows NT 3.50 Workstation", VER_PLATFORM_WIN32_NT,
      3, 5, 0, VER_NT_WORKSTATION, -1, -1, -1 },
    { "Windows NT 3.50 Server", VER_PLATFORM_WIN32_NT,
      3, 5, 0, VER_NT_SERVER, -1, -1, -1 },
    { "Windows NT 3.51 Workstation", VER_PLATFORM_WIN32_NT,
      3, 51, 0, VER_NT_WORKSTATION, -1, -1, -1 },
    { "Windows NT 3.51 Server", VER_PLATFORM_WIN32_NT,
      3, 51, 0, VER_NT_SERVER, -1, -1, -1 },
    { "Windows NT 4.0 Workstation", VER_PLATFORM_WIN32_NT,
      4, 0, 1, VER_NT_WORKSTATION, 0, -1, -1 },
    { "Windows NT 4.0 Terminal Server", VER_PLATFORM_WIN32_NT,
      4, 0, 1, VER_NT_SERVER, VER_SUITE_TERMINAL, -1, -1 },
    { "Windows NT 4.0 Enterprise Server", VER_PLATFORM_WIN32_NT,
      4, 0, 1, VER_NT_SERVER, VER_SUITE_ENTERPRISE, -1, -1 },
    { "Windows NT 4.0 Small Bsiness Server", VER_PLATFORM_WIN32_NT,
      4, 0, 1, VER_NT_SERVER, VER_SUITE_SMALLBUSINESS, -1, -1 },
    { "Windows NT 4.0 Embedded", VER_PLATFORM_WIN32_NT,
      4, 0, 1, -1, VER_SUITE_EMBEDDEDNT, -1, -1 },
    { "Windows NT 4.0 Server", VER_PLATFORM_WIN32_NT,
      4, 0, 1, VER_NT_SERVER, 0, -1, -1 },
    { "Windows 2000 Professional", VER_PLATFORM_WIN32_NT,
      5, 0, 6, VER_NT_WORKSTATION, 0, -1, -1 },
    { "Windows 2000 Advanced Server / Windows 2000 Advanced Server Limited Edition", VER_PLATFORM_WIN32_NT,
      5, 0, 6, VER_NT_SERVER, VER_SUITE_ENTERPRISE, -1, -1 },
    { "Windows 2000 Datacenter Server / Windows 2000 Datacenter Server Limited Edition", VER_PLATFORM_WIN32_NT,
      5, 0, 6, VER_NT_SERVER, VER_SUITE_DATACENTER, -1, -1 },
    { "Windows 2000 Server", VER_PLATFORM_WIN32_NT,
      5, 0, 6, VER_NT_SERVER, 0, -1, -1 },
    { "Windows XP Home", VER_PLATFORM_WIN32_NT,
      5, 1, 8, VER_NT_WORKSTATION, VER_SUITE_PERSONAL, -1, -1 },
    { "Windows XP Starter", VER_PLATFORM_WIN32_NT,
      5, 1, 8, VER_NT_WORKSTATION, 0, -1, VICE_SM_STARTER },
    { "Windows XP Tablet PC", VER_PLATFORM_WIN32_NT,
      5, 1, 8, VER_NT_WORKSTATION, 0, -1, VICE_SM_TABLETPC },
    { "Windows XP Media Center", VER_PLATFORM_WIN32_NT,
      5, 1, 8, VER_NT_WORKSTATION, 0, -1, VICE_SM_MEDIACENTER },
    { "Windows XP Professional", VER_PLATFORM_WIN32_NT,
      5, 1, 8, VER_NT_WORKSTATION, 0, -1, -1 },
    { "Windows XP Embedded", VER_PLATFORM_WIN32_NT,
      5, 1, 8, VER_NT_WORKSTATION, VER_SUITE_EMBEDDEDNT, -1, -1 },
    { "Windows 2003 R2 Web Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_BLADE, -1, VICE_SM_SERVERR2 },
    { "Windows 2003 Web Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_BLADE, -1, -1 },
    { "Windows 2003 R2 Enterprise Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_ENTERPRISE, -1, VICE_SM_SERVERR2 },
    { "Windows 2003 Enterprise Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_ENTERPRISE, -1, -1 },
    { "Windows 2003 R2 Datacenter Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_DATACENTER, -1, VICE_SM_SERVERR2 },
    { "Windows 2003 Datacenter Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_DATACENTER, -1, -1 },
    { "Windows 2003 R2 Compute Cluster Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_COMPUTE_SERVER, -1, VICE_SM_SERVERR2 },
    { "Windows 2003 Compute Cluster Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_COMPUTE_SERVER, -1, -1 },
    { "Windows 2003 R2 Small Business Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_SMALLBUSINESS, -1, VICE_SM_SERVERR2 },
    { "Windows 2003 Small Business Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_SMALLBUSINESS, -1, -1 },
    { "Windows 2003 R2 Storage Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_STORAGE_SERVER, -1, VICE_SM_SERVERR2 },
    { "Windows 2003 Storage Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_STORAGE_SERVER, -1, -1 },
    { "Windows Home Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, VER_SUITE_WH_SERVER, -1, -1 },
    { "Windows XP64", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_WORKSTATION, 0, -1, -1 },
    { "Windows 2003 R2 Standard Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, 0, -1, VICE_SM_SERVERR2 },
    { "Windows 2003 Standard Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8, VER_NT_SERVER, 0, -1, -1 },
    { "Windows Vista Starter", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_WORKSTATION, VER_SUITE_PERSONAL, PRODUCT_STARTER, -1 },
    { "Windows Vista Home Basic", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_WORKSTATION, VER_SUITE_PERSONAL, PRODUCT_HOME_BASIC, -1 },
    { "Windows Vista Home Premium", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_WORKSTATION, VER_SUITE_PERSONAL, PRODUCT_HOME_PREMIUM, -1 },
    { "Windows Vista Business", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_WORKSTATION, 0, PRODUCT_BUSINESS, -1 },
    { "Windows Vista Enterprise", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_WORKSTATION, 0, PRODUCT_ENTERPRISE, -1 },
    { "Windows Vista Ultimate", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_WORKSTATION, 0, PRODUCT_ULTIMATE, -1 },
    { "Windows 2008 Web Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, VER_SUITE_BLADE, PRODUCT_WEB_SERVER, -1 },
    { "Windows 2008 Enterprise Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, VER_SUITE_ENTERPRISE, PRODUCT_ENTERPRISE_SERVER, -1 },
    { "Windows 2008 Datacenter Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, VER_SUITE_DATACENTER, PRODUCT_DATACENTER_SERVER, -1 },
    { "Windows 2008 Standard Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, 0, PRODUCT_STANDARD_SERVER, -1 },
    { "Windows 2008 Small Business Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, 0, PRODUCT_SMALLBUSINESS_SERVER, -1 },
    { "Windows 2008 Enterprise Server for IA64 Systems", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, 0, PRODUCT_ENTERPRISE_SERVER_IA64, -1 },
    { "Windows 2008 HPC Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, 0, PRODUCT_CLUSTER_SERVER, -1 },
    { "Windows 2008 Essential Business Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, 0, PRODUCT_SERVER_FOR_SMALLBUSINESS, -1 },
    { "Windows 2008 Foundation Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10, VER_NT_SERVER, 0, PRODUCT_SERVER_FOUNDATION, -1 },
    { "Windows 7 Starter", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_WORKSTATION, 0, PRODUCT_STARTER, -1 },
    { "Windows 7 Home Basic", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_WORKSTATION, 0, PRODUCT_HOME_BASIC, -1 },
    { "Windows 7 Home Premium", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_WORKSTATION, 0, PRODUCT_HOME_PREMIUM, -1 },
    { "Windows 7 Enterprise", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_WORKSTATION, 0, PRODUCT_ENTERPRISE, -1 },
    { "Windows 7 Business", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_WORKSTATION, 0, PRODUCT_BUSINESS, -1 },
    { "Windows 7 Ultimate", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_WORKSTATION, 0, PRODUCT_ULTIMATE, -1 },
    { "Windows 2008 R2 Web Server", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_SERVER, 0, PRODUCT_WEB_SERVER, -1 },
    { "Windows 2008 R2 Standard Server", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_SERVER, 0, PRODUCT_STANDARD_SERVER, -1 },
    { "Windows 2008 R2 Enterprise Server", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_SERVER, 0, PRODUCT_ENTERPRISE_SERVER, -1 },
    { "Windows 2008 R2 Datacenter Server", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_SERVER, 0, PRODUCT_DATACENTER_SERVER, -1 },
    { "Windows 2008 R2 Enterprise Server for IA64 Systems", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_SERVER, 0, PRODUCT_ENTERPRISE_SERVER_IA64, -1 },
    { "Windows 2008 R2 Foundation Server", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_SERVER, 0, PRODUCT_SERVER_FOUNDATION, -1 },
    { "Windows 2008 R2 HPC Server", VER_PLATFORM_WIN32_NT,
      6, 1, 10, VER_NT_SERVER, 0, PRODUCT_CLUSTER_SERVER, -1 },
    { "Windows 8 (Home/Pro/Enterprise)", VER_PLATFORM_WIN32_NT,
      6, 2, 10, VER_NT_WORKSTATION, 0, 0, -1 },
    { "Windows 2012 Server (Foundation/Essentials/Standard/Datacenter)", VER_PLATFORM_WIN32_NT,
      6, 2, 10, VER_NT_SERVER, 0, 0, -1 },
    { NULL, 0,
      0, 0, 0, 0, 0, 0, 0 }
};

/* 
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        CHAR szCSDVersion[128];
*/
static OSVERSIONINFO os_version_info;

/* define our own structure since the windows headers
   don't seem to agree on what OSVERSIONINFOEX should
   be defined as */
typedef struct _VICE_OSVERSIONINFOEX {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR szCSDVersion[128];
    WORD wServicePackMajor;
    WORD wServicePackMinor;
    WORD wSuiteMask;
    BYTE wProductType;
    BYTE wReserved;
} VICE_OSVERSIONINFOEX;

static VICE_OSVERSIONINFOEX os_version_ex_info;

/* 
0: NT3, 1: NT4, 2: 95, 3: 95OSR2, 4: 98, 5: ME, 6: 2000, 7: XP, 8: XPSP1, 9: 2003, 10: VISTA+
*/

static int GetRealOS(void)
{
    HMODULE k = GetModuleHandle(TEXT("kernel32.dll"));

    if (GetProcAddress(k, "GetLocaleInfoEx") != NULL) {
        return 10;
    }
    if (GetProcAddress(k, "GetLargePageMinimum") != NULL) {
        return 9;
    }
    if (GetProcAddress(k, "GetDLLDirectory") != NULL) {
        return 8;
    }
    if (GetProcAddress(k, "GetNativeSystemInfo") != NULL) {
        return 7;
    }
    if (GetProcAddress(k, "ReplaceFile") != NULL) {
        return 6;
    }
    if (GetProcAddress(k, "OpenThread") != NULL) {
        return 5;
    }
    if (GetProcAddress(k, "GetThreadPriorityBoost") != NULL) {
        return 1;
    }
    if (GetProcAddress(k, "ConnectNamedPipe") != NULL) {
        return 0;
    }
    if (GetProcAddress(k, "IsDebuggerPresent") != NULL) {
        return 4;
    }
    if (GetProcAddress(k, "GetDiskFreeSpaceEx") != NULL) {
        return 3;
    }
    return 2;
}

static int IsWow64(void)
{
    int bIsWow64 = 0;
    typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    HMODULE module = GetModuleHandle(TEXT("kernel32"));
    const char funcName[] = "IsWow64Process";

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(module, funcName);

    if (NULL != fnIsWow64Process) {
        fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
    }

    return bIsWow64;
}

static int IsReactOS(void)
{
    char *s = os_version_info.szCSDVersion;
    int i = 0;

    if (s[0] == 0) {
        return 0;
    }

    while (s[i++] != 0);

    if (!strncmp(s, "ReactOS", 7)) {
        return 1;
    }
    return 0;
}

static int IsWine(void)
{
    if (GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "wine_get_unix_file_name") != NULL) {
        return 1;
    }
    return 0;
}

static int IsOdin32(void)
{
    if (GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "RegisterLXExe") != NULL) {
        return 1;
    }
    return 0;
}

static int IsHxDos(void)
{
    if (GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetDKrnl32Version") != NULL) {
        return 1;
    }
    return 0;
}

static char *get_win95_version(void)
{
    if (!strncmp(os_version_info.szCSDVersion, " A ", 3)) {
        return "A";
    }
    if (!strncmp(os_version_info.szCSDVersion, " B ", 3)) {
        return "B";
    }
    if (!strncmp(os_version_info.szCSDVersion, " C ", 3)) {
        return "C";
    }
    return "";
}

static char *get_win98_version(void)
{
    if (!strncmp(os_version_info.szCSDVersion, " A ", 3)) {
        return "SE";
    }
    return "";
}


static int optional_mask_compare(int a, int b)
{
    if (b == -1 || (a & b) == b) {
		return 1;
    }
    return 0;
}

static int optional_compare(int a, int b)
{
    if (b == -1 || a == b) {
		return 1;
    }
    return 0;
}

static char windows_version[256];

static inline char *archdep_get_runtime_windows_os(void)
{
    int found = 0;
    int i;
    VGPI ViceGetProductInfo;
    VGNSI ViceGetNativeSystemInfo;
    SYSTEM_INFO systeminfo;
    DWORD PT;

    ZeroMemory(&os_version_info, sizeof(os_version_info));
    os_version_info.dwOSVersionInfoSize = sizeof(os_version_info);

    ZeroMemory(&os_version_ex_info, sizeof(os_version_ex_info));
    os_version_ex_info.dwOSVersionInfoSize = sizeof(os_version_ex_info);

    GetVersionEx(&os_version_info);

    windows_versions[0].platformid = (DWORD)os_version_info.dwPlatformId;
    windows_versions[0].majorver = (DWORD)os_version_info.dwMajorVersion;
    windows_versions[0].minorver = (DWORD)os_version_info.dwMinorVersion;
    windows_versions[0].realos = GetRealOS();

    if (windows_versions[0].platformid == VER_PLATFORM_WIN32_NT) {
        GetVersionEx(&os_version_ex_info);
        windows_versions[0].producttype = (BYTE)os_version_ex_info.wProductType;
        windows_versions[0].suite = (WORD)os_version_ex_info.wSuiteMask;
        if (windows_versions[0].majorver >= 6) {
            ViceGetProductInfo = (VGPI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
            ViceGetProductInfo(os_version_ex_info.dwMajorVersion, os_version_ex_info.dwMinorVersion, 0, 0, &PT);
            windows_versions[0].pt6 = PT;
        } else {
            windows_versions[0].pt6 = -1;
        }
    } else {
		windows_versions[0].producttype = -1;
		windows_versions[0].suite = -1;
    }

/* Metrics: 0000RMST
   R: Windows Server 2003 R2			SM_SERVERR2
   M: Windows XP Media Center Edition		SM_MEDIACENTER
   S: Windows XP Starter Edition		SM_STARTER
   T: Windows XP Tablet PC Edition		SM_TABLETPC
*/
    windows_versions[0].metrics = 0;
    if (GetSystemMetrics(SM_TABLETPC)) {
        windows_versions[0].metrics |= 1;
    }
    if (GetSystemMetrics(SM_STARTER)) {
        windows_versions[0].metrics |= 2;
    }
    if (GetSystemMetrics(SM_MEDIACENTER)) {
        windows_versions[0].metrics |= 4;
    }
    if (GetSystemMetrics(SM_SERVERR2)) {
        windows_versions[0].metrics |= 8;
    }

    for (i = 1; found == 0 && windows_versions[i].name != NULL; i++) {
        if (windows_versions[0].platformid == windows_versions[i].platformid) {
            if (windows_versions[0].majorver == windows_versions[i].majorver) {
                if (windows_versions[0].minorver == windows_versions[i].minorver) {
                    if (windows_versions[0].producttype == windows_versions[i].producttype) {
                        if (optional_mask_compare(windows_versions[0].suite, windows_versions[i].suite)) {
                            if (optional_compare(windows_versions[0].pt6, windows_versions[i].pt6)) {
                                if (optional_mask_compare(windows_versions[0].metrics, windows_versions[i].metrics)) {
                                    found = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (found) {
        sprintf(windows_version, "%s", windows_versions[i - 1].name);
        if (windows_versions[0].platformid == VER_PLATFORM_WIN32_WINDOWS) {
            if (windows_versions[0].minorver == 0) {
                sprintf(windows_version, "%s%s", windows_version, get_win95_version());
            }
            if (windows_versions[0].minorver == 10) {
                sprintf(windows_version, "%s%s", windows_version, get_win98_version());
            }
        } else {
            if (os_version_ex_info.wServicePackMajor) {
                sprintf(windows_version, "%s SP%d", windows_version, os_version_ex_info.wServicePackMajor);
            }
        }
        if (windows_versions[0].realos > windows_versions[i - 1].realos) {
            sprintf(windows_version, "%s (compatibility mode)", windows_version);
        }
        if (IsWow64()) {
            ViceGetNativeSystemInfo = (VGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
            ViceGetNativeSystemInfo(&systeminfo);
            if (systeminfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                sprintf(windows_version, "%s (WOW64 X64)", windows_version);
            } else {
                sprintf(windows_version, "%s (WOW64 IA64)", windows_version);
            }
        } else {
            if (windows_versions[0].majorver >= 5) {
                GetSystemInfo(&systeminfo);
                if (systeminfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) {
                    sprintf(windows_version, "%s (64bit IA64)", windows_version);
                } else if (systeminfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
                    sprintf(windows_version, "%s (64bit X64)", windows_version);
                } else {
                    sprintf(windows_version, "%s (32bit)", windows_version);
                }
            }
        }
        if (IsReactOS()) {
            sprintf(windows_version, "%s (ReactOS)", windows_version);
        }
        if (IsWine()) {
            sprintf(windows_version, "%s (Wine)", windows_version);
        }
        if (IsOdin32()) {
            sprintf(windows_version, "%s (Odin32)", windows_version);
        }
        if (IsHxDos()) {
            sprintf(windows_version, "%s (HXDOS)", windows_version);
        }
    } else {
        sprintf(windows_version, "%s (%d %d %d %d %d %d)", "Unknown Windows version",
                                                            windows_versions[0].platformid,
                                                            windows_versions[0].majorver,
                                                            windows_versions[0].minorver,
                                                            windows_versions[0].realos,
                                                            windows_versions[0].producttype,
                                                            windows_versions[0].suite);
    }
    return windows_version;
}
#endif
