/*
 * Modern UI Mod - Equipment Panel Component
 *
 * Paperdoll-style equipment display showing worn items
 * in their relative body positions.
 */

#include <string.h>
#include <stdio.h>
#include "uimod.h"

// Item flags
#define IF_WNTWOHANDED (1 << 17)

// ============================================================================
// Equipment slot definitions
// ============================================================================

// weatab[12] = {9, 6, 8, 11, 0, 1, 2, 4, 5, 3, 7, 10}
// Maps UI slot index to inventory index

// Slot meanings by UI index:
// 0: Right Ring  -> inv[9]
// 1: Right Hand  -> inv[6]
// 2: Left Hand   -> inv[8]
// 3: Left Ring   -> inv[11]
// 4: Neck        -> inv[0]
// 5: Head        -> inv[1]
// 6: Back/Cloak  -> inv[2]
// 7: Body        -> inv[4]
// 8: Belt        -> inv[5]
// 9: Arms        -> inv[3]
// 10: Legs       -> inv[7]
// 11: Feet       -> inv[10]

#define EQ_SLOT_RRING  0
#define EQ_SLOT_RHAND  1
#define EQ_SLOT_LHAND  2
#define EQ_SLOT_LRING  3
#define EQ_SLOT_NECK   4
#define EQ_SLOT_HEAD   5
#define EQ_SLOT_CLOAK  6
#define EQ_SLOT_BODY   7
#define EQ_SLOT_BELT   8
#define EQ_SLOT_ARMS   9
#define EQ_SLOT_LEGS   10
#define EQ_SLOT_FEET   11

static const char *slot_names[12] = {
    "R.Ring", "R.Hand", "L.Hand", "L.Ring",
    "Neck", "Head", "Cloak", "Body",
    "Belt", "Arms", "Legs", "Feet"
};

// Paperdoll layout - positions relative to panel origin
// Layout designed to resemble a humanoid figure
//
//         [HEAD]
//    [CLOAK]  [NECK]
//  [R.HAND] [BODY] [L.HAND]
//  [R.RING] [BELT] [L.RING]
//          [ARMS]
//          [LEGS]
//          [FEET]

struct slot_pos {
    int x, y;       // Position offset from panel origin
    int w, h;       // Slot size
};

static const struct slot_pos slot_layout[12] = {
    // R.Ring (0)
    {10, 110, 28, 28},
    // R.Hand (1)
    {10, 70, 32, 32},
    // L.Hand (2)
    {110, 70, 32, 32},
    // L.Ring (3)
    {115, 110, 28, 28},
    // Neck (4)
    {95, 25, 28, 28},
    // Head (5)
    {60, 5, 34, 34},
    // Cloak (6)
    {25, 25, 28, 28},
    // Body (7)
    {55, 50, 44, 44},
    // Belt (8)
    {55, 100, 44, 24},
    // Arms (9)
    {55, 130, 44, 24},
    // Legs (10)
    {55, 160, 44, 34},
    // Feet (11)
    {55, 200, 44, 28},
};

// ============================================================================
// State
// ============================================================================

static int eq_hover = -1;
static int eq_selected = -1;

// ============================================================================
// Initialization
// ============================================================================

void ui_equipment_init(void)
{
    eq_hover = -1;
    eq_selected = -1;
}

void ui_equipment_exit(void)
{
    // Nothing to clean up
}

// ============================================================================
// Rendering helpers
// ============================================================================

static void draw_eq_slot(int px, int py, int slot_idx, int is_hovered, int is_selected)
{
    const struct slot_pos *pos = &slot_layout[slot_idx];
    int x = px + pos->x;
    int y = py + pos->y;
    int w = pos->w;
    int h = pos->h;

    // Get inventory index for this equipment slot
    int inv_idx = weatab[slot_idx];
    int sprite = item[inv_idx];

    // Slot background color
    unsigned short bg_color = blackcolor;
    if (is_selected) {
        bg_color = ui_highlight_color;
    } else if (is_hovered) {
        bg_color = darkgraycolor;
    } else if (sprite == 0) {
        // Empty slot - slightly different shade
        bg_color = 0x0842;  // Very dark gray
    }

    // Draw slot background
    render_rect(x, y, x + w, y + h, bg_color);

    // Draw border
    unsigned short border = ui_border_color;
    if (is_hovered) border = graycolor;
    if (is_selected) border = whitecolor;

    render_line(x, y, x + w, y, border);
    render_line(x, y + h, x + w, y + h, border);
    render_line(x, y, x, y + h, border);
    render_line(x + w, y, x + w, y + h, border);

    // Draw item sprite if equipped
    if (sprite != 0) {
        // Center sprite in slot
        int sx = x + w / 2;
        int sy = y + h / 2;
        render_sprite(sprite, sx, sy, 15, 1);  // Light=15, align=center
    }

    // Draw slot label when empty or hovered
    if (sprite == 0 || is_hovered) {
        // Small label at top of slot
        unsigned short label_color = (sprite == 0) ? darkgraycolor : graycolor;
        render_text(x + 2, y + 2, label_color, RENDER_TEXT_SMALL, slot_names[slot_idx]);
    }
}

