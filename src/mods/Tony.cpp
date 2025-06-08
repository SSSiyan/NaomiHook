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

void Tony::on_frame() {
    if (!mod_enabled) { return; }
    auto now = std::chrono::steady_clock::now();
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float fontSize = ImGui::GetFontSize();
    static constexpr float displayDuration = 2.0f;
        
    struct ConsecutiveGroup {
        std::string trickName;
        int money;
        bool isReward;
        int count;
        std::chrono::steady_clock::time_point mostRecentTime;
        std::chrono::steady_clock::time_point firstAppearanceTime;
        int startIndex;
        int endIndex;
        bool isNew;
    };
        
    std::vector<ConsecutiveGroup> consecutiveGroups;
        
    if (!trickScores.empty()) {
        ConsecutiveGroup currentGroup;
        currentGroup.trickName = trickScores[0].text;
        currentGroup.money = trickScores[0].money;
        currentGroup.isReward = trickScores[0].isReward;
        currentGroup.count = 1;
        currentGroup.mostRecentTime = trickScores[0].timePerformed;
        currentGroup.firstAppearanceTime = trickScores[0].timePerformed;
        currentGroup.startIndex = 0;
        currentGroup.endIndex = 0;
        currentGroup.isNew = false;
            
        for (uint32_t i = 1; i < trickScores.size(); i++) {
            if (trickScores[i].text == currentGroup.trickName) {
                // same trick, extend the current group
                currentGroup.count++;
                currentGroup.money += trickScores[i].money;
                currentGroup.isReward = trickScores[i].isReward;
                currentGroup.endIndex = i;
                if (trickScores[i].timePerformed > currentGroup.mostRecentTime) {
                    currentGroup.mostRecentTime = trickScores[i].timePerformed;
                }
            } else {
                // different trick, save current group and start a new one
                consecutiveGroups.push_back(currentGroup);
                    
                currentGroup.trickName = trickScores[i].text;
                currentGroup.money = trickScores[i].money;
                currentGroup.isReward = trickScores[i].isReward;
                currentGroup.count = 1;
                currentGroup.mostRecentTime = trickScores[i].timePerformed;
                currentGroup.firstAppearanceTime = trickScores[i].timePerformed;
                currentGroup.startIndex = i;
                currentGroup.endIndex = i;
                currentGroup.isNew = false;
            }
        }
            
        // add the last group
        consecutiveGroups.push_back(currentGroup);
    }
        
    // check which groups have timed out
    std::vector<ConsecutiveGroup> activeGroups;
    std::vector<int> trickIndicesToRemove;
        
    for (auto& group : consecutiveGroups) {
        float elapsed = std::chrono::duration<float>(now - group.mostRecentTime).count();
        bool timedOut = (elapsed > displayDuration);
        group.isNew = (group.count == 1);
            
        if (timedOut) {
            // Mark all tricks in this group for removal
            for (int i = group.startIndex; i <= group.endIndex; i++) {
                trickIndicesToRemove.push_back(i);
            }
        } else {
            activeGroups.push_back(group);
        }
    }
        
    // sort active groups by how recent they were
    std::sort(activeGroups.begin(), activeGroups.end(), 
        [](const ConsecutiveGroup& a, const ConsecutiveGroup& b) {
            return a.mostRecentTime > b.mostRecentTime;
        });
    
    ImGui::SetNextWindowPos(ImVec2(0.0f, screenSize.y * 0.3f));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.3f, screenSize.y * 0.3f));
    ImGui::Begin("TrickScoresWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(1.5f);
        
    // display active groups
    for (size_t groupIndex = 0; groupIndex < activeGroups.size(); ++groupIndex) {
        const auto& group = activeGroups[groupIndex];
        float elapsed = std::chrono::duration<float>(now - group.mostRecentTime).count();
        float animationOffset = 0.0f;
        
        if (group.isNew) {
            float timeSinceFirstAppearance = std::chrono::duration<float>(now - group.firstAppearanceTime).count();
            float animationDuration = 0.2f;
            float progress = timeSinceFirstAppearance / animationDuration;
            progress = std::max(0.0f, std::min(1.0f, progress));
            
            progress = 1.0f - std::pow(1.0f - progress, 3.0f);
            
            animationOffset = -screenSize.x * (1.0f - progress);
        } else {
            float slideOutStartTime = displayDuration - 0.3f;
            if (elapsed > slideOutStartTime) {
                float slideOutDuration = 0.3f;
                float slideOutProgress = (elapsed - slideOutStartTime) / slideOutDuration;
                slideOutProgress = std::max(0.0f, std::min(1.0f, slideOutProgress));
                
                slideOutProgress = slideOutProgress * slideOutProgress * slideOutProgress;
                
                animationOffset = -screenSize.x * slideOutProgress;
            }
        }
            
        ImVec4 color1(1.0f, 1.0f, 1.0f, 1.0f); // white
        ImVec4 color2(1.0f, 1.0f, 0.0f, 1.0f); // yellow
            
        std::string displayText = group.trickName;
        if (group.count > 1) {
            displayText += " x" + std::to_string(group.count);
        }
            
        float windowWidth = ImGui::GetContentRegionAvail().x;
        char scoreBuffer[32];
        snprintf(scoreBuffer, sizeof(scoreBuffer), "%i", group.money);
        float textWidth = ImGui::CalcTextSize(displayText.c_str()).x;
        float scoreWidth = ImGui::CalcTextSize(scoreBuffer).x;
        float customSpacing = fontSize * 1.0f;
        
        float targetX = screenSize.x * 0.01f;
        float leftAlignX = targetX + animationOffset;
        float scoreX = targetX + textWidth + customSpacing + animationOffset;
        
        // draw trick name (left-aligned)
        ImGui::SetCursorPosX(leftAlignX);
        if (!group.isReward) ImGui::PushStyleColor(ImGuiCol_Text, color1);
        else ImGui::PushStyleColor(ImGuiCol_Text, color2);
        ImGui::Text("%s", displayText.c_str());
        ImGui::PopStyleColor();
            
        // draw score
        ImGui::SameLine(scoreX);
        ImGui::PushStyleColor(ImGuiCol_Text, color2);
        if (!group.isReward) ImGui::Text("+%i", group.money);
        ImGui::PopStyleColor();
    }
        
    // sort to remove in descending order
    std::sort(trickIndicesToRemove.begin(), trickIndicesToRemove.end(), std::greater<int>());
        
    // remove timed-out tricks
    for (uint32_t index : trickIndicesToRemove) {
        if (index >= 0 && index < trickScores.size()) {
            trickScores.erase(trickScores.begin() + index);
        }
    }
        
    ImGui::End();
}

static void AddTrickScore(int id, int money, bool isReward) {
    TrickScore newScore;
    newScore.text = isReward ? "Kill Reward" : MoveNames[id];
    newScore.money = money;
    newScore.isReward = isReward;
    newScore.timePerformed = std::chrono::steady_clock::now();

    if (trickScores.size() >= maxScores) {
        trickScores.pop_back();
    }
    trickScores.insert(trickScores.begin(), newScore);
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
