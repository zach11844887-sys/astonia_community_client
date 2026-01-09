/*
 * Modern UI Mod - Inventory Panel Component
 *
 * Modern grid-based inventory display with item tooltips.
 */

#include <string.h>
#include <stdio.h>
#include "uimod.h"

// Item flags from client.h
#define IF_USE         (1 << 4)
#define IF_WNHEAD      (1 << 5)
#define IF_WNNECK      (1 << 6)
#define IF_WNBODY      (1 << 7)
#define IF_WNARMS      (1 << 8)
#define IF_WNBELT      (1 << 9)
#define IF_WNLEGS      (1 << 10)
#define IF_WNFEET      (1 << 11)
#define IF_WNLHAND     (1 << 12)
#define IF_WNRHAND     (1 << 13)
#define IF_WNCLOAK     (1 << 14)
#define IF_WNLRING     (1 << 15)
#define IF_WNRRING     (1 << 16)
#define IF_WNTWOHANDED (1 << 17)

// ============================================================================
// Configuration
// ============================================================================

#define INV_COLS 5          // Items per row
#define INV_ROWS 8          // Visible rows (scrollable)
#define SLOT_SIZE 36        // Pixel size of each slot
#define SLOT_SPACING 2      // Gap between slots

// ============================================================================
// State
// ============================================================================

static int inv_scroll = 0;      // Scroll offset (rows)
static int inv_hover = -1;      // Hovered slot (-1 = none)
static int inv_selected = -1;   // Selected slot for interaction

// ============================================================================
// Initialization
// ============================================================================

void ui_inventory_init(void)
{
    inv_scroll = 0;
    inv_hover = -1;
    inv_selected = -1;
}

void ui_inventory_exit(void)
{
    // Nothing to clean up
}

// ============================================================================
// Rendering helpers
// ============================================================================

static void draw_panel_header(int x, int y, int w, const char *title)
{
    // Draw header background
    render_line(x, y, x + w, y, ui_border_color);
    render_text(x + 5, y + 2, ui_highlight_color, RENDER_TEXT_SMALL, title);
    render_line(x, y + 12, x + w, y + 12, ui_border_color);
}

static void draw_item_slot(int x, int y, int slot_idx, int is_hovered, int is_selected)
{
    int sprite = item[slot_idx];
    int flags = item_flags[slot_idx];

    // Draw slot background
    unsigned short bg_color = blackcolor;
    if (is_selected) {
        bg_color = ui_highlight_color;
    } else if (is_hovered) {
        bg_color = darkgraycolor;
    }

    // Slot border
    render_rect(x, y, x + SLOT_SIZE, y + SLOT_SIZE, bg_color);
    render_line(x, y, x + SLOT_SIZE, y, ui_border_color);
    render_line(x, y + SLOT_SIZE, x + SLOT_SIZE, y + SLOT_SIZE, ui_border_color);
    render_line(x, y, x, y + SLOT_SIZE, ui_border_color);
    render_line(x + SLOT_SIZE, y, x + SLOT_SIZE, y + SLOT_SIZE, ui_border_color);

    if (sprite != 0) {
        // Draw item sprite centered in slot
        int sprite_x = x + SLOT_SIZE / 2;
        int sprite_y = y + SLOT_SIZE / 2;
        render_sprite(sprite, sprite_x, sprite_y, 15, 1);  // Light=15 (normal), align=1 (center)

        // Draw usability indicator
        if (flags & IF_USE) {
            // Small dot for usable items
            render_pixel(x + SLOT_SIZE - 4, y + 3, greencolor);
        }

        // Draw equipment type indicator
        unsigned short equip_color = 0;
        if (flags & IF_WNHEAD) equip_color = lightbluecolor;
        else if (flags & IF_WNNECK) equip_color = lightorangecolor;
        else if (flags & IF_WNBODY) equip_color = lightgreencolor;
        else if (flags & IF_WNARMS) equip_color = lightredcolor;
        else if (flags & (IF_WNLHAND | IF_WNRHAND)) equip_color = whitecolor;

        if (equip_color) {
            render_pixel(x + 3, y + 3, equip_color);
        }
    }
}

