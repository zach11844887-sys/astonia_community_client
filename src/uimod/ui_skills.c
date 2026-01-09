/*
 * Modern UI Mod - Skills Panel Component
 *
 * Semi-transparent skill tree view with categories.
 * Shows current/base values and cost to raise.
 */

#include <string.h>
#include <stdio.h>
#include "uimod.h"

// ============================================================================
// Skill categories for organized display
// ============================================================================

// Category indices
#define CAT_POWERS     0
#define CAT_ATTRIBUTES 1
#define CAT_FIGHTING   2
#define CAT_SECONDARY  3
#define CAT_MISC       4
#define CAT_SPELLS     5
#define CAT_OTHER      6
#define CAT_COUNT      7

static const char *category_names[CAT_COUNT] = {
    "Powers",
    "Attributes",
    "Fighting",
    "Secondary",
    "Misc",
    "Spells",
    "Other"
};

// Map skill index to category
static int get_skill_category(int v)
{
    if (v >= V_HP && v <= V_MANA) return CAT_POWERS;
    if (v >= V_WIS && v <= V_STR) return CAT_ATTRIBUTES;
    if (v >= V_DAGGER && v <= V_TWOHAND) return CAT_FIGHTING;
    if (v >= V_ARMORSKILL && v <= V_SPEEDSKILL) return CAT_SECONDARY;
    if (v >= V_BARTER && v <= V_STEALTH) return CAT_MISC;
    if (v >= V_BLESS && v <= V_RAGE) return CAT_SPELLS;
    return CAT_OTHER;
}

// ============================================================================
// State
// ============================================================================

static int skills_scroll = 0;
static int skills_visible_lines = 15;
static int skills_category_filter = -1;  // -1 = show all
static int skills_hover_idx = -1;

// ============================================================================
// Initialization
// ============================================================================

void ui_skills_init(void)
{
    skills_scroll = 0;
    skills_category_filter = -1;
    skills_hover_idx = -1;
}

void ui_skills_exit(void)
{
    // Nothing to clean up
}

// ============================================================================
// Rendering helpers
// ============================================================================

static void draw_panel(int x, int y, int w, int h, int alpha)
{
    render_shaded_rect(x, y, x + w, y + h, ui_panel_color, (unsigned short)alpha);

    render_line(x, y, x + w, y, ui_border_color);
    render_line(x, y + h, x + w, y + h, ui_border_color);
    render_line(x, y, x, y + h, ui_border_color);
    render_line(x + w, y, x + w, y + h, ui_border_color);

    render_pixel(x, y, ui_highlight_color);
    render_pixel(x + w, y, ui_highlight_color);
    render_pixel(x, y + h, ui_highlight_color);
    render_pixel(x + w, y + h, ui_highlight_color);
}

static void draw_scrollbar(int x, int y, int h, int total, int visible, int offset)
{
    if (total <= visible) return;

    int bar_height = (visible * h) / total;
    if (bar_height < 10) bar_height = 10;

    int bar_y = y + (offset * (h - bar_height)) / (total - visible);

    render_line(x + 2, y, x + 2, y + h, darkgraycolor);
    render_line(x, bar_y, x + 4, bar_y, graycolor);
    render_line(x, bar_y + bar_height, x + 4, bar_y + bar_height, graycolor);
    render_line(x, bar_y, x, bar_y + bar_height, graycolor);
    render_line(x + 4, bar_y, x + 4, bar_y + bar_height, graycolor);
}

// Get color based on skill value comparison
static unsigned short get_value_color(int current, int base)
{
    if (current > base) return lightgreencolor;
    if (current < base) return lightredcolor;
    return textcolor;
}

// ============================================================================
// Frame rendering
// ============================================================================

