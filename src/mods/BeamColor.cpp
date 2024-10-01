#include "BeamColor.hpp"

// clang-format off
#if 0
static naked void detour() {
	__asm {
		mov dword ptr [BeamColor::variable], rbx
		mov eax, 0xDEADBEEF
		jmp dword ptr [jmp_ret]
	}
}
#endif
// clang-format on

std::optional<std::string> BeamColor::on_initialize() {
  return Mod::on_initialize();
}

void BeamColor::on_draw_ui()
{
    static ImVec4 ass = ImColor(-1);
    ImGui::ColorEdit3("Beam Color", (float*)&ass);
}

// during load
//void BeamColor::on_config_load(const utility::Config &cfg) {}
// during save
//void BeamColor::on_config_save(utility::Config &cfg) {}
// do something every frame
//void BeamColor::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void BeamColor::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
//void BeamColor::on_draw_ui() {}
