#include "Tony.hpp"
#include "GuiFunctions.hpp"
#if 1

static constexpr float DISPLAY_DURATION = 2.0f;
static constexpr float SLIDE_OUT_DURATION = 0.3f;
static constexpr float ANIMATION_DURATION = 0.2f;
static constexpr float SCREEN_WIDTH_RATIO = 0.3f;
static constexpr float REWARD_WINDOW_HEIGHT_RATIO = 0.15f;
static constexpr float TRICK_WINDOW_HEIGHT_RATIO = 0.3f;
static constexpr float REWARD_START_Y_RATIO = 0.25f;
static constexpr float TRICK_START_Y_RATIO = 0.35f;
static constexpr float TEXT_SPACING_RATIO = 0.5f;
static constexpr float SCREEN_MARGIN_RATIO = 0.01f;
static constexpr float LINE_HEIGHT_MULTIPLIER = 1.8f;

bool Tony::mod_enabled = false;
uintptr_t Tony::jmp_ret4 = NULL;
uintptr_t Tony::jmp_ret5 = NULL;

static std::chrono::steady_clock::time_point lastRealTime = std::chrono::steady_clock::now();
static std::chrono::duration<float> accumulatedGameTime = std::chrono::duration<float>::zero();
static bool wasGamePaused = false;

static float getGameTimeSeconds() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player) { 
        return accumulatedGameTime.count(); 
    }
    
    bool isPaused = player->mInputMode == 6;
    auto currentRealTime = std::chrono::steady_clock::now();
    
    if (!isPaused) {
        if (wasGamePaused) {
            lastRealTime = currentRealTime;
            wasGamePaused = false;
        } else {
            accumulatedGameTime += std::chrono::duration_cast<std::chrono::duration<float>>(currentRealTime - lastRealTime);
            lastRealTime = currentRealTime;
        }
    } else {
        wasGamePaused = true;
    }
    
    return accumulatedGameTime.count();
}

struct TrickGroup {
    std::string trickName;
    int money;
    bool isReward;
    int count;
    float mostRecentTime;
    float firstAppearanceTime;
    bool isNew;
    float timeRemainingWhenPaused;
    float animationTimeRemainingWhenPaused;
    bool wasSlideOutActive;
    float slideOutProgressWhenPaused;
    
    TrickGroup() = default;
    
    TrickGroup(const std::string& name, int moneyAmount, bool reward, float currentTime)
        : trickName(name), money(moneyAmount), isReward(reward), count(1),
          mostRecentTime(currentTime), firstAppearanceTime(currentTime), isNew(true),
          timeRemainingWhenPaused(DISPLAY_DURATION), animationTimeRemainingWhenPaused(ANIMATION_DURATION),
          wasSlideOutActive(false), slideOutProgressWhenPaused(0.0f) {}
};

static std::vector<TrickGroup> trickGroups;

static bool IsGamePaused() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    return player && player->mInputMode == 6;
}

static void UpdateTimestampsOnUnpause() {
    float now = getGameTimeSeconds();
    
    for (auto& group : trickGroups) {
        bool wasSlideOutActive = (group.timeRemainingWhenPaused < 0.0f);
        
        if (wasSlideOutActive) {
            float slideOutProgress = std::min(1.0f, -group.timeRemainingWhenPaused / SLIDE_OUT_DURATION);
            float remainingSlideOut = SLIDE_OUT_DURATION * (1.0f - slideOutProgress);
            float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
            group.mostRecentTime = now - slideOutStartTime - (SLIDE_OUT_DURATION - remainingSlideOut);
        } else {
            float desiredRemainingTime = std::max(0.0f, group.timeRemainingWhenPaused);
            group.mostRecentTime = now - (DISPLAY_DURATION - desiredRemainingTime);
        }
        
        if (group.isNew) {
            float desiredAnimationRemainingTime = group.animationTimeRemainingWhenPaused;
            group.firstAppearanceTime = now - (ANIMATION_DURATION - desiredAnimationRemainingTime);
        }
    }
}

static void StoreRemainingTimeOnPause() {
    float now = getGameTimeSeconds();
    
    for (auto& group : trickGroups) {
        float elapsed = now - group.mostRecentTime;
        group.timeRemainingWhenPaused = DISPLAY_DURATION - elapsed;
        
        if (group.isNew) {
            float animationElapsed = now - group.firstAppearanceTime;
            group.animationTimeRemainingWhenPaused = std::max(0.0f, ANIMATION_DURATION - animationElapsed);
        }
        
        float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
        if (elapsed > slideOutStartTime) {
            group.wasSlideOutActive = true;
            float slideOutProgress = (elapsed - slideOutStartTime) / SLIDE_OUT_DURATION;
            group.slideOutProgressWhenPaused = std::max(0.0f, std::min(1.0f, slideOutProgress));
        } else {
            group.wasSlideOutActive = false;
            group.slideOutProgressWhenPaused = 0.0f;
        }
    }
}

