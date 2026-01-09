/*
 * ImGui Skills Panel
 * Displays character attributes and skills with progress bars
 */

#include <cstdio>
#include <cstring>

#include "../imgui/imgui.h"
#include "imgui_mod_imports.h"

void ui_skills_init()
{
}

// Skill categories
struct SkillCategory {
    const char* name;
    int start;
    int end;
};

static SkillCategory categories[] = {
    {"Powers",      0,  2},
    {"Attributes",  3,  6},
    {"Combat",      12, 24},
    {"Magic",       28, 37},
    {"Misc",        25, 27},
};

#define NUM_CATEGORIES (sizeof(categories) / sizeof(categories[0]))

// External toggle function from imgui_mod.cpp
extern "C" void toggle_skills_panel();

static bool skills_visible = true;

void ui_skills_frame()
{
    // Allow window to be positioned/sized by user, with default centered position
    ImGui::SetNextWindowPos(ImVec2(240, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(320, 450), ImGuiCond_FirstUseEver);

    // Window with close button (X)
    bool window_open = true;
    if (!ImGui::Begin("Skills & Stats", &window_open, ImGuiWindowFlags_None)) {
        ImGui::End();
        return;
    }

    // If user closed with X button, toggle off
    if (!window_open) {
        toggle_skills_panel();
        ImGui::End();
        return;
    }

    // Player level and experience
    int level = exp2level((int)experience);
    ImGui::Text("Level: %d", level);
    ImGui::Text("Experience: %u / %u", experience_used, experience);

    int exp_free = (int)experience - (int)experience_used;
    ImGui::TextColored(
        exp_free > 0 ? ImVec4(0.4f, 0.8f, 0.4f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
        "Available: %d", exp_free > 0 ? exp_free : 0
    );

    ImGui::Separator();

    // Vital stats bars
    if (ImGui::CollapsingHeader("Vitals", ImGuiTreeNodeFlags_DefaultOpen)) {
        // HP
        float hp_pct = hp.act > 0 ? (float)hp.act / (float)hp.max : 0.0f;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
        char hp_text[32];
        snprintf(hp_text, sizeof(hp_text), "HP: %d / %d", hp.act, hp.max);
        ImGui::ProgressBar(hp_pct, ImVec2(-1, 0), hp_text);
        ImGui::PopStyleColor();

        // Mana
        float mana_pct = mana.max > 0 ? (float)mana.act / (float)mana.max : 0.0f;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
        char mana_text[32];
        snprintf(mana_text, sizeof(mana_text), "Mana: %d / %d", mana.act, mana.max);
        ImGui::ProgressBar(mana_pct, ImVec2(-1, 0), mana_text);
        ImGui::PopStyleColor();

        // Endurance
        float end_pct = endurance.max > 0 ? (float)endurance.act / (float)endurance.max : 0.0f;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.9f, 0.7f, 0.2f, 1.0f));
        char end_text[32];
        snprintf(end_text, sizeof(end_text), "End: %d / %d", endurance.act, endurance.max);
        ImGui::ProgressBar(end_pct, ImVec2(-1, 0), end_text);
        ImGui::PopStyleColor();
    }

    ImGui::Separator();

    // Skills by category
    if (ImGui::BeginChild("SkillsScroll", ImVec2(0, 0), ImGuiChildFlags_None)) {
        for (size_t cat = 0; cat < NUM_CATEGORIES; cat++) {
            if (ImGui::CollapsingHeader(categories[cat].name, ImGuiTreeNodeFlags_DefaultOpen)) {
                for (int v = categories[cat].start; v <= categories[cat].end && v < *game_v_max; v++) {
                    // Skip empty/invalid skills
                    if (game_skill[v].cost == 0) continue;
                    if (game_skill[v].name[0] == '\0' || strcmp(game_skill[v].name, "empty") == 0) continue;

                    uint16_t base_val = value[0][v];
                    uint16_t curr_val = value[1][v];

                    // Skip skills we don't have
                    if (base_val == 0 && curr_val == 0) continue;

                    ImGui::PushID(v);

                    // Skill name and values
                    bool show_raise = (game_skill[v].cost > 0 && exp_free > 0);

                    if (curr_val != base_val) {
                        // Modified value - show in different color
                        ImVec4 mod_color = curr_val > base_val ?
                            ImVec4(0.4f, 0.9f, 0.4f, 1.0f) :  // Green if buffed
                            ImVec4(0.9f, 0.4f, 0.4f, 1.0f);   // Red if debuffed
                        ImGui::Text("%s:", game_skill[v].name);
                        ImGui::SameLine(150);
                        ImGui::Text("%d", base_val);
                        ImGui::SameLine();
                        ImGui::TextColored(mod_color, "(%d)", curr_val);
                    } else {
                        ImGui::Text("%s:", game_skill[v].name);
                        ImGui::SameLine(150);
                        ImGui::Text("%d", base_val);
                    }

                    // Raise button
                    if (show_raise) {
                        int cost = raise_cost(v, (int)base_val);
                        bool can_raise = (cost <= exp_free);

                        ImGui::SameLine(220);
                        if (!can_raise) {
                            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                        }

                        char raise_label[32];
                        snprintf(raise_label, sizeof(raise_label), "+##raise%d", v);
                        if (ImGui::SmallButton(raise_label) && can_raise) {
                            cmd_raise(v);
                        }

                        if (!can_raise) {
                            ImGui::PopStyleVar();
                        }

                        if (ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            ImGui::Text("Cost: %d exp", cost);
                            if (!can_raise) {
                                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Not enough experience");
                            }
                            ImGui::EndTooltip();
                        }
                    }

                    // Skill description tooltip
                    if (ImGui::IsItemHovered() && game_skilldesc && game_skilldesc[v]) {
                        ImGui::BeginTooltip();
                        ImGui::PushTextWrapPos(300.0f);
                        ImGui::TextWrapped("%s", game_skilldesc[v]);
                        ImGui::PopTextWrapPos();
                        ImGui::EndTooltip();
                    }

                    ImGui::PopID();
                }
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();
}
