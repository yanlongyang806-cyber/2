#include <windows.h>
#include <fstream>
#include <sstream>

void WriteLog(const std::string& msg) {
    std::ofstream logFile("veh_patch.log", std::ios::app);
    if (logFile.is_open()) {
        SYSTEMTIME st;
        GetLocalTime(&st);
        logFile << "[" << st.wYear << "-" << st.wMonth << "-" << st.wDay << " "
                << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "] "
                << msg << std::endl;
    }
}

std::string HexStr(DWORD64 ptr) {
    std::stringstream ss;
    ss << "0x" << std::hex << ptr;
    return ss.str();
}

LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 crashAddr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;

        WriteLog("[VEH] 捕获访问异常 -> 地址: " + HexStr(crashAddr));

        // ✅ 智能跳过：检测是否为 null 引用类指令
        BYTE* instr = (BYTE*)crashAddr;
        if (*instr == 0x48 || *instr == 0x8B || *instr == 0x89) { 
            ExceptionInfo->ContextRecord->Rip += 2;
            WriteLog("[VEH] 自动跳过异常指令，继续运行");
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        WriteLog("[VEH] 未识别的异常，交给系统处理");
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        AddVectoredExceptionHandler(1, VehHandler);
        WriteLog("[DllMain] veh_patch_smart.dll 已注入，自动异常防护启用");
    }
    return TRUE;
}
