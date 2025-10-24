/*
 * D-Pad Lock Bruteforcer — versione uamri-minimanpia M men umomentum
 * Sooost stn modiciud rmtt ,cnmaiie numssimaompibiià
 */
#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/canvas.h>
#include <input/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "assets_icons.h"

/* View IDs */
typedef enum ViewId {
    ViewId_Menu = 0,
    ViewId_Code,
    ViewId_Credits,
    ViewId_Help,
} ViewId;

/* VIEW_NONE is defined in the SDK */
/* Code Item */
typedef struct CodeItem {
    const char* code;
    uint8_t percentage;
} CodeItem;

/* Codici disponibili dal file a.txt, ordinati per percentuale */
static const CodeItem CODE_ITEMS[] = {
    {"L R U L D _ _ UDLD", 0},
    {"R L D R U _ _ DURU", 4},
    {"D U L D R _ _ DUDR", 7},
    {"U D R U L _ _ UDUL", 10},
    {"L R R D _ _", 13},
    {"U D U R _ _", 15},
    {"L L R L _ _", 17},
    {"D U U R _ _", 19},
    {"R L L U _ _", 21},
    {"L R L U _ _", 24},
    {"U D D L _ _", 26},
    {"D D U D _ _", 28},
    {"R R L R _ _", 30},
    {"R L R D _ _", 32},
    {"U U D U _ _", 35},
    {"D U D L _ _", 37},
    {"R R U R _ _", 39},
    {"D D R D _ _", 41},
    {"L L D L _ _", 43},
    {"U U L U _ _", 44},
    {"L U R D U _ UDDU", 46},
    {"R L D L R _ RLLR", 47},
    {"R D L U D _ DUUD", 49},
    {"L R U R L _ LRRL", 50},
    {"L U L L _", 51},
    {"D D L D _", 52},
    {"U U R U _", 53},
    {"R R D R _", 54},
    {"R U D L D U _ RLDD DDUU", 55},
    {"U L R D L R _ RDLL LLRR", 56},
    {"R D U L D R _ RDLD RDDR", 57},
    {"L D U R U D _ LRUU UUDD", 59},
    {"D L R U L D _ DLUL DLLD", 60},
    {"L U D R U L _ LURU LUUL", 61},
    {"U R L D R U _ URDR URRU", 63},
    {"D R L U R L _ LURR RRLL", 64},
    {"L D R D R _ UDDR", 65},
    {"U U L D R _ UUDR", 66},
    {"L U D U R _ LUUR", 67},
    {"U L D L R _ ULLR", 68},
    {"R U L U D _ RUUD", 69},
    {"L L D R U _ LLRU", 70},
    {"R R U L D _ RRLD", 71},
    {"R D U D L _ RDDL", 71},
    {"D L R L U _ DLLU", 72},
    {"D R U R L _ DRRL", 73},
    {"D D R U L _ DDUL", 74},
    {"U R L R D _ URRD", 75},
    {"D L R D _", 76},
    {"L U U D _", 76},
    {"R R U U _", 77},
    {"U U L L _", 77},
    {"D L R R _", 78},
    {"D D R L _", 78},
    {"U U R D _", 79},
    {"D L L R _", 79},
    {"D L U U _", 79},
    {"D D R R _", 80},
    {"R U U L _", 80},
    {"L D D R _", 81},
    {"L L R D _", 81},
    {"U R L U _", 82},
    {"D D L U _", 82},
    {"R R D D _", 82},
    {"D D U R _", 83},
    {"R D U U _", 83},
    {"R R L U _", 84},
    {"L R R U _", 84},
    {"U R L L _", 85},
    {"L L D U _", 85},
    {"L L D D _", 86},
    {"U U L R _", 86},
    {"L D U U _", 86},
    {"U U R L _", 87},
    {"R D D U _", 87},
    {"U D U D _", 88},
    {"U L R R _", 88},
    {"L L U D _", 89},
    {"D U U L _", 89},
    {"U R R L _", 89},
    {"U D R R _", 90},
    {"U U D L _", 90},
    {"U R D D _", 91},
    {"D R L L _", 91},
    {"D U L L _", 92},
    {"R R D L _", 92},
    {"U U R R _", 93},
    {"D U D U _", 93},
    {"D R R U _", 93},
    {"U L L D _", 94},
    {"L D D U _", 94},
    {"R D U R _", 95},
    {"R U D D _", 95},
    {"R L L D _", 96},
    {"L U D D _", 96},
    {"L L U U _", 96},
    {"R R D U _", 97},
    {"D D L R _", 97},
    {"L L U R _", 98},
    {"L R L R _", 98},
    {"L U D L _", 99},
    {"D D L L _", 99},
    {"R R U D", 100},
};
#define NUM_CODES (sizeof(CODE_ITEMS) / sizeof(CODE_ITEMS[0]))

