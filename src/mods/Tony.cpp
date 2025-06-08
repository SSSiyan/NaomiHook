#include "Tony.hpp"

#if 1
bool Tony::mod_enabled = false;
// uintptr_t Tony::jmp_ret1 = NULL;
// uintptr_t Tony::jmp_ret2 = NULL;
// uintptr_t Tony::jmp_ret3 = NULL;
uintptr_t Tony::jmp_ret4 = NULL;
uintptr_t Tony::jmp_ret5 = NULL;

// static uintptr_t gpBattle = NULL;

struct TrickScore {
    std::string text;
    int money;
    bool isReward;
    std::chrono::steady_clock::time_point timePerformed;

    TrickScore()
        : text(""),
        money(0),
        isReward(false),
        timePerformed(std::chrono::steady_clock::now()) {
    }

    TrickScore(const std::string& text, int money, bool isReward, float time)
        : text(text),
        money(money),
        isReward(isReward),
        timePerformed(std::chrono::steady_clock::now()) {
    }
};
static std::vector<TrickScore>trickScores{};
static constexpr int maxScores = INT_MAX;

struct TrickGroup {
    std::string trickName;
    int money;
    bool isReward;
    int count;
    std::chrono::steady_clock::time_point mostRecentTime;
    std::chrono::steady_clock::time_point firstAppearanceTime;
    bool isNew;
};
static std::vector<TrickGroup> trickGroups;

void Tony::on_frame() {
    if (!mod_enabled) { return; }
    
    auto now = std::chrono::steady_clock::now();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float fontSize = ImGui::GetFontSize();
    static constexpr float displayDuration = 2.0f;
    static constexpr float slideOutDuration = 0.3f;
    std::vector<TrickGroup> activeGroups;
    
    // Create activeGroups vector and clean up expired groups
    for (auto it = trickGroups.begin(); it != trickGroups.end();) {
        float elapsed = std::chrono::duration<float>(now - it->mostRecentTime).count();
        bool timedOut = (elapsed > displayDuration);
        
        if (timedOut) {
            it = trickGroups.erase(it);
        } else {
            activeGroups.push_back(*it);
            ++it;
        }
    }
    
    // sort active groups by how recent they were
    std::sort(activeGroups.begin(), activeGroups.end(), 
        [](const TrickGroup& a, const TrickGroup& b) {
            return a.mostRecentTime > b.mostRecentTime;
        });
    
    if (activeGroups.empty()) { return; } // Nothing to display
    
    ImGui::SetNextWindowPos(ImVec2(0.0f, screenSize.y * 0.3f));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.3f, screenSize.y * 0.3f));
    ImGui::Begin("TrickScoresWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(1.5f);
    
    // Calculate line height with some padding
    float lineHeight = fontSize * 1.5f * 1.2f; // font scale * padding multiplier
    
    // Display all active groups
    for (size_t groupIndex = 0; groupIndex < activeGroups.size(); ++groupIndex) {
        const auto& group = activeGroups[groupIndex];
        float elapsed = std::chrono::duration<float>(now - group.mostRecentTime).count();
        float animationOffset = 0.0f;
        float slideOutStartTime = displayDuration - slideOutDuration;
        
        // Handle slide-in animation for new groups
        if (group.isNew) {
            float timeSinceFirstAppearance = std::chrono::duration<float>(now - group.firstAppearanceTime).count();
            float animationDuration = 0.2f;
            float progress = timeSinceFirstAppearance / animationDuration;
            progress = std::max(0.0f, std::min(1.0f, progress));
            
            progress = 1.0f - std::pow(1.0f - progress, 3.0f);
            
            animationOffset = -screenSize.x * (1.0f - progress);
            
            // reset isNew flag in the original trickGroups after animation completes
            if (progress >= 1.0f) {
                for (auto& originalGroup : trickGroups) {
                    if (originalGroup.trickName == group.trickName && 
                        originalGroup.mostRecentTime == group.mostRecentTime) {
                        originalGroup.isNew = false;
                        break;
                    }
                }
            }
        } 
        else if (elapsed > slideOutStartTime) {
            float slideOutProgress = (elapsed - slideOutStartTime) / slideOutDuration;
            slideOutProgress = std::max(0.0f, std::min(1.0f, slideOutProgress));
            
            slideOutProgress = slideOutProgress * slideOutProgress * slideOutProgress;
            
            animationOffset = -screenSize.x * slideOutProgress;
        }
        
        ImVec4 color1(1.0f, 1.0f, 1.0f, 1.0f); // white
        ImVec4 color2(1.0f, 1.0f, 0.0f, 1.0f); // yellow
        
        std::string displayText = group.trickName;
        if (group.count > 1 && !group.isReward) {
            displayText += "x" + std::to_string(group.count);
        }
        
        char scoreBuffer[32];
        snprintf(scoreBuffer, sizeof(scoreBuffer), "%i", group.money);
        float textWidth = ImGui::CalcTextSize(displayText.c_str()).x;
        float customSpacing = fontSize * 0.5f;
        
        float targetX = screenSize.x * 0.01f;
        float leftAlignX = targetX + animationOffset;
        float scoreX = targetX + textWidth + customSpacing + animationOffset;
        
        float yPos = lineHeight * groupIndex;
        ImGui::SetCursorPos(ImVec2(leftAlignX, yPos));
        
        // Display trick name
        if (!group.isReward) ImGui::PushStyleColor(ImGuiCol_Text, color1);
        else ImGui::PushStyleColor(ImGuiCol_Text, color2);
        ImGui::Text("%s", displayText.c_str());
        ImGui::PopStyleColor();
        
        // Display money gain
        ImGui::SameLine(scoreX);
        ImGui::PushStyleColor(ImGuiCol_Text, color2);
        if (group.isReward) {
            ImGui::Text("=%i", group.money);
        } else {
            ImGui::Text("+%i", group.money);
        }
        ImGui::PopStyleColor();
    }
    
    ImGui::End();
}

