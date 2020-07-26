#ifndef _VIEW_H_
#define _VIEW_H_

typedef struct appdata appdata_s;  // Forward declarations
class View {
  public:
    View() {};
    virtual ~View() {};

    /*
     * @brief: radio list selection and checkbox list
     */
    static const char* IDS_frequency_label1;
    static const char* IDS_frequency_label2[];
    static const char* IDS_sensors_label1;
    static const char* IDS_sensors_label2[];
    static int frequency_index_;
    static int get_sample_frequency();
    static const int frequency_list_size;
    static const int sensors_list_size;
    static bool is_sensor_selected(int i);

    /*
     * @brief: set app data for the class
     */
    static void set_data(appdata_s *app_data);

    /*
     * @brief Gets system time by milliseconds.
     */
    static long long get_current_ms_time(void);

    static Eina_Bool main_menu_finished_cb(void* data, Elm_Object_Item* it);

    static void create_recording_progress_view(void *data, Evas_Object *obj, void *event_info);

    static void app_get_resource(const char *edj_file_in, char *edj_path_out, int edj_path_max);

    static Eina_Bool recording_progress_finished_cb(void* data, Elm_Object_Item* it);

    static Eina_Bool animator_recording_progress(void* data);

    static Evas_Object* genlist_add_radio_(void *data, Evas_Object *obj, const char *part);

    static void radio_btn_changed_cb_(void *data, Evas_Object *obj, void *event_info);

    static void create_radio_list_view();

    static Evas_Object* genlist_add_checkbox_(void *data, Evas_Object *obj, const char *part);

    static void checkbox_btn_changed_cb_(void *data, Evas_Object *obj, void *event_info);

	static void create_checkbox_list_view(void* data, Evas_Object* obj, void* event_info);

    static void btn_start(void* data, Evas_Object* obj, void* event_info);

    static void btn_stop(void* data, Evas_Object* obj, void* event_info);

    static appdata_s* ad;
    static const size_t width_limit = 320;
    static const double animation_frametime; // in sec
};

#endif
