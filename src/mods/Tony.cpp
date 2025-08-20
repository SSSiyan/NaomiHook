#include "Tony.hpp"
#include "GuiFunctions.hpp"
#include <algorithm>
#include <cmath> // sinf, cosf
#if 1

static constexpr float DISPLAY_DURATION           = 2.0f;
static constexpr float SLIDE_OUT_DURATION         = 0.3f;
static constexpr float ANIMATION_DURATION         = 0.2f;
static constexpr float SCREEN_WIDTH_RATIO         = 0.3f;
static constexpr float REWARD_WINDOW_HEIGHT_RATIO = 0.15f;
static constexpr float TRICK_WINDOW_HEIGHT_RATIO  = 0.3f;
static constexpr float REWARD_START_Y_RATIO       = 0.25f;
static constexpr float TRICK_START_Y_RATIO        = 0.35f;
static constexpr float TEXT_SPACING_RATIO         = 0.5f;
static constexpr float SCREEN_MARGIN_RATIO        = 0.01f;
static constexpr float LINE_HEIGHT_MULTIPLIER     = 2.0f;

// score pop tuning (tricks only)
static constexpr float SCORE_POP_DURATION  = 0.15f; // seconds
static constexpr float SCORE_POP_AMPLITUDE = 0.35f; // base bump

// money chromatic aberration (existing)
static constexpr float CA_DURATION                = 0.0090f;
static constexpr float CA_MAX_OFFSET_F            = 0.6f; // fraction of font size for horiz offset
static constexpr float CA_BASE_ALPHA              = 0.75f;
static constexpr bool CA_TRAILER_STYLE            = true;
static constexpr float CA_TRAILER_OFFSET_MULT     = 1.75f;
static constexpr float CA_TRAILER_ALPHA_MULT      = 1.35f;
static constexpr int CA_TRAILER_EXTRA_PASSES      = 1;
static constexpr float CA_TRAILER_PASS_OFFSET_PX  = 0.5f;
static constexpr float CA_TRAILER_CENTER_NUDGE_PX = 0.6f;

// amount scaling (tricks only)
static constexpr float AMT_MIN_MULT = 0.35f;
static constexpr float AMT_MAX_MULT = 3.00f;

// simple reward count-up tuning
static constexpr float COUNT_RATE_PER_SEC = 200.0f; // numbers per second
static constexpr float COUNT_MIN_DURATION = 0.18f;
static constexpr float COUNT_MAX_DURATION = 0.90f;

// tiny deterministic hash helpers
static inline float sc_fract(float x) {
    return x - std::floor(x);
}
static inline float sc_hash11(float x) {
    return sc_fract(std::sin(x) * 43758.5453f);
}
static inline float sc_hash21(float x, float y) {
    return sc_fract(std::sin(x * 12.9898f + y * 78.233f) * 43758.5453f);
}

// ----------------- DEATH BLOW label CA (now part of Score Visualizer) -----------------
// Always enabled when Score Visualizer is enabled (no public UI toggle).
static constexpr bool DBCA_ENABLED = true;
static float DBCA_DURATION         = 0.22f; // seconds (roughly matches slide-in)
static float DBCA_OFFSET_F         = 0.75f; // fraction of font size for R/B offset
static float DBCA_ALPHA            = 0.90f; // base opacity for RGB fringes
static float DBCA_FADE_POW         = 1.30f; // >1 = faster fade near the end
static int DBCA_EXTRA_PASSES       = 1;     // small trailing copies for punch
static float DBCA_PASS_OFFSET_PX   = 0.50f; // px per extra pass
static float DBCA_CENTER_NUDGE     = 0.40f; // px offset jitter for G layer

// Solo-first pop boost (internal only)
static float DBCA_SOLO_MULT       = 2.00f; // multiplies offset distance
static float DBCA_SOLO_ALPHA_MULT = 1.20f; // multiplies alpha

static inline float clamp01(float v) {
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}
static inline float easeOutCubic(float t) {
    float u = 1.0f - t;
    return 1.0f - u * u * u;
}

bool Tony::mod_enabled    = false;
uintptr_t Tony::jmp_ret1  = NULL;
uintptr_t Tony::jmp_ret2  = NULL;
uintptr_t Tony::jmp_ret3  = NULL;
uintptr_t Tony::jmp_ret4  = NULL;
uintptr_t Tony::jmp_ret5  = NULL;
static uintptr_t gpBattle = NULL;

static std::chrono::steady_clock::time_point lastRealTime = std::chrono::steady_clock::now();
static std::chrono::duration<float> accumulatedGameTime   = std::chrono::duration<float>::zero();
static bool wasGamePaused                                 = false;