static void AddTrickScore(int id, int money, bool isReward) {
    std::string trickName = isReward ? "Kill Reward" : MoveNames[id];
    auto now = std::chrono::steady_clock::now();
    static constexpr float displayDuration = 2.0f;
    
    // First, clean up expired groups
    for (auto it = trickGroups.begin(); it != trickGroups.end();) {
        float elapsed = std::chrono::duration<float>(now - it->mostRecentTime).count();
        bool timedOut = (elapsed > displayDuration);
        
        if (timedOut) {
            it = trickGroups.erase(it);
        } else {
            ++it;
        }
    }
    
    // Look for existing group with the same trick name AND same reward status
    bool foundExisting = false;
    
    if (!trickGroups.empty() && 
        trickGroups[0].trickName == trickName && 
        trickGroups[0].isReward == isReward) {
        
        // Update the most recent group
        trickGroups[0].count++;
        trickGroups[0].money += money;
        trickGroups[0].mostRecentTime = now;
        trickGroups[0].isNew = false;
        foundExisting = true;
    }
    
    if (!foundExisting) {
        // Create new group
        TrickGroup newGroup;
        newGroup.trickName = trickName;
        newGroup.money = money;
        newGroup.isReward = isReward;
        newGroup.count = 1;
        newGroup.mostRecentTime = now;
        newGroup.firstAppearanceTime = now;
        newGroup.isNew = true;
        
        trickGroups.insert(trickGroups.begin(), newGroup);
    }
}

// clang-format off

/*naked void detour1() { // most attacks // player in edi
    __asm {
        // 
            cmp byte ptr [Tony::mod_enabled], 0
            je originalcode

            pushad
            push [esp+0x20+0xC] // damageAmount
            push ecx // moveID
            call AddTrickScore
            add esp, 8
            popad

        originalcode:
            cmp ecx,0x000000AB
            jmp dword ptr [Tony::jmp_ret1]
    }
}

naked void detour2() { // execution qtes // player in edi
    __asm {
        // 
            cmp byte ptr [Tony::mod_enabled], 0
            je originalcode

            pushad
            push 0
            push [edi+0x18C] // moveID
            call AddTrickScore
            add esp, 8
            popad

        originalcode:
            mov edx, 0x00000379
            jmp dword ptr [Tony::jmp_ret2]
    }
}

naked void detour3() { // throws hitting the ground // player in nowhere??
    __asm {
        // original movss
            movss [edi+0x24], xmm0
            cmp byte ptr [Tony::mod_enabled], 0
            je originalcode
            
            pushad

            mov edx, [gpBattle]
            mov edx, [edx]
            mov edx, [edx+0x164]

            push [edi+0x24] // damage
            push [edx+0x18C] // moveID
            call AddTrickScore
            add esp, 8

        popcode:
            popad
        originalcode:
            jmp dword ptr [Tony::jmp_ret3]
    }
}*/

naked void detour4() { // +5 money gains // player in edi
    __asm {
        // 
            cmp byte ptr [Tony::mod_enabled], 0
            je originalcode

            pushad
            push 0 // is reward
            push 5 // money gain
            push [edi+0x18C] // moveID
            call AddTrickScore
            add esp, 0xC
            popad

        originalcode:
            cmp ecx, 0x000000C2
            jmp dword ptr [Tony::jmp_ret4]
    }
}

naked void detour5() { // money rewards // player in edi
    __asm {
        // 
            cmp byte ptr [Tony::mod_enabled], 0
            je originalcode

            pushad
            push 1 // is reward
            push eax // money gained
            push [ecx+0x18C] // moveID
            call AddTrickScore
            add esp, 0xC
            popad

        originalcode:
            add [ecx+0x000012EC], eax
            jmp dword ptr [Tony::jmp_ret5]
    }
}

 // clang-format on

void Tony::on_draw_ui() {
    ImGui::Checkbox("Tony", &mod_enabled);
}

std::optional<std::string> Tony::on_initialize() {
    // gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    /*if (!install_hook_offset(0x3CB850, m_hook1, &detour1, &Tony::jmp_ret1, 6)) { // most hits
        spdlog::error("Failed to init Tony mod\n");
        return "Failed to init Tony mod";
    }

    if (!install_hook_offset(0x3CAFA1, m_hook2, &detour2, &Tony::jmp_ret2, 5)) { // executions
        spdlog::error("Failed to init Tony mod 2\n");
        return "Failed to init Tony mod 2";
    }

    if (!install_hook_offset(0xA4160, m_hook3, &detour3, &Tony::jmp_ret3, 5)) { // throws hitting the ground // doesn't work idk why yet
        spdlog::error("Failed to init Tony mod 3\n");
        return "Failed to init Tony mod 3";
    }*/

    if (!install_hook_offset(0x3CB92D, m_hook4, &detour4, &Tony::jmp_ret4, 6)) { // +5 money gains
        spdlog::error("Failed to init Tony mod 4\n");
        return "Failed to init Tony mod 4";
    }

    if (!install_hook_offset(0x3E1CD6, m_hook5, &detour5, &Tony::jmp_ret5, 6)) { // +5 money gains
        spdlog::error("Failed to init Tony mod 4\n");
        return "Failed to init Tony mod 4";
    }

    return Mod::on_initialize();
}

// during load
void Tony::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("Tony").value_or(false);
}
// during save
void Tony::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("Tony", mod_enabled);
}

// will show up in debug window, dump ImGui widgets you want here
//void Tony::on_draw_debug_ui() {}
// will show up in main window, dump ImGui widgets you want here
#endif
