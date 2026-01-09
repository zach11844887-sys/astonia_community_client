/*
 * Modern UI Mod - Chat Panel Component
 *
 * Semi-transparent chat window with modern styling.
 * Uses the client's exported text buffer for chat history.
 */

#include <string.h>
#include <stdio.h>
#include "uimod.h"

// ============================================================================
// State
// ============================================================================

static int chat_scroll = 0;     // Scroll offset (0 = newest at bottom)
static int chat_visible_lines = 12;

// ============================================================================
// Initialization
// ============================================================================

void ui_chat_init(void)
{
    chat_scroll = 0;
}

void ui_chat_exit(void)
{
    // Nothing to clean up
}

// ============================================================================
// Rendering helpers
// ============================================================================

// Draw a panel background with border
static void draw_panel(int x, int y, int w, int h, int alpha)
{
    // Semi-transparent background
    render_shaded_rect(x, y, x + w, y + h, ui_panel_color, (unsigned short)alpha);

    // Draw border
    render_line(x, y, x + w, y, ui_border_color);           // Top
    render_line(x, y + h, x + w, y + h, ui_border_color);   // Bottom
    render_line(x, y, x, y + h, ui_border_color);           // Left
    render_line(x + w, y, x + w, y + h, ui_border_color);   // Right

    // Corner accents
    render_pixel(x, y, ui_highlight_color);
    render_pixel(x + w, y, ui_highlight_color);
    render_pixel(x, y + h, ui_highlight_color);
    render_pixel(x + w, y + h, ui_highlight_color);
}

// Draw a scrollbar
static void draw_scrollbar(int x, int y, int h, int total, int visible, int offset)
{
    if (total <= visible) return;  // No scrollbar needed

    int bar_height = (visible * h) / total;
    if (bar_height < 10) bar_height = 10;

    int bar_y = y + (offset * (h - bar_height)) / (total - visible);

    // Track
    render_line(x + 2, y, x + 2, y + h, darkgraycolor);

    // Thumb
    render_line(x, bar_y, x + 4, bar_y, graycolor);
    render_line(x, bar_y + bar_height, x + 4, bar_y + bar_height, graycolor);
    render_line(x, bar_y, x, bar_y + bar_height, graycolor);
    render_line(x + 4, bar_y, x + 4, bar_y + bar_height, graycolor);
}

// Get color from palette index
static unsigned short get_text_color(unsigned char color_idx)
{
    if (color_idx == 0) return textcolor;
    if (color_idx < 16) {
        // Use palette if available
        return palette[color_idx] ? palette[color_idx] : textcolor;
    }
    return textcolor;
}

// ============================================================================
// Frame rendering
// ============================================================================

