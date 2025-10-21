#include <windows.h>
#include <iostream>
#include <fstream>   // ✅ 用于写日志
#include <string>    // ✅ 让 std::to_string 可用

// 崩溃的相对偏移地址（你要根据自己版本调整）
const DWORD64 TARGET_OFFSET = 0xE5EE7E; 
DWORD64 g_ModuleBase = 0;

// 写日志函数
void WriteLog(const std::string& msg) {
    std::ofstream logFile("veh_patch.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << msg << std::endl;
        logFile.close();
    }
}

// 异常处理函数
LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 crashAddr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;
        DWORD64 targetAddr = g_ModuleBase + TARGET_OFFSET;

        if (crashAddr == targetAddr) {
            WriteLog("[VEH] 捕获到空指针访问，自动跳过指令防止崩溃。崩溃地址: 0x" + std::to_string(crashAddr));

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

            WriteLog("[DllMain] veh_patch.dll 已注入，正在安装 VEH 异常处理程序...");
            AddVectoredExceptionHandler(1, VehHandler);
            WriteLog("[DllMain] VEH 异常处理程序安装成功。");
        } else {
            WriteLog("[DllMain] 获取 worldserver.exe 基址失败。");
        }
    }
    return TRUE;
}
