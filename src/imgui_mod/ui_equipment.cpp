/*
 * ImGui Equipment Panel (Paperdoll)
 * Displays equipped items in a character paper-doll layout with actual sprites
 */

#include <cstdio>

#include "../imgui/imgui.h"
#include "imgui_mod_imports.h"

#define SPRITE_MAX_SIZE 30  // Max sprite size within equipment slot

// Helper to draw a sprite centered in a slot
static void draw_equip_sprite(ImDrawList* draw_list, float slot_x, float slot_y, float slot_size, uint32_t sprite_id)
{
    int tex_w, tex_h;
    SDL_Texture* tex = sdl_get_sprite_texture(sprite_id, &tex_w, &tex_h);

    if (tex) {
        // Calculate scale to fit in slot while preserving aspect ratio
        float max_size = slot_size - 4; // Leave small margin
        float scale = 1.0f;
        if (tex_w > max_size || tex_h > max_size) {
            float scale_x = max_size / tex_w;
            float scale_y = max_size / tex_h;
            scale = (scale_x < scale_y) ? scale_x : scale_y;
        }

        float draw_w = tex_w * scale;
        float draw_h = tex_h * scale;

        // Center the sprite in the slot
        float draw_x = slot_x + (slot_size - draw_w) * 0.5f;
        float draw_y = slot_y + (slot_size - draw_h) * 0.5f;

        // Draw sprite using ImGui's AddImage
        draw_list->AddImage(
            (ImTextureID)tex,
            ImVec2(draw_x, draw_y),
            ImVec2(draw_x + draw_w, draw_y + draw_h)
        );
    }
}

// Equipment slots (indices 0-29 in item array, but we map specific ones)
// Based on IF_WN* flags:
// HEAD=5, NECK=6, BODY=7, ARMS=8, BELT=9, LEGS=10, FEET=11
// LHAND=12, RHAND=13, CLOAK=14, LRING=15, RRING=16
// Actual slot layout may vary - these are typical positions

struct EquipSlot {
    const char* name;
    int slot_id;    // Index into item[] array for worn items
    float x, y;     // Position in paperdoll layout
};

// Paperdoll slot positions (normalized 0-1, will be scaled)
static EquipSlot equip_slots[] = {
    {"Head",    0,  0.5f, 0.0f},
    {"Neck",    1,  0.5f, 0.12f},
    {"Cloak",   14, 0.0f, 0.12f},
    {"Body",    2,  0.5f, 0.28f},
    {"Arms",    3,  0.5f, 0.44f},
    {"L.Hand",  6,  0.15f, 0.44f},
    {"R.Hand",  7,  0.85f, 0.44f},
    {"Belt",    4,  0.5f, 0.56f},
    {"L.Ring",  8,  0.0f, 0.56f},
    {"R.Ring",  9,  1.0f, 0.56f},
    {"Legs",    5,  0.5f, 0.72f},
    {"Feet",    10, 0.5f, 0.88f},
};

#define NUM_EQUIP_SLOTS (sizeof(equip_slots) / sizeof(equip_slots[0]))
#define EQUIP_SLOT_SIZE 36
#define PANEL_WIDTH 180
#define PANEL_HEIGHT 280

static bool equipment_open = true;

void ui_equipment_init()
{
    equipment_open = true;
}