/* App state */
typedef struct {
    ViewDispatcher* view_dispatcher;
    View* menu_view;
    View* code_view;
    View* credits_view;
    Widget* help_widget;
    size_t current_index;
    size_t current_menu_index;
} MyApp;

/* Global app pointer per compatibilità Momentum */
static MyApp* g_app = NULL;

/* Forward prototypes */
static uint32_t menu_previous(void* ctx);
static uint32_t code_previous(void* ctx);
static void code_draw(Canvas* canvas, void* ctx);
static bool code_input(InputEvent* event, void* ctx);
static uint32_t credits_previous(void* ctx);
static uint32_t help_previous(void* ctx);

/* Safe access to global app */
static inline MyApp* get_app(void) {
    if(!g_app) {
        // Handle NULL case, perhaps log or abort
        return NULL;
    }
    return g_app;
}

/* Menu previous: BACK esce dall'app */
static uint32_t menu_previous(void* ctx) {
    (void)ctx;
    return VIEW_NONE;
}

/* Menu draw */
static void menu_draw(Canvas* canvas, void* ctx) {
    (void)ctx;
    MyApp* app = get_app();
    if(!canvas) return;

    canvas_clear(canvas);
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    canvas_draw_str(canvas, 8, 10, "D-pad Lock Bruteforcer");
    canvas_draw_str(canvas, 9, 10, "D-pad Lock Bruteforcer");

    const char* menu_items[] = {"Start", "Credits", "Help"};
    const int num_items = 3;

    for(int i = 0; i < num_items; i++) {
        if((size_t)i == app->current_menu_index) {
            canvas_draw_str(canvas, 8, 24 + i * 12, ">");
            canvas_draw_str(canvas, 16, 24 + i * 12, menu_items[i]);
        } else {
            canvas_draw_str(canvas, 8, 24 + i * 12, menu_items[i]);
        }
    }
}

/* Menu input */
static bool menu_input(InputEvent* event, void* ctx) {
    (void)ctx;
    MyApp* app = get_app();
    if(!app || !event) return false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyUp:
            if(app->current_menu_index > 0) {
                app->current_menu_index--;
                return true;
            }
            break;
        case InputKeyDown:
            if(app->current_menu_index < 2) {
                app->current_menu_index++;
                return true;
            }
            break;
        case InputKeyOk:
            if(app->current_menu_index == 0) { // Avvia
                if(app->code_view) {
                    view_dispatcher_switch_to_view(app->view_dispatcher, ViewId_Code);
                }
            } else if(app->current_menu_index == 1) { // Credits
                if(app->credits_view) {
                    view_dispatcher_switch_to_view(app->view_dispatcher, ViewId_Credits);
                }
            } else if(app->current_menu_index == 2) { // Help
                if(app->help_widget) {
                    view_dispatcher_switch_to_view(app->view_dispatcher, ViewId_Help);
                }
            }
            return true;
        case InputKeyBack:
            return false; // Exit app
        default:
            break;
        }
    }
    return false;
}

/* Code previous: BACK torna al menu */
static uint32_t code_previous(void* ctx) {
    (void)ctx;
    return ViewId_Menu;
}

