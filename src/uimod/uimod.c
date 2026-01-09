/*
 * Modern UI Mod for Astonia Community Client
 *
 * Main mod file - lifecycle management and event routing
 */

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uimod.h"

// ============================================================================
// Configuration defaults
// ============================================================================

// UI component enable flags
int ui_chat_enabled = 1;
int ui_inventory_enabled = 1;
int ui_equipment_enabled = 1;
int ui_clock_enabled = 1;
int ui_skills_enabled = 0;   // Off by default (press hotkey to show)
int ui_quests_enabled = 0;   // Off by default (press hotkey to show)

// UI positions (screen coordinates)
int ui_chat_x = 10, ui_chat_y = 400;
int ui_chat_width = 300, ui_chat_height = 150;
int ui_inventory_x = 520, ui_inventory_y = 100;
int ui_equipment_x = 520, ui_equipment_y = 300;
int ui_clock_x = 700, ui_clock_y = 10;
int ui_skills_x = 10, ui_skills_y = 50;
int ui_skills_width = 220, ui_skills_height = 300;
int ui_quests_x = 240, ui_quests_y = 50;
int ui_quests_width = 280, ui_quests_height = 300;

// UI appearance
int ui_panel_alpha = 180;  // Semi-transparent (0=invisible, 255=opaque)
unsigned short ui_panel_color = 0x0000;      // Black background
unsigned short ui_border_color = 0x4210;     // Dark gray border
unsigned short ui_highlight_color = 0x7C00;  // Blue highlight

// ============================================================================
// Internal state
// ============================================================================

static int initialized = 0;
static int game_started = 0;

// ============================================================================
// Mod lifecycle
// ============================================================================

DLL_EXPORT char *amod_version(void)
{
    return "Modern UI Mod v0.1";
}

DLL_EXPORT void amod_init(void)
{
    if (initialized) return;

    note("Modern UI Mod initializing...");

    // Initialize UI components
    ui_chat_init();
    ui_inventory_init();
    ui_equipment_init();
    ui_clock_init();
    ui_skills_init();
    ui_quests_init();

    initialized = 1;
    note("Modern UI Mod initialized.");
}

DLL_EXPORT void amod_exit(void)
{
    if (!initialized) return;

    note("Modern UI Mod shutting down...");

    // Cleanup UI components
    ui_chat_exit();
    ui_inventory_exit();
    ui_equipment_exit();
    ui_clock_exit();
    ui_skills_exit();
    ui_quests_exit();

    initialized = 0;
    game_started = 0;
}

DLL_EXPORT void amod_gamestart(void)
{
    game_started = 1;
    addline("Modern UI Mod v0.1 active. Type #uihelp for commands.");
}

// ============================================================================
// Per-frame rendering
// ============================================================================

DLL_EXPORT void amod_frame(void)
{
    if (!initialized || !game_started) return;

    // Render UI components (order matters for layering)
    if (ui_chat_enabled) {
        ui_chat_frame();
    }
    if (ui_inventory_enabled) {
        ui_inventory_frame();
    }
    if (ui_equipment_enabled) {
        ui_equipment_frame();
    }
    if (ui_clock_enabled) {
        ui_clock_frame();
    }
    if (ui_skills_enabled) {
        ui_skills_frame();
    }
    if (ui_quests_enabled) {
        ui_quests_frame();
    }
}

DLL_EXPORT void amod_tick(void)
{
    // Called 24 times per second
    // Use for animations, periodic updates
}

// ============================================================================
// Input handling
// ============================================================================

DLL_EXPORT void amod_mouse_move(int x, int y)
{
    (void)x;
    (void)y;
    // Track mouse for hover effects
}

DLL_EXPORT int amod_mouse_click(int x, int y, int what)
{
    if (!initialized || !game_started) return 0;

    // Route clicks to UI components
    // Return 1 to consume the click, 0 to pass through

    if (ui_skills_enabled && ui_skills_click(x, y, what)) {
        return 1;
    }
    if (ui_quests_enabled && ui_quests_click(x, y, what)) {
        return 1;
    }
    if (ui_chat_enabled && ui_chat_click(x, y, what)) {
        return 1;
    }
    if (ui_inventory_enabled && ui_inventory_click(x, y, what)) {
        return 1;
    }
    if (ui_equipment_enabled && ui_equipment_click(x, y, what)) {
        return 1;
    }

    return 0;  // Pass click to client
}

DLL_EXPORT int amod_keydown(SDL_Keycode key)
{
    if (!initialized || !game_started) return 0;

    // Handle keyboard shortcuts
    // Return 1 to consume, 0 to pass through

    // K = toggle skills panel
    if (key == SDLK_K && !vk_control && !vk_alt) {
        ui_skills_enabled = !ui_skills_enabled;
        return 1;
    }

    // J = toggle quest log panel
    if (key == SDLK_J && !vk_control && !vk_alt) {
        ui_quests_enabled = !ui_quests_enabled;
        return 1;
    }

    return 0;
}

DLL_EXPORT int amod_keyup(SDL_Keycode key)
{
    (void)key;
    return 0;
}

DLL_EXPORT void amod_update_hover_texts(void)
{
    // Update custom hover text if needed
}

// ============================================================================
// Custom commands
// ============================================================================

static void show_help(void)
{
    addline("--- Modern UI Mod Commands ---");
    addline("#uichat [on|off]     - Toggle chat panel");
    addline("#uiinv [on|off]      - Toggle inventory panel");
    addline("#uieq [on|off]       - Toggle equipment panel");
    addline("#uiclock [on|off]    - Toggle clock widget");
    addline("#uiskills [on|off]   - Toggle skills panel");
    addline("#uiquests [on|off]   - Toggle quest log panel");
    addline("#uialpha <0-255>     - Set panel transparency");
    addline("#uipos <panel> <x> <y> - Move panel");
    addline("#uireset             - Reset to defaults");
}

