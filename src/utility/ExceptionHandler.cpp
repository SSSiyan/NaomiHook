#include <windows.h>
#include <DbgHelp.h>
#include <spdlog/spdlog.h>

#include "utility/Module.hpp"
#include "utility/Scan.hpp"
#include "utility/Patch.hpp"

#include "ExceptionHandler.hpp"
#include "StackTrace.hpp"

#include "../Config.hpp"
#include <TlHelp32.h>

static const char* get_exception_code_info(UINT code) {
    switch (code) {
    case EXCEPTION_ACCESS_VIOLATION:
        return "The thread tried to read from or write to a virtual address for which it does not have the appropriate access.";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return "The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.";
    case EXCEPTION_BREAKPOINT:
        return "A breakpoint was encountered.";
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return "The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit "
               "values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.";
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return "One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a "
               "standard floating-point value.";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return "The thread tried to divide a floating-point value by a floating-point divisor of zero.";
    case EXCEPTION_FLT_INEXACT_RESULT:
        return "The result of a floating-point operation cannot be represented exactly as a decimal fraction.";
    case EXCEPTION_FLT_INVALID_OPERATION:
        return "This exception represents any floating-point exception not included in this list.";
    case EXCEPTION_FLT_OVERFLOW:
        return "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.";
    case EXCEPTION_FLT_STACK_CHECK:
        return "The stack overflowed or underflowed as the result of a floating-point operation.";
    case EXCEPTION_FLT_UNDERFLOW:
        return "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.";
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return "The thread tried to execute an invalid instruction.";
    case EXCEPTION_IN_PAGE_ERROR:
        return "The thread tried to access a page that was not present, and the system was unable to load the page. For example, this "
               "exception might occur if a network connection is lost while running a program over the network.";
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return "The thread tried to divide an integer value by an integer divisor of zero.";
    case EXCEPTION_INT_OVERFLOW:
        return "The result of an integer operation caused a carry out of the most significant bit of the result.";
    case EXCEPTION_INVALID_DISPOSITION:
        return "An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language "
               "such as C should never encounter this exception.";
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return "The thread tried to continue execution after a noncontinuable exception occurred.";
    case EXCEPTION_PRIV_INSTRUCTION:
        return "The thread tried to execute an instruction whose operation is not allowed in the current machine mode.";
    case EXCEPTION_SINGLE_STEP:
        return "A trace trap or other single-instruction mechanism signaled that one instruction has been executed.";
    case EXCEPTION_STACK_OVERFLOW:
        return "The thread used up its stack.";
    default:
        return "Unknown exception";
    }
}

static void get_modules(std::stringstream& ss) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snap == INVALID_HANDLE_VALUE) {
        return;
    }

    MODULEENTRY32 mod;
    mod.dwSize = sizeof(mod);
    BOOL cont = Module32First(snap, &mod);
    while (cont) {
        auto nameA = mod.szModule;
        auto pathA = mod.szExePath;
        char buffer[512] = {0};
        int i = sprintf(buffer, "Module: %p %06X %-16s %s\n", mod.modBaseAddr, mod.modBaseSize, nameA, pathA);
        if (i == -1) {
            continue;
        }
        ss << buffer;
        cont = Module32Next(snap, &mod);
    }
    CloseHandle(snap);
}