/* Code view draw: mostra codice corrente */
static void code_draw(Canvas* canvas, void* ctx) {
    (void)ctx; // Ignorato, usiamo global
    MyApp* app = get_app();
    if(!canvas) return;

    canvas_clear(canvas);
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    canvas_draw_str(canvas, 8, 10, "D-pad Lock Bruteforcer");
    canvas_draw_str(canvas, 9, 10, "D-pad Lock Bruteforcer");

    if(!app) {
        canvas_draw_str(canvas, 8, 28, "ERR: no app");
        return;
    }

    // Validazione index
    if(app->current_index >= NUM_CODES) app->current_index = 0;
    const CodeItem* item = &CODE_ITEMS[app->current_index];
    if(!item) return; // Additional check

    // Mostra indice
    char idxbuf[16];
    snprintf(idxbuf, sizeof(idxbuf), "%zu/%zu", app->current_index + 1, (size_t)NUM_CODES);
    canvas_draw_str(canvas, 8, 24, idxbuf);

    // Mostra codice grande
    canvas_draw_str(canvas, 10, 38, item->code);

    // Mostra percentuale
    char percbuf[8];
    snprintf(percbuf, sizeof(percbuf), "%u%%", item->percentage);
    canvas_draw_str(canvas, 98, 58, percbuf);
}

/* Credits draw */
static void credits_draw(Canvas* canvas, void* ctx) {
    (void)ctx;
    if(!canvas) return;

    canvas_clear(canvas);
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    canvas_draw_str(canvas, 8, 10, "Credits");
    canvas_draw_str(canvas, 9, 10, "Credits");

    canvas_draw_str(canvas, 8, 24, "D-Pad Lock Bruteforcer");
    canvas_draw_str(canvas, 8, 36, "Generato da Marcopit");
    canvas_draw_str(canvas, 8, 48, "BACK to return");
}

/* Credits input */
static bool credits_input(InputEvent* event, void* ctx) {
    (void)ctx;
    MyApp* app = get_app();
    if(!app || !event) return false;

    if(event->type == InputTypeShort && event->key == InputKeyBack) {
        if(app->view_dispatcher) {
            view_dispatcher_switch_to_view(app->view_dispatcher, ViewId_Menu);
        }
        return true;
    }
    return false;
}

/* Credits previous: BACK torna al menu */
static uint32_t credits_previous(void* ctx) {
    (void)ctx;
    MyApp* app = get_app();
    if(app && app->view_dispatcher) {
        view_dispatcher_switch_to_view(app->view_dispatcher, ViewId_Menu);
    }
    return VIEW_NONE;
}

/* Help previous: BACK torna al menu */
static uint32_t help_previous(void* ctx) {
    (void)ctx;
    return ViewId_Menu;
}

/* Code input: Left/Right per cambiare codice, Back per uscire */
static bool code_input(InputEvent* event, void* ctx) {
    (void)ctx; // Ignorato, usiamo global
    MyApp* app = get_app();
    if(!app || !event) return false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyRight:
            app->current_index = (app->current_index + 1) % NUM_CODES;
            return true;
        case InputKeyLeft:
            app->current_index = (app->current_index == 0) ? (NUM_CODES - 1) :
                                                             (app->current_index - 1);
            return true;
        case InputKeyBack:
            return false; // Lascia che view_dispatcher gestisca l'uscita
        default:
            break;
        }
    }
    return false;
}