static void CleanupExpiredGroups() {
    if (IsGamePaused()) { 
        return; 
    }
    
    float now = getGameTimeSeconds();
    
    trickGroups.erase(
        std::remove_if(trickGroups.begin(), trickGroups.end(),
            [now](const TrickGroup& group) {
                float elapsed = now - group.mostRecentTime;
                return elapsed > DISPLAY_DURATION;
            }),
        trickGroups.end()
    );
}

static float CalculateAnimationOffset(const TrickGroup& group, float now, float screenWidth) {
    float elapsed = now - group.mostRecentTime;
    float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
    
    if (group.isNew) {
        float timeSinceFirstAppearance = now - group.firstAppearanceTime;
        float progress = std::max(0.0f, std::min(1.0f, timeSinceFirstAppearance / ANIMATION_DURATION));
        progress = 1.0f - std::pow(1.0f - progress, 3.0f);
        return -screenWidth * (1.0f - progress);
    }
    else if (elapsed > slideOutStartTime) {
        float slideOutProgress = std::max(0.0f, std::min(1.0f, (elapsed - slideOutStartTime) / SLIDE_OUT_DURATION));
        slideOutProgress = slideOutProgress * slideOutProgress * slideOutProgress;
        return -screenWidth * slideOutProgress;
    }
    
    return 0.0f;
}

static void MarkAnimationComplete(TrickGroup& group, float now) {
    if (group.isNew) {
        float timeSinceFirstAppearance = now - group.firstAppearanceTime;
        if (timeSinceFirstAppearance >= ANIMATION_DURATION) {
            group.isNew = false;
        }
    }
}

static void RenderGroupText(const TrickGroup& group, float animationOffset, float yPos, 
                           float fontSize, float screenWidth, bool isRewardGroup) {
    ImVec4 textColor = isRewardGroup ? ImVec4(1.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 scoreColor(1.0f, 1.0f, 0.0f, 1.0f);
    
    ImVec4 shadowColor(0.0f, 0.0f, 0.0f, 0.7f);
    float shadowOffsetX = 2.0f;
    float shadowOffsetY = 2.0f;
    
    std::string displayText = group.trickName;
    if (!isRewardGroup && group.count > 1) {
        displayText += " x " + std::to_string(group.count);
    }
    
    float textWidth = ImGui::CalcTextSize(displayText.c_str()).x;
    float customSpacing = fontSize * TEXT_SPACING_RATIO;
    float targetX = screenWidth * SCREEN_MARGIN_RATIO;
    float leftAlignX = targetX + animationOffset;
    float scoreX = targetX + textWidth + customSpacing + animationOffset;
    
    // text shadow
    ImGui::SetCursorPos(ImVec2(leftAlignX + shadowOffsetX, yPos + shadowOffsetY));
    ImGui::PushStyleColor(ImGuiCol_Text, shadowColor);
    ImGui::Text("%s", displayText.c_str());
    ImGui::PopStyleColor();
    
    // text
    ImGui::SetCursorPos(ImVec2(leftAlignX, yPos));
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::Text("%s", displayText.c_str());
    ImGui::PopStyleColor();
    
    // score shadow
    ImGui::SameLine(scoreX + shadowOffsetX);
    ImGui::SetCursorPosY(yPos + shadowOffsetY);
    ImGui::PushStyleColor(ImGuiCol_Text, shadowColor);
    const char* scorePrefix = isRewardGroup ? " =" : " +";
    ImGui::Text("%s%d", scorePrefix, group.money);
    ImGui::PopStyleColor();
    
    // score
    ImGui::SameLine(scoreX);
    ImGui::SetCursorPosY(yPos);
    ImGui::PushStyleColor(ImGuiCol_Text, scoreColor);
    ImGui::Text("%s%d", scorePrefix, group.money);
    ImGui::PopStyleColor();
}

static void DisplayGroups(const ImVec2& screenSize, float fontSize, float startY, 
                         bool isRewardGroup, const char* windowName, float windowHeightRatio) {
    if (IsGamePaused()) { 
        return; 
    }
    
    float now = getGameTimeSeconds();
    
    std::vector<std::reference_wrapper<TrickGroup>> activeGroups;
    for (auto& group : trickGroups) {
        if (group.isReward == isRewardGroup) {
            activeGroups.emplace_back(group);
        }
    }
    
    if (activeGroups.empty()) { 
        return; 
    }
    
    std::sort(activeGroups.begin(), activeGroups.end(),
        [](const TrickGroup& a, const TrickGroup& b) {
            return a.firstAppearanceTime > b.firstAppearanceTime;
        });
    
    ImGui::SetNextWindowPos(ImVec2(0.0f, startY));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * SCREEN_WIDTH_RATIO, screenSize.y * windowHeightRatio));
    ImGui::Begin(windowName, nullptr, 
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | 
                ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
    ImGui::PushFont(g_framework->get_our_imgui_ctx()->tony_font);
    
    float lineHeight = fontSize * LINE_HEIGHT_MULTIPLIER;
    for (size_t groupIndex = 0; groupIndex < activeGroups.size(); ++groupIndex) {
        TrickGroup& group = activeGroups[groupIndex].get();
        
        float animationOffset = CalculateAnimationOffset(group, now, screenSize.x);
        float yPos = lineHeight * groupIndex;
        
        RenderGroupText(group, animationOffset, yPos, fontSize, screenSize.x, isRewardGroup);
        MarkAnimationComplete(group, now);
    }
    
    ImGui::PopFont();
    ImGui::End();
}

