#include "Mod.hpp"
#include "spdlog/spdlog.h"
#include "utility/Patch.hpp"
#include "utility/FunctionHook.hpp"

void Mod::install_patch_absolute(uintptr_t location, std::unique_ptr<Patch>& patch, const char* patch_bytes, uint8_t length) {
    spdlog::info("{}: Installing patch at {:x}.\n", get_mod_name().c_str(), location);
    patch.reset(nullptr);
    std::vector<int16_t> bytes;
    while (length > 0) {
        bytes.push_back((short)(*patch_bytes) & 0x00FF);
        patch_bytes++;
        length--;
    }
    patch = Patch::create(location, bytes, true);
}

void Mod::install_patch_offset(ptrdiff_t offset, std::unique_ptr<Patch>& patch, const char* patch_bytes, uint8_t length) {
    uintptr_t base     = g_framework->get_module().as<uintptr_t>();
    uintptr_t location = base + offset;

    spdlog::info("{}: Installing patch at {:x}.\n", get_mod_name().c_str(), location);
    patch.reset(nullptr);

    std::vector<int16_t> bytes;
    while (length > 0) {
        bytes.push_back((short)(*patch_bytes) & 0x00FF);
        patch_bytes++;
        length--;
    }

    patch = Patch::create(location, bytes, true);
}

bool Mod::install_hook_offset(ptrdiff_t offset, std::unique_ptr<FunctionHook>& hook, void* detour, uintptr_t* ret, ptrdiff_t next_instruction_offset) {
    uintptr_t base = g_framework->get_module().as<uintptr_t>();
    uintptr_t location = base + offset;

    spdlog::info("{}: Installing hook at {:x}.\n", get_mod_name().c_str(), location);
    assert(!hook);

    hook = std::make_unique<FunctionHook>(location, detour);
    if (!hook->create()) {
        spdlog::error("Failed to create hook!");
        return false;
    }

    if (next_instruction_offset) {
        if (ret) {
            *ret = location + next_instruction_offset;
        }
    }
    else {
        if (ret) {
            *ret = hook->get_original();
        }
    }
    return true;
}

bool Mod::install_hook_absolute(uintptr_t location, std::unique_ptr<FunctionHook>& hook, void* detour, uintptr_t* ret, ptrdiff_t next_instruction_offset) {
    assert(!hook);
    spdlog::info("{}: Installing hook at {:x}.\n", get_mod_name().c_str(), location);
    hook = std::make_unique<FunctionHook>(location, detour);
    if (!hook->create()) {
        spdlog::error("Failed to create hook!");
        return false;
    }

    if (next_instruction_offset) {
        if (ret) {
            *ret = location + next_instruction_offset;
        }
    }
    else {
        if (ret) {
            *ret = hook->get_original();
        }
    }
    return true;
}
