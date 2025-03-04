#include "ResolutionScaleFix.hpp"

// convert member fn pointer to void*
template<typename T>
void* addressof_memberfn(T fn) {
    return reinterpret_cast<void*&>(fn);
}

/*
.text:0060EFD0 ??0ApplicationWindows@EE@@QAE@PAVPlatformWindows@1@_NPBDPAUHICON__@@31H@Z proc near
.text:0060EFD0                                         ; CODE XREF: RealMain(HINSTANCE__ *,HINSTANCE__ *,char *,int)+3FC↑p
.text:0060EFD0
.text:0060EFD0 var_80          = dword ptr -80h
.text:0060EFD0 var_7C          = dword ptr -7Ch
.text:0060EFD0 var_78          = tagRECT ptr -78h
.text:0060EFD0 Rect            = tagRECT ptr -68h
.text:0060EFD0 rc              = tagRECT ptr -58h
.text:0060EFD0 ps              = tagPAINTSTRUCT ptr -48h
.text:0060EFD0 var_4           = dword ptr -4
.text:0060EFD0 platform        = dword ptr  8
.text:0060EFD0 icon            = dword ptr  14h
.text:0060EFD0 cursor          = dword ptr  18h
.text:0060EFD0 title           = byte ptr  1Ch
.text:0060EFD0
.text:0060EFD0 this = ecx
*/

// hate hooking __thiscall, simple as
static std::unique_ptr<FunctionHook> g_ee_app_constructor_hook {};
struct EE_ApplicationWindows_ApplicationWindows {

public:
    char pad_0000[240]; //0x0000
    uint32_t minWindowSizeX; //0x00F0
    uint32_t minWindowSizeY; //0x00F4

    // actually is a constructor, but cepples or msvc prohibits taking address of constructors for some reason
    void ass(void* platform,
        bool icon,
        const char *cursor,
        HICON__ *icona,
        HICON__ *multiThreadedInterface,
        bool title,
        int a8) {

        EE_ApplicationWindows_ApplicationWindows& pthis = *this;

        BOOL dpi_success = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        assert(dpi_success);

#if 1
        // lmao
        using MemberFunctionPtr =
            void (EE_ApplicationWindows_ApplicationWindows::*)(void*, bool, const char*, HICON__*, HICON__*, bool, int);
        uintptr_t orig = g_ee_app_constructor_hook->get_original();
        MemberFunctionPtr orig_ = *(MemberFunctionPtr*)&orig;
        (this->*orig_)(platform, icon, cursor, icona, multiThreadedInterface, title, a8);
#endif
    };
};

std::optional<std::string> ResolutionScaleFix::on_initialize() {
    using member_function_ptr = void(EE_ApplicationWindows_ApplicationWindows::*)(void*, bool, const char*, HICON__*, HICON__*, bool, int);
    auto pvoid = addressof_memberfn(&EE_ApplicationWindows_ApplicationWindows::ass);

#if 1 // dpi fix
    g_ee_app_constructor_hook = std::make_unique<FunctionHook>(0x0060EFD0, pvoid);
    if (!g_ee_app_constructor_hook->create()) {
        return "failed to initialize dpi hook";
    }
#endif
#if 0 // d3d11 bb size
    if (!install_hook_absolute(0x0060ADB5, m_hook, &detour, &jump_back, 6)) {
        return "failed to initialize dpi hook";
    }
#endif
    return Mod::on_initialize();
}