void ui_equipment_frame()
{
    // Fixed position: top-left, next to game area
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(PANEL_WIDTH + 20, equipment_open ? PANEL_HEIGHT + 40 : 30), ImGuiCond_Always);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    if (!ImGui::Begin("Equipment", nullptr, flags)) {
        ImGui::End();
        return;
    }

    // Toggle button for open/close
    if (ImGui::Button(equipment_open ? "[-] Hide" : "[+] Show", ImVec2(-1, 0))) {
        equipment_open = !equipment_open;
    }

    if (!equipment_open) {
        ImGui::End();
        return;
    }

    ImGui::Separator();

    // Get content region for positioning
    ImVec2 content_pos = ImGui::GetCursorScreenPos();
    ImVec2 content_size = ImVec2(PANEL_WIDTH, PANEL_HEIGHT);

    // Invisible button for the paperdoll area
    ImGui::InvisibleButton("PaperdollArea", content_size);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Draw background silhouette hint
    draw_list->AddRectFilled(
        ImVec2(content_pos.x + PANEL_WIDTH * 0.35f, content_pos.y + 20),
        ImVec2(content_pos.x + PANEL_WIDTH * 0.65f, content_pos.y + PANEL_HEIGHT - 20),
        IM_COL32(30, 30, 35, 100),
        8.0f
    );

    // Draw equipment slots - first pass: draw backgrounds and sprites
    for (size_t i = 0; i < NUM_EQUIP_SLOTS; i++) {
        EquipSlot& slot = equip_slots[i];

        // Calculate slot position
        float slot_x = content_pos.x + slot.x * (PANEL_WIDTH - EQUIP_SLOT_SIZE);
        float slot_y = content_pos.y + slot.y * (PANEL_HEIGHT - EQUIP_SLOT_SIZE);

        ImVec2 slot_min(slot_x, slot_y);
        ImVec2 slot_max(slot_x + EQUIP_SLOT_SIZE, slot_y + EQUIP_SLOT_SIZE);

        // Check if slot has an item
        uint32_t item_sprite = item[slot.slot_id];
        bool has_item = (item_sprite != 0);

        // Draw slot background
        ImU32 bg_color = has_item ? IM_COL32(40, 50, 60, 200) : IM_COL32(25, 25, 30, 180);
        ImU32 border_color = has_item ? IM_COL32(80, 120, 160, 255) : IM_COL32(50, 50, 60, 200);

        draw_list->AddRectFilled(slot_min, slot_max, bg_color, 4.0f);
        draw_list->AddRect(slot_min, slot_max, border_color, 4.0f);

        // Draw item sprite
        if (has_item) {
            draw_equip_sprite(draw_list, slot_x, slot_y, EQUIP_SLOT_SIZE, item_sprite);
        }

        // Slot label below
        ImVec2 label_size = ImGui::CalcTextSize(slot.name);
        float label_x = slot_x + (EQUIP_SLOT_SIZE - label_size.x) * 0.5f;
        float label_y = slot_y + EQUIP_SLOT_SIZE + 2;

        draw_list->AddText(ImVec2(label_x, label_y), IM_COL32(150, 150, 160, 200), slot.name);
    }

    // Second pass: create invisible buttons for each slot for drag-drop support
    for (size_t i = 0; i < NUM_EQUIP_SLOTS; i++) {
        EquipSlot& slot = equip_slots[i];

        // Calculate slot position
        float slot_x = content_pos.x + slot.x * (PANEL_WIDTH - EQUIP_SLOT_SIZE);
        float slot_y = content_pos.y + slot.y * (PANEL_HEIGHT - EQUIP_SLOT_SIZE);

        ImVec2 slot_min(slot_x, slot_y);
        ImVec2 slot_max(slot_x + EQUIP_SLOT_SIZE, slot_y + EQUIP_SLOT_SIZE);

        // Check if slot has an item
        uint32_t item_sprite = item[slot.slot_id];
        bool has_item = (item_sprite != 0);

        // Set cursor position for this slot
        ImGui::SetCursorScreenPos(slot_min);
        ImGui::PushID((int)(1000 + i));

        // Invisible button for the slot
        ImGui::InvisibleButton("##equip_slot", ImVec2(EQUIP_SLOT_SIZE, EQUIP_SLOT_SIZE));

        bool hovered = ImGui::IsItemHovered();
        bool is_drop_target = false;

        // Drop target for equipping items
        if (ImGui::BeginDragDropTarget()) {
            is_drop_target = true;
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_SLOT")) {
                int src_slot = *(const int*)payload->Data;
                // Swap/equip: set selected slot and swap with this equipment slot
                invsel = src_slot;
                cmd_swap(slot.slot_id);
            }
            ImGui::EndDragDropTarget();
        }

        // Drag source for equipped items
        if (has_item && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            int equip_slot = slot.slot_id;
            ImGui::SetDragDropPayload("ITEM_SLOT", &equip_slot, sizeof(int));
            ImGui::Text("%s", slot.name);
            ImGui::EndDragDropSource();
        }

        // Highlight when hovered or drag target
        if (hovered || is_drop_target) {
            ImU32 highlight_color = is_drop_target ? IM_COL32(120, 180, 255, 255) : IM_COL32(100, 150, 200, 255);
            draw_list->AddRect(slot_min, slot_max, highlight_color, 4.0f, 0, 2.0f);

            // Tooltip
            if (!is_drop_target && !ImGui::IsItemActive()) {
                if (has_item) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", slot.name);
                    ImGui::Text("Sprite: %u", item_sprite);
                    ImGui::Text("Right-click to unequip");
                    ImGui::EndTooltip();
                } else {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s (Empty)", slot.name);
                    ImGui::Text("Drag item here to equip");
                    ImGui::EndTooltip();
                }
            }
        }

        // Click handling
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && has_item) {
            cmd_look_inv(slot.slot_id);
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && has_item) {
            cmd_use_inv(slot.slot_id);  // Unequip
        }

        ImGui::PopID();
    }

    ImGui::End();
}
