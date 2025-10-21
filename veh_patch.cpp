#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

const DWORD64 TARGET_OFFSET = 0xE5EE7E; // 崩溃偏移地址
DWORD64 g_ModuleBase = 0;

// 日志函数
void WriteLog(const std::string& msg) {
    std::ofstream logFile("veh_patch.log", std::ios::app);
    if (logFile.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        logFile << "[" << st.wYear << "-" << st.wMonth << "-" << st.wDay << " "
                << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "] "
                << msg << std::endl;
        logFile.close();
    }
}

// 异常处理函数
LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 crashAddr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;
        DWORD64 targetAddr = g_ModuleBase + TARGET_OFFSET;

        if (crashAddr == targetAddr) {
            WriteLog("[VEH] 捕获到空指针访问异常，崩溃地址: 0x" + std::to_string(crashAddr) + " ，已跳过导致崩溃的指令。");

            // 假设导致崩溃的指令长度为 2 字节（常见 MOV/CALL）
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
            WriteLog("[DllMain] veh_patch.dll 已成功注入，worldserver.exe 基址: 0x" + std::to_string((DWORD64)hMain));
            AddVectoredExceptionHandler(1, VehHandler);
            WriteLog("[DllMain] VEH 异常处理程序安装完成，等待捕获崩溃...");
        } else {
            WriteLog("[DllMain] 获取 worldserver.exe 模块基址失败！");
        }
    }
    return TRUE;
}
