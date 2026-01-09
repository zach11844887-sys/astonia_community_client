/*
 * Modern UI Mod for Astonia Community Client
 *
 * Provides customizable UI components:
 * - Semi-transparent chat panel
 * - Modern inventory display
 * - Paperdoll equipment view
 * - World clock widget
 * - Skill tree panel
 * - Quest log panel
 */

#ifndef UIMOD_H
#define UIMOD_H

#include "../dll.h"
#include "../astonia.h"
#include "../amod/amod_structs.h"
#include <SDL3/SDL_keycode.h>

// ============================================================================
// Mod lifecycle exports (required by mod loader)
// ============================================================================
DLL_EXPORT void amod_init(void);
DLL_EXPORT void amod_exit(void);
DLL_EXPORT char *amod_version(void);
DLL_EXPORT void amod_gamestart(void);
DLL_EXPORT void amod_frame(void);
DLL_EXPORT void amod_tick(void);

// ============================================================================
// Input handling exports
// ============================================================================
DLL_EXPORT void amod_mouse_move(int x, int y);
DLL_EXPORT int amod_mouse_click(int x, int y, int what);
DLL_EXPORT int amod_keydown(SDL_Keycode key);
DLL_EXPORT int amod_keyup(SDL_Keycode key);
DLL_EXPORT int amod_client_cmd(const char *buf);
DLL_EXPORT void amod_update_hover_texts(void);

// ============================================================================
// Client imported functions (from main client)
// ============================================================================

// Logging and chat
DLL_IMPORT int note(const char *format, ...) __attribute__((format(printf, 1, 2)));
DLL_IMPORT int warn(const char *format, ...) __attribute__((format(printf, 1, 2)));
DLL_IMPORT int fail(const char *format, ...) __attribute__((format(printf, 1, 2)));
DLL_IMPORT void paranoia(const char *format, ...) __attribute__((format(printf, 1, 2)));
DLL_IMPORT void addline(const char *format, ...) __attribute__((format(printf, 1, 2)));

// Rendering primitives
DLL_IMPORT void render_push_clip(void);
DLL_IMPORT void render_pop_clip(void);
DLL_IMPORT void render_more_clip(int sx, int sy, int ex, int ey);
DLL_IMPORT void render_sprite(int sprite, int scrx, int scry, int light, int align);
DLL_IMPORT int render_sprite_fx(RenderFX *ddfx, int scrx, int scry);
DLL_IMPORT void render_rect(int sx, int sy, int ex, int ey, unsigned short int color);
DLL_IMPORT void render_line(int fx, int fy, int tx, int ty, unsigned short col);
DLL_IMPORT void render_pixel(int x, int y, unsigned short col);

// Text rendering
DLL_IMPORT int render_text_length(int flags, const char *text);
DLL_IMPORT int render_text(int sx, int sy, unsigned short int color, int flags, const char *text);
DLL_IMPORT int render_text_break(int x, int y, int breakx, unsigned short color, int flags, const char *ptr);
DLL_IMPORT int render_text_break_length(int x, int y, int breakx, unsigned short color, int flags, const char *ptr);
DLL_IMPORT int render_text_fmt(int sx, int sy, unsigned short int color, int flags, const char *format, ...);
DLL_IMPORT int render_text_break_fmt(int sx, int sy, int breakx, unsigned short int color, int flags, const char *format, ...);
DLL_IMPORT int render_text_nl(int x, int y, int unsigned short color, int flags, const char *ptr);

// Extended rendering (needs client patch for some)
DLL_IMPORT void render_shaded_rect(int sx, int sy, int ex, int ey, unsigned short color, unsigned short alpha);

// GUI position helpers
DLL_IMPORT int dotx(int didx);
DLL_IMPORT int doty(int didx);
DLL_IMPORT int butx(int bidx);
DLL_IMPORT int buty(int bidx);

// Map/character helpers
DLL_IMPORT size_t get_near_ground(int x, int y);
DLL_IMPORT size_t get_near_item(int x, int y, unsigned int flag, unsigned int looksize);
DLL_IMPORT size_t get_near_char(int x, int y, unsigned int looksize);
DLL_IMPORT map_index_t mapmn(unsigned int x, unsigned int y);

// Misc utilities
DLL_IMPORT void set_teleport(int idx, int x, int y);
DLL_IMPORT int exp2level(int val);
DLL_IMPORT int level2exp(int level);
DLL_IMPORT int mil_rank(int exp);

