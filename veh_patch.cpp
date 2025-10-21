#include <windows.h>
#include <iostream>
#include <fstream>

const DWORD64 TARGET_OFFSET = 0xE5EE7E; // 崩溃偏移地址
DWORD64 g_ModuleBase = 0;

// 写入日志的函数
void WriteLog(const std::string& message) {
    std::ofstream log("veh_patch.log", std::ios::app);
    if (log.is_open()) {
        log << message << std::endl;
        log.close();
    }
}

// 异常处理函数（安全跳过型）
LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 crashAddr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;
        DWORD64 targetAddr = g_ModuleBase + TARGET_OFFSET;

        if (crashAddr == targetAddr) {
            WriteLog("[VEH] 捕获到空指针访问，自动跳过指令防止崩溃。");
            WriteLog("异常地址: 0x" + std::to_string(crashAddr));
            
            // 假设导致崩溃的指令是 2 字节（常见 MOV/CALL）
            ExceptionInfo->ContextRecord->Rip += 2;

            return EXCEPTION_CONTINUE_EXECUTION;
        } else {
            WriteLog("[VEH] 捕获到异常，但地址不匹配: 0x" + std::to_string(crashAddr));
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
            WriteLog("[DllMain] VEH Patch 已加载，正在安装异常处理器...");
            AddVectoredExceptionHandler(1, VehHandler);
            WriteLog("[InstallHandler] VEH 安装完成");
        }
    }
    return TRUE;
}
