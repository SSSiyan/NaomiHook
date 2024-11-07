#include "StanceControl.hpp"
#if 1
bool StanceControl::mod_enabled = false;
bool StanceControl::invert_input = false;
bool StanceControl::invert_mid = false;
bool StanceControl::show_new_ui = false;
bool StanceControl::edit_old_ui = false;
uintptr_t StanceControl::gpPad = NULL;
uintptr_t StanceControl::jmp_ret1 = NULL;

float StanceControl::r2Mult = 127.5f;
float StanceControl::r2Sub = 1.0f;
float StanceControl::highBound = 0.0f;
float StanceControl::lowBound = 0.0f;
float StanceControl::invert = -1.0f;

float StanceControl::r2MultGuard = 255.0f;
float StanceControl::highBoundGuard = 0.0f;
float StanceControl::lowBoundGuard = 0.0f;
float StanceControl::invertGuard = 1.0f;

uintptr_t StanceControl::jmp_ret2 = NULL;
uintptr_t StanceControl::jmp2je = NULL;
bool StanceControl::wasL3PressedLastFrame = false;

uintptr_t StanceControl::jmp_ret3 = NULL;
uintptr_t StanceControl::jmp_jne3 = NULL;
uintptr_t StanceControl::clashing = NULL;

void StanceControl::toggle(bool enable) {
    if (enable) {
        install_patch_offset(0x3DE09F, m_patch1, "\xEB\x10", 2); // jmp nmh.exe+3DE0B1 // disable low stance set
        install_patch_offset(0x3DE067, m_patch2, "\xEB\x0A", 2); // jmp nmh.exe+3DE073 // disable high stance set
        install_patch_offset(0x3D7E48, m_patch3, "\x80\xBE\x50\x13\x00\x00\x01", 7); // cmp byte ptr [esi+00001350],01 // force mid stance
    }
    else {
        install_patch_offset(0x3DE09F, m_patch1, "\x74\x10", 2); // jmp nmh.exe+3DE0B1 // enable low stance set
        install_patch_offset(0x3DE067, m_patch2, "\x74\x0A", 2); // je nmh.exe+3DE073 // enable high stance set
        install_patch_offset(0x3D7E48, m_patch3, "\x80\xBE\x49\x16\x00\x00\x01", 7); // cmp byte ptr [esi+00001649],01 // disable mid stance
    }
} 

void StanceControl::toggle_display_edit(bool enable) {
    if (enable) {
        install_patch_offset(0x409B70, m_patch4, "\x83\xE8\x01", 3); // sub eax,01
    }
    else {
        install_patch_offset(0x409B70, m_patch4, "\x83\xE8\x02", 3); // sub eax,02
    }
} 

// clang-format off
naked void detour1() { // originalcode writes stance blend to 0, we write actual values and set stance using it
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled], 0
        je originalcode
    //
        push eax
        mov eax, [StanceControl::gpPad]
        movss xmm0, [eax+0x64]
        pop eax
        cmp dword ptr [esi+0x18C], 48 // guarding
        je guardMath
        divss xmm0, [StanceControl::r2Mult] // Gamepad reads 0-255
        subss xmm0, [StanceControl::r2Sub] // we need it to read -1 to 1
    contAfterMath:
        cmp byte ptr [StanceControl::invert_input], 0
        je skipInvert
        mulss xmm0, [StanceControl::invert]
    skipInvert:
        comiss xmm0, [StanceControl::highBound]
        ja writeHigh
        comiss xmm0, [StanceControl::lowBound]
        jb writeLow
    writeMid:
        cmp byte ptr [StanceControl::invert_mid], 1
        je writeLowInstead
        mov dword ptr [esi+0x1350], 1 // 2
        jmp originalcode

    writeHigh:
        mov dword ptr [esi+0x1350], 0
        jmp originalcode

    writeLow:
        cmp byte ptr [StanceControl::invert_mid], 1
        je writeMidInstead
        mov dword ptr [esi+0x1350], 2
        jmp originalcode

    writeMidInstead:
        mov dword ptr [esi+0x1350], 1
        jmp originalcode
    writeLowInstead:
        mov dword ptr [esi+0x1350], 2
        jmp originalcode

    originalcode:
        movss [esi+0x0000139C], xmm0
        jmp dword ptr [StanceControl::jmp_ret1]

    guardMath:
        divss xmm0, [StanceControl::r2MultGuard] // Gamepad reads 0-255
        cmp byte ptr [StanceControl::invert_input], 0
        je skipInvert2
        movss xmm3, [StanceControl::invertGuard]
        subss xmm3, xmm0
        movss xmm0, xmm3 // @siy potentially unsafe, I haven't checked xmm3's use
    skipInvert2:
        comiss xmm0, [StanceControl::highBoundGuard]
        ja writeHigh
        comiss xmm0, [StanceControl::lowBoundGuard]
        jb writeLow
        jmp writeMid
    }
}

