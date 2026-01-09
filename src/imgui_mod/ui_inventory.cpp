/*
 * ImGui Inventory Panel
 * Displays player inventory in a grid layout with actual item sprites
 */

#include <cstdio>

#include "../imgui/imgui.h"
#include "imgui_mod_imports.h"

// Inventory layout - match original UI
#define INV_START_SLOT 30   // First inventory slot (0-29 are worn items)
#define INV_COLS 5
#define SLOT_SIZE 36        // Match original inventory slot size
#define SPRITE_MAX_SIZE 32  // Max sprite size within slot

static int selected_slot = -1;

// Helper to draw a sprite centered in a slot
static void draw_item_sprite(ImDrawList* draw_list, float slot_x, float slot_y, uint32_t sprite_id)
{
    int tex_w, tex_h;
    SDL_Texture* tex = sdl_get_sprite_texture(sprite_id, &tex_w, &tex_h);

    if (tex) {
        // Calculate scale to fit in slot while preserving aspect ratio
        float scale = 1.0f;
        if (tex_w > SPRITE_MAX_SIZE || tex_h > SPRITE_MAX_SIZE) {
            float scale_x = (float)SPRITE_MAX_SIZE / tex_w;
            float scale_y = (float)SPRITE_MAX_SIZE / tex_h;
            scale = (scale_x < scale_y) ? scale_x : scale_y;
        }

        float draw_w = tex_w * scale;
        float draw_h = tex_h * scale;

        // Center the sprite in the slot
        float draw_x = slot_x + (SLOT_SIZE - draw_w) * 0.5f;
        float draw_y = slot_y + (SLOT_SIZE - draw_h) * 0.5f;

        // Draw sprite using ImGui's AddImage
        draw_list->AddImage(
            (ImTextureID)tex,
            ImVec2(draw_x, draw_y),
            ImVec2(draw_x + draw_w, draw_y + draw_h)
        );
    }
}

void ui_inventory_init()
{
    selected_slot = -1;
}

void ui_inventory_frame()
{
    // Fixed position: bottom-right corner
    float inv_width = INV_COLS * (SLOT_SIZE + 4) + 20;
    float inv_height = 220;
    ImGui::SetNextWindowPos(ImVec2(800 - inv_width, 400), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(inv_width, inv_height), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize;
    if (!ImGui::Begin("Inventory", nullptr, flags)) {
        ImGui::End();
        return;
    }

    // Gold display
    ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.0f, 1.0f), "Gold: %u", gold);
    ImGui::Separator();

    // Inventory grid
    ImGui::BeginChild("InvGrid", ImVec2(0, 0), ImGuiChildFlags_None);

    int visible_slots = INVENTORYSIZE - INV_START_SLOT;
    int rows = (visible_slots + INV_COLS - 1) / INV_COLS;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < INV_COLS; col++) {
            int slot_idx = INV_START_SLOT + row * INV_COLS + col;

            if (slot_idx >= INVENTORYSIZE) break;

            if (col > 0) ImGui::SameLine();

            ImGui::PushID(slot_idx);

            // Check if slot has an item
            uint32_t item_sprite = item[slot_idx];
            bool has_item = (item_sprite != 0);
            bool is_selected = (slot_idx == selected_slot) || (slot_idx == invsel);

            // Get cursor position before drawing slot
            ImVec2 slot_pos = ImGui::GetCursorScreenPos();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            // Draw slot background
            ImU32 bg_color;
            if (is_selected) {
                bg_color = IM_COL32(50, 80, 120, 200);
            } else if (has_item) {
                bg_color = IM_COL32(40, 40, 50, 200);
            } else {
                bg_color = IM_COL32(25, 25, 30, 180);
            }

            ImVec2 slot_min = slot_pos;
            ImVec2 slot_max = ImVec2(slot_pos.x + SLOT_SIZE, slot_pos.y + SLOT_SIZE);

            draw_list->AddRectFilled(slot_min, slot_max, bg_color, 4.0f);

            // Draw item sprite if slot has an item
            if (has_item) {
                draw_item_sprite(draw_list, slot_pos.x, slot_pos.y, item_sprite);
            }

            // Draw border
            ImU32 border_color = is_selected ? IM_COL32(100, 150, 200, 255) : IM_COL32(60, 60, 70, 200);
            draw_list->AddRect(slot_min, slot_max, border_color, 4.0f);

            // Invisible button for click handling
            if (ImGui::InvisibleButton("##slot", ImVec2(SLOT_SIZE, SLOT_SIZE))) {
                // Left click - select or use
                if (has_item) {
                    selected_slot = slot_idx;
                    // Trigger look on click
                    cmd_look_inv(slot_idx);
                }
            }

            // Drag source - allow dragging items from inventory
            if (has_item && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("ITEM_SLOT", &slot_idx, sizeof(int));
                // Show sprite during drag
                ImGui::Text("Slot %d", slot_idx);
                ImGui::EndDragDropSource();
            }

            // Drop target - allow dropping items to swap positions
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_SLOT")) {
                    int src_slot = *(const int*)payload->Data;
                    if (src_slot != slot_idx) {
                        // Swap items between slots
                        invsel = src_slot;
                        cmd_swap(slot_idx);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // Hover highlight
            if (ImGui::IsItemHovered()) {
                draw_list->AddRect(slot_min, slot_max, IM_COL32(120, 180, 255, 255), 4.0f, 0, 2.0f);
            }

            // Right click context menu
            if (ImGui::BeginPopupContextItem("SlotContext")) {
                if (has_item) {
                    if (ImGui::MenuItem("Use/Equip")) {
                        cmd_use_inv(slot_idx);
                    }
                    if (ImGui::MenuItem("Look")) {
                        cmd_look_inv(slot_idx);
                    }
                }
                ImGui::EndPopup();
            }

            // Tooltip
            if (has_item && ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
                ImGui::BeginTooltip();
                ImGui::Text("Slot %d", slot_idx);
                ImGui::Text("Sprite: %u", item_sprite);
                uint32_t flags = item_flags[slot_idx];
                if (flags & IF_USE) ImGui::Text("Usable/Equippable");
                ImGui::EndTooltip();
            }

            ImGui::PopID();
        }
    }

    ImGui::EndChild();
    ImGui::End();
}