static float getGameTimeSeconds() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player)
        return accumulatedGameTime.count();

    bool isPaused        = player->mInputMode == 6;
    auto currentRealTime = std::chrono::steady_clock::now();

    if (!isPaused) {
        if (wasGamePaused) {
            lastRealTime  = currentRealTime;
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

    // tricks: score-pop timestamp
    float scorePopStartTime;

    // reward: simple count-up state
    int rewardStartValue;
    int rewardEndValue;
    float rewardCountStartTime;

    // special flags
    bool isDeathblow = false;

    // DEATH BLOW label CA
    float labelCAStartTime = -1.0f;
    bool dbcaSoloBoost     = false; // true for the very first, solo Death Blow popup only
    float labelCASeed      = 0.0f;  // keeps randomized CA order stable per event

    TrickGroup() = default;

    TrickGroup(const std::string& name, int moneyAmount, bool reward, float currentTime)
        : trickName(name)
        , money(moneyAmount)
        , isReward(reward)
        , count(1)
        , mostRecentTime(currentTime)
        , firstAppearanceTime(currentTime)
        , isNew(true)
        , timeRemainingWhenPaused(DISPLAY_DURATION)
        , animationTimeRemainingWhenPaused(ANIMATION_DURATION)
        , wasSlideOutActive(false)
        , slideOutProgressWhenPaused(0.0f)
        , scorePopStartTime(-1.0f)
        , rewardStartValue(reward ? 0 : 0)
        , rewardEndValue(reward ? moneyAmount : 0)
        , rewardCountStartTime(reward ? currentTime : -1.0f) {}
};

static std::vector<TrickGroup> trickGroups;

// Deathblow detection by motID
static inline bool IsDeathblowID(int id) {
    switch (id) {
    case 227:
    case 236:
    case 244:
    case 245:
    case 278:
    case 287:
    case 288:
    case 289:
    case 324:
    case 333:
    case 338:
    case 339:
    case 370:
    case 378:
    case 384:
    case 385:
        return true;
    default:
        return false;
    }
}

static bool IsGamePaused() {
    mHRPc* player = nmh_sdk::get_mHRPc();
    return player && player->mInputMode == 6;
}

static void UpdateTimestampsOnUnpause() {
    float now = getGameTimeSeconds();

    for (auto& group : trickGroups) {
        bool wasSlideOutActive = (group.timeRemainingWhenPaused < 0.0f);

        if (wasSlideOutActive) {
            float slideOutProgress  = std::min(1.0f, -group.timeRemainingWhenPaused / SLIDE_OUT_DURATION);
            float remainingSlideOut = SLIDE_OUT_DURATION * (1.0f - slideOutProgress);
            float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
            group.mostRecentTime    = now - slideOutStartTime - (SLIDE_OUT_DURATION - remainingSlideOut);
        } else {
            float desiredRemainingTime = std::max(0.0f, group.timeRemainingWhenPaused);
            group.mostRecentTime       = now - (DISPLAY_DURATION - desiredRemainingTime);
        }

        if (group.isNew) {
            float desiredAnimationRemainingTime = group.animationTimeRemainingWhenPaused;
            group.firstAppearanceTime           = now - (ANIMATION_DURATION - desiredAnimationRemainingTime);
        }
    }
}

static void StoreRemainingTimeOnPause() {
    float now = getGameTimeSeconds();

    for (auto& group : trickGroups) {
        float elapsed                 = now - group.mostRecentTime;
        group.timeRemainingWhenPaused = DISPLAY_DURATION - elapsed;

        if (group.isNew) {
            float animationElapsed                 = now - group.firstAppearanceTime;
            group.animationTimeRemainingWhenPaused = std::max(0.0f, ANIMATION_DURATION - animationElapsed);
        }

        float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;
        if (elapsed > slideOutStartTime) {
            group.wasSlideOutActive          = true;
            float slideOutProgress           = (elapsed - slideOutStartTime) / SLIDE_OUT_DURATION;
            group.slideOutProgressWhenPaused = std::max(0.0f, std::min(1.0f, slideOutProgress));
        } else {
            group.wasSlideOutActive          = false;
            group.slideOutProgressWhenPaused = 0.0f;
        }
    }
}

static void CleanupExpiredGroups() {
    if (IsGamePaused())
        return;

    float now = getGameTimeSeconds();
    trickGroups.erase(std::remove_if(trickGroups.begin(), trickGroups.end(),
                          [now](const TrickGroup& group) {
                              float elapsed = now - group.mostRecentTime;
                              return elapsed > DISPLAY_DURATION;
                          }),
        trickGroups.end());
}

static float CalculateAnimationOffset(const TrickGroup& group, float now, float screenWidth) {
    float elapsed           = now - group.mostRecentTime;
    float slideOutStartTime = DISPLAY_DURATION - SLIDE_OUT_DURATION;

    if (group.isNew) {
        float t        = std::max(0.0f, std::min(1.0f, (now - group.firstAppearanceTime) / ANIMATION_DURATION));
        const float c1 = 0.7f, c3 = c1 + 1.0f;
        float t1    = t - 1.0f;
        float eased = 1.0f + c3 * (t1 * t1 * t1) + c1 * (t1 * t1);
        return -screenWidth * (1.0f - eased);
    } else if (elapsed > slideOutStartTime) {
        float slideOutProgress = std::max(0.0f, std::min(1.0f, (elapsed - slideOutStartTime) / SLIDE_OUT_DURATION));
        slideOutProgress       = slideOutProgress * slideOutProgress * slideOutProgress;
        return -screenWidth * slideOutProgress;
    }
    return 0.0f;
}

static void MarkAnimationComplete(TrickGroup& group, float now) {
    if (group.isNew) {
        float timeSinceFirstAppearance = now - group.firstAppearanceTime;
        if (timeSinceFirstAppearance >= ANIMATION_DURATION)
            group.isNew = false;
    }
}

static int RewardDisplayValue(const TrickGroup& group, float now) {
    if (!group.isReward)
        return group.money;

    int start = group.rewardStartValue;
    int end   = group.rewardEndValue;
    float t0  = group.rewardCountStartTime;

    if (t0 < 0.0f || start == end)
        return end;

    int delta = std::abs(end - start);
    float dur = std::min(COUNT_MAX_DURATION, std::max(COUNT_MIN_DURATION, delta / COUNT_RATE_PER_SEC));
    float t   = std::min(1.0f, std::max(0.0f, (getGameTimeSeconds() - t0) / dur));
    float e   = 1.0f - (1.0f - t) * (1.0f - t); // ease-out

    int value = start + static_cast<int>(std::round((end - start) * e));
    if ((end - start) > 0)
        value = std::min(value, end);
    else
        value = std::max(value, end);
    return value;
}

// -------------------- DEATH BLOW label CA renderer --------------------
static void RenderDeathblowLabelCA(const std::string& labelText, float leftAlignX, float yPos, float fontSize, const TrickGroup& group) {
    if (!DBCA_ENABLED)
        return;
    if (!group.isDeathblow)
        return;
    if (group.labelCAStartTime < 0.0f)
        return;

    float now = getGameTimeSeconds();
    float dt  = now - group.labelCAStartTime;
    if (dt < 0.0f || dt > DBCA_DURATION)
        return;

    float t01  = clamp01(dt / DBCA_DURATION);
    float fade = std::pow(1.0f - t01, DBCA_FADE_POW);

    // Solo-first boost only on the first ever appearance (group.dbcaSoloBoost set on creation)
    float boost    = group.dbcaSoloBoost ? DBCA_SOLO_MULT : 1.0f;
    float alphaMul = group.dbcaSoloBoost ? DBCA_SOLO_ALPHA_MULT : 1.0f;

    float px = fontSize * DBCA_OFFSET_F * easeOutCubic(1.0f - t01) * boost;

    // Slight center jitter to keep it organic but deterministic per event
    float seed = (group.labelCASeed != 0.0f) ? group.labelCASeed : (float)((uintptr_t)&group & 0xFFFF);
    float jx   = (sc_hash21(seed + 12.3f, group.labelCAStartTime + 19.7f) - 0.5f) * 2.0f * DBCA_CENTER_NUDGE;
    float jy   = (sc_hash21(seed + 21.1f, group.labelCAStartTime + 7.3f) - 0.5f) * 2.0f * (DBCA_CENTER_NUDGE * 0.6f);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* font   = ImGui::GetFont();
    ImVec2 win     = ImGui::GetWindowPos();

    // Colors
    ImU32 cR = ImGui::GetColorU32(ImVec4(1.00f, 0.10f, 0.10f, std::min(1.0f, DBCA_ALPHA * fade * alphaMul)));
    ImU32 cG = ImGui::GetColorU32(ImVec4(0.10f, 1.00f, 0.10f, std::min(1.0f, DBCA_ALPHA * fade * alphaMul)));
    ImU32 cB = ImGui::GetColorU32(ImVec4(0.15f, 0.45f, 1.00f, std::min(1.0f, DBCA_ALPHA * fade * alphaMul)));

    // Randomize the CA order (which color sits left/center/right)
    const int maps[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};
    int permPick         = (int)(sc_hash21(seed + 0.123f, group.labelCAStartTime + 0.789f) * 6.0f) % 6;

    // Randomize which side is "left" (flip horizontally)
    bool flipLR  = sc_hash11(seed + 3.14159f) > 0.5f;
    float leftX  = flipLR ? (+px) : (-px);
    float rightX = -leftX;

    // Position triplet for [left, center, right]
    float posX[3] = {leftX, jx, rightX};
    float posY[3] = {0.0f, jy, 0.0f};

    ImU32 cols[3] = {cR, cG, cB};
    ImVec2 posBase(win.x + leftAlignX, win.y + yPos);

    // Draw fringes in randomized left/center/right color assignment
    for (int i = 0; i < 3; ++i) {
        int colorIdx = maps[permPick][i];
        float x = posX[i], y = posY[i];
        dl->AddText(font, fontSize, ImVec2(posBase.x + x, posBase.y + y), cols[colorIdx], labelText.c_str());

        // tiny trailing copies for left/right slots (skip center)
        if (i != 1) {
            for (int p = 0; p < DBCA_EXTRA_PASSES; ++p) {
                float o = (p + 1) * DBCA_PASS_OFFSET_PX * (i == 0 ? -1.0f : +1.0f);
                dl->AddText(font, fontSize, ImVec2(posBase.x + x + o, posBase.y + y), cols[colorIdx], labelText.c_str());
            }
        }
    }
}
// ----------------------------------------------------------------------

static void RenderGroupText(TrickGroup& group, float animationOffset, float yPos, float fontSize, float screenWidth, bool isRewardGroup) {
    ImColor orangeCol = ImColor(0.970f, 0.803f, 0.165f, 1.00f);
    ImColor whiteCol  = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 textColor  = isRewardGroup ? (ImVec4)orangeCol : (ImVec4)whiteCol;
    ImVec4 scoreColor = (ImVec4)orangeCol;

    ImVec4 shadowColor(0.0f, 0.0f, 0.0f, 1.0f);
    float shadowOffsetX = 2.0f, shadowOffsetY = 2.0f;

    std::string displayText = group.trickName;
    float now               = getGameTimeSeconds();

    // "New" pop highlight
    float popAlpha = 0.0f;
    if (group.isNew) {
        float t  = std::max(0.0f, std::min(1.0f, (now - group.firstAppearanceTime) / ANIMATION_DURATION));
        popAlpha = (1.0f - t);
    }

    float textWidth     = ImGui::CalcTextSize(displayText.c_str()).x;
    float customSpacing = fontSize * TEXT_SPACING_RATIO;
    float targetX       = screenWidth * SCREEN_MARGIN_RATIO;
    float leftAlignX    = targetX + animationOffset;
    float scoreX        = targetX + textWidth + customSpacing + animationOffset;

    // DEATH BLOW label CA: draw BEFORE main text so white sits on top
    if (!isRewardGroup && group.isDeathblow) {
        RenderDeathblowLabelCA(displayText, leftAlignX, yPos, fontSize, group);
    }

    // shadow for label
    ImGui::SetCursorPos(ImVec2(leftAlignX + shadowOffsetX, yPos + shadowOffsetY));
    ImGui::PushStyleColor(ImGuiCol_Text, shadowColor);
    ImGui::Text("%s", displayText.c_str());
    ImGui::PopStyleColor();

    // main label
    ImGui::SetCursorPos(ImVec2(leftAlignX, yPos));
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::Text("%s", displayText.c_str());
    ImGui::PopStyleColor();

    // pop highlight overlay
    if (popAlpha > 0.001f) {
        ImGui::SetCursorPos(ImVec2(leftAlignX, yPos));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, std::min(1.0f, 0.6f * popAlpha)));
        ImGui::Text("%s", displayText.c_str());
        ImGui::PopStyleColor();
    }

    // Score draw
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* font   = ImGui::GetFont();
    ImVec2 win     = ImGui::GetWindowPos();

    float popScale = 1.0f;
    ImVec2 popOffset(0.0f, 0.0f);

    if (!isRewardGroup) {
        int amt      = std::max(0, std::min(group.money, 50));
        float amt01  = amt / 50.0f;
        float amtMul = AMT_MIN_MULT + (AMT_MAX_MULT - AMT_MIN_MULT) * amt01;

        if (group.scorePopStartTime >= 0.0f) {
            float dtPop = now - group.scorePopStartTime;
            if (dtPop >= 0.0f && dtPop <= SCORE_POP_DURATION) {
                float t     = dtPop / SCORE_POP_DURATION;
                float decay = (1.0f - t);
                float amp   = SCORE_POP_AMPLITUDE * amtMul;
                popScale    = 1.0f + amp * decay * decay;

                // Random burst direction
                char previewBuf[64];
                snprintf(previewBuf, sizeof(previewBuf), " +%d", group.money);
                ImVec2 scoreSize = ImGui::CalcTextSize(previewBuf);

                float seed = (float)((uintptr_t)&group & 0xFFFF);
                float ang  = sc_hash21(seed + 33.0f, group.scorePopStartTime + 77.0f) * 6.2831853f;
                float dirx = std::cos(ang);
                float diry = std::sin(ang);

                popOffset.x = -(popScale - 1.0f) * 0.5f * scoreSize.x * dirx;
                popOffset.y = -(popScale - 1.0f) * 0.5f * fontSize * diry;
            }
        }
    }

    // formatted score text
    char buf[64];
    if (isRewardGroup) {
        int disp = RewardDisplayValue(group, now);
        snprintf(buf, sizeof(buf), " =%d", disp);
    } else {
        snprintf(buf, sizeof(buf), " +%d", group.money);
    }

    ImVec2 posShadow(win.x + scoreX + 2.0f + popOffset.x, win.y + yPos + 2.0f + popOffset.y);
    ImVec2 posMain(win.x + scoreX + popOffset.x, win.y + yPos + popOffset.y);

    // shadow
    dl->AddText(font, fontSize * popScale, posShadow, ImGui::GetColorU32(ImVec4(0, 0, 0, 1)), buf);
    // main value
    dl->AddText(font, fontSize * popScale, posMain, ImGui::GetColorU32(scoreColor), buf);

    // existing money CA (unchanged)
    if (!isRewardGroup && group.scorePopStartTime >= 0.0f) {
        float dtPop = now - group.scorePopStartTime;
        if (dtPop >= 0.0f && dtPop <= CA_DURATION) {
            int amt      = std::max(0, std::min(group.money, 50));
            float amt01  = amt / 50.0f;
            float amtMul = AMT_MIN_MULT + (AMT_MAX_MULT - AMT_MIN_MULT) * amt01;

            float t         = dtPop / CA_DURATION;
            float intensity = (1.0f - t);
            intensity       = intensity * intensity;

            float basePx = fontSize * CA_MAX_OFFSET_F * amtMul;

            if (CA_TRAILER_STYLE) {
                float px = basePx * CA_TRAILER_OFFSET_MULT * intensity;
                float dx = px, dy = 0.0f;

                float alpha   = std::min(1.0f, CA_BASE_ALPHA * CA_TRAILER_ALPHA_MULT * intensity);
                ImU32 cR      = ImGui::GetColorU32(ImVec4(1.00f, 0.05f, 0.05f, alpha));
                ImU32 cG      = ImGui::GetColorU32(ImVec4(0.05f, 1.00f, 0.05f, alpha));
                ImU32 cB      = ImGui::GetColorU32(ImVec4(0.10f, 0.40f, 1.00f, alpha));
                ImU32 cols[3] = {cR, cG, cB};

                float seedF = (float)((uintptr_t)&group & 0xFFFF);
                float cxN   = (sc_hash21(seedF + 12.3f, group.scorePopStartTime + 19.7f) - 0.5f) * 2.0f * CA_TRAILER_CENTER_NUDGE_PX;
                float cyN   = (sc_hash21(seedF + 21.1f, group.scorePopStartTime + 7.3f) - 0.5f) * 2.0f * CA_TRAILER_CENTER_NUDGE_PX;

                ImVec2 offs[3] = {ImVec2(-dx, 0.0f), ImVec2(cxN, cyN), ImVec2(+dx, 0.0f)};

                int permPick         = (int)(sc_hash21(seedF + 6.0f, group.scorePopStartTime + 17.0f) * 6.0f) % 6;
                const int maps[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}};

                for (int i = 0; i < 3; ++i) {
                    int cIdx = maps[permPick][i];
                    ImVec2 p = ImVec2(posMain.x + offs[i].x, posMain.y + offs[i].y);
                    dl->AddText(font, fontSize * popScale, p, cols[cIdx], buf);

                    for (int pidx = 0; pidx < CA_TRAILER_EXTRA_PASSES; ++pidx) {
                        float o  = (pidx + 1) * CA_TRAILER_PASS_OFFSET_PX;
                        float nx = p.x + (i == 0 ? -o : (i == 2 ? +o : 0.0f));
                        dl->AddText(font, fontSize * popScale, ImVec2(nx, p.y), cols[cIdx], buf);
                    }
                }
            } else {
                float seed     = (float)((uintptr_t)&group & 0xFFFF);
                float baseRand = sc_hash21(seed, group.scorePopStartTime);
                float angle0   = baseRand * 6.2831853f;

                float freqRand = sc_hash21(seed + 1.0f, group.scorePopStartTime + 3.0f);
                float freq     = 10.0f + 30.0f * freqRand;
                float angle    = angle0 + now * freq;

                float ampRand = sc_hash21(seed + 2.0f, group.scorePopStartTime + 7.0f);
                float ampMul2 = 0.6f + 1.8f * ampRand;
                float jigRand = sc_hash21(seed + 4.0f, group.scorePopStartTime + 11.0f);
                float jigFreq = 6.0f + 14.0f * jigRand;
                float jigMul  = 0.75f + 0.25f * std::sin(now * jigFreq);

                float px = basePx * ampMul2 * jigMul * intensity;
                float dx = px * std::cos(angle);
                float dy = px * std::sin(angle);

                int palIndex = (int)(sc_hash21(seed + 5.0f, group.scorePopStartTime + 13.0f) * 3.0f);
                if (palIndex == 0) {
                    ImVec4 cA(0.25f, 0.95f, 1.00f, CA_BASE_ALPHA * intensity); // cyan
                    ImVec4 cB(1.00f, 0.25f, 0.25f, CA_BASE_ALPHA * intensity); // red
                    ImVec2 posA(posMain.x - dx, posMain.y - dy);
                    ImVec2 posB(posMain.x + dx, posMain.y + dy);
                    dl->AddText(font, fontSize * popScale, posA, ImGui::GetColorU32(cA), buf);
                    dl->AddText(font, fontSize * popScale, posB, ImGui::GetColorU32(cB), buf);
                } else if (palIndex == 1) {
                    ImVec4 cA(1.00f, 0.25f, 0.80f, CA_BASE_ALPHA * intensity); // magenta
                    ImVec4 cB(0.25f, 1.00f, 0.35f, CA_BASE_ALPHA * intensity); // green
                    ImVec2 posA(posMain.x - dx, posMain.y - dy);
                    ImVec2 posB(posMain.x + dx, posMain.y + dy);
                    dl->AddText(font, fontSize * popScale, posA, ImGui::GetColorU32(cA), buf);
                    dl->AddText(font, fontSize * popScale, posB, ImGui::GetColorU32(cB), buf);
                } else {
                    ImVec4 cA(1.00f, 0.95f, 0.25f, CA_BASE_ALPHA * intensity); // yellow
                    ImVec4 cB(0.25f, 0.45f, 1.00f, CA_BASE_ALPHA * intensity); // blue
                    ImVec2 posA(posMain.x - dx, posMain.y - dy);
                    ImVec2 posB(posMain.x + dx, posMain.y + dy);
                    dl->AddText(font, fontSize * popScale, posA, ImGui::GetColorU32(cA), buf);
                    dl->AddText(font, fontSize * popScale, posB, ImGui::GetColorU32(cB), buf);
                }
            }
        }
    }
}

