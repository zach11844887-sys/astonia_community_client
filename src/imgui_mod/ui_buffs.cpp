/*
 * ImGui Buff/Status Bars Panel
 * Displays active buffs, rage, potion, etc. with timers
 */

#include <cstdio>
#include <cstring>

#include "../imgui/imgui.h"
#include "imgui_mod_imports.h"

void ui_buffs_init()
{
}

// Helper to parse duration from hover text like "Bless: 45s to go" or "Rage: 75%"
static bool parse_buff_active(const char* hover_text, const char* prefix)
{
    if (strstr(hover_text, "Not active") != nullptr) {
        return false;
    }
    return strstr(hover_text, prefix) != nullptr;
}

static int parse_seconds_remaining(const char* hover_text)
{
    // Look for pattern like "45s to go" or "45us to go"
    const char* s = strstr(hover_text, "s to go");
    if (!s) return 0;

    // Walk back to find the number
    const char* num_start = s - 1;
    while (num_start > hover_text && (*num_start >= '0' && *num_start <= '9')) {
        num_start--;
    }
    num_start++;

    return atoi(num_start);
}

static int parse_percentage(const char* hover_text)
{
    // Look for pattern like "75%"
    const char* pct = strstr(hover_text, "%");
    if (!pct) return 0;

    // Walk back to find the number
    const char* num_start = pct - 1;
    while (num_start > hover_text && (*num_start >= '0' && *num_start <= '9')) {
        num_start--;
    }
    num_start++;

    return atoi(num_start);
}

void ui_buffs_frame()
{
    // Position at top-right corner, below any other UI
    float bar_width = 150;
    float bar_height = 20;
    float padding = 4;
    float margin = 5;

    // Calculate number of active buffs
    int num_buffs = 0;
    bool has_rage = (rage > 0 && value[0][V_RAGE] > 0);
    bool has_bless = parse_buff_active(hover_bless_text, "Bless:");
    bool has_freeze = parse_buff_active(hover_freeze_text, "Freeze:");
    bool has_potion = parse_buff_active(hover_potion_text, "Potion:");

    if (has_rage) num_buffs++;
    if (has_bless) num_buffs++;
    if (has_freeze) num_buffs++;
    if (has_potion) num_buffs++;

    if (num_buffs == 0) return;  // Nothing to display

    float window_height = num_buffs * (bar_height + padding) + padding * 2;

    ImGui::SetNextWindowPos(ImVec2(800 - bar_width - margin, margin), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(bar_width + padding * 2, window_height), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(padding, padding));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.08f, 0.10f, 0.85f));

    if (ImGui::Begin("BuffBars", nullptr, flags)) {

        // Rage bar (yellow/orange)
        if (has_rage) {
            int pct = parse_percentage(hover_rage_text);
            float rage_pct = pct / 100.0f;

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.9f, 0.6f, 0.1f, 1.0f));
            char rage_label[32];
            snprintf(rage_label, sizeof(rage_label), "Rage: %d%%", pct);
            ImGui::ProgressBar(rage_pct, ImVec2(bar_width, bar_height), rage_label);
            ImGui::PopStyleColor();
        }

        // Bless bar (blue/white)
        if (has_bless) {
            int seconds = parse_seconds_remaining(hover_bless_text);
            float bless_pct = seconds > 0 ? (float)seconds / 120.0f : 0.0f;  // Assume max 2 min
            if (bless_pct > 1.0f) bless_pct = 1.0f;

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.3f, 0.6f, 0.95f, 1.0f));
            char bless_label[32];
            snprintf(bless_label, sizeof(bless_label), "Bless: %ds", seconds);
            ImGui::ProgressBar(bless_pct, ImVec2(bar_width, bar_height), bless_label);
            ImGui::PopStyleColor();
        }

        // Freeze bar (cyan)
        if (has_freeze) {
            int seconds = parse_seconds_remaining(hover_freeze_text);
            float freeze_pct = seconds > 0 ? (float)seconds / 60.0f : 0.0f;  // Assume max 1 min
            if (freeze_pct > 1.0f) freeze_pct = 1.0f;

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.8f, 0.9f, 1.0f));
            char freeze_label[32];
            snprintf(freeze_label, sizeof(freeze_label), "Freeze: %ds", seconds);
            ImGui::ProgressBar(freeze_pct, ImVec2(bar_width, bar_height), freeze_label);
            ImGui::PopStyleColor();
        }

        // Potion bar (green)
        if (has_potion) {
            int seconds = parse_seconds_remaining(hover_potion_text);
            float potion_pct = seconds > 0 ? (float)seconds / 60.0f : 0.0f;  // Assume max 1 min
            if (potion_pct > 1.0f) potion_pct = 1.0f;

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.8f, 0.3f, 1.0f));
            char potion_label[32];
            snprintf(potion_label, sizeof(potion_label), "Potion: %ds", seconds);
            ImGui::ProgressBar(potion_pct, ImVec2(bar_width, bar_height), potion_label);
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}