LONG WINAPI reframework::global_exception_handler(struct _EXCEPTION_POINTERS* ei) {
    spdlog::flush_on(spdlog::level::err);

    spdlog::error("Exception caught!");
   
    std::stringstream err_msg_buf;
    err_msg_buf << fmt::format("Exception occurred: {:x}", ei->ExceptionRecord->ExceptionCode) << '\n';
    err_msg_buf << get_exception_code_info(ei->ExceptionRecord->ExceptionCode) << '\n';
    err_msg_buf << "Check for naomihook_crash.dmp and NaomiHook_log.txt in NMH1 installation directory" << '\n';
    err_msg_buf << "Please describe what you were doing when NMH 1 crashed and send these files to the developers" << '\n';
    err_msg_buf << fmt::format("EIP: {:x}", ei->ContextRecord->Eip) << '\n';
    err_msg_buf << fmt::format("ESP: {:x}", ei->ContextRecord->Esp) << '\n';
    err_msg_buf << fmt::format("ECX: {:x}", ei->ContextRecord->Ecx) << '\n';
    err_msg_buf << fmt::format("EDX: {:x}", ei->ContextRecord->Edx) << '\n';
    err_msg_buf << fmt::format("EAX: {:x}", ei->ContextRecord->Eax) << '\n';
    err_msg_buf << fmt::format("EBX: {:x}", ei->ContextRecord->Ebx) << '\n';
    err_msg_buf << fmt::format("EBP: {:x}", ei->ContextRecord->Ebp) << '\n';
    err_msg_buf << fmt::format("ESI: {:x}", ei->ContextRecord->Esi) << '\n';
    err_msg_buf << fmt::format("EDI: {:x}", ei->ContextRecord->Edi) << '\n';
    err_msg_buf << fmt::format("EFLAGS: {:x}", ei->ContextRecord->EFlags) << '\n';
    err_msg_buf << fmt::format("CS: {:x}", ei->ContextRecord->SegCs) << '\n';
    err_msg_buf << fmt::format("DS: {:x}", ei->ContextRecord->SegDs) << '\n';
    err_msg_buf << fmt::format("ES: {:x}", ei->ContextRecord->SegEs) << '\n';
    err_msg_buf << fmt::format("FS: {:x}", ei->ContextRecord->SegFs) << '\n';
    err_msg_buf << fmt::format("GS: {:x}", ei->ContextRecord->SegGs) << '\n';
    err_msg_buf << fmt::format("SS: {:x}", ei->ContextRecord->SegSs) << '\n';

    get_modules(err_msg_buf);

    spdlog::error(err_msg_buf.str());

    const auto module_within = utility::get_module_within(ei->ContextRecord->Eip);

    if (module_within) {

        const auto module_path = utility::get_module_path(*module_within);

        if (module_path) {
            spdlog::error("Module: {:x} {}", (uintptr_t)*module_within, *module_path);
        } else {
            spdlog::error("Module: Unknown");
        }
    } else {
        spdlog::error("Module: Unknown");
    }

    auto dbghelp = LoadLibrary("dbghelp.dll");

    if (dbghelp) {
        const auto mod_dir = utility::get_module_directory(GetModuleHandle(0));
        const auto real_mod_dir = mod_dir ? (*mod_dir + "\\") : "";
        const auto final_path = real_mod_dir + "naomihook_crash.dmp";
        const auto final_path_log = real_mod_dir + LOG_FILENAME;
        spdlog::error("Attempting to write dump to {}", final_path);

        auto f = CreateFile(final_path.c_str(), 
            GENERIC_WRITE, 
            FILE_SHARE_WRITE, 
            nullptr, 
            CREATE_ALWAYS, 
            FILE_ATTRIBUTE_NORMAL, 
            nullptr
        );

        if (!f || f == INVALID_HANDLE_VALUE) {
            spdlog::error("Exception occurred, but could not create dump file");
            return EXCEPTION_CONTINUE_SEARCH;
        }

        MINIDUMP_EXCEPTION_INFORMATION ei_info{
            GetCurrentThreadId(),
            ei,
            FALSE
        };

        auto minidump_write_dump = (decltype(MiniDumpWriteDump)*)GetProcAddress(dbghelp, "MiniDumpWriteDump");

        minidump_write_dump(GetCurrentProcess(), 
            GetCurrentProcessId(),
            f,
            MINIDUMP_TYPE::MiniDumpNormal, 
            &ei_info, 
            nullptr, 
            nullptr
        );
        std::string log = err_msg_buf.str();
        spdlog::error(err_msg_buf.str().c_str());
        char error_message[256];
        ::strncpy(error_message, log.c_str(), sizeof(error_message));
        error_message[255] = '\0';
        MessageBoxA(NULL, error_message, "Caught exception", MB_ICONINFORMATION);
        
        CloseHandle(f);
    } else {
        spdlog::error("Exception occurred, but could not load dbghelp.dll");
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

void reframework::setup_exception_handler() {
    SetUnhandledExceptionFilter(global_exception_handler);
}