static void DisplayGroups(
    const ImVec2& screenSize, float fontSize, float startY, bool isRewardGroup, const char* windowName, float windowHeightRatio) {
    if (IsGamePaused())
        return;

    float now = getGameTimeSeconds();

    std::vector<std::reference_wrapper<TrickGroup>> activeGroups;
    for (auto& group : trickGroups) {
        if (group.isReward == isRewardGroup)
            activeGroups.emplace_back(group);
    }
    if (activeGroups.empty())
        return;

    std::sort(activeGroups.begin(), activeGroups.end(),
        [](const TrickGroup& a, const TrickGroup& b) { return a.firstAppearanceTime > b.firstAppearanceTime; });

    ImGui::SetNextWindowPos(ImVec2(0.0f, startY));
    ImGui::SetNextWindowSize(ImVec2(screenSize.x * SCREEN_WIDTH_RATIO, screenSize.y * windowHeightRatio));
    ImGui::Begin(windowName, nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollbar);
    ImGui::PushFont(g_framework->get_our_imgui_ctx()->tony_font, fontSize);

    float lineHeight = fontSize * LINE_HEIGHT_MULTIPLIER;
    for (size_t groupIndex = 0; groupIndex < activeGroups.size(); ++groupIndex) {
        TrickGroup& group     = activeGroups[groupIndex].get();
        float animationOffset = CalculateAnimationOffset(group, now, screenSize.x);
        float yPos            = lineHeight * groupIndex;

        RenderGroupText(group, animationOffset, yPos, fontSize, screenSize.x, isRewardGroup);
        MarkAnimationComplete(group, now);
    }

    ImGui::PopFont();
    ImGui::End();
}