void ui_skills_frame(void)
{
    int x = ui_skills_x;
    int y = ui_skills_y;
    int w = ui_skills_width;
    int h = ui_skills_height;

    int line_height = 11;
    skills_visible_lines = (h - 40) / line_height;

    render_push_clip();
    render_more_clip(x, y, x + w, y + h);

    draw_panel(x, y, w, h, ui_panel_alpha);

    // Header
    render_text(x + 5, y + 2, ui_highlight_color, RENDER_TEXT_SMALL, "Skills");
    render_line(x + 1, y + 12, x + w - 1, y + 12, ui_border_color);

    // Category filter buttons
    int cat_x = x + 40;
    int cat_y = y + 2;
    for (int c = 0; c < CAT_COUNT; c++) {
        unsigned short col = (skills_category_filter == c) ? orangecolor : graycolor;
        if (skills_category_filter == -1 && c == 0) {
            // "All" indicator
        }
        char cat_char = category_names[c][0];
        char buf[2] = {cat_char, 0};
        render_text(cat_x + c * 12, cat_y, col, RENDER_TEXT_SMALL, buf);
    }
    // "All" button
    render_text(x + w - 20, cat_y, skills_category_filter == -1 ? orangecolor : graycolor, RENDER_TEXT_SMALL, "*");

    // Build filtered skill list
    int skill_indices[V_MAX];
    int skill_count = 0;

    for (int v = 0; v < V_MAX && v < skltab_cnt; v++) {
        // Skip empty/hidden skills
        if (!skltab || skltab[v].name[0] == '\0') continue;
        if (strcmp(skltab[v].name, "empty") == 0) continue;

        // Skip if no value
        if (value[0][v] == 0 && value[1][v] == 0) continue;

        // Apply category filter
        if (skills_category_filter >= 0) {
            if (get_skill_category(v) != skills_category_filter) continue;
        }

        skill_indices[skill_count++] = v;
    }

    // Calculate scroll bounds
    int max_scroll = skill_count - skills_visible_lines;
    if (max_scroll < 0) max_scroll = 0;
    if (skills_scroll > max_scroll) skills_scroll = max_scroll;

    // Draw skills
    int draw_y = y + 16;
    int draw_x = x + 5;

    for (int i = 0; i < skills_visible_lines && (skills_scroll + i) < skill_count; i++) {
        int v = skill_indices[skills_scroll + i];
        int cur = value[0][v];
        int base = value[1][v];

        // Highlight hovered skill
        if (v == skills_hover_idx) {
            render_shaded_rect(x + 2, draw_y - 1, x + w - 10, draw_y + line_height - 1,
                             ui_highlight_color, 40);
        }

        // Skill name (truncated if needed)
        char name_buf[32];
        strncpy(name_buf, skltab[v].name, 20);
        name_buf[20] = '\0';
        render_text(draw_x, draw_y, textcolor, RENDER_TEXT_SMALL, name_buf);

        // Current value with color coding
        unsigned short val_color = get_value_color(cur, base);
        char val_buf[32];

        if (cur != base) {
            snprintf(val_buf, sizeof(val_buf), "%d(%d)", cur, base);
        } else {
            snprintf(val_buf, sizeof(val_buf), "%d", cur);
        }
        render_text(x + w - 50, draw_y, val_color, RENDER_TEXT_SMALL | RENDER_TEXT_RIGHT, val_buf);

        // Show raise button if skill is raisable
        if (game_skill && v < V_MAX && game_skill[v].cost > 0) {
            render_text(x + w - 12, draw_y, greencolor, RENDER_TEXT_SMALL, "+");
        }

        draw_y += line_height;
    }

    // Draw scrollbar
    if (skill_count > skills_visible_lines) {
        draw_scrollbar(x + w - 8, y + 16, h - 24, skill_count, skills_visible_lines, skills_scroll);
    }

    // Show skill count
    char count_buf[32];
    snprintf(count_buf, sizeof(count_buf), "%d/%d",
             skills_scroll + 1, skill_count > 0 ? skill_count : 1);
    render_text(x + 5, y + h - 10, graycolor, RENDER_TEXT_SMALL, count_buf);

    render_pop_clip();
}

// ============================================================================
// Input handling
// ============================================================================

int ui_skills_click(int mx, int my, int what)
{
    int x = ui_skills_x;
    int y = ui_skills_y;
    int w = ui_skills_width;
    int h = ui_skills_height;

    if (mx < x || mx > x + w || my < y || my > y + h) {
        return 0;
    }

    // Build filtered skill list (same as in frame)
    int skill_indices[V_MAX];
    int skill_count = 0;

    for (int v = 0; v < V_MAX && v < skltab_cnt; v++) {
        if (!skltab || skltab[v].name[0] == '\0') continue;
        if (strcmp(skltab[v].name, "empty") == 0) continue;
        if (value[0][v] == 0 && value[1][v] == 0) continue;

        if (skills_category_filter >= 0) {
            if (get_skill_category(v) != skills_category_filter) continue;
        }

        skill_indices[skill_count++] = v;
    }

    int max_scroll = skill_count - skills_visible_lines;
    if (max_scroll < 0) max_scroll = 0;

    // Category filter clicks (header area)
    if (my < y + 14) {
        int cat_x = x + 40;
        for (int c = 0; c < CAT_COUNT; c++) {
            if (mx >= cat_x + c * 12 && mx < cat_x + c * 12 + 10) {
                skills_category_filter = c;
                skills_scroll = 0;
                return 1;
            }
        }
        // "All" button
        if (mx > x + w - 25) {
            skills_category_filter = -1;
            skills_scroll = 0;
            return 1;
        }
    }

    // Mouse wheel scrolling
    if (what == SDL_MOUM_WHEEL) {
        if (my < y + h/2) {
            if (skills_scroll > 0) skills_scroll--;
        } else {
            if (skills_scroll < max_scroll) skills_scroll++;
        }
        return 1;
    }

    // Scrollbar click
    if (what == SDL_MOUM_LDOWN && mx > x + w - 15 && my > y + 14) {
        int scroll_y = my - (y + 16);
        int scroll_h = h - 24;
        if (scroll_h > 0 && max_scroll > 0) {
            skills_scroll = (scroll_y * max_scroll) / scroll_h;
            if (skills_scroll < 0) skills_scroll = 0;
            if (skills_scroll > max_scroll) skills_scroll = max_scroll;
        }
        return 1;
    }

    // Skill list click
    if (what == SDL_MOUM_LDOWN && my > y + 14) {
        int line_height = 11;
        int clicked_line = (my - (y + 16)) / line_height;
        int skill_idx = skills_scroll + clicked_line;

        if (skill_idx >= 0 && skill_idx < skill_count) {
            int v = skill_indices[skill_idx];

            // Check if clicked on raise button area
            if (mx > x + w - 20 && game_skill && v < V_MAX && game_skill[v].cost > 0) {
                // Send raise skill command
                // Format: CL_RAISE (15) + 2-byte skill index (little-endian)
                unsigned char cmd[3];
                cmd[0] = 15;  // CL_RAISE
                cmd[1] = (unsigned char)(v & 0xFF);
                cmd[2] = (unsigned char)((v >> 8) & 0xFF);
                client_send(cmd, 3);
                return 1;
            }
        }
    }

    return 1;  // Consume click in our area
}

void ui_skills_scroll_up(void)
{
    if (skills_scroll > 0) {
        skills_scroll--;
    }
}

void ui_skills_scroll_down(void)
{
    // Max scroll calculated dynamically in frame
    skills_scroll++;
}
