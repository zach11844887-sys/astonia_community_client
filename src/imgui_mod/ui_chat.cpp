/*
 * ImGui Chat Panel
 * Displays real-time chat messages from the game's text buffer
 */

#include <cstring>
#include <cstdio>

#include "../imgui/imgui.h"
#include "imgui_mod_imports.h"

// Convert RGB565 to ImGui color
static ImU32 palette_to_imgui(int color_idx)
{
    if (color_idx < 0 || color_idx > 17) color_idx = 0;

    unsigned short rgb565 = palette[color_idx];

    // Extract RGB components from RGB565
    int r = ((rgb565 >> 11) & 0x1F) << 3;
    int g = ((rgb565 >> 5) & 0x3F) << 2;
    int b = (rgb565 & 0x1F) << 3;

    return IM_COL32(r, g, b, 255);
}

// Chat input buffer
static char chat_input[200] = "";
static bool scroll_to_bottom = true;
static int last_textlines = 0;

void ui_chat_init()
{
    chat_input[0] = '\0';
}

void ui_chat_frame()
{
    // Fixed position: bottom-left corner
    ImGui::SetNextWindowPos(ImVec2(0, 400), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
    if (!ImGui::Begin("Chat", nullptr, flags)) {
        ImGui::End();
        return;
    }

    // Chat history area
    float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if (ImGui::BeginChild("ChatScroll", ImVec2(0, -footer_height), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar)) {

        // Check if new messages arrived
        if (textlines != last_textlines) {
            scroll_to_bottom = true;
            last_textlines = textlines;
        }

        // Render chat lines
        int lines_to_show = textlines < 256 ? textlines : 256;

        for (int i = 0; i < lines_to_show; i++) {
            // Calculate line index (circular buffer)
            int line_idx = (textnextline - lines_to_show + i + 256) % 256;
            int pos = line_idx * 256;  // MAXTEXTLETTERS = 256

            // Build line string with color changes
            char line_buf[512];
            int buf_pos = 0;
            int current_color = 0;
            bool first_segment = true;

            for (int j = 0; j < 256 && text[pos + j].c != '\0'; j++) {
                char c = text[pos + j].c;
                int color = text[pos + j].color;

                // Skip special positioning characters
                if (c < 32) continue;

                // Color change - render accumulated text
                if (color != current_color && buf_pos > 0) {
                    line_buf[buf_pos] = '\0';
                    if (!first_segment) ImGui::SameLine(0, 0);
                    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(palette_to_imgui(current_color)), "%s", line_buf);
                    first_segment = false;
                    buf_pos = 0;
                }

                current_color = color;
                if (buf_pos < 510) {
                    line_buf[buf_pos++] = c;
                }
            }

            // Render remaining text
            if (buf_pos > 0) {
                line_buf[buf_pos] = '\0';
                if (!first_segment) ImGui::SameLine(0, 0);
                ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(palette_to_imgui(current_color)), "%s", line_buf);
            } else if (first_segment) {
                // Empty line
                ImGui::TextUnformatted("");
            }
        }

        // Auto-scroll to bottom when new messages arrive
        if (scroll_to_bottom && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20) {
            ImGui::SetScrollHereY(1.0f);
        }
        scroll_to_bottom = false;
    }
    ImGui::EndChild();

    // Input area
    ImGui::Separator();

    bool reclaim_focus = false;
    ImGuiInputTextFlags input_flags = ImGuiInputTextFlags_EnterReturnsTrue;

    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##ChatInput", chat_input, sizeof(chat_input), input_flags)) {
        if (chat_input[0] != '\0') {
            // Send the message through the game's command system
            cmd_add_text(chat_input, 0);
            chat_input[0] = '\0';
        }
        reclaim_focus = true;
        scroll_to_bottom = true;
    }

    // Auto-focus on input when window is focused
    if (reclaim_focus) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    ImGui::End();
}
