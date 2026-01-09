/*
 * ImGui Mod Imports Header
 * Minimal declarations needed for mod without pulling in mimalloc C++ issues
 */

#ifndef IMGUI_MOD_IMPORTS_H
#define IMGUI_MOD_IMPORTS_H

#include <stdint.h>
#include <SDL3/SDL.h>

// DLL import/export macros
#ifdef _WIN32
    #ifdef DLL_EXPORTS
        #define DLL_EXPORT __declspec(dllexport)
    #else
        #define DLL_EXPORT __declspec(dllimport)
    #endif
    #define DLL_IMPORT __declspec(dllimport)
#else
    #define DLL_EXPORT __attribute__((visibility("default")))
    #define DLL_IMPORT
#endif

// Constants from client
#define INVENTORYSIZE 110
#define MAXTEXTLINES 256
#define MAXTEXTLETTERS 256
#define V_MAX 200
#define V_RAGE 40

// IF_* flags for items
#define IF_USE (1 << 4)

// Stat structure
typedef struct {
    int32_t act;
    int32_t max;
} stat_t;

// Letter structure for chat
struct letter {
    char c;
    unsigned char color;
    unsigned char link;
};

// Skill structure
struct skill {
    char name[80];
    int base1, base2, base3;
    int cost;
    int start;
};

#ifdef __cplusplus
extern "C" {
#endif

// SDL window/renderer from client
DLL_IMPORT extern SDL_Window *sdlwnd;
DLL_IMPORT extern SDL_Renderer *sdlren;

// Chat system
DLL_IMPORT extern struct letter *text;
DLL_IMPORT extern int textnextline, textdisplayline, textlines;
DLL_IMPORT extern unsigned short palette[256];

// Inventory data
DLL_IMPORT extern uint32_t item[INVENTORYSIZE];
DLL_IMPORT extern uint32_t item_flags[INVENTORYSIZE];
DLL_IMPORT extern uint32_t gold;
DLL_IMPORT extern int invsel;

// Stats
DLL_IMPORT extern stat_t hp;
DLL_IMPORT extern stat_t mana;
DLL_IMPORT extern stat_t endurance;
DLL_IMPORT extern uint32_t experience;
DLL_IMPORT extern uint32_t experience_used;

// Skills
DLL_IMPORT extern uint16_t value[2][V_MAX];
DLL_IMPORT extern struct skill *game_skill;
DLL_IMPORT extern char **game_skilldesc;
DLL_IMPORT extern int *game_v_max;

// Game options
DLL_IMPORT extern uint64_t game_options;

// Functions
DLL_IMPORT void addline(const char *format, ...);
DLL_IMPORT void cmd_add_text(const char *buf, int typ);
DLL_IMPORT void cmd_use_inv(int with);
DLL_IMPORT void cmd_look_inv(int pos);
DLL_IMPORT void cmd_swap(int with);
DLL_IMPORT int exp2level(int val);
DLL_IMPORT int raise_cost(int v, int n);
DLL_IMPORT void cmd_raise(int vn);

// Get SDL_Texture* for a sprite with default item lighting
// Returns NULL if sprite loading fails - texture is cache-managed, don't destroy
DLL_IMPORT SDL_Texture *sdl_get_sprite_texture(unsigned int sprite, int *out_width, int *out_height);

// Buff/status text strings (updated by display_selfspells)
DLL_IMPORT extern char hover_bless_text[120];
DLL_IMPORT extern char hover_freeze_text[120];
DLL_IMPORT extern char hover_potion_text[120];
DLL_IMPORT extern char hover_rage_text[120];

// Rage value (current)
DLL_IMPORT extern uint16_t rage;

#ifdef __cplusplus
}
#endif

// GO_HIDE_* flags for UI suppression
#define GO_HIDE_CHAT    (1ull << 19)
#define GO_HIDE_INV     (1ull << 20)
#define GO_HIDE_EQUIP   (1ull << 21)
#define GO_HIDE_SKILLS  (1ull << 22)
#define GO_HIDE_BOTTOM  (1ull << 23)

#endif // IMGUI_MOD_IMPORTS_H
