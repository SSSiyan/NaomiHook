#pragma once

// Game can't use virtual keys unless the menu is open.
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include "utility/Patch.hpp"

#include <imgui/imgui.h>

#include "sdk/ReClass.hpp"
#include "sdk/nmh.hpp"
#include "utility/Config.hpp"

#include "ModFramework.hpp"

#define naked static __declspec(naked)

class Patch;
class FunctionHook;

enum class ModCategory {
    NONE,
    GAMEPLAY,
    COSMETICS,
    STAGE,
    SOUND,
    SYSTEM,
    MAX
};

class IModValue {
public:
    using Ptr = std::unique_ptr<IModValue>;

    virtual ~IModValue() {};
    virtual bool draw(std::string_view name) = 0;
    virtual void draw_value(std::string_view name) = 0;
    virtual void config_load(const utility::Config& cfg) = 0;
    virtual void config_save(utility::Config& cfg) = 0;
};

// Convenience classes for imgui
template <typename T>
class ModValue : public IModValue {
public:
    using Ptr = std::unique_ptr<ModValue<T>>;

    static auto create(std::string_view config_name, T default_value = T{}) {
        return std::make_unique<ModValue<T>>(config_name, default_value);
    }

    ModValue(std::string_view config_name, T default_value) 
        : m_config_name{ config_name },
        m_value{ default_value }
    {
    }

    virtual ~ModValue() override {};

    virtual void config_load(const utility::Config& cfg) override {
        auto v = cfg.get<T>(m_config_name);

        if (v) {
            m_value = *v;
        }
    };

    virtual void config_save(utility::Config& cfg) override {
        cfg.set<T>(m_config_name, m_value);
    };

    operator T&() {
        return m_value;
    }

    T& value() {
        return m_value;
    }

    const auto& get_config_name() const {
        return m_config_name;
    }

protected:
    T m_value{};
    std::string m_config_name{ "Default_ModValue" };
};

class ModToggle : public ModValue<bool> {
public:
    using Ptr = std::unique_ptr<ModToggle>;

    ModToggle(std::string_view config_name, bool default_value) 
        : ModValue<bool>{ config_name, default_value } 
    { 
    }

    static auto create(std::string_view config_name, bool default_value = false) {
        return std::make_unique<ModToggle>(config_name, default_value);
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::Checkbox(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), m_value);
    }

    bool toggle() {
        return m_value = !m_value;
    }
};

class ModFloat : public ModValue<float> {
public:
    using Ptr = std::unique_ptr<ModFloat>;

    ModFloat(std::string_view config_name, float default_value) 
        : ModValue<float>{ config_name, default_value } { }

    static auto create(std::string_view config_name, float default_value = 0.0f) {
        return std::make_unique<ModFloat>(config_name, default_value);
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::InputFloat(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        ImGui::Text("%s: %f", name.data(), m_value);
    }
};

class ModSlider : public ModFloat {
public:
    using Ptr = std::unique_ptr<ModSlider>;

    static auto create(std::string_view config_name, float mn = 0.0f, float mx = 1.0f, float default_value = 0.0f) {
        return std::make_unique<ModSlider>(config_name, mn, mx, default_value);
    }

    ModSlider(std::string_view config_name, float mn = 0.0f, float mx = 1.0f, float default_value = 0.0f)
        : ModFloat{ config_name, default_value },
        m_range{ mn, mx }
    {
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::SliderFloat(name.data(), &m_value, m_range.x, m_range.y);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        ImGui::Text("%s: %f [%f, %f]", name.data(), m_value, m_range.x, m_range.y);
    }

    auto& range() {
        return m_range;
    }

protected:
    Vector2f m_range{ 0.0f, 1.0f };
};

class ModInt32 : public ModValue<int32_t> {
public:
    using Ptr = std::unique_ptr<ModInt32>;

    static auto create(std::string_view config_name, int32_t default_value = 0) {
        return std::make_unique<ModInt32>(config_name, default_value);
    }

    ModInt32(std::string_view config_name, int32_t default_value = 0)
        : ModValue{ config_name, static_cast<int>(default_value) }
    {
    }

    bool draw(std::string_view name) override {
        ImGui::PushID(this);
        auto ret = ImGui::InputInt(name.data(), &m_value);
        ImGui::PopID();

        return ret;
    }

    void draw_value(std::string_view name) override {
        ImGui::Text("%s: %i", name.data(), m_value);
    }
};

class ModKey: public ModInt32 {
public:
    using Ptr = std::unique_ptr<ModKey>;