static void draw_character_silhouette(int x, int y)
{
    // Draw a simple character silhouette as background reference
    // This helps players understand the paperdoll layout

    unsigned short sil_color = 0x1084;  // Very dark gray

    // Head (circle approximation)
    render_line(x + 70, y + 8, x + 84, y + 8, sil_color);
    render_line(x + 66, y + 12, x + 88, y + 12, sil_color);
    render_line(x + 66, y + 28, x + 88, y + 28, sil_color);
    render_line(x + 70, y + 32, x + 84, y + 32, sil_color);

    // Neck
    render_line(x + 73, y + 32, x + 73, y + 45, sil_color);
    render_line(x + 81, y + 32, x + 81, y + 45, sil_color);

    // Body (torso)
    render_line(x + 55, y + 45, x + 99, y + 45, sil_color);
    render_line(x + 52, y + 95, x + 102, y + 95, sil_color);
    render_line(x + 55, y + 45, x + 52, y + 95, sil_color);
    render_line(x + 99, y + 45, x + 102, y + 95, sil_color);

    // Arms
    render_line(x + 35, y + 50, x + 55, y + 50, sil_color);
    render_line(x + 99, y + 50, x + 119, y + 50, sil_color);
    render_line(x + 35, y + 95, x + 50, y + 95, sil_color);
    render_line(x + 104, y + 95, x + 119, y + 95, sil_color);

    // Legs
    render_line(x + 60, y + 95, x + 60, y + 195, sil_color);
    render_line(x + 72, y + 95, x + 72, y + 195, sil_color);
    render_line(x + 82, y + 95, x + 82, y + 195, sil_color);
    render_line(x + 94, y + 95, x + 94, y + 195, sil_color);
}

static void draw_tooltip(int mx, int my, int slot_idx)
{
    int inv_idx = weatab[slot_idx];
    int sprite = item[inv_idx];
    if (sprite == 0) return;

    // Position tooltip
    int tx = mx + 15;
    int ty = my;
    if (tx + 120 > 800) tx = mx - 125;
    if (ty + 40 > 600) ty = 600 - 40;

    // Draw tooltip background
    render_rect(tx, ty, tx + 120, ty + 35, blackcolor);
    render_line(tx, ty, tx + 120, ty, ui_border_color);
    render_line(tx, ty + 35, tx + 120, ty + 35, ui_border_color);
    render_line(tx, ty, tx, ty + 35, ui_border_color);
    render_line(tx + 120, ty, tx + 120, ty + 35, ui_border_color);

    // Slot name
    render_text(tx + 5, ty + 5, whitecolor, RENDER_TEXT_SMALL, slot_names[slot_idx]);

    // Sprite info
    render_text_fmt(tx + 5, ty + 17, graycolor, RENDER_TEXT_SMALL, "Sprite: %d", sprite);
}

// ============================================================================
// Frame rendering
// ============================================================================