// Client/server communication
DLL_IMPORT void client_send(void *buf, size_t len);

// ============================================================================
// Client imported data (game state)
// ============================================================================

// Skill table
DLL_IMPORT extern int skltab_cnt;
DLL_IMPORT extern struct skltab *skltab;

// Current action/selection
DLL_IMPORT extern int act;
DLL_IMPORT extern int actx;
DLL_IMPORT extern int acty;

// Resolution
DLL_IMPORT extern int __yres;

// Keyboard modifiers
DLL_IMPORT extern int vk_shift, vk_control, vk_alt;

// Current item being held
DLL_IMPORT extern unsigned int cflags;
DLL_IMPORT extern unsigned int csprite;

// Map data
DLL_IMPORT extern int originx;
DLL_IMPORT extern int originy;
DLL_IMPORT extern struct map map[MAPDX * MAPDY];
DLL_IMPORT extern struct map map2[MAPDX * MAPDY];

// Character stats
DLL_IMPORT extern int value[2][V_MAX];
DLL_IMPORT extern int hp;
DLL_IMPORT extern int mana;
DLL_IMPORT extern int rage;
DLL_IMPORT extern int endurance;
DLL_IMPORT extern int lifeshield;
DLL_IMPORT extern int experience;
DLL_IMPORT extern int experience_used;
DLL_IMPORT extern int mil_exp;
DLL_IMPORT extern int gold;

// Inventory and equipment
DLL_IMPORT extern int item[INVENTORYSIZE];
DLL_IMPORT extern int item_flags[INVENTORYSIZE];
DLL_IMPORT extern int weatab[12];

// Container (shop/grave)
DLL_IMPORT extern int con_type;
DLL_IMPORT extern char con_name[80];
DLL_IMPORT extern int con_cnt;
DLL_IMPORT extern int container[CONTAINERSIZE];
DLL_IMPORT extern int price[CONTAINERSIZE];
DLL_IMPORT extern int itemprice[CONTAINERSIZE];
DLL_IMPORT extern int cprice;

// Look window data
DLL_IMPORT extern int lookinv[12];
DLL_IMPORT extern int looksprite, lookc1, lookc2, lookc3;
DLL_IMPORT extern char look_name[80];
DLL_IMPORT extern char look_desc[1024];

// Players
DLL_IMPORT extern struct player player[MAXCHARS];

// Effects
DLL_IMPORT extern union ceffect ceffect[MAXEF];
DLL_IMPORT extern unsigned char ueffect[MAXEF];

// Quests
DLL_IMPORT extern struct quest quest[MAXQUEST];
DLL_IMPORT extern struct shrine_ppd shrine;

// Skill system
DLL_IMPORT extern struct skill *game_skill;
DLL_IMPORT extern char **game_skilldesc;
DLL_IMPORT extern int *game_v_max;
DLL_IMPORT extern int *game_v_profbase;

// Quest log system
DLL_IMPORT extern struct questlog *game_questlog;
DLL_IMPORT extern int *game_questcount;

// UI Colors (modifiable!)
DLL_IMPORT extern unsigned short int healthcolor, manacolor, endurancecolor, shieldcolor;
DLL_IMPORT extern unsigned short int whitecolor, lightgraycolor, graycolor, darkgraycolor, blackcolor;
DLL_IMPORT extern unsigned short int lightredcolor, redcolor, darkredcolor;
DLL_IMPORT extern unsigned short int lightgreencolor, greencolor, darkgreencolor;
DLL_IMPORT extern unsigned short int lightbluecolor, bluecolor, darkbluecolor;
DLL_IMPORT extern unsigned short int lightorangecolor, orangecolor, darkorangecolor;
DLL_IMPORT extern unsigned short int textcolor;

// Hover text strings
DLL_IMPORT extern char hover_bless_text[120];
DLL_IMPORT extern char hover_freeze_text[120];
DLL_IMPORT extern char hover_potion_text[120];
DLL_IMPORT extern char hover_rage_text[120];
DLL_IMPORT extern char hover_level_text[120];
DLL_IMPORT extern char hover_rank_text[120];
DLL_IMPORT extern char hover_time_text[120];

