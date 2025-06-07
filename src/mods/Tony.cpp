#include "Tony.hpp"
#if 1
bool Tony::mod_enabled = false;
uintptr_t Tony::jmp_ret1 = NULL;
static constexpr float displayDuration = 1.0f;

struct TrickScore {
    std::string text;
    float score;
    std::chrono::steady_clock::time_point timePerformed;

    TrickScore()
        : text(""),
        score(0.0f),
        timePerformed(std::chrono::steady_clock::now()) {
    }

    TrickScore(const std::string& text, float score, float time)
        : text(text),
        score(score),
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
        
    struct ConsecutiveGroup {
        std::string trickName;
        float score;
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
        currentGroup.score = trickScores[0].score;
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
                currentGroup.endIndex = i;
                if (trickScores[i].timePerformed > currentGroup.mostRecentTime) {
                    currentGroup.mostRecentTime = trickScores[i].timePerformed;
                }
            } else {
                // different trick, save current group and start a new one
                consecutiveGroups.push_back(currentGroup);
                    
                currentGroup.trickName = trickScores[i].text;
                currentGroup.score = trickScores[i].score;
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
        
        // check if this is a new entry (appeared within the last 0.5 seconds)
        float timeSinceFirstAppearance = std::chrono::duration<float>(now - group.firstAppearanceTime).count();
        group.isNew = (timeSinceFirstAppearance < 0.5f);
            
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
    
    ImGui::SetNextWindowPos(ImVec2(0.0f, screenSize.y * 0.4f));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.3f, screenSize.y * 0.3f));
    ImGui::Begin("TrickScoresWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(1.5f);
        
    // display active groups with individual entry animation
    for (size_t groupIndex = 0; groupIndex < activeGroups.size(); ++groupIndex) {
        const auto& group = activeGroups[groupIndex];
        float elapsed = std::chrono::duration<float>(now - group.mostRecentTime).count();
        float fade = 1.0f - (elapsed / displayDuration);
        fade = std::max(0.0f, std::min(1.0f, fade)); // Clamp fade between 0 and 1
        
        // animation
        float entryFlyInOffset = 0.0f;
        if (group.isNew) {
            float timeSinceFirstAppearance = std::chrono::duration<float>(now - group.firstAppearanceTime).count();
            float animationDuration = 0.2f;
            float progress = timeSinceFirstAppearance / animationDuration;
            progress = std::max(0.0f, std::min(1.0f, progress));
            
            progress = 1.0f - std::pow(1.0f - progress, 3.0f);
            
            entryFlyInOffset = screenSize.x * (1.0f - progress);
        }
            
        ImVec4 color(1.0f, 1.0f, 1.0f, fade);
        ImVec4 color2(1.0f, 1.0f, 0.0f, fade);
            
        std::string displayText = group.trickName;
        if (group.count > 1) {
            displayText += " x" + std::to_string(group.count);
        }
            
        float windowWidth = ImGui::GetContentRegionAvail().x;
        char scoreBuffer[32];
        snprintf(scoreBuffer, sizeof(scoreBuffer), "%.1f", group.score);
        float textWidth = ImGui::CalcTextSize(displayText.c_str()).x;
        float scoreWidth = ImGui::CalcTextSize(scoreBuffer).x;
        float customSpacing = fontSize * 1.0f;
        
        float targetX = screenSize.x * 0.01f;
        float leftAlignX = targetX - entryFlyInOffset; // Start offscreen, end at target
        float scoreX = targetX + textWidth + customSpacing - entryFlyInOffset; // Score positioned after text
        
        // draw trick name (left-aligned)
        ImGui::SetCursorPosX(leftAlignX);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::Text("%s", displayText.c_str());
        ImGui::PopStyleColor();
            
        // draw score
        ImGui::SameLine(scoreX);
        ImGui::PushStyleColor(ImGuiCol_Text, color2);
        ImGui::Text("+%.1f", group.score);
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

static void AddTrickScore(int id, float score) {
    TrickScore newScore;
    newScore.text = MoveNames[id];
    newScore.score = score;
    newScore.timePerformed = std::chrono::steady_clock::now();

    if (trickScores.size() >= maxScores) {
        trickScores.pop_back();
    }
    trickScores.insert(trickScores.begin(), newScore);
}

// clang-format off
naked void detour1() { // basic attacks // player in edi
    __asm {
        // 
            cmp byte ptr [Tony::mod_enabled], 0
            je originalcode

            pushad
            push [esp+0x20+0xC] // damageAmount
            push ecx // damageID
            call AddTrickScore
            add esp, 8
            popad

        originalcode:
            cmp ecx,0x000000AB
            jmp dword ptr [Tony::jmp_ret1]
    }
}
 
 // clang-format on

void Tony::on_draw_ui() {
    ImGui::Checkbox("Tony", &mod_enabled);
}

std::optional<std::string> Tony::on_initialize() {
    if (!install_hook_offset(0x3CB850, m_hook1, &detour1, &Tony::jmp_ret1, 6)) {
        spdlog::error("Failed to init Tony mod\n");
        return "Failed to init Tony mod";
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