naked void detour2() { // remap lock on cycle
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled], 0
        je originalcode
    //
        push eax
        push ecx
        mov eax, [StanceControl::gpPad]
        mov al, [eax+0x6] // R3 && L3
        mov cl, [StanceControl::wasL3PressedLastFrame]
        and al, 2
        and cl, 2
        test al, al
        je nopress
        test cl, cl
        jne nopress
        mov [StanceControl::wasL3PressedLastFrame], al
        pop ecx
        pop eax
        jmp retcode

    nopress:
        mov [StanceControl::wasL3PressedLastFrame], al
        pop ecx
        pop eax
    jecode:
        jmp dword ptr [StanceControl::jmp2je]

    originalcode:
        push eax
        mov eax, [StanceControl::gpPad]
        cmp byte ptr [eax+0x1C16], 00
        pop eax
        je jecode
    retcode:
        jmp dword ptr [StanceControl::jmp_ret2]
    }
}

naked void detour3() { // enable guard stance blend unless clashing
    __asm {
    //
        cmp byte ptr [StanceControl::mod_enabled], 0
        je originalcode
    //
        push eax
        push ecx
        push edx
        push -1
        call dword ptr [StanceControl::clashing]
        cmp al, 1
        pop edx
        pop ecx
        pop eax
        jne jnecode
    originalcode:
        cmp dword ptr [esi+0x00002990],03
        jmp dword ptr [StanceControl::jmp_ret3]
    jnecode:
        jmp dword ptr [StanceControl::jmp_jne3]
    }
}
 // clang-format on

void StanceControl::on_draw_ui() {
    if (ImGui::Checkbox("Stance Control on R2", &mod_enabled)) {
        toggle(mod_enabled);
    }
    help_marker("Remaps lock on cycle to R3");
    if (ImGui::SliderFloat("highBound", &StanceControl::highBound, 0.0f, 1.0f)) {
        highBoundGuard = (highBound + 1.0f) / 2.0f;
    }
    help_marker("How far should r2 be pushed to enter high stance\n0.9 default");
    if (ImGui::SliderFloat("lowBound", &StanceControl::lowBound, -1.0f, 0.0f)) {
        lowBoundGuard = (lowBound + 1.0f) / 2.0f;
    }
    help_marker("How little should r2 be pushed to enter low stance\n-0.9 default");
    ImGui::Checkbox("Invert", &StanceControl::invert_input);
    help_marker("Swap low and high");
    ImGui::Checkbox("Invert Mid", &StanceControl::invert_mid);
    help_marker("Swap medium and low");
    ImGui::Checkbox("Show Custom Stance UI", &StanceControl::show_new_ui);
    if (ImGui::Checkbox("Swap Vanilla Mid And Low UI", &StanceControl::edit_old_ui)) {
        toggle_display_edit(edit_old_ui);
    }
}

