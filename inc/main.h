#if !defined(__MAIN_H__)
#define __MAIN_H__

#if !defined(PACKAGE)
#define PACKAGE "com.datacapture.watch.app"
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "datacapture"

#include <app.h>
#include <Elementary.h>
#include <system_settings.h>
#include <efl_extension.h>
#include <dlog.h>
#include <string>
#include <queue>
#include "view.h"
#include "dc.h"

#define EDJ_FILE "edje/layout.edj"
#define GRP_MAIN "main"

/**
 * Implementation of Animator Params holding Evas objects for view update
 *
 */
struct AnimatorParams {
    Evas_Object* label1;
    Evas_Object* label2;
};

/**
 * Implementation of App user data object, passed around between functions and callbacks
 *
 */

typedef struct appdata {
    Evas_Object *win;
    Evas_Object *conform;
    Evas_Object *naviframe;
    Eext_Circle_Surface *circle_surface;
    Evas_Object *circle_genlist;

    // Animator data
    Ecore_Animator *animator;
    long long time_ref;
    AnimatorParams animatorParams;

    // Dc (Data Capture) data
    std::vector<Dc::Item> items;

    // View data
    View view;

    Evas_Object *genlist;

    // Initialize variables
    appdata() : win(nullptr), naviframe(nullptr), circle_surface(nullptr),
                circle_genlist(nullptr), animator(nullptr), time_ref(0) {
        genlist = NULL;
        conform = NULL;
    }
} appdata_s;


/**
 * @brief: Implementation of the MainUI App callbacks
 */
class MainUI {
    public:
        static void ui_app_lang_changed(app_event_info_h event_info, void *user_data);
        static void ui_app_orient_changed(app_event_info_h event_info, void *user_data);
        static void ui_app_region_changed(app_event_info_h event_info, void *user_data);
        static void ui_app_low_battery(app_event_info_h event_info, void *user_data);
        static void ui_app_low_memory(app_event_info_h event_info, void *user_data);
        static bool app_create(void *user_data);
        static void app_control(app_control_h app_control, void *user_data);
        static void app_pause(void *user_data);
        static void app_resume(void *user_data);
        static void app_terminate(void *user_data);

    private:
        MainUI() {};
        static void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info);
        static void app_get_resource(const char *edj_file_in, char *edj_path_out, int edj_path_max);
        static void create_base_gui(appdata_s *ad);
};

#endif
