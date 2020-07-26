#include <tizen.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <app.h>
#include <dlog.h>
#include <app_preference.h>
#include <string>
#include <cassert>
#include "Ecore.h"
#include "view.h"
#include "main.h"

const char* IDS_btn_ok = "OK";
const char* View::IDS_frequency_label1 = "Sample frequency";
const char* View::IDS_frequency_label2[] = {"100 Hz", "50 Hz", "25 Hz", "10 Hz", "1 Hz"};
const char* View::IDS_sensors_label1 = "Sensors";

// Do not change the sensor order, new sensor must be added at the end !
const char* View::IDS_sensors_label2[] = {
		"Accelerometer",
		"Linear Acceleration",
		"Gyroscope",
		"Magnetometer",
		"Rotation vector",
		"Orientation",
		"Pedometer",
		"HRM-pulse",
		"HRM-Green",
};
const int View::frequency_list_size = sizeof(IDS_frequency_label2)/sizeof(IDS_frequency_label2[0]);
const int View::sensors_list_size = sizeof(IDS_sensors_label2)/sizeof(IDS_sensors_label2[0]);
int View::frequency_index_ = -1;
int View::get_sample_frequency() {
	return atoi(IDS_frequency_label2[frequency_index_ >= 0 ? frequency_index_ : 0]);
}
static int sensors_indices_[View::sensors_list_size] = {0,};

bool View::is_sensor_selected(int i) {
	if (i < 0 || i >= sensors_list_size) return false;
	return sensors_indices_[i] > 0 ? true : false;
}


//#define LABEL_STYLE_START "<font=Tizen:style=Regular><font_size=36><align=center><color=#FAFAFA><wrap=mixed>"
//#define LABEL_STYLE_END "</wrap></color></align></font_size></font>"

#define LABEL_STYLE_START "<align=center><wrap=mixed>"
#define LABEL_STYLE_END "</wrap></align>"

#define LABEL_STYLE2_START "<align=center><wrap=mixed>"
#define LABEL_STYLE2_END "</wrap></align>"

#define LABEL_STYLE_START3 "<align=center><wrap=mixed><font_size=55>"
#define LABEL_STYLE_END3 "</font_size></wrap></align>"

appdata_s* View::ad = nullptr;
const double View::animation_frametime = 0.100; // in sec

void View::set_data(appdata_s *app_data) {
	assert(app_data);
	ad = app_data;
}

long long View::get_current_ms_time(void) {
    struct timespec tp;
    long long res = 0;
    if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
        return 0;
    }
    else {
        res = tp.tv_sec * 1000 + tp.tv_nsec / 1000000;
        return res;
    }
}

Eina_Bool View::main_menu_finished_cb(void* data, Elm_Object_Item* it) {
    dlog_print(DLOG_INFO, LOG_TAG, "main_menu_finished_cb: log: called");
	ui_app_exit();
	return EINA_FALSE;
}