static int parse_onoff(const char *arg, int *value)
{
    if (!arg || !*arg) {
        // Toggle if no argument
        *value = !(*value);
        return 1;
    }
    if (strcmp(arg, "on") == 0 || strcmp(arg, "1") == 0) {
        *value = 1;
        return 1;
    }
    if (strcmp(arg, "off") == 0 || strcmp(arg, "0") == 0) {
        *value = 0;
        return 1;
    }
    return 0;
}

DLL_EXPORT int amod_client_cmd(const char *buf)
{
    // Return 1 if command handled, 0 to pass to client

    if (strncmp(buf, "#uihelp", 7) == 0) {
        show_help();
        return 1;
    }

    if (strncmp(buf, "#uichat", 7) == 0) {
        const char *arg = buf[7] == ' ' ? &buf[8] : NULL;
        if (parse_onoff(arg, &ui_chat_enabled)) {
            addline("Chat panel: %s", ui_chat_enabled ? "ON" : "OFF");
        }
        return 1;
    }

    if (strncmp(buf, "#uiinv", 6) == 0) {
        const char *arg = buf[6] == ' ' ? &buf[7] : NULL;
        if (parse_onoff(arg, &ui_inventory_enabled)) {
            addline("Inventory panel: %s", ui_inventory_enabled ? "ON" : "OFF");
        }
        return 1;
    }

    if (strncmp(buf, "#uieq", 5) == 0) {
        const char *arg = buf[5] == ' ' ? &buf[6] : NULL;
        if (parse_onoff(arg, &ui_equipment_enabled)) {
            addline("Equipment panel: %s", ui_equipment_enabled ? "ON" : "OFF");
        }
        return 1;
    }

    if (strncmp(buf, "#uiclock", 8) == 0) {
        const char *arg = buf[8] == ' ' ? &buf[9] : NULL;
        if (parse_onoff(arg, &ui_clock_enabled)) {
            addline("Clock widget: %s", ui_clock_enabled ? "ON" : "OFF");
        }
        return 1;
    }

    if (strncmp(buf, "#uiskills", 9) == 0) {
        const char *arg = buf[9] == ' ' ? &buf[10] : NULL;
        if (parse_onoff(arg, &ui_skills_enabled)) {
            addline("Skills panel: %s", ui_skills_enabled ? "ON" : "OFF");
        }
        return 1;
    }

    if (strncmp(buf, "#uiquests", 9) == 0) {
        const char *arg = buf[9] == ' ' ? &buf[10] : NULL;
        if (parse_onoff(arg, &ui_quests_enabled)) {
            addline("Quest log panel: %s", ui_quests_enabled ? "ON" : "OFF");
        }
        return 1;
    }

    if (strncmp(buf, "#uialpha ", 9) == 0) {
        int alpha = atoi(&buf[9]);
        if (alpha >= 0 && alpha <= 255) {
            ui_panel_alpha = alpha;
            addline("Panel alpha set to %d", ui_panel_alpha);
        } else {
            addline("Alpha must be 0-255");
        }
        return 1;
    }

    if (strncmp(buf, "#uipos ", 7) == 0) {
        char panel[32];
        int x, y;
        if (sscanf(&buf[7], "%31s %d %d", panel, &x, &y) == 3) {
            if (strcmp(panel, "chat") == 0) {
                ui_chat_x = x; ui_chat_y = y;
                addline("Chat panel moved to %d, %d", x, y);
            } else if (strcmp(panel, "inv") == 0) {
                ui_inventory_x = x; ui_inventory_y = y;
                addline("Inventory panel moved to %d, %d", x, y);
            } else if (strcmp(panel, "eq") == 0) {
                ui_equipment_x = x; ui_equipment_y = y;
                addline("Equipment panel moved to %d, %d", x, y);
            } else if (strcmp(panel, "clock") == 0) {
                ui_clock_x = x; ui_clock_y = y;
                addline("Clock moved to %d, %d", x, y);
            } else if (strcmp(panel, "skills") == 0) {
                ui_skills_x = x; ui_skills_y = y;
                addline("Skills panel moved to %d, %d", x, y);
            } else if (strcmp(panel, "quests") == 0) {
                ui_quests_x = x; ui_quests_y = y;
                addline("Quest log moved to %d, %d", x, y);
            } else {
                addline("Unknown panel: %s (use: chat, inv, eq, clock, skills, quests)", panel);
            }
        } else {
            addline("Usage: #uipos <panel> <x> <y>");
        }
        return 1;
    }

    if (strncmp(buf, "#uireset", 8) == 0) {
        ui_chat_x = 10; ui_chat_y = 400;
        ui_chat_width = 300; ui_chat_height = 150;
        ui_inventory_x = 520; ui_inventory_y = 100;
        ui_equipment_x = 520; ui_equipment_y = 300;
        ui_clock_x = 700; ui_clock_y = 10;
        ui_skills_x = 10; ui_skills_y = 50;
        ui_skills_width = 220; ui_skills_height = 300;
        ui_quests_x = 240; ui_quests_y = 50;
        ui_quests_width = 280; ui_quests_height = 300;
        ui_panel_alpha = 180;
        ui_chat_enabled = 1;
        ui_inventory_enabled = 1;
        ui_equipment_enabled = 1;
        ui_clock_enabled = 1;
        ui_skills_enabled = 0;
        ui_quests_enabled = 0;
        addline("UI settings reset to defaults");
        return 1;
    }

    return 0;  // Command not handled
}