static void StartOrRestartRewardCount(TrickGroup& g, float now) {
    int currentDisplay     = RewardDisplayValue(g, now);
    g.rewardStartValue     = currentDisplay;
    g.rewardEndValue       = g.money;
    g.rewardCountStartTime = now;
}

static bool AnyActiveDeathblowVisible(float now) {
    for (const auto& tg : trickGroups) {
        if (!tg.isReward && tg.isDeathblow) {
            float alive = now - tg.mostRecentTime;
            if (alive <= DISPLAY_DURATION)
                return true;
        }
    }
    return false;
}

static void AddTrickScore(int id, int money, bool isReward) {
    if (IsGamePaused())
        return;

    std::string trickName = isReward ? "Kill Reward" : MoveNames[id];
    float now             = getGameTimeSeconds();

    bool foundExisting = false;
    if (!trickGroups.empty()) {
        for (auto it = trickGroups.rbegin(); it != trickGroups.rend(); ++it) {
            if (it->isReward == isReward && it->trickName == trickName) {
                it->count++;
                it->money += money;
                it->mostRecentTime = now;
                it->isNew          = false;

                if (!isReward) {
                    it->scorePopStartTime = now;
                    if (it->isDeathblow) {
                        it->labelCAStartTime = now;
                        it->dbcaSoloBoost    = false; // subsequent hits keep normal intensity
                        it->labelCASeed      = sc_hash21((float)id + 0.5f, now + 1.0f);
                    }
                } else {
                    StartOrRestartRewardCount(*it, now);
                }

                foundExisting = true;
                break;
            }
        }
    }

    if (!foundExisting) {
        TrickGroup newGroup(trickName, money, isReward, now);
        if (isReward) {
            newGroup.rewardStartValue     = 0;
            newGroup.rewardEndValue       = money;
            newGroup.rewardCountStartTime = now;
            trickGroups.push_back(newGroup);
        } else {
            newGroup.isDeathblow = IsDeathblowID(id);
            if (newGroup.isDeathblow) {
                bool anyActive            = AnyActiveDeathblowVisible(now);
                newGroup.dbcaSoloBoost    = !anyActive; // solo boost only if this is the only one up
                newGroup.labelCAStartTime = now;
                newGroup.labelCASeed      = sc_hash21((float)id + 0.5f, now + 1.0f);
            }
            trickGroups.insert(trickGroups.begin(), newGroup);
        }
    }
}