static void draw_tooltip(int x, int y, int slot_idx)
{
    int sprite = item[slot_idx];
    if (sprite == 0) return;

    // Position tooltip to the right of cursor
    int tx = x + 20;
    int ty = y;

    // Keep on screen
    if (tx + 150 > 800) tx = x - 160;
    if (ty + 60 > 600) ty = 600 - 60;

    // Draw tooltip background
    render_rect(tx, ty, tx + 150, ty + 50, blackcolor);
    render_line(tx, ty, tx + 150, ty, ui_border_color);
    render_line(tx, ty + 50, tx + 150, ty + 50, ui_border_color);
    render_line(tx, ty, tx, ty + 50, ui_border_color);
    render_line(tx + 150, ty, tx + 150, ty + 50, ui_border_color);

    // Item info
    render_text_fmt(tx + 5, ty + 5, whitecolor, RENDER_TEXT_SMALL, "Slot %d", slot_idx);
    render_text_fmt(tx + 5, ty + 17, graycolor, RENDER_TEXT_SMALL, "Sprite: %d", sprite);

    // Flags info
    int flags = item_flags[slot_idx];
    if (flags & IF_USE) {
        render_text(tx + 5, ty + 29, greencolor, RENDER_TEXT_SMALL, "Usable");
    }
    if (flags & (IF_WNHEAD | IF_WNNECK | IF_WNBODY | IF_WNARMS | IF_WNBELT |
                 IF_WNLEGS | IF_WNFEET | IF_WNLHAND | IF_WNRHAND | IF_WNCLOAK |
                 IF_WNLRING | IF_WNRRING)) {
        render_text(tx + 50, ty + 29, bluecolor, RENDER_TEXT_SMALL, "Equippable");
    }
}

// ============================================================================
// Frame rendering
// ============================================================================

void ui_inventory_frame(void)
{
    int x = ui_inventory_x;
    int y = ui_inventory_y;

    // Calculate panel size
    int panel_width = INV_COLS * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING + 10;  // +10 for scrollbar
    int panel_height = INV_ROWS * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING + 15; // +15 for header

    // Clip to panel area
    render_push_clip();
    render_more_clip(x, y, x + panel_width, y + panel_height);

    // Draw panel border
    render_line(x, y, x + panel_width, y, ui_border_color);
    render_line(x, y + panel_height, x + panel_width, y + panel_height, ui_border_color);
    render_line(x, y, x, y + panel_height, ui_border_color);
    render_line(x + panel_width, y, x + panel_width, y + panel_height, ui_border_color);

    // Draw header
    draw_panel_header(x, y, panel_width, "Inventory");

    // Draw gold count
    render_text_fmt(x + 70, y + 2, orangecolor, RENDER_TEXT_SMALL, "%dg", gold);

    // Draw inventory slots
    int start_slot = inv_scroll * INV_COLS;
    int slot_x_start = x + SLOT_SPACING;
    int slot_y_start = y + 15 + SLOT_SPACING;

    for (int row = 0; row < INV_ROWS; row++) {
        for (int col = 0; col < INV_COLS; col++) {
            int slot_idx = start_slot + row * INV_COLS + col;

            if (slot_idx >= INVENTORYSIZE) break;

            int sx = slot_x_start + col * (SLOT_SIZE + SLOT_SPACING);
            int sy = slot_y_start + row * (SLOT_SIZE + SLOT_SPACING);

            int is_hovered = (slot_idx == inv_hover);
            int is_selected = (slot_idx == inv_selected);

            draw_item_slot(sx, sy, slot_idx, is_hovered, is_selected);
        }
    }

    // Draw scrollbar if needed
    int max_scroll = (INVENTORYSIZE / INV_COLS) - INV_ROWS + 1;
    if (max_scroll > 0) {
        int sb_x = x + panel_width - 8;
        int sb_y = y + 15;
        int sb_h = panel_height - 17;

        // Track
        render_line(sb_x + 2, sb_y, sb_x + 2, sb_y + sb_h, darkgraycolor);

        // Thumb
        int thumb_h = (INV_ROWS * sb_h) / (INVENTORYSIZE / INV_COLS);
        if (thumb_h < 10) thumb_h = 10;
        int thumb_y = sb_y + (inv_scroll * (sb_h - thumb_h)) / max_scroll;

        render_rect(sb_x, thumb_y, sb_x + 4, thumb_y + thumb_h, graycolor);
    }

    // Draw tooltip for hovered slot
    if (inv_hover >= 0 && inv_hover < INVENTORYSIZE && item[inv_hover] != 0) {
        // Get actual screen position of hovered slot
        int hover_row = (inv_hover - start_slot) / INV_COLS;
        int hover_col = (inv_hover - start_slot) % INV_COLS;
        int hx = slot_x_start + hover_col * (SLOT_SIZE + SLOT_SPACING) + SLOT_SIZE;
        int hy = slot_y_start + hover_row * (SLOT_SIZE + SLOT_SPACING);

        draw_tooltip(hx, hy, inv_hover);
    }

    // Draw item count
    int filled = 0;
    for (int i = 0; i < INVENTORYSIZE; i++) {
        if (item[i] != 0) filled++;
    }
    render_text_fmt(x + 5, y + panel_height - 10, graycolor, RENDER_TEXT_SMALL,
                    "%d/%d", filled, INVENTORYSIZE);

    render_pop_clip();
}

