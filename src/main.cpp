#include <app.h>
#include <app_preference.h>
#include <string>
#include <cassert>
#include "Ecore.h"
#include "main.h"

/*
 * @brief Function will be operated when window deletion is requested
 * @param[in] data The data to be passed to the callback function
 * @param[in] obj The Evas object handle to be passed to the callback function
 * @param[in] event_info The system event information
 */
void MainUI::win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
    ui_app_exit();
}

/*
 * @brief Function to create base gui structure
 * @param[in] ad The data structure to manage gui object
 */
void MainUI::create_base_gui(appdata_s *ad)
{
    Evas_Object *conform = NULL;

    /* Window */
    ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
    elm_win_autodel_set(ad->win, EINA_TRUE);

    evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);

    /* Conformant */
    conform = elm_conformant_add(ad->win);
    evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(ad->win, conform);
    evas_object_show(conform);

    /* Naviframe */
    ad->naviframe = elm_naviframe_add(conform);
    evas_object_size_hint_weight_set(ad->naviframe, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(ad->naviframe, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_object_content_set(conform, ad->naviframe);
    evas_object_show(ad->naviframe);

    /* Eext Circle Surface*/
    ad->circle_surface = eext_circle_surface_naviframe_add(ad->naviframe);

    /* Main View */
    ad->conform = conform;
    ad->view.set_data(ad);
    ad->view.create_radio_list_view();

    eext_object_event_callback_add(ad->naviframe, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
    eext_object_event_callback_add(ad->naviframe, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

    /* Show window after base gui is set up */
    evas_object_show(ad->win);
}

/*
 * @brief Hook to take necessary actions before main event loop starts
 * Initialize UI resources and application's data
 * If this function return true, the main loop of application starts
 * If this function return false, the application is terminated
 * @param[in] user_data The data to be passed to the callback function
 */
bool MainUI::app_create(void *user_data)
{
    /* Hook to take necessary actions before main event loop starts
       Initialize UI resources and application's data
       If this function returns true, the main loop of application starts
       If this function returns false, the application is terminated */
    appdata_s *ad = reinterpret_cast<appdata_s *>(user_data);
    create_base_gui(ad);
    return true;
}

/*
 * @brief This callback function is called when another application
 * sends the launch request to the application
 * @param[in] app_control The handle to the app_control
 * @param[in] user_data The data to be passed to the callback function
 */
void MainUI::app_control(app_control_h app_control, void *user_data)
{
    /* Handle the launch request. */
}

/*
 * @brief This callback function is called each time
 * the application is completely obscured by another application
 * and becomes invisible to the user
 * @param[in] user_data The data to be passed to the callback function
 */
void MainUI::app_pause(void *user_data)
{
    /* Take necessary actions when application becomes invisible. */
}

/*
 * @brief This callback function is called each time
 * the application becomes visible to the user
 * @param[in] user_data The data to be passed to the callback function
 */
void MainUI::app_resume(void *user_data)
{
    /* Take necessary actions when application becomes visible. */
}

/*
 * @brief This callback function is called once after the main loop of the application exits
 * @param[in] user_data The data to be passed to the callback function
 */
void MainUI::app_terminate(void *user_data)
{
}

/*
 * @brief This function will be called when the language is changed
 * @param[in] event_info The system event information
 * @param[in] user_data The user data to be passed to the callback function
 */
void MainUI::ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_LANGUAGE_CHANGED*/
    char *locale = NULL;
    system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
    elm_language_set(locale);
    free(locale);
    return;
}

/*
 * @brief This function will be called when the orientation is changed
 * @param[in] event_info The system event information
 * @param[in] user_data The user data to be passed to the callback function
 */
void MainUI::ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
    return;
}

/*
 * @brief This function will be called when the region is changed
 * @param[in] event_info The system event information
 * @param[in] user_data The user data to be passed to the callback function
 */
void MainUI::ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_REGION_FORMAT_CHANGED*/
}

/*
 * @brief This function will be called when the battery is low
 * @param[in] event_info The system event information
 * @param[in] user_data The user data to be passed to the callback function
 */
void MainUI::ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_LOW_BATTERY*/
}

/*
 * @brief This function will be called when the memory is low
 * @param[in] event_info The system event information
 * @param[in] user_data The user data to be passed to the callback function
 */
void MainUI::ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_LOW_MEMORY*/
}

/*
 * @brief Main function of the application
 * @param[in] argc The argument count
 * @param[in] argv The argument vector
 */
int main(int argc, char *argv[])
{
    appdata_s ad;
    int ret;

    ui_app_lifecycle_callback_s event_callback = {0, };
    app_event_handler_h handlers[5] = {NULL, };

    event_callback.create = MainUI::app_create;
    event_callback.terminate = MainUI::app_terminate;
    event_callback.pause = MainUI::app_pause;
    event_callback.resume = MainUI::app_resume;
    event_callback.app_control = MainUI::app_control;

    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, MainUI::ui_app_low_battery, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, MainUI::ui_app_low_memory, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, MainUI::ui_app_orient_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, MainUI::ui_app_lang_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, MainUI::ui_app_region_changed, &ad);
    ui_app_remove_event_handler(handlers[APP_EVENT_LOW_MEMORY]);

    ret = ui_app_main(argc, argv, &event_callback, &ad);
    if (ret != APP_ERROR_NONE)
        dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);

    return ret;
}