static void AddTrickScore2(const char* trickName, int money, bool isReward) {
    if (IsGamePaused())
        return;

    std::string trickNameStr = trickName;
    float now                = getGameTimeSeconds();

    bool foundExisting = false;
    if (!trickGroups.empty()) {
        for (auto it = trickGroups.rbegin(); it != trickGroups.rend(); ++it) {
            if (it->isReward == isReward && it->trickName == trickNameStr) {
                it->count++;
                it->money += money;
                it->mostRecentTime = now;
                it->isNew          = false;

                if (!isReward) {
                    it->scorePopStartTime = now;
                    if (it->isDeathblow) {
                        it->labelCAStartTime = now;
                        it->dbcaSoloBoost    = false;
                        it->labelCASeed      = sc_hash21((float)it->count + 0.5f, now + 1.0f);
                    }
                } else {
                    StartOrRestartRewardCount(*it, now);
                }

                foundExisting = true;
                break;
            }
        }
    }

    if (!foundExisting) {
        TrickGroup newGroup(trickNameStr, money, isReward, now);
        if (isReward) {
            newGroup.rewardStartValue     = 0;
            newGroup.rewardEndValue       = money;
            newGroup.rewardCountStartTime = now;
            trickGroups.push_back(newGroup);
        } else {
            newGroup.isDeathblow = (trickNameStr.find("DEATH BLOW") != std::string::npos);
            if (newGroup.isDeathblow) {
                bool anyActive            = AnyActiveDeathblowVisible(now);
                newGroup.dbcaSoloBoost    = !anyActive;
                newGroup.labelCAStartTime = now;
                newGroup.labelCASeed      = sc_hash21((float)trickNameStr.size() + 0.25f, now + 1.0f);
            }
            trickGroups.insert(trickGroups.begin(), newGroup);
        }
    }
}

