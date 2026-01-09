/*
 * Modern UI Mod - World Clock Widget
 *
 * Displays in-game time with day/night indicator.
 * Game time runs at 12x real time (2 real hours = 24 game hours)
 */

#include <string.h>
#include <stdio.h>
#include "uimod.h"

// ============================================================================
// Time conversion constants (from client)
// ============================================================================

#define DAYLEN  (60 * 60 * 2)    // 7200 seconds = 2 real hours per game day
#define HOURLEN (DAYLEN / 24)    // 300 seconds = 5 real minutes per game hour
#define MINLEN  (HOURLEN / 60)   // 5 seconds per game minute

// ============================================================================
// State
// ============================================================================

static int last_hour = -1;
static int last_minute = -1;

// ============================================================================
// Initialization
// ============================================================================

void ui_clock_init(void)
{
    last_hour = -1;
    last_minute = -1;
}

void ui_clock_exit(void)
{
    // Nothing to clean up
}

// ============================================================================
// Time helpers
// ============================================================================

static void get_game_time(int *hour, int *minute)
{
    int t = realtime;
    if (minute) {
        *minute = (t / MINLEN) % 60;
    }
    if (hour) {
        *hour = (t / HOURLEN) % 24;
    }
}

// Get time of day description
static const char *get_time_period(int hour)
{
    if (hour >= 5 && hour < 7) return "Dawn";
    if (hour >= 7 && hour < 12) return "Morning";
    if (hour >= 12 && hour < 14) return "Midday";
    if (hour >= 14 && hour < 17) return "Afternoon";
    if (hour >= 17 && hour < 20) return "Evening";
    if (hour >= 20 && hour < 22) return "Dusk";
    return "Night";
}

// Get color for time of day
static unsigned short get_time_color(int hour)
{
    if (hour >= 6 && hour < 8) return lightorangecolor;   // Dawn - orange
    if (hour >= 8 && hour < 17) return whitecolor;        // Day - white
    if (hour >= 17 && hour < 20) return orangecolor;      // Evening - orange
    if (hour >= 20 && hour < 22) return darkorangecolor;  // Dusk - dark orange
    return bluecolor;  // Night - blue
}

// Get brightness level (0-100)
static int get_brightness(int hour)
{
    // Smooth brightness curve
    if (hour >= 6 && hour <= 8) {
        // Dawn: 20 -> 100
        return 20 + (hour - 6) * 40;
    }
    if (hour >= 8 && hour <= 17) {
        // Day: 100
        return 100;
    }
    if (hour >= 17 && hour <= 20) {
        // Evening: 100 -> 40
        return 100 - (hour - 17) * 20;
    }
    if (hour >= 20 && hour <= 22) {
        // Dusk: 40 -> 20
        return 40 - (hour - 20) * 10;
    }
    // Night: 20
    return 20;
}

// ============================================================================
// Rendering helpers
// ============================================================================

// Draw a simple sun/moon icon
static void draw_celestial_icon(int x, int y, int hour)
{
    // Determine if day or night
    int is_day = (hour >= 6 && hour < 20);

    if (is_day) {
        // Draw sun (circle with rays)
        unsigned short sun_color = (hour >= 7 && hour < 17) ? orangecolor : darkorangecolor;

        // Center dot
        render_pixel(x, y, sun_color);
        render_pixel(x + 1, y, sun_color);
        render_pixel(x, y + 1, sun_color);
        render_pixel(x + 1, y + 1, sun_color);

        // Rays
        render_pixel(x - 2, y, sun_color);
        render_pixel(x + 3, y, sun_color);
        render_pixel(x, y - 2, sun_color);
        render_pixel(x, y + 3, sun_color);
        render_pixel(x - 1, y - 1, sun_color);
        render_pixel(x + 2, y - 1, sun_color);
        render_pixel(x - 1, y + 2, sun_color);
        render_pixel(x + 2, y + 2, sun_color);
    } else {
        // Draw moon (crescent)
        unsigned short moon_color = lightbluecolor;

        // Moon body
        render_pixel(x, y - 1, moon_color);
        render_pixel(x - 1, y, moon_color);
        render_pixel(x, y, moon_color);
        render_pixel(x - 1, y + 1, moon_color);
        render_pixel(x, y + 2, moon_color);

        // Stars
        render_pixel(x + 4, y - 2, graycolor);
        render_pixel(x + 6, y + 1, graycolor);
        render_pixel(x + 3, y + 3, graycolor);
    }
}

// Draw brightness bar
static void draw_brightness_bar(int x, int y, int w, int brightness)
{
    // Background
    render_line(x, y, x + w, y, darkgraycolor);

    // Fill based on brightness
    int fill = (brightness * w) / 100;
    if (fill > 0) {
        unsigned short bar_color;
        if (brightness > 70) bar_color = whitecolor;
        else if (brightness > 40) bar_color = graycolor;
        else bar_color = darkgraycolor;

        render_line(x, y, x + fill, y, bar_color);
    }
}

// ============================================================================
// Frame rendering
// ============================================================================

void ui_clock_frame(void)
{
    int x = ui_clock_x;
    int y = ui_clock_y;
    int panel_width = 85;
    int panel_height = 50;

    // Get current game time
    int hour, minute;
    get_game_time(&hour, &minute);

    // Update cache
    last_hour = hour;
    last_minute = minute;

    // Clip to panel
    render_push_clip();
    render_more_clip(x, y, x + panel_width, y + panel_height);

    // Draw panel border
    render_line(x, y, x + panel_width, y, ui_border_color);
    render_line(x, y + panel_height, x + panel_width, y + panel_height, ui_border_color);
    render_line(x, y, x, y + panel_height, ui_border_color);
    render_line(x + panel_width, y, x + panel_width, y + panel_height, ui_border_color);

    // Corner accents
    render_pixel(x, y, ui_highlight_color);
    render_pixel(x + panel_width, y, ui_highlight_color);

    // Draw celestial icon
    draw_celestial_icon(x + 12, y + 12, hour);

    // Draw time in HH:MM format
    unsigned short time_color = get_time_color(hour);
    render_text_fmt(x + 25, y + 5, time_color, RENDER_TEXT_SMALL, "%02d:%02d", hour, minute);

    // Draw time period text
    const char *period = get_time_period(hour);
    render_text(x + 25, y + 17, graycolor, RENDER_TEXT_SMALL, period);

    // Draw brightness indicator
    int brightness = get_brightness(hour);
    render_text(x + 5, y + 32, darkgraycolor, RENDER_TEXT_SMALL, "Light");
    draw_brightness_bar(x + 32, y + 35, 48, brightness);

    // Draw tick counter (debug info, small)
    render_text_fmt(x + 5, y + 42, darkgraycolor, RENDER_TEXT_SMALL, "t:%d", tick % 1000);

    render_pop_clip();
}

// ============================================================================
// Public API
// ============================================================================

void ui_clock_get_time(int *hour, int *minute)
{
    get_game_time(hour, minute);
}

int ui_clock_is_day(void)
{
    int hour;
    get_game_time(&hour, NULL);
    return (hour >= 6 && hour < 20);
}

int ui_clock_get_brightness(void)
{
    int hour;
    get_game_time(&hour, NULL);
    return get_brightness(hour);
}