// Session info
DLL_IMPORT extern char *target_server;
DLL_IMPORT extern char password[16];
DLL_IMPORT extern char username[40];
DLL_IMPORT extern int tick;
DLL_IMPORT extern int mirror;
DLL_IMPORT extern int realtime;
DLL_IMPORT extern char server_url[256];
DLL_IMPORT extern int server_port;

// Display settings
DLL_IMPORT extern int want_width;
DLL_IMPORT extern int want_height;
DLL_IMPORT extern int sdl_scale;
DLL_IMPORT extern int sdl_frames;
DLL_IMPORT extern int sdl_multi;
DLL_IMPORT extern int sdl_cache_size;
DLL_IMPORT extern int frames_per_second;
DLL_IMPORT extern uint64_t game_options;
DLL_IMPORT extern int game_slowdown;

// ============================================================================
// NEW IMPORTS - Client export patch has been applied
// ============================================================================

// Chat/text history
#define MAXTEXTLINES   256
#define MAXTEXTLETTERS 256

struct letter {
    char c;
    unsigned char color;
    unsigned char link;
};

DLL_IMPORT extern struct letter *text;
DLL_IMPORT extern int textnextline, textdisplayline, textlines;
DLL_IMPORT extern unsigned short palette[256];

// UI selection state
DLL_IMPORT extern int invsel;
DLL_IMPORT extern int weasel;
DLL_IMPORT extern int consel;
DLL_IMPORT extern map_index_t mapsel;
DLL_IMPORT extern map_index_t itmsel;
DLL_IMPORT extern map_index_t chrsel;

// Scroll positions
DLL_IMPORT extern int invoff, max_invoff;
DLL_IMPORT extern int skloff, max_skloff;
DLL_IMPORT extern int conoff, max_conoff;

// ============================================================================
// UI Component declarations
// ============================================================================

// ui_chat.c
void ui_chat_init(void);
void ui_chat_exit(void);
void ui_chat_frame(void);
int ui_chat_click(int x, int y, int what);
void ui_chat_scroll_up(void);
void ui_chat_scroll_down(void);
void ui_chat_scroll_to_bottom(void);

// ui_inventory.c
void ui_inventory_init(void);
void ui_inventory_exit(void);
void ui_inventory_frame(void);
int ui_inventory_click(int x, int y, int what);
void ui_inventory_scroll_up(void);
void ui_inventory_scroll_down(void);
int ui_inventory_get_selected(void);
void ui_inventory_clear_selection(void);

// ui_equipment.c
void ui_equipment_init(void);
void ui_equipment_exit(void);
void ui_equipment_frame(void);
int ui_equipment_click(int x, int y, int what);
int ui_equipment_get_selected(void);
void ui_equipment_clear_selection(void);

// ui_clock.c
void ui_clock_init(void);
void ui_clock_exit(void);
void ui_clock_frame(void);
void ui_clock_get_time(int *hour, int *minute);
int ui_clock_is_day(void);
int ui_clock_get_brightness(void);

// ui_skills.c
void ui_skills_init(void);
void ui_skills_exit(void);
void ui_skills_frame(void);
int ui_skills_click(int x, int y, int what);
void ui_skills_scroll_up(void);
void ui_skills_scroll_down(void);

// ui_quests.c
void ui_quests_init(void);
void ui_quests_exit(void);
void ui_quests_frame(void);
int ui_quests_click(int x, int y, int what);
void ui_quests_scroll_up(void);
void ui_quests_scroll_down(void);

// ============================================================================
// Configuration
// ============================================================================

// UI component enable flags
extern int ui_chat_enabled;
extern int ui_inventory_enabled;
extern int ui_equipment_enabled;
extern int ui_clock_enabled;
extern int ui_skills_enabled;
extern int ui_quests_enabled;

// UI positions (user configurable)
extern int ui_chat_x, ui_chat_y;
extern int ui_chat_width, ui_chat_height;
extern int ui_inventory_x, ui_inventory_y;
extern int ui_equipment_x, ui_equipment_y;
extern int ui_clock_x, ui_clock_y;
extern int ui_skills_x, ui_skills_y;
extern int ui_skills_width, ui_skills_height;
extern int ui_quests_x, ui_quests_y;
extern int ui_quests_width, ui_quests_height;

// UI appearance
extern int ui_panel_alpha;        // 0-255, transparency for panels
extern unsigned short ui_panel_color;
extern unsigned short ui_border_color;
extern unsigned short ui_highlight_color;

#endif // UIMOD_H