void Tony::on_frame() {
    if (!mod_enabled)
        return;

    mHRPc* player = nmh_sdk::get_mHRPc();
    if (!player)
        return;

    bool isPaused               = IsGamePaused();
    static bool lastFramePaused = false;

    if (!lastFramePaused && isPaused) {
        StoreRemainingTimeOnPause();
    } else if (lastFramePaused && !isPaused) {
        UpdateTimestampsOnUnpause();
    }
    lastFramePaused = isPaused;

    if (!isPaused)
        CleanupExpiredGroups();

    bool hasRewards = false, hasTricks = false;
    for (const auto& group : trickGroups) {
        if (group.isReward)
            hasRewards = true;
        else
            hasTricks = true;
        if (hasRewards && hasTricks)
            break;
    }
    if (!hasRewards && !hasTricks)
        return;

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    float fontSize    = 14.0f * (screenSize.y / 1080.0f);
    if (fontSize <= 0.0f)
        fontSize = 14.0f;

    float rewardStartY = screenSize.y * REWARD_START_Y_RATIO;
    float trickStartY  = screenSize.y * TRICK_START_Y_RATIO;

    if (hasRewards)
        DisplayGroups(screenSize, fontSize, rewardStartY, true, "RewardWindow", REWARD_WINDOW_HEIGHT_RATIO);
    if (hasTricks)
        DisplayGroups(screenSize, fontSize, trickStartY, false, "TrickWindow", TRICK_WINDOW_HEIGHT_RATIO);
}

