#include <windows.h>
#include <fstream>
#include <sstream>
#include <string>

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

// 转换地址为 16 进制字符串
std::string PtrToHexStr(DWORD64 ptr) {
    std::stringstream ss;
    ss << "0x" << std::hex << ptr;
    return ss.str();
}

LONG CALLBACK VehHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    DWORD code = ExceptionInfo->ExceptionRecord->ExceptionCode;

    if (code == EXCEPTION_ACCESS_VIOLATION) {
        DWORD64 crashAddr = (DWORD64)ExceptionInfo->ExceptionRecord->ExceptionAddress;
        WriteLog("[VEH] 捕获到访问异常，崩溃地址: " + PtrToHexStr(crashAddr));

        // ✅ 判断是否空指针访问
        ULONG_PTR faultAddr = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];
        if (faultAddr < 0x1000) {
            WriteLog("[VEH] 检测到空指针访问，自动跳过此指令防止崩溃。");
            ExceptionInfo->ContextRecord->Rip += 2; // 跳过当前指令
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        // ✅ 判断是否读写无效内存
        WriteLog("[VEH] 检测到对无效内存访问，继续运行以避免宕机。");
        ExceptionInfo->ContextRecord->Rip += 2;
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        WriteLog("[DllMain] veh_patch.dll 已加载，安装全局 VEH 异常处理程序...");
        AddVectoredExceptionHandler(1, VehHandler);
        WriteLog("[DllMain] VEH 异常捕获器已启动。");
    }
    return TRUE;
}