void ui_equipment_frame(void)
{
    int x = ui_equipment_x;
    int y = ui_equipment_y;
    int panel_width = 155;
    int panel_height = 240;

    // Clip to panel area
    render_push_clip();
    render_more_clip(x, y, x + panel_width, y + panel_height);

    // Draw panel border
    render_line(x, y, x + panel_width, y, ui_border_color);
    render_line(x, y + panel_height, x + panel_width, y + panel_height, ui_border_color);
    render_line(x, y, x, y + panel_height, ui_border_color);
    render_line(x + panel_width, y, x + panel_width, y + panel_height, ui_border_color);

    // Draw title
    render_text(x + 5, y + 2, ui_highlight_color, RENDER_TEXT_SMALL, "Equipment");

    // Draw title border
    render_line(x + 1, y + 12, x + panel_width - 1, y + 12, ui_border_color);

    // Offset content area
    int cx = x;
    int cy = y + 15;

    // Draw character silhouette
    draw_character_silhouette(cx, cy);

    // Draw equipment slots
    for (int i = 0; i < 12; i++) {
        int is_hovered = (eq_hover == i);
        int is_selected = (eq_selected == i);

        // Handle two-handed weapon
        if (i == EQ_SLOT_LHAND) {
            int rhand_inv = weatab[EQ_SLOT_RHAND];
            if (item[rhand_inv] != 0 && (item_flags[rhand_inv] & IF_WNTWOHANDED)) {
                // Right hand has two-handed weapon, left hand shows as blocked
                // Draw a grayed-out slot
                const struct slot_pos *pos = &slot_layout[i];
                int sx = cx + pos->x;
                int sy = cy + pos->y;
                render_rect(sx, sy, sx + pos->w, sy + pos->h, 0x1084);
                render_text(sx + 2, sy + 2, darkgraycolor, RENDER_TEXT_SMALL, "2H");
                continue;
            }
        }

        draw_eq_slot(cx, cy, i, is_hovered, is_selected);
    }

    // Draw character stats summary at bottom
    int stats_y = y + panel_height - 25;
    render_line(x + 1, stats_y - 3, x + panel_width - 1, stats_y - 3, ui_border_color);

    // Health/Mana/Endurance bars
    int bar_w = 45;
    int bar_h = 6;

    // HP bar
    render_text(x + 5, stats_y, healthcolor, RENDER_TEXT_SMALL, "HP");
    int hp_fill = (hp > 0) ? (hp * bar_w / 100) : 0;
    if (hp_fill > bar_w) hp_fill = bar_w;
    render_rect(x + 20, stats_y + 1, x + 20 + bar_w, stats_y + 1 + bar_h, darkgraycolor);
    if (hp_fill > 0) {
        render_rect(x + 20, stats_y + 1, x + 20 + hp_fill, stats_y + 1 + bar_h, healthcolor);
    }

    // Mana bar
    render_text(x + 75, stats_y, manacolor, RENDER_TEXT_SMALL, "MP");
    int mp_fill = (mana > 0) ? (mana * bar_w / 100) : 0;
    if (mp_fill > bar_w) mp_fill = bar_w;
    render_rect(x + 90, stats_y + 1, x + 90 + bar_w, stats_y + 1 + bar_h, darkgraycolor);
    if (mp_fill > 0) {
        render_rect(x + 90, stats_y + 1, x + 90 + mp_fill, stats_y + 1 + bar_h, manacolor);
    }

    // Endurance bar
    render_text(x + 5, stats_y + 10, endurancecolor, RENDER_TEXT_SMALL, "EN");
    int en_fill = (endurance > 0) ? (endurance * bar_w / 100) : 0;
    if (en_fill > bar_w) en_fill = bar_w;
    render_rect(x + 20, stats_y + 11, x + 20 + bar_w, stats_y + 11 + bar_h, darkgraycolor);
    if (en_fill > 0) {
        render_rect(x + 20, stats_y + 11, x + 20 + en_fill, stats_y + 11 + bar_h, endurancecolor);
    }

    // Shield bar (if active)
    if (lifeshield > 0) {
        render_text(x + 75, stats_y + 10, shieldcolor, RENDER_TEXT_SMALL, "SH");
        int sh_fill = (lifeshield * bar_w / 100);
        if (sh_fill > bar_w) sh_fill = bar_w;
        render_rect(x + 90, stats_y + 11, x + 90 + bar_w, stats_y + 11 + bar_h, darkgraycolor);
        render_rect(x + 90, stats_y + 11, x + 90 + sh_fill, stats_y + 11 + bar_h, shieldcolor);
    }

    render_pop_clip();

    // Draw tooltip outside clip region
    if (eq_hover >= 0 && eq_hover < 12) {
        // Use mouse position for tooltip (would need mouse tracking)
        // For now, position relative to slot
        const struct slot_pos *pos = &slot_layout[eq_hover];
        draw_tooltip(x + pos->x + pos->w, y + 15 + pos->y, eq_hover);
    }
}

// ============================================================================
// Input handling
// ============================================================================

int ui_equipment_click(int mx, int my, int what)
{
    int x = ui_equipment_x;
    int y = ui_equipment_y;
    int panel_width = 155;
    int panel_height = 240;

    // Check if within panel
    if (mx < x || mx > x + panel_width || my < y || my > y + panel_height) {
        eq_hover = -1;
        return 0;
    }

    // Content area offset
    int cx = x;
    int cy = y + 15;

    // Check which slot was clicked
    eq_hover = -1;
    for (int i = 0; i < 12; i++) {
        const struct slot_pos *pos = &slot_layout[i];
        int sx = cx + pos->x;
        int sy = cy + pos->y;

        if (mx >= sx && mx <= sx + pos->w && my >= sy && my <= sy + pos->h) {
            eq_hover = i;

            if (what == SDL_MOUM_LDOWN) {
                if (eq_selected == i) {
                    // Double-click to unequip
                    eq_selected = -1;
                    // TODO: Send unequip command
                } else {
                    eq_selected = i;
                }
                return 1;
            }

            if (what == SDL_MOUM_RDOWN) {
                // Right-click to look/examine
                // TODO: Send look command
                return 1;
            }

            break;
        }
    }

    return 1;  // Consume click if in panel
}

// ============================================================================
// Public API
// ============================================================================

int ui_equipment_get_selected(void)
{
    return eq_selected;
}

void ui_equipment_clear_selection(void)
{
    eq_selected = -1;
}
