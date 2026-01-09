/*
 * Modern UI Mod - Quest Log Panel Component
 *
 * Semi-transparent quest log with filtering and progress tracking.
 * Shows open/completed quests with details.
 */

#include <string.h>
#include <stdio.h>
#include "uimod.h"

// Quest status flags (from client)
#define QF_OPEN 1
#define QF_DONE 2

// ============================================================================
// State
// ============================================================================

static int quests_scroll = 0;
static int quests_visible = 6;
static int quests_filter = 0;      // 0=all, 1=open, 2=done
static int quests_hover_idx = -1;
static int quests_requested = 0;   // Track if we've requested quest data

// Sorted quest list
static int quest_list[MAXQUEST];
static int quest_count = 0;

// ============================================================================
// Initialization
// ============================================================================

void ui_quests_init(void)
{
    quests_scroll = 0;
    quests_filter = 0;
    quests_hover_idx = -1;
    quests_requested = 0;
    quest_count = 0;
}

void ui_quests_exit(void)
{
    // Nothing to clean up
}

// ============================================================================
// Quest list management
// ============================================================================

// Calculate exp percentage for repeated quests
static int quest_exp_percent(int done_count)
{
    float val = 100.0f;
    for (int n = 0; n < done_count; n++) {
        val *= 0.825f;
    }
    return (int)val;
}

// Comparison for sorting quests by level
static int quest_compare(const void *a, const void *b)
{
    const int *va = (const int *)a;
    const int *vb = (const int *)b;

    // Sort by min level descending, then max level
    if (game_questlog[*va].minlevel != game_questlog[*vb].minlevel) {
        return game_questlog[*vb].minlevel - game_questlog[*va].minlevel;
    }
    if (game_questlog[*va].maxlevel != game_questlog[*vb].maxlevel) {
        return game_questlog[*vb].maxlevel - game_questlog[*va].maxlevel;
    }
    return *va - *vb;
}