// ============================================================================
// Input handling
// ============================================================================

int ui_inventory_click(int mx, int my, int what)
{
    int x = ui_inventory_x;
    int y = ui_inventory_y;
    int panel_width = INV_COLS * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING + 10;
    int panel_height = INV_ROWS * (SLOT_SIZE + SLOT_SPACING) + SLOT_SPACING + 15;

    // Check if click is within panel
    if (mx < x || mx > x + panel_width || my < y || my > y + panel_height) {
        inv_hover = -1;
        return 0;
    }

    // Calculate which slot is being interacted with
    int slot_x_start = x + SLOT_SPACING;
    int slot_y_start = y + 15 + SLOT_SPACING;

    int col = (mx - slot_x_start) / (SLOT_SIZE + SLOT_SPACING);
    int row = (my - slot_y_start) / (SLOT_SIZE + SLOT_SPACING);

    if (col >= 0 && col < INV_COLS && row >= 0 && row < INV_ROWS) {
        int slot_idx = (inv_scroll * INV_COLS) + row * INV_COLS + col;

        if (slot_idx >= 0 && slot_idx < INVENTORYSIZE) {
            inv_hover = slot_idx;

            if (what == SDL_MOUM_LDOWN) {
                // Left click - select/use item
                if (inv_selected == slot_idx) {
                    // Double-click to use
                    inv_selected = -1;
                    // TODO: Send use command to server
                } else {
                    inv_selected = slot_idx;
                }
                return 1;
            }

            if (what == SDL_MOUM_RDOWN) {
                // Right click - context menu or look
                // TODO: Implement context menu
                return 1;
            }
        }
    } else {
        inv_hover = -1;
    }

    // Handle scroll wheel
    if (what == SDL_MOUM_WHEEL) {
        int max_scroll = (INVENTORYSIZE / INV_COLS) - INV_ROWS + 1;
        if (max_scroll < 0) max_scroll = 0;

        // Determine scroll direction from mouse position
        if (my < y + panel_height / 2) {
            // Scroll up
            if (inv_scroll > 0) inv_scroll--;
        } else {
            // Scroll down
            if (inv_scroll < max_scroll) inv_scroll++;
        }
        return 1;
    }

    // Scrollbar click
    if (mx > x + panel_width - 10) {
        if (what == SDL_MOUM_LDOWN) {
            int sb_y = y + 15;
            int sb_h = panel_height - 17;
            int max_scroll = (INVENTORYSIZE / INV_COLS) - INV_ROWS + 1;
            if (max_scroll > 0 && sb_h > 0) {
                inv_scroll = ((my - sb_y) * max_scroll) / sb_h;
                if (inv_scroll < 0) inv_scroll = 0;
                if (inv_scroll > max_scroll) inv_scroll = max_scroll;
            }
            return 1;
        }
    }

    return 1;  // Consume click if in panel
}

// ============================================================================
// Public API
// ============================================================================

void ui_inventory_scroll_up(void)
{
    if (inv_scroll > 0) inv_scroll--;
}

void ui_inventory_scroll_down(void)
{
    int max_scroll = (INVENTORYSIZE / INV_COLS) - INV_ROWS + 1;
    if (inv_scroll < max_scroll) inv_scroll++;
}

int ui_inventory_get_selected(void)
{
    return inv_selected;
}

void ui_inventory_clear_selection(void)
{
    inv_selected = -1;
}