static void AddTrickScore(int id, int money, bool isReward) {
    if (IsGamePaused()) {
        return;
    }
    
    std::string trickName = isReward ? "Kill Reward" : MoveNames[id];
    float now = getGameTimeSeconds();
    
    bool foundExisting = false;
    if (!trickGroups.empty()) {
        for (auto it = trickGroups.rbegin(); it != trickGroups.rend(); ++it) {
            if (it->isReward == isReward && it->trickName == trickName) {
                it->count++;
                it->money += money;
                it->mostRecentTime = now;
                it->isNew = false;
                foundExisting = true;
                break;
            }
        }
    }
    
    if (!foundExisting) {
        TrickGroup newGroup(trickName, money, isReward, now);
        
        if (isReward) {
            trickGroups.push_back(newGroup);
        } else {
            trickGroups.insert(trickGroups.begin(), newGroup);
        }
    }
}

void Tony::on_frame() {
    if (!mod_enabled) { 
        return; 
    }
    
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player) { 
        return; 
    }
    
    bool isPaused = IsGamePaused();
    static bool lastFramePaused = false;
    
    if (!lastFramePaused && isPaused) {
        StoreRemainingTimeOnPause();
    } else if (lastFramePaused && !isPaused) {
        UpdateTimestampsOnUnpause();
    }
    lastFramePaused = isPaused;
    
    if (!isPaused) {
        CleanupExpiredGroups();
    }
    
    bool hasRewards = false;
    bool hasTricks = false;
    for (const auto& group : trickGroups) {
        if (group.isReward) {
            hasRewards = true;
        } else {
            hasTricks = true;
        }
        if (hasRewards && hasTricks) break;
    }
    
    if (!hasRewards && !hasTricks) { 
        return; 
    }
    
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float fontSize = ImGui::GetFontSize();
    float rewardStartY = screenSize.y * REWARD_START_Y_RATIO;
    float trickStartY = screenSize.y * TRICK_START_Y_RATIO;
    
    if (hasRewards) {
        DisplayGroups(screenSize, fontSize, rewardStartY, true, "RewardWindow", REWARD_WINDOW_HEIGHT_RATIO);
    }
    
    if (hasTricks) {
        DisplayGroups(screenSize, fontSize, trickStartY, false, "TrickWindow", TRICK_WINDOW_HEIGHT_RATIO);
    }
}

// clang-format off
naked void detour4() { // +5 money gains // player in edi
    __asm {
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
    if (!install_hook_offset(0x3CB92D, m_hook4, &detour4, &Tony::jmp_ret4, 6)) { // +5 money gains
        spdlog::error("Failed to init Tony mod 4\n");
        return "Failed to init Tony mod 4";
    }

    if (!install_hook_offset(0x3E1CD6, m_hook5, &detour5, &Tony::jmp_ret5, 6)) { // money rewards
        spdlog::error("Failed to init Tony mod 4\n");
        return "Failed to init Tony mod 4";
    }

    return Mod::on_initialize();
}

void Tony::on_config_load(const utility::Config &cfg) {
    mod_enabled = cfg.get<bool>("Tony").value_or(false);
}

void Tony::on_config_save(utility::Config &cfg) {
    cfg.set<bool>("Tony", mod_enabled);
}

#endif