// Rebuild sorted/filtered quest list
static void rebuild_quest_list(void)
{
    quest_count = 0;

    if (!game_questlog || !game_questcount) return;

    // First pass: open quests
    for (int n = 0; n < *game_questcount && n < MAXQUEST; n++) {
        if (quest[n].flags == QF_OPEN && quest[n].done < 10) {
            if (quests_filter == 0 || quests_filter == 1) {
                quest_list[quest_count++] = n;
            }
        }
    }

    // Second pass: done quests
    for (int n = 0; n < *game_questcount && n < MAXQUEST; n++) {
        if (quest[n].flags == QF_DONE) {
            if (quests_filter == 0 || quests_filter == 2) {
                quest_list[quest_count++] = n;
            }
        }
    }

    // Sort by level
    if (quest_count > 0) {
        // Simple bubble sort to avoid qsort linkage
        for (int i = 0; i < quest_count - 1; i++) {
            for (int j = 0; j < quest_count - i - 1; j++) {
                if (quest_compare(&quest_list[j], &quest_list[j+1]) > 0) {
                    int tmp = quest_list[j];
                    quest_list[j] = quest_list[j+1];
                    quest_list[j+1] = tmp;
                }
            }
        }
    }
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

// ============================================================================
// Frame rendering
// ============================================================================

void ui_quests_frame(void)
{
    int x = ui_quests_x;
    int y = ui_quests_y;
    int w = ui_quests_width;
    int h = ui_quests_height;

    // Each quest entry takes about 32 pixels
    int entry_height = 32;
    quests_visible = (h - 30) / entry_height;
    if (quests_visible < 1) quests_visible = 1;

    render_push_clip();
    render_more_clip(x, y, x + w, y + h);

    draw_panel(x, y, w, h, ui_panel_alpha);

    // Header
    render_text(x + 5, y + 2, ui_highlight_color, RENDER_TEXT_SMALL, "Quest Log");
    render_line(x + 1, y + 12, x + w - 1, y + 12, ui_border_color);

    // Filter tabs
    unsigned short all_col = (quests_filter == 0) ? orangecolor : graycolor;
    unsigned short open_col = (quests_filter == 1) ? orangecolor : graycolor;
    unsigned short done_col = (quests_filter == 2) ? orangecolor : graycolor;

    render_text(x + 60, y + 2, all_col, RENDER_TEXT_SMALL, "All");
    render_text(x + 85, y + 2, open_col, RENDER_TEXT_SMALL, "Open");
    render_text(x + 115, y + 2, done_col, RENDER_TEXT_SMALL, "Done");

    // Rebuild quest list
    rebuild_quest_list();

    // Calculate scroll bounds
    int max_scroll = quest_count - quests_visible;
    if (max_scroll < 0) max_scroll = 0;
    if (quests_scroll > max_scroll) quests_scroll = max_scroll;

    if (quest_count == 0) {
        render_text(x + w/2, y + h/2 - 5, graycolor, RENDER_TEXT_SMALL | RENDER_TEXT_CENTER,
                   "No quests to display");
        render_pop_clip();
        return;
    }

    // Draw quest entries
    int draw_y = y + 16;

    for (int i = 0; i < quests_visible && (quests_scroll + i) < quest_count; i++) {
        int q = quest_list[quests_scroll + i];

        // Background for hovered entry
        if (q == quests_hover_idx) {
            render_shaded_rect(x + 2, draw_y - 1, x + w - 10, draw_y + entry_height - 2,
                             ui_highlight_color, 40);
        }

        // Quest status indicator
        unsigned short status_col;
        const char *status_txt;
        if (quest[q].flags == QF_OPEN) {
            status_col = lightgreencolor;
            status_txt = "[Open]";
        } else {
            status_col = graycolor;
            status_txt = "[Done]";
        }

        // Quest name
        char name_buf[40];
        strncpy(name_buf, game_questlog[q].name, 35);
        name_buf[35] = '\0';
        render_text(x + 5, draw_y, whitecolor, RENDER_TEXT_SMALL, name_buf);

        // Status
        render_text(x + w - 40, draw_y, status_col, RENDER_TEXT_SMALL | RENDER_TEXT_RIGHT, status_txt);

        // Second line: giver and area
        char info_buf[60];
        snprintf(info_buf, sizeof(info_buf), "From: %s in %s (Lv %d-%d)",
                game_questlog[q].giver, game_questlog[q].area,
                game_questlog[q].minlevel, game_questlog[q].maxlevel);
        render_text(x + 10, draw_y + 10, graycolor, RENDER_TEXT_SMALL, info_buf);

        // Third line: completion info
        char done_buf[60];
        if (game_questlog[q].flags & QLF_REPEATABLE) {
            snprintf(done_buf, sizeof(done_buf), "Done %d time%s (%d%% exp)",
                    quest[q].done, quest[q].done != 1 ? "s" : "",
                    quest_exp_percent(quest[q].done));

            // Show re-open option for repeatable quests that are done
            if ((quest[q].flags == QF_DONE) && quest[q].done < 10) {
                render_text(x + w - 12, draw_y + 10, lightbluecolor, RENDER_TEXT_SMALL, "R");
            }
        } else if (game_questlog[q].flags & QLF_XREPEAT) {
            snprintf(done_buf, sizeof(done_buf), "Done %d time%s (Junk Item)",
                    quest[q].done, quest[q].done != 1 ? "s" : "");
        } else {
            snprintf(done_buf, sizeof(done_buf), "Done %d time (not repeatable)",
                    quest[q].done);
        }
        render_text(x + 10, draw_y + 20, darkgraycolor, RENDER_TEXT_SMALL, done_buf);

        draw_y += entry_height;
    }

    // Scrollbar
    if (quest_count > quests_visible) {
        draw_scrollbar(x + w - 8, y + 16, h - 24, quest_count, quests_visible, quests_scroll);
    }

    // Quest count footer
    char count_buf[32];
    snprintf(count_buf, sizeof(count_buf), "%d quest%s",
             quest_count, quest_count != 1 ? "s" : "");
    render_text(x + 5, y + h - 10, graycolor, RENDER_TEXT_SMALL, count_buf);

    render_pop_clip();
}

// ============================================================================
// Input handling
// ============================================================================

int ui_quests_click(int mx, int my, int what)
{
    int x = ui_quests_x;
    int y = ui_quests_y;
    int w = ui_quests_width;
    int h = ui_quests_height;

    if (mx < x || mx > x + w || my < y || my > y + h) {
        return 0;
    }

    int max_scroll = quest_count - quests_visible;
    if (max_scroll < 0) max_scroll = 0;

    // Filter tab clicks
    if (my < y + 14) {
        if (mx >= x + 55 && mx < x + 80) {
            quests_filter = 0;  // All
            quests_scroll = 0;
            return 1;
        }
        if (mx >= x + 80 && mx < x + 110) {
            quests_filter = 1;  // Open
            quests_scroll = 0;
            return 1;
        }
        if (mx >= x + 110 && mx < x + 145) {
            quests_filter = 2;  // Done
            quests_scroll = 0;
            return 1;
        }
    }

    // Mouse wheel scrolling
    if (what == SDL_MOUM_WHEEL) {
        if (my < y + h/2) {
            if (quests_scroll > 0) quests_scroll--;
        } else {
            if (quests_scroll < max_scroll) quests_scroll++;
        }
        return 1;
    }

    // Scrollbar click
    if (what == SDL_MOUM_LDOWN && mx > x + w - 15 && my > y + 14) {
        int scroll_y = my - (y + 16);
        int scroll_h = h - 24;
        if (scroll_h > 0 && max_scroll > 0) {
            quests_scroll = (scroll_y * max_scroll) / scroll_h;
            if (quests_scroll < 0) quests_scroll = 0;
            if (quests_scroll > max_scroll) quests_scroll = max_scroll;
        }
        return 1;
    }

    // Quest entry click
    if (what == SDL_MOUM_LDOWN && my > y + 14) {
        int entry_height = 32;
        int clicked_idx = (my - (y + 16)) / entry_height;
        int quest_idx = quests_scroll + clicked_idx;

        if (quest_idx >= 0 && quest_idx < quest_count) {
            int q = quest_list[quest_idx];

            // Check for re-open click (right side of entry)
            if (mx > x + w - 20) {
                if ((game_questlog[q].flags & QLF_REPEATABLE) &&
                    (quest[q].flags == QF_DONE) && quest[q].done < 10) {
                    // Send reopen quest command (CL_REOPENQUEST = 41)
                    unsigned char cmd[2];
                    cmd[0] = 41;  // CL_REOPENQUEST
                    cmd[1] = (unsigned char)q;
                    client_send(cmd, 2);
                    addline("Requesting to re-open quest: %s", game_questlog[q].name);
                    return 1;
                }
            }

            // Regular click - show quest info in chat
            addline("Quest: %s", game_questlog[q].name);
            addline("  From: %s in %s", game_questlog[q].giver, game_questlog[q].area);
            addline("  Level: %d-%d, Exp: %d",
                   game_questlog[q].minlevel, game_questlog[q].maxlevel,
                   game_questlog[q].exp);
        }
        return 1;
    }

    return 1;  // Consume click in our area
}

void ui_quests_scroll_up(void)
{
    if (quests_scroll > 0) {
        quests_scroll--;
    }
}

void ui_quests_scroll_down(void)
{
    quests_scroll++;
}