void TextCentered(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

// do something every frame
void StanceControl::on_frame() {
    if (mHRPc* mHRPc = nmh_sdk::get_mHRPc()) {
        if (mHRPc->mOperate && StanceControl::mod_enabled && show_new_ui) {
            ImVec2 windowPos = ImVec2((ImGui::GetIO().DisplaySize.x * 0.924f), (ImGui::GetIO().DisplaySize.y * 0.2f));
            ImVec2 windowSize = (ImVec2((ImGui::GetIO().DisplaySize.x * 0.05f), (ImGui::GetIO().DisplaySize.y * 0.1f)));
            ImGui::SetNextWindowPos(windowPos);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
            ImGui::Begin("StanceControl", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
            if (ImGui::BeginTable("StanceControlTable", 1, NULL, ImVec2(windowSize.x, windowSize.y / 3.0f))) {
                int pose = mHRPc->mPcStatus.pose;
                ImU32 redBgColor = IM_COL32(255, 0, 0, 255);
                ImU32 defaultBgColor = IM_COL32(0, 0, 0, 255);
                ImGui::TableNextColumn();
                if (pose == 0) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, redBgColor);
                }
                else {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, defaultBgColor);
                }
                TextCentered("HIGH");

                ImGui::TableNextColumn();
                if (pose == 1 && !invert_mid || (pose == 2 && invert_mid)) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, redBgColor);
                }
                else {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, defaultBgColor);
                }
                TextCentered("MID");

                ImGui::TableNextColumn();
                if (pose == 2 && !invert_mid || pose == 1 && invert_mid) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, redBgColor);
                }
                else {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, defaultBgColor);
                }
                TextCentered("LOW");
            }
            ImGui::EndTable();
            ImGui::PopStyleColor();
            ImGui::End();
        }
    }
}

std::optional<std::string> StanceControl::on_initialize() {
    gpPad = g_framework->get_module().as<uintptr_t>() + 0x849D10;
    clashing = g_framework->get_module().as<uintptr_t>() + 0x3DFFC0;
    if (!install_hook_offset(0x3D7D6B, m_hook1, &detour1, &StanceControl::jmp_ret1, 8)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
    }

    jmp2je = g_framework->get_module().as<uintptr_t>() + 0x3C46F8;
    if (!install_hook_offset(0x3C4645, m_hook2, &detour2, &StanceControl::jmp_ret2, 13)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
    }

    jmp_jne3 = g_framework->get_module().as<uintptr_t>() + 0x3D7EC8;
    if (!install_hook_offset(0x3D7EBF, m_hook3, &detour3, &StanceControl::jmp_ret3, 7)) {
        spdlog::error("Failed to init StanceControl mod\n");
        return "Failed to init StanceControl mod";
    }

    return Mod::on_initialize();
}

// during load
void StanceControl::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("stance_control").value_or(false);
    toggle(mod_enabled);
    invert_input = cfg.get<bool>("stance_control_invert").value_or(false);
    show_new_ui = cfg.get<bool>("stance_control_ui").value_or(true);
    edit_old_ui = cfg.get<bool>("stance_control_edit_old_ui").value_or(false);
    toggle_display_edit(edit_old_ui);
    invert_mid = cfg.get<bool>("stance_control_invert_mid").value_or(true);
    highBound = cfg.get<float>("stance_control_high_bound").value_or(0.9f);
    lowBound = cfg.get<float>("stance_control_low_bound").value_or(-0.9f);
    highBoundGuard = (highBound + 1.0f) / 2.0f;
    lowBoundGuard = (lowBound + 1.0f) / 2.0f;
}
// during save
void StanceControl::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("stance_control", mod_enabled);
    cfg.set<bool>("stance_control_invert", invert_input);
    cfg.set<bool>("stance_control_ui", show_new_ui);
    cfg.set<bool>("stance_control_edit_old_ui", edit_old_ui);
    cfg.set<bool>("stance_control_invert_mid", invert_mid);
    cfg.set<float>("stance_control_high_bound", highBound);
    cfg.set<float>("stance_control_low_bound", lowBound);
}

// will show up in debug window, dump ImGui widgets you want here
//void StanceControl::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
