#include <chrono>
#include <windows.h>

#include "ModFramework.hpp"
#include "mods/ResolutionScaleFix.hpp"
#include "mods/ScreenInfo.hpp"

#define DLLPATH "\\\\.\\GLOBALROOT\\SystemRoot\\SysWOW64\\XInput1_4.dll"

#pragma comment(linker, "/EXPORT:DllMain=" DLLPATH ".DllMain,@1")
#pragma comment(linker, "/EXPORT:XInputEnable=" DLLPATH ".XInputEnable,@5")
#pragma comment(linker, "/EXPORT:XInputGetAudioDeviceIds=" DLLPATH ".XInputGetAudioDeviceIds,@10")
#pragma comment(linker, "/EXPORT:XInputGetBatteryInformation=" DLLPATH ".XInputGetBatteryInformation,@7")
#pragma comment(linker, "/EXPORT:XInputGetCapabilities=" DLLPATH ".XInputGetCapabilities,@4")
#pragma comment(linker, "/EXPORT:XInputGetKeystroke=" DLLPATH ".XInputGetKeystroke,@8")
#pragma comment(linker, "/EXPORT:XInputGetState=" DLLPATH ".XInputGetState,@2")
#pragma comment(linker, "/EXPORT:XInputSetState=" DLLPATH ".XInputSetState,@3")
#pragma comment(linker, "/EXPORT:__proxy100=" DLLPATH ".#100,@100,NONAME")
#pragma comment(linker, "/EXPORT:__proxy101=" DLLPATH ".#101,@101,NONAME")
#pragma comment(linker, "/EXPORT:__proxy102=" DLLPATH ".#102,@102,NONAME")
#pragma comment(linker, "/EXPORT:__proxy103=" DLLPATH ".#103,@103,NONAME")
#pragma comment(linker, "/EXPORT:__proxy104=" DLLPATH ".#104,@104,NONAME")
#pragma comment(linker, "/EXPORT:__proxy108=" DLLPATH ".#108,@108,NONAME")


static DWORD WINAPI startup_thread([[maybe_unused]] LPVOID parameter) {

#ifndef NDEBUG
    AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD console_mode{0};
    GetConsoleMode(handle_out, &console_mode);
    console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    console_mode |= DISABLE_NEWLINE_AUTO_RETURN;
    SetConsoleMode(handle_out, console_mode);

    FILE* new_stdin  = nullptr;
    FILE* new_stdout = nullptr;
    FILE* new_stderr = nullptr;

    freopen_s(&new_stdin, "CONIN$", "r", stdin);
    freopen_s(&new_stdout, "CONOUT$", "w", stdout);
    freopen_s(&new_stderr, "CONOUT$", "w", stderr);
#endif

    g_framework = std::make_unique<ModFramework>();

    return ERROR_SUCCESS;
}

HMODULE g_nmhfix_handle {NULL};
Mod* g_dpi_fix_mod {nullptr};
Mod* g_screen_info_mod {nullptr};

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
#ifndef NDEBUG
        MessageBox(NULL, "Debug attach opportunity", "NMH1", MB_ICONINFORMATION);
#endif
        // Load NMHFix if it's not already
        HMODULE nmhfix_handle = GetModuleHandleA("NMHFix");
        if(!nmhfix_handle) {
            g_nmhfix_handle = LoadLibraryA("NMHFix.asi");
        }
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, nullptr, 0, nullptr);
        g_dpi_fix_mod = new ResolutionScaleFix();
        auto maybe_error = g_dpi_fix_mod->on_initialize();
        if (maybe_error.has_value()) {
            char buffer[512];
            sprintf(buffer, "Failed to initialize ResoultionScaleFix: %s", maybe_error.value().c_str() );
            MessageBox(NULL, buffer, "NMH1", MB_ICONINFORMATION);
        }
        g_screen_info_mod = new ScreenInfo();
        maybe_error = g_screen_info_mod->on_initialize();
        if (maybe_error.has_value()) {
            char buffer[512];
            sprintf(buffer, "Failed to initialize ScreenInfo: %s", maybe_error.value().c_str() );
            MessageBox(NULL, buffer, "NMH1", MB_ICONINFORMATION);
        }
    }
    if (reason == DLL_PROCESS_DETACH) {
        FreeLibrary(g_nmhfix_handle);
        delete g_dpi_fix_mod;
        delete g_screen_info_mod;
    }
    return TRUE;
}
