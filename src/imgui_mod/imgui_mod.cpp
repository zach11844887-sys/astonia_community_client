/*
 * ImGui Modern UI Mod for Astonia Community Client
 *
 * Provides a modern, dark-themed UI using Dear ImGui:
 * - Chat panel with real-time messages
 * - Inventory grid
 * - Equipment paperdoll
 * - Skills panel
 *
 * Toggle with #imgui or /imgui command, or F9 key
 */

#include <cstdio>
#include <cstring>

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl3.h"
#include "../imgui/backends/imgui_impl_sdlrenderer3.h"

#include "imgui_mod_imports.h"

// Forward declarations for UI panels
void ui_chat_init();
void ui_chat_frame();
void ui_inventory_init();
void ui_inventory_frame();
void ui_equipment_init();
void ui_equipment_frame();
void ui_skills_init();
void ui_skills_frame();
void ui_buffs_init();
void ui_buffs_frame();

// Mod state
static bool imgui_initialized = false;
static bool imgui_ui_active = true;
static bool show_chat = true;
static bool show_inventory = true;
static bool show_equipment = true;
static bool show_skills = false;
static bool show_demo = false;

// UI hiding flags (added to game_options)
#define GO_HIDE_CHAT    (1ull << 19)
#define GO_HIDE_INV     (1ull << 20)
#define GO_HIDE_EQUIP   (1ull << 21)
#define GO_HIDE_SKILLS  (1ull << 22)

// Setup dark modern theme
static void setup_dark_modern_theme()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Rounded corners
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;

    // Borders
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;

    // Padding
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(8.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);

    // Colors - Dark modern theme
    ImVec4* colors = style.Colors;

    // Backgrounds
    colors[ImGuiCol_WindowBg]           = ImVec4(0.10f, 0.10f, 0.12f, 0.94f);
    colors[ImGuiCol_ChildBg]            = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_PopupBg]            = ImVec4(0.12f, 0.12f, 0.14f, 0.98f);

    // Borders
    colors[ImGuiCol_Border]             = ImVec4(0.30f, 0.30f, 0.35f, 0.50f);
    colors[ImGuiCol_BorderShadow]       = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frame backgrounds
    colors[ImGuiCol_FrameBg]            = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]     = ImVec4(0.20f, 0.20f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgActive]      = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);

    // Title bar
    colors[ImGuiCol_TitleBg]            = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive]      = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]   = ImVec4(0.08f, 0.08f, 0.10f, 0.75f);

    // Menu bar
    colors[ImGuiCol_MenuBarBg]          = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg]        = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]      = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);

    // Buttons - Blue accent
    colors[ImGuiCol_Button]             = ImVec4(0.20f, 0.40f, 0.60f, 1.00f);
    colors[ImGuiCol_ButtonHovered]      = ImVec4(0.25f, 0.50f, 0.70f, 1.00f);
    colors[ImGuiCol_ButtonActive]       = ImVec4(0.15f, 0.35f, 0.55f, 1.00f);

    // Headers
    colors[ImGuiCol_Header]             = ImVec4(0.20f, 0.40f, 0.60f, 0.50f);
    colors[ImGuiCol_HeaderHovered]      = ImVec4(0.25f, 0.50f, 0.70f, 0.60f);
    colors[ImGuiCol_HeaderActive]       = ImVec4(0.30f, 0.55f, 0.75f, 0.70f);

    // Separator
    colors[ImGuiCol_Separator]          = ImVec4(0.30f, 0.30f, 0.35f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]   = ImVec4(0.40f, 0.55f, 0.70f, 0.80f);
    colors[ImGuiCol_SeparatorActive]    = ImVec4(0.50f, 0.65f, 0.80f, 1.00f);

    // Resize grip
    colors[ImGuiCol_ResizeGrip]         = ImVec4(0.20f, 0.40f, 0.60f, 0.40f);
    colors[ImGuiCol_ResizeGripHovered]  = ImVec4(0.25f, 0.50f, 0.70f, 0.60f);
    colors[ImGuiCol_ResizeGripActive]   = ImVec4(0.30f, 0.55f, 0.75f, 0.80f);

    // Tabs
    colors[ImGuiCol_Tab]                = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    colors[ImGuiCol_TabHovered]         = ImVec4(0.25f, 0.50f, 0.70f, 0.80f);
    colors[ImGuiCol_TabSelected]        = ImVec4(0.20f, 0.40f, 0.60f, 1.00f);

    // Text
    colors[ImGuiCol_Text]               = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled]       = ImVec4(0.50f, 0.50f, 0.52f, 1.00f);

    // Checkmark/slider
    colors[ImGuiCol_CheckMark]          = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrab]         = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]   = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
}

static void update_ui_suppression()
{
    if (imgui_ui_active) {
        // Hide all original UI components
        game_options |= GO_HIDE_CHAT | GO_HIDE_INV | GO_HIDE_EQUIP | GO_HIDE_SKILLS | GO_HIDE_BOTTOM;
    } else {
        // Show original UI
        game_options &= ~(GO_HIDE_CHAT | GO_HIDE_INV | GO_HIDE_EQUIP | GO_HIDE_SKILLS | GO_HIDE_BOTTOM);
    }
}