    static auto create(std::string_view config_name, ImGuiKey default_value = ImGuiKey_None) {
        return std::make_unique<ModKey>(config_name, default_value);
    }

    ModKey(std::string_view config_name, ImGuiKey default_value = UNBOUND_KEY)
        : ModInt32 { config_name, default_value }
    {
    }

    bool draw(std::string_view name) override {
        if (name.empty()) {
            return false;
        }

        ImGui::PushID(this);
        ImGui::Button(name.data());

        if (ImGui::IsItemHovered()) {
#if 0
            auto& keys = g_framework->get_keyboard_state();

            for (auto k = 0; k < keys.size(); ++k) {
                if (keys[k]) {
                    m_value = is_erase_key(k) ? UNBOUND_KEY : k;
                    break;
                }
            }
#endif
            for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; ++k) {
                const ImGuiKey key = (ImGuiKey)k;
                if (ImGui::IsKeyPressed(key)) {
                    m_value = is_erase_key(key) ? UNBOUND_KEY : key;
                    break;
                }
            }

            ImGui::SameLine();
            ImGui::Text("Press any key...");
            ImGui::Text("ESCAPE or BACKSPACE to cancel");
        }
        else {
            ImGui::SameLine();

            if (m_value >= ImGuiKey_NamedKey_BEGIN && m_value <= ImGuiKey_NamedKey_END) {
                ImGui::Text("%i", m_value);
            }
            else {
                ImGui::Text("Not bound");
            }
        }

        ImGui::PopID();

        return true;
    }

    bool is_key_down() const {
        return ImGui::IsKeyDown((ImGuiKey)m_value);
    }

    bool is_key_down_once() {
        return ImGui::IsKeyPressed((ImGuiKey)m_value);
    }

    bool is_erase_key(ImGuiKey k) const {
        switch (k) {
        case ImGuiKey_Backspace:
        case ImGuiKey_Escape:
            return true;

        default:
            return false;
        }
    }

    static constexpr ImGuiKey UNBOUND_KEY = ImGuiKey_None;

};

class Mod {
public:
    using ValueList = std::vector<std::reference_wrapper<IModValue>>;

public:
    virtual ~Mod() {};
    virtual std::string get_mod_name() const { return "UnknownMod"; };
    virtual std::string get_human_readable_name() const { return "Unknown Mod"; }
    virtual const char* get_description() const { return R"(Unknown Description)"; };
    virtual void render_description() const {
        ImGui::TextWrapped(get_description());
    }

    // can be used for ModValues, like Mod_ValueName
    virtual std::string generate_name(std::string_view name) { return std::string{ get_mod_name() } + "_" + name.data(); }

    // Called when ModFramework::initialize finishes in the first render frame
    // Returns an error string if it fails
    virtual std::optional<std::string> on_initialize() { return std::nullopt; };

    // Functionally equivalent, but on_frame will always get called, on_draw_ui can be disabled by ModFramework
    virtual void on_frame() {};
    virtual void on_draw_ui() {};
	virtual void on_draw_debug_ui() {};
    virtual void on_d3d_reset() {};

    virtual ModCategory get_category() { return ModCategory::NONE; };

    virtual void on_config_load(const utility::Config& cfg) {};
    virtual void on_config_save(utility::Config& cfg) {};

    void install_patch_absolute(uintptr_t location, std::unique_ptr<Patch>& patch, const char* patch_bytes, uint8_t length);
    void install_patch_offset(ptrdiff_t offset, std::unique_ptr<Patch>& patch, const char* patch_bytes, uint8_t length);
    bool install_hook_offset(ptrdiff_t offset, std::unique_ptr<FunctionHook>& hook, void* detour, uintptr_t* ret, ptrdiff_t next_instruction_offset = 0);
    bool install_hook_absolute(uintptr_t location, std::unique_ptr<FunctionHook>& hook, void* detour, uintptr_t* ret, ptrdiff_t next_instruction_offset = 0);

    // Game-specific callbacks
    /*
    virtual void on_pre_update_transform(RETransform* transform) {};
    virtual void on_update_transform(RETransform* transform) {};
    virtual void on_pre_update_camera_controller(RopewayPlayerCameraController* controller) {};
    virtual void on_update_camera_controller(RopewayPlayerCameraController* controller) {};
    virtual void on_pre_update_camera_controller2(RopewayPlayerCameraController* controller) {};
    virtual void on_update_camera_controller2(RopewayPlayerCameraController* controller) {};
    */
};

inline void help_marker(const char* desc) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