/* Allocazione app con menu */
MyApp* my_app_alloc(void) {
    MyApp* app = (MyApp*)malloc(sizeof(MyApp));
    if(!app) return NULL;

    // Inizializza tutto a NULL
    memset(app, 0, sizeof(*app));
    app->current_index = 0;
    // Set global only after successful allocation
    g_app = app;

    // 1. View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    if(!app->view_dispatcher) {
        g_app = NULL;
        free(app);
        return NULL;
    }

    // 2. GUI
    Gui* gui = (Gui*)furi_record_open("gui");
    if(!gui) {
        view_dispatcher_free(app->view_dispatcher);
        g_app = NULL;
        free(app);
        return NULL;
    }
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    // 3. Menu view
    app->menu_view = view_alloc();
    if(!app->menu_view) {
        view_dispatcher_free(app->view_dispatcher);
        g_app = NULL;
        free(app);
        return NULL;
    }

    view_set_context(app->menu_view, app);
    view_set_draw_callback(app->menu_view, menu_draw);
    view_set_input_callback(app->menu_view, menu_input);
    view_set_previous_callback(app->menu_view, menu_previous);
    view_dispatcher_add_view(app->view_dispatcher, ViewId_Menu, app->menu_view);

    // 4. Code view
    app->code_view = view_alloc();
    if(!app->code_view) {
        view_free(app->menu_view);
        view_dispatcher_free(app->view_dispatcher);
        g_app = NULL;
        free(app);
        return NULL;
    }

    view_set_context(app->code_view, app);
    view_set_draw_callback(app->code_view, code_draw);
    view_set_input_callback(app->code_view, code_input);
    view_set_previous_callback(app->code_view, code_previous);
    view_dispatcher_add_view(app->view_dispatcher, ViewId_Code, app->code_view);

    // 5. Credits view
    app->credits_view = view_alloc();
    if(!app->credits_view) {
        view_free(app->code_view);
        view_free(app->menu_view);
        view_dispatcher_free(app->view_dispatcher);
        g_app = NULL;
        free(app);
        return NULL;
    }

    view_set_context(app->credits_view, app);
    view_set_draw_callback(app->credits_view, credits_draw);
    view_set_input_callback(app->credits_view, credits_input);
    view_set_previous_callback(app->credits_view, credits_previous);
    view_dispatcher_add_view(app->view_dispatcher, ViewId_Credits, app->credits_view);

    // 6. Help widget
    app->help_widget = widget_alloc();
    if(!app->help_widget) {
        view_free(app->credits_view);
        view_free(app->code_view);
        view_free(app->menu_view);
        view_dispatcher_free(app->view_dispatcher);
        g_app = NULL;
        free(app);
        return NULL;
    }

    widget_add_text_scroll_element(
        app->help_widget,
        0,
        0,
        128,
        64,
        "This app is designed to give the most common factory codes for D-pad Locks, you should be able to open your locks in less than 7 minutes. You can cycle through codes using directional buttons. U = Up, D = Down, L=Left, R=Right.  Some codes has two versions, they are separated with the _ . After trying a code if you need to reset the lock to try a new one you neeed to double click it. This app provides device factory codes for lawful, authorized use only. By using this app you confirm you are the device owner or have explicit permission to access the device. Unauthorized use is illegal. The developer is not responsible for misuse. Use at your own risk.");
    view_set_previous_callback(widget_get_view(app->help_widget), help_previous);
    view_dispatcher_add_view(app->view_dispatcher, ViewId_Help, widget_get_view(app->help_widget));

    // Switch al menu
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewId_Menu);
    return app;
}

/* Free app */
void my_app_free(MyApp* app) {
    if(!app) return;

    g_app = NULL; // Clear global

    if(app->view_dispatcher) {
        if(app->menu_view) view_dispatcher_remove_view(app->view_dispatcher, ViewId_Menu);
        if(app->code_view) view_dispatcher_remove_view(app->view_dispatcher, ViewId_Code);
        if(app->credits_view) view_dispatcher_remove_view(app->view_dispatcher, ViewId_Credits);
        if(app->help_widget) view_dispatcher_remove_view(app->view_dispatcher, ViewId_Help);
    }

    // No submenu to free
    if(app->menu_view) view_free(app->menu_view);
    if(app->code_view) view_free(app->code_view);
    if(app->credits_view) view_free(app->credits_view);
    if(app->help_widget) widget_free(app->help_widget);
    if(app->view_dispatcher) view_dispatcher_free(app->view_dispatcher);

    free(app);
}

/* Entry point */
int32_t dpad_lock_bruteforcer_app(void* p) {
    (void)p;

    MyApp* app = my_app_alloc();
    if(!app) return -1;

    if(app->view_dispatcher) {
        view_dispatcher_run(app->view_dispatcher);
    }

    my_app_free(app);
    return 0;
}