void ui_chat_frame(void)
{
    int x = ui_chat_x;
    int y = ui_chat_y;
    int w = ui_chat_width;
    int h = ui_chat_height;

    // Calculate visible lines
    int line_height = 10;  // Small font
    chat_visible_lines = (h - 20) / line_height;  // Leave margin for header/border

    // Clip to our panel area
    render_push_clip();
    render_more_clip(x, y, x + w, y + h);

    // Draw panel background and border
    draw_panel(x, y, w, h, ui_panel_alpha);

    // Draw header
    render_text(x + 5, y + 2, ui_highlight_color, RENDER_TEXT_SMALL, "Chat");
    render_line(x + 1, y + 12, x + w - 1, y + 12, ui_border_color);

    // Draw messages from client's text buffer
    int msg_y = y + 15;
    int msg_x = x + 5;

    // Calculate how many lines are available in the circular buffer
    int total_lines = textlines;
    if (total_lines > MAXTEXTLINES) total_lines = MAXTEXTLINES;

    if (total_lines > 0) {
        // Calculate which lines to display
        int lines_to_show = chat_visible_lines;
        if (lines_to_show > total_lines) {
            lines_to_show = total_lines;
        }

        // Apply scroll offset
        int scroll_pos = chat_scroll;
        int max_scroll = total_lines - lines_to_show;
        if (max_scroll < 0) max_scroll = 0;
        if (scroll_pos > max_scroll) scroll_pos = max_scroll;

        // Find starting line (accounting for circular buffer)
        // textdisplayline is the first visible line in the default UI
        // textnextline is where the next message will be written
        int start_line = (textdisplayline + scroll_pos) % MAXTEXTLINES;

        for (int i = 0; i < lines_to_show; i++) {
            int line_idx = (start_line + i) % MAXTEXTLINES;
            int pos = line_idx * MAXTEXTLETTERS;

            // Build the line string and determine color
            char line_buf[MAXTEXTLETTERS + 1];
            int buf_idx = 0;
            unsigned short line_color = textcolor;
            int got_color = 0;

            for (int j = 0; j < MAXTEXTLETTERS && text[pos + j].c != '\0'; j++) {
                char c = text[pos + j].c;

                // Handle color codes (characters 1-31 are color codes)
                if (c > 0 && c < 32) {
                    if (!got_color) {
                        line_color = get_text_color(text[pos + j].color);
                        got_color = 1;
                    }
                    continue;
                }

                // Get color from first character with color info
                if (!got_color && text[pos + j].color) {
                    line_color = get_text_color(text[pos + j].color);
                    got_color = 1;
                }

                line_buf[buf_idx++] = c;
            }
            line_buf[buf_idx] = '\0';

            if (buf_idx > 0) {
                render_text(msg_x, msg_y + i * line_height, line_color, RENDER_TEXT_SMALL, line_buf);
            }
        }

        // Draw scrollbar if needed
        if (total_lines > chat_visible_lines) {
            draw_scrollbar(x + w - 8, y + 15, h - 20, total_lines, chat_visible_lines, scroll_pos);
        }
    } else {
        // No messages yet
        render_text(msg_x, msg_y, graycolor, RENDER_TEXT_SMALL, "(No messages)");
    }

    // Scroll indicator
    if (chat_scroll > 0) {
        render_text(x + w - 20, y + 2, orangecolor, RENDER_TEXT_SMALL, "^");
    }

    render_pop_clip();
}

// ============================================================================
// Input handling
// ============================================================================

int ui_chat_click(int mx, int my, int what)
{
    int x = ui_chat_x;
    int y = ui_chat_y;
    int w = ui_chat_width;
    int h = ui_chat_height;

    // Check if click is within our panel
    if (mx < x || mx > x + w || my < y || my > y + h) {
        return 0;  // Not in our area
    }

    int total_lines = textlines;
    if (total_lines > MAXTEXTLINES) total_lines = MAXTEXTLINES;
    int max_scroll = total_lines - chat_visible_lines;
    if (max_scroll < 0) max_scroll = 0;

    // Handle mouse wheel for scrolling
    if (what == SDL_MOUM_WHEEL) {
        // Determine scroll direction from mouse position
        if (my < y + h/2) {
            // Upper half - scroll up (show older)
            if (chat_scroll < max_scroll) {
                chat_scroll++;
            }
        } else {
            // Lower half - scroll down (show newer)
            if (chat_scroll > 0) {
                chat_scroll--;
            }
        }
        return 1;
    }

    // Left click on scrollbar area
    if (what == SDL_MOUM_LDOWN && mx > x + w - 15) {
        // Click on scrollbar - scroll to clicked position
        int scroll_y = my - (y + 15);
        int scroll_h = h - 20;
        if (scroll_h > 0 && max_scroll > 0) {
            chat_scroll = (scroll_y * max_scroll) / scroll_h;
            if (chat_scroll < 0) chat_scroll = 0;
            if (chat_scroll > max_scroll) chat_scroll = max_scroll;
        }
        return 1;
    }

    // Click in chat area
    if (what == SDL_MOUM_LDOWN) {
        return 1;  // Consume click
    }

    return 0;
}

// ============================================================================
// Public API
// ============================================================================

void ui_chat_scroll_up(void)
{
    int total_lines = textlines;
    if (total_lines > MAXTEXTLINES) total_lines = MAXTEXTLINES;
    int max_scroll = total_lines - chat_visible_lines;
    if (max_scroll < 0) max_scroll = 0;

    if (chat_scroll < max_scroll) {
        chat_scroll++;
    }
}

void ui_chat_scroll_down(void)
{
    if (chat_scroll > 0) {
        chat_scroll--;
    }
}

void ui_chat_scroll_to_bottom(void)
{
    chat_scroll = 0;
}
