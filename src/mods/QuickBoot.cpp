#if 1
#include "QuickBoot.hpp"
#include "StageWarp.hpp" // Stage list

bool QuickBoot::mod_enabled = false;
const char* QuickBoot::originalBootStage = "STG00011";
int QuickBoot::newBootStageIndex = 0;

std::vector<const char*> QuickBoot::stage_names; // Just names
std::vector<std::string> QuickBoot::stage_display_names; // Names + Descriptions
std::vector<const char*> QuickBoot::stage_display_names_cstr;

std::optional<std::string> QuickBoot::on_initialize() {
    for (const auto& stage : StageWarp::stage_items) {
        stage_names.push_back(stage.name);
    }
    for (const auto& stage : StageWarp::stage_items) {
        stage_display_names.push_back(std::string(stage.name) + " - " + stage.info);
    }
    for (const auto& name : stage_display_names) {
        stage_display_names_cstr.push_back(name.c_str());
    }

    return Mod::on_initialize();
}

void QuickBoot::on_draw_ui() {
    ImGui::Checkbox("Quick Boot", &mod_enabled);
    help_marker("Load into a stage on boot rather than the motel");
    if (mod_enabled) {
        ImGui::Combo("Boot Stage", &newBootStageIndex, stage_display_names_cstr.data(), stage_display_names_cstr.size());
    }
}

// during load
void QuickBoot::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("quick_boot").value_or(false);
    newBootStageIndex = cfg.get<int>("quick_boot_stage").value_or(40);
}
// during save
void QuickBoot::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("quick_boot", mod_enabled);
    cfg.set<int>("quick_boot_stage", newBootStageIndex);
}

// do something every frame
//void QuickBoot::on_frame() {}
// will show up in debug window, dump ImGui widgets you want here
//void QuickBoot::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