// clang-format off
naked void detour1() { // most attacks // player in edi
    __asm {
        cmp byte ptr [Tony::mod_enabled], 0
        je originalcode
    
        pushad
        push [esp+0x20+0xC] // damage
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
        cmp byte ptr[Tony::mod_enabled], 0
        je originalcode

        pushad
        push 0
        push 0
        push [edi+0x18C] // moveID
        call AddTrickScore
        add esp, 0xC
        popad

    originalcode:
        mov edx, 0x00000379
        jmp dword ptr[Tony::jmp_ret2]
    }
}

static constexpr const char* throwSuccessName = "Successful input";
naked void detour3() { // throw input success // player in edi
    __asm {
        // original movss
        mov eax, [ecx+0x0000030C]
        cmp byte ptr [Tony::mod_enabled], 0
        je originalcode
            
        pushad

        push 0 // is reward
        push 0 // [edi+0x24] // damage
        push [edi+0x18C] // moveID
        call AddTrickScore
        add esp, 0xC
    
    popcode:
        popad
    originalcode:
        jmp dword ptr [Tony::jmp_ret3]
    }
}
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
    ImGui::Checkbox("Score Visualizer", &mod_enabled);
    if (mod_enabled) {
        // Previously: separate Deathblow CA toggle and Solo Boost sliders.
        // Those are now core behavior of Score Visualizer, so the public UI is clean.
        // (Internal tuning sliders intentionally omitted.)
    }
}