// =============================================================================
// Mod exports (C interface)
// =============================================================================

extern "C" {

// Lazy initialization - called from amod_frame when SDL is ready
static void imgui_lazy_init(void)
{
    if (imgui_initialized) return;
    if (!sdlwnd || !sdlren) return;

    // Create ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;  // Don't save window positions

    // Initialize ImGui backends
    ImGui_ImplSDL3_InitForSDLRenderer(sdlwnd, sdlren);
    ImGui_ImplSDLRenderer3_Init(sdlren);

    // Apply theme
    setup_dark_modern_theme();

    // Initialize UI panels
    ui_chat_init();
    ui_inventory_init();
    ui_equipment_init();
    ui_skills_init();
    ui_buffs_init();

    imgui_initialized = true;

    addline("ImGui Modern UI loaded. Press TAB for Skills. #imgui to toggle UI.");
}

DLL_EXPORT void amod_init(void)
{
    // Initialization deferred to amod_frame when SDL is ready
}

DLL_EXPORT void amod_exit(void)
{
    if (!imgui_initialized) return;

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    imgui_initialized = false;
}

DLL_EXPORT char *amod_version(void)
{
    return (char*)"ImGui Modern UI v1.0";
}

DLL_EXPORT void amod_gamestart(void)
{
    // Game has started, enable UI
    imgui_ui_active = true;
    update_ui_suppression();
}

DLL_EXPORT void amod_frame(void)
{
    // Lazy init - wait for SDL to be ready
    if (!imgui_initialized) {
        imgui_lazy_init();
        if (!imgui_initialized) return;
    }

    if (!imgui_ui_active) return;

    // Start ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Render UI panels
    if (show_chat) ui_chat_frame();
    if (show_inventory) ui_inventory_frame();
    if (show_equipment) ui_equipment_frame();
    if (show_skills) ui_skills_frame();
    ui_buffs_frame();  // Always show buff bars when active

    // Debug: show demo window (requires imgui_demo.cpp)
    // if (show_demo) ImGui::ShowDemoWindow(&show_demo);

    // Render ImGui
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlren);
}

DLL_EXPORT void amod_tick(void)
{
    // Called 24 times per second
}

DLL_EXPORT int amod_sdl_event(SDL_Event *event)
{
    if (!imgui_initialized) return 0;

    // Let ImGui process the event
    ImGui_ImplSDL3_ProcessEvent(event);

    // Consume keyboard/text events when ImGui wants them
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        if (event->type == SDL_EVENT_KEY_DOWN ||
            event->type == SDL_EVENT_KEY_UP ||
            event->type == SDL_EVENT_TEXT_INPUT ||
            event->type == SDL_EVENT_TEXT_EDITING) {
            return 1;  // Consume - ImGui handles this
        }
    }
    if (io.WantCaptureMouse) {
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN ||
            event->type == SDL_EVENT_MOUSE_BUTTON_UP ||
            event->type == SDL_EVENT_MOUSE_MOTION ||
            event->type == SDL_EVENT_MOUSE_WHEEL) {
            return 1;  // Consume - ImGui handles this
        }
    }

    return 0;  // Don't consume - let game also process
}

DLL_EXPORT int amod_keydown(SDL_Keycode key)
{
    if (!imgui_initialized) return 0;

    // If ImGui wants keyboard input (e.g., typing in text field), consume
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        return 1;
    }

    // Toggle skills panel with Tab key
    if (key == SDLK_TAB) {
        show_skills = !show_skills;
        return 1;  // Consume - we handled it
    }

    return 0;
}

DLL_EXPORT int amod_keyup(SDL_Keycode key)
{
    (void)key;
    return 0;
}

DLL_EXPORT int amod_mouse_click(int x, int y, int what)
{
    (void)x; (void)y; (void)what;

    if (!imgui_initialized || !imgui_ui_active) return 0;

    // If ImGui wants mouse input, consume
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return 1;
    }

    return 0;
}

DLL_EXPORT void amod_mouse_move(int x, int y)
{
    (void)x; (void)y;
}

DLL_EXPORT int amod_client_cmd(const char *buf)
{
    if (strcmp(buf, "#imgui") == 0 || strcmp(buf, "/imgui") == 0) {
        imgui_ui_active = !imgui_ui_active;
        update_ui_suppression();
        addline("ImGui UI: %s", imgui_ui_active ? "ON" : "OFF");
        return 1;
    }

    if (strcmp(buf, "#skills") == 0 || strcmp(buf, "/skills") == 0) {
        show_skills = !show_skills;
        addline("Skills panel: %s", show_skills ? "ON" : "OFF");
        return 1;
    }

    if (strcmp(buf, "#imgui demo") == 0) {
        show_demo = !show_demo;
        return 1;
    }

    return 0;
}

// External function to toggle skills panel (can be called from other UI modules)
void toggle_skills_panel()
{
    show_skills = !show_skills;
}

} // extern "C"