void View::create_recording_progress_view(void *data, Evas_Object *obj, void *event_info) {
    Evas_Object *naviframe = ad->naviframe;
    Evas_Object *box = NULL;
    Evas_Object *label1 = NULL;
    Evas_Object *label2 = NULL;
    Elm_Object_Item *nf_it = NULL;

    Evas_Object *parent = naviframe;
    box = elm_box_add(parent);
    evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    elm_win_resize_object_add(parent, box);
    evas_object_show(box);
    std::string text;

	// add a button to box
	{
	Evas_Object* btn = elm_button_add(box);
	std::string text = std::string(LABEL_STYLE_START) + std::string(_("START")) + std::string(LABEL_STYLE_END);
	elm_object_text_set(btn, text.c_str());
    elm_object_style_set(btn, "bottom");					// curve button OK
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(btn, "clicked", btn_start, (void *)ad);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);
	}

    {  // empty label for spacing
    label2 = elm_label_add(box);
    evas_object_size_hint_weight_set(label2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(label2, EVAS_HINT_FILL, EVAS_HINT_FILL);
    text = "";
    elm_object_text_set(label2, text.c_str());
    elm_label_line_wrap_set(label2, ELM_WRAP_MIXED);
    elm_label_wrap_width_set(label2, width_limit);  // must set
    elm_box_pack_end(box, label2);
    evas_object_show(label2);
    }

    {  // text: timer count up in seconds
    label1 = elm_label_add(box);
    evas_object_size_hint_weight_set(label1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    //evas_object_size_hint_align_set(label1, 0.5, 0.25);
    evas_object_size_hint_align_set(label1, EVAS_HINT_FILL, EVAS_HINT_FILL);
    text = std::string(LABEL_STYLE_START3) + "00:00:00" + std::string(LABEL_STYLE_END3);
    elm_object_text_set(label1, text.c_str());
    elm_label_line_wrap_set(label1, ELM_WRAP_MIXED);
    elm_label_wrap_width_set(label1, width_limit);  // must set
    elm_box_pack_end(box, label1);
    evas_object_show(label1);
    }

    {  // empty label for spacing
    label2 = elm_label_add(box);
    evas_object_size_hint_weight_set(label2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(label2, EVAS_HINT_FILL, EVAS_HINT_FILL);
    text = "";
    elm_object_text_set(label2, text.c_str());
    elm_label_line_wrap_set(label2, ELM_WRAP_MIXED);
    elm_label_wrap_width_set(label2, width_limit);  // must set
    elm_box_pack_end(box, label2);
    evas_object_show(label2);
    }

	// add a button to box
	{
	Evas_Object* btn = elm_button_add(box);
	std::string text = std::string(LABEL_STYLE_START) + std::string(_("STOP")) + std::string(LABEL_STYLE_END);
	elm_object_text_set(btn, text.c_str());
    elm_object_style_set(btn, "bottom");					// curve button OK
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(btn, "clicked", btn_stop, (void *)ad);
	evas_object_show(btn);
	elm_box_pack_end(box, btn);
	}
	{
        ad->animatorParams.label1 = label1;
        ad->animatorParams.label2 = label2;
	}
    nf_it = elm_naviframe_item_push(naviframe, "recording_progress", NULL, NULL, box, "empty");
    elm_naviframe_item_pop_cb_set(nf_it, main_menu_finished_cb, ad);  // exit for backkey
    return;
}

void View::btn_start(void* data, Evas_Object* obj, void* event_info) {
    appdata_s *ad = reinterpret_cast<appdata_s*>(data);
    static bool initialised = false;
    if (!initialised) {
    	initialised = true;
    } else {
    	return;
    }
	Dc::start(data);
    {  // set up animation
        if (ad->animator) {
            ecore_animator_del(ad->animator);
            ad->animator = NULL;
        }
        ad->time_ref = get_current_ms_time();
        ad->animator = ecore_animator_add(animator_recording_progress, (void *)ad);
        ecore_animator_frametime_set(animation_frametime);  // in sec
    }
}

void View::btn_stop(void* data, Evas_Object* obj, void* event_info) {
    appdata_s *ad = reinterpret_cast<appdata_s*>(data);
    static bool initialised = false;
    if (!initialised) {
    	initialised = true;
    } else {
    	return;
    }

	Dc::stop(data);
    if (ad->animator) {
        ecore_animator_del(ad->animator);
        ad->animator = NULL;
    }
}

void View::app_get_resource(const char *edj_file_in, char *edj_path_out, int edj_path_max) {
    char *res_path = app_get_resource_path();
    if (res_path) {
        snprintf(edj_path_out, edj_path_max, "%s%s", res_path, edj_file_in);
        free(res_path);
    }
}

Eina_Bool View::animator_recording_progress(void* data) {
    double t1 = static_cast<double>(get_current_ms_time());
    double t0 = static_cast<double>(ad->time_ref);
    AnimatorParams *params = &ad->animatorParams;
    unsigned total_ms = static_cast<int>(t1 - t0);
    int total_sec = static_cast<int>(total_ms / 1000);

    int hh = total_sec / 3600;
    int mm = (total_sec - 3600 * hh) / 60;
    int ss = total_sec - 3600 * hh - 60 * mm;
    std::string text1;
    if (hh < 10) {
    	text1.append("0");
    	text1.append(std::to_string(hh));
    	text1.append(":");
    } else {
    	text1.append(std::to_string( hh % 100 ));
    	text1.append(":");
    }

    if (mm < 10) {
    	text1.append("0");
    	text1.append(std::to_string(mm));
    	text1.append(":");
    } else {
    	text1.append(std::to_string( mm % 60 ));
    	text1.append(":");
    }

    if (ss < 10) {
    	text1.append("0");
    	text1.append(std::to_string(ss));
    } else {
    	text1.append(std::to_string( ss % 60 ));
    }
    std::string text = std::string(LABEL_STYLE_START3) + text1 + std::string(LABEL_STYLE_END3);
    elm_object_text_set(params->label1, text.c_str());
    return ECORE_CALLBACK_RENEW;
}

struct Genlist_item_data {
    int index;
    Elm_Object_Item *item;
};

static char* genlist_title_radio_get_(void *data, Evas_Object *obj, const char *part) {
    char buf[101];
    std::string text = std::string(LABEL_STYLE_START) + std::string(_(View::IDS_frequency_label1)) + std::string(LABEL_STYLE_END);
    snprintf(buf, 100, "%s", text.c_str());
    return strdup(buf);
}

static char* genlist_text_radio_get_(void *data, Evas_Object *obj, const char *part) {
    char buf[101];
    Genlist_item_data *id = reinterpret_cast<Genlist_item_data *>(data);
    int index = id->index;
    int size = sizeof(View::IDS_frequency_label2)/sizeof(View::IDS_frequency_label2[0]);
    assert(index < size);
    if (!strcmp(part, "elm.text") && index < size) {
    	const char* str = View::IDS_frequency_label2[index];
        std::string text = std::string(LABEL_STYLE_START) + std::string(_(str)) + std::string(LABEL_STYLE_END);
        snprintf(buf, 100, "%s", text.c_str());
        return strdup(buf);
    }
    return NULL;
}

static void genlist_del_(void *data, Evas_Object *obj) {
    // FIXME: Unrealized callback can be called after this.
    // Accessing Item_Data can be dangerous on unrealized callback.
	Genlist_item_data *id = reinterpret_cast<Genlist_item_data *>(data);
    if (id) free(id);
}

void View::radio_btn_changed_cb_(void *data, Evas_Object *obj, void *event_info) {
	elm_genlist_realized_items_update(View::ad->genlist);
}

Evas_Object* View::genlist_add_radio_(void *data, Evas_Object *obj, const char *part) {
	    Genlist_item_data *id = reinterpret_cast<Genlist_item_data *>(data);
	    int index = id->index;
	    size_t size = View::frequency_list_size;
	    assert(index < size);

    	if (!strcmp(part, "elm.icon")) {
    		Evas_Object *radio = elm_radio_add(obj);
    		elm_radio_state_value_set(radio, index);
    		elm_radio_value_pointer_set(radio, &frequency_index_);  // 0, 1, 2, 3, 4
    		evas_object_smart_callback_add(radio, "changed", radio_btn_changed_cb_, data);
    		evas_object_propagate_events_set(radio, EINA_FALSE);   // avoid genlist is selected
    		return radio;
    	}
    	return NULL;
}

void View::create_radio_list_view() {
	char edj_path[PATH_MAX] = {0, };
    Evas_Object *genlist = NULL;
    Evas_Object *naviframe = ad->naviframe;
    Evas_Object *layout = NULL;
    Elm_Object_Item *nf_it = NULL;
    Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
    Elm_Genlist_Item_Class *titc = elm_genlist_item_class_new();
    Elm_Genlist_Item_Class *pitc = elm_genlist_item_class_new();
    Genlist_item_data *id = NULL;

    /* Create Genlist */
    app_get_resource(EDJ_FILE, edj_path, (int)PATH_MAX);
    layout = elm_layout_add(naviframe);
    elm_layout_file_set(layout, edj_path, "activity_layout");
    evas_object_show(layout);
    genlist = elm_genlist_add(layout);
    elm_object_part_content_set(layout, "elm.swallow.content.genlist", genlist);
    evas_object_show(genlist);

    elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
    evas_object_smart_callback_add(genlist, "selected", NULL, NULL);
    ad->circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);
    eext_circle_object_genlist_scroller_policy_set(ad->circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    eext_rotary_object_event_activated_set(ad->circle_genlist, EINA_TRUE);

    frequency_index_ = -1; // radio button value is from 0, 1, 2, ...!
    memset(&sensors_indices_[0], 0, sizeof(sensors_indices_)); // clear index markers

    /* Genlist Title Item Style */
    titc->item_style = "title";
    // titc->item_style = "title_with_groupindex";
    titc->func.text_get = genlist_title_radio_get_;
    // itc->func.content_get = ;
    titc->func.del = genlist_del_;

    /* Genlist Item Style */
    //itc->item_style = "default";
    itc->item_style = "1text.1icon.1";
    itc->func.text_get = genlist_text_radio_get_;
    itc->func.content_get = genlist_add_radio_;
    //itc->func.content_get = genlist_add_checkbox_;

    itc->func.del = genlist_del_;

    /* Genlist Padding Item Style */
    pitc->item_style = "padding";

    /* Title Item Here */
    Elm_Object_Item *item = elm_genlist_item_append(genlist, titc, NULL, NULL, ELM_GENLIST_ITEM_GROUP, NULL, ad);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

    /* Main Menu Items Here */
    for (int index = 0; index < frequency_list_size; index++) {
        id = reinterpret_cast<Genlist_item_data *>(calloc(sizeof(Genlist_item_data), 1));
        id->index = index;
        id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, NULL, ad);
    }

    /* Padding Item Here */
    elm_genlist_item_append(genlist, pitc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, ad);

    Evas_Object *button = elm_button_add(layout);
    elm_object_style_set(button, "bottom");
    std::string text = std::string(LABEL_STYLE_START) + std::string(_("OK")) + std::string(LABEL_STYLE_END);
    elm_object_text_set(button, text.c_str());
    elm_object_part_content_set(layout, "elm.swallow.button.done", button);

    evas_object_smart_callback_add(button, "clicked", create_checkbox_list_view, ad);

    evas_object_show(button);

    ad->genlist = genlist;  // save it for refresh radio button later
    nf_it = elm_naviframe_item_push(naviframe, "radio_list", NULL, NULL, layout, "empty");
    elm_naviframe_item_pop_cb_set(nf_it, main_menu_finished_cb, ad);  // exit if Backkey pressed
}

static char* genlist_title_checkbox_get_(void *data, Evas_Object *obj, const char *part) {
    char buf[101];
    std::string text = std::string(LABEL_STYLE_START) + std::string(_(View::IDS_sensors_label1)) + std::string(LABEL_STYLE_END);
    snprintf(buf, 100, "%s", text.c_str());
    return strdup(buf);
}

static char* genlist_text_checkbox_get_(void *data, Evas_Object *obj, const char *part) {
    char buf[101];
    Genlist_item_data *id = reinterpret_cast<Genlist_item_data *>(data);
    int index = id->index;
    int size = sizeof(View::IDS_sensors_label2)/sizeof(View::IDS_sensors_label2[0]);
    assert(index < size);
    if (!strcmp(part, "elm.text") && index < size) {
    	const char* str = View::IDS_sensors_label2[index];
        std::string text = std::string(LABEL_STYLE_START) + std::string(_(str)) + std::string(LABEL_STYLE_END);
        snprintf(buf, 100, "%s", text.c_str());
        return strdup(buf);
    }
    return NULL;
}

void View::checkbox_btn_changed_cb_(void *data, Evas_Object *obj, void *event_info) {
}

Evas_Object* View::genlist_add_checkbox_(void *data, Evas_Object *obj, const char *part) {
    Genlist_item_data *id = reinterpret_cast<Genlist_item_data *>(data);
    int index = id->index;
    size_t size = sizeof(IDS_sensors_label2)/sizeof(IDS_sensors_label2[0]);
    assert(index < size);
	if (!strcmp(part, "elm.icon")) {
		Evas_Object *check = elm_check_add(obj);
		elm_check_state_set(check, EINA_FALSE);
		elm_check_state_pointer_set(check, (Eina_Bool *)&sensors_indices_[index]);  // 0 or 1
		evas_object_smart_callback_add(check, "changed", checkbox_btn_changed_cb_, data);
		evas_object_propagate_events_set(check, EINA_FALSE);
		return check;
	}
    return NULL;
}

void View::create_checkbox_list_view(void* data, Evas_Object* obj, void* event_info) {
    assert(ad == data);
	char edj_path[PATH_MAX] = {0, };
    Evas_Object *genlist = NULL;
    Evas_Object *naviframe = ad->naviframe;
    Evas_Object *layout = NULL;
    Elm_Object_Item *nf_it = NULL;
    Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
    Elm_Genlist_Item_Class *titc = elm_genlist_item_class_new();
    Elm_Genlist_Item_Class *pitc = elm_genlist_item_class_new();
    Genlist_item_data *id = NULL;

    /* Create Genlist */
    app_get_resource(EDJ_FILE, edj_path, (int)PATH_MAX);
    layout = elm_layout_add(naviframe);
    elm_layout_file_set(layout, edj_path, "activity_layout");  // reuse the activity layout
    evas_object_show(layout);
    genlist = elm_genlist_add(layout);
    elm_object_part_content_set(layout, "elm.swallow.content.genlist", genlist);
    evas_object_show(genlist);

    elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
    evas_object_smart_callback_add(genlist, "selected", NULL, NULL);
    ad->circle_genlist = eext_circle_object_genlist_add(genlist, ad->circle_surface);
    eext_circle_object_genlist_scroller_policy_set(ad->circle_genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
    eext_rotary_object_event_activated_set(ad->circle_genlist, EINA_TRUE);

    /* Genlist Title Item Style */
    titc->item_style = "title";
    titc->func.text_get = genlist_title_checkbox_get_;
    titc->func.del = genlist_del_;

    /* Genlist Item Style */
    itc->item_style = "1text.1icon.1";
    itc->func.text_get = genlist_text_checkbox_get_;
    itc->func.content_get = genlist_add_checkbox_;

    itc->func.del = genlist_del_;

    /* Genlist Padding Item Style */
    pitc->item_style = "padding";

    /* Title Item Here */
    Elm_Object_Item *item = elm_genlist_item_append(genlist, titc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, ad);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

    /* Main Menu Items Here */
    for (int index = 0; index < sensors_list_size; index++) {
        id = reinterpret_cast<Genlist_item_data *>(calloc(sizeof(Genlist_item_data), 1));
        id->index = index;
        id->item = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, NULL, ad);
    }

    /* Padding Item Here */
    elm_genlist_item_append(genlist, pitc, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, ad);

    Evas_Object *button = elm_button_add(layout);
    elm_object_style_set(button, "bottom");
    std::string text = std::string(LABEL_STYLE_START) + std::string(_("OK")) + std::string(LABEL_STYLE_END);
    elm_object_text_set(button, text.c_str());
    elm_object_part_content_set(layout, "elm.swallow.button.done", button);
    evas_object_smart_callback_add(button, "clicked", create_recording_progress_view, ad);
    evas_object_show(button);

    nf_it = elm_naviframe_item_push(naviframe, "checkbox_list", NULL, NULL, layout, "empty");
    elm_naviframe_item_pop_cb_set(nf_it, main_menu_finished_cb, ad);  // exit if Backkey pressed
}