std::optional<std::string> Tony::on_initialize() {
    gpBattle = g_framework->get_module().as<uintptr_t>() + 0x843584;
    /*if (!install_hook_offset(0x3CB850, m_hook1, &detour1, &Tony::jmp_ret1, 6)) { // most hits
        spdlog::error("Failed to init Tony mod");
        return "Failed to init Tony mod";
    }

    if (!install_hook_offset(0x3CAFA1, m_hook2, &detour2, &Tony::jmp_ret2, 5)) { // executions
        spdlog::error("Failed to init Tony mod 2");
        return "Failed to init Tony mod 2";
    }*/
    // nmh.mHRChara::mSetCharaHit is called on auto executions but probably also elsewhen
    if (!install_hook_offset(0xA0D33, m_hook3, &detour3, &Tony::jmp_ret3, 6)) { // throw input success
        spdlog::error("Failed to init Tony mod 3");
        return "Failed to init Tony mod 3";
    }

    if (!install_hook_offset(0x3CB92D, m_hook4, &detour4, &Tony::jmp_ret4, 6)) { // +5 money gains
        spdlog::error("Failed to init Tony mod 4");
        return "Failed to init Tony mod 4";
    }

    if (!install_hook_offset(0x3E1CD6, m_hook5, &detour5, &Tony::jmp_ret5, 6)) { // money rewards
        spdlog::error("Failed to init Tony mod 4");
        return "Failed to init Tony mod 4";
    }

    return Mod::on_initialize();
}

void Tony::on_config_load(const utility::Config& cfg) {
    mod_enabled = cfg.get<bool>("Tony").value_or(false);
}

void Tony::on_config_save(utility::Config& cfg) {
    cfg.set<bool>("Tony", mod_enabled);
}

#endif
