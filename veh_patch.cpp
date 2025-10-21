#include <windows.h>
#include <iostream>

const DWORD64 TARGET_OFFSET = 0xE5EE7E; // 崩溃偏移地址
DWORD64 g_ModuleBase = 0;

// 异常处理函数（安全跳过型）
LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 crashAddr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;
        DWORD64 targetAddr = g_ModuleBase + TARGET_OFFSET;

        if (crashAddr == targetAddr) {
            OutputDebugStringA("[VEH] 捕获到空指针访问，自动跳过指令防止崩溃。\n");

            // 假设导致崩溃的指令是 2 字节（常见 MOV/CALL）
            ExceptionInfo->ContextRecord->Rip += 2;

            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

// DLL 主入口
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        HMODULE hMain = GetModuleHandleA("worldserver.exe");
        if (hMain) {
            g_ModuleBase = (DWORD64)hMain;
            OutputDebugStringA("[DllMain] attached, installing VEH...\n");
            AddVectoredExceptionHandler(1, VehHandler);
            OutputDebugStringA("[InstallHandler] VEH installed\n");
        }
    }
    return TRUE;
}
