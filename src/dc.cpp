#include "dc.h"
#include "main.h"

//#define USE_LOG_FILE

#ifdef USE_LOG_FILE
class MyLog {
public:
	MyLog() {
		FILE* fp = fopen(mylog, "a");
		if (fp) {
			logfp = fp;
		} else {
			assert(false);
		}
	}
	FILE* getfp() {
		return logfp;
	}
	~MyLog() {
		if (logfp) {
			fclose(logfp);
		}
	}
private:
	const char* mylog = "/opt/usr/media/Downloads/mylog-dc.txt";
	FILE* logfp = NULL;
};

static MyLog g_mylog;
#define PrintF(...) fprintf(g_mylog.getfp(), __VA_ARGS__)
#else
#define PrintF(...)
#endif

/**
 * @breif: Construct an Item for a sensor, set options, filename and flags for Data Capture module
 */
Dc::Item::Item(sensor_type_e id,
    size_t num_ms,  		// time interval
    std::string id_name,	// sensor name id
    size_t target_num,		// number of samples to save
    sensor_event_cb cb,		// sensor event callback
	bool save_data,			// true/false: save to file or not
    sensor_option_e option) :
		id(id),
		num_ms(num_ms),
		option(option),
		listener(nullptr),
		enabled(false),
		fp(nullptr),
		timestamp0(0.0),
		start_stop(Dc::Item::START_RUNNING),
		target_num(target_num),
		infile_num(0),
		callback(cb),
		save_data(save_data) {
			samples_count = 0;
			filename = create_file_name(id_name, "csv");
        	PrintF("MLOG: Item::Item: filename=%s\n", filename.c_str());
}

/**
 * @breif: Make sure file is properly closed or conditionally removed when an item is destroyed
 */
Dc::Item::~Item() {
    if (fp) {
       fclose(fp);
       fp = nullptr;
    }
    if (listener) {
        int ok = SENSOR_ERROR_NONE;
        sensor_listener_unset_event_cb(listener);
        if ((sensor_listener_stop(listener) == ok)
            && (sensor_destroy_listener(listener) == ok)) {
            // listener destroy: OK
        	PrintF("MLOG: Item::~Item: sensor destroy: OK: name=%s\n", filename.c_str());
        } else {
            // listener destroy: Error
        	PrintF("MLOG: Item::~Item: sensor destroy: Error: name=%s\n", filename.c_str());
        }
    }
}

void Dc::SENSOR_ACCELEROMETER_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
		case SENSOR_ACCELEROMETER:
		{
		  auto elem = Dc::get(type, ad->items);
		  if (elem->start_stop != Item::START_RUNNING) break;
		  if (elem->target_num == 0) {
			  break;
		  }
		  if (write_to_file3(elem, event)) {  // last sample, file closed
			elem->start_stop = 0;
		  }
		}
		break;
		default:
		break;
    }
}

void Dc::SENSOR_LINEAR_ACCELERATION_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
        case SENSOR_LINEAR_ACCELERATION:
        {
			auto elem = Dc::get(type, ad->items);
			if (elem->start_stop != Item::START_RUNNING) break;
			if (elem->target_num == 0) {
				break;
			}
			if (write_to_file3(elem, event)) {  // last sample, file closed
			  elem->start_stop = 0;
			}
        }
		break;
		default:
		break;
    }
}

void Dc::SENSOR_GYROSCOPE_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
      case SENSOR_GYROSCOPE:
      {
        auto elem = Dc::get(type, ad->items);
        if (elem->start_stop != Item::START_RUNNING) break;
        if (elem->target_num == 0) {
            break;
        }
        if (write_to_file3(elem, event)) {  // last sample, file closed
            elem->start_stop = 0;
        }
      }
		break;
		default:
		break;
    }
}

void Dc::SENSOR_MAGNETIC_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
      case SENSOR_MAGNETIC:
      {
        auto elem = Dc::get(type, ad->items);
        if (elem->start_stop != Item::START_RUNNING) break;
        if (elem->target_num == 0) {
            break;
        }
        if (write_to_file3(elem, event)) {  // last sample, file closed
            elem->start_stop = 0;
        }
      }
		break;
		default:
		break;
    }
}

void Dc::SENSOR_ROTATION_VECTOR_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
      case SENSOR_ROTATION_VECTOR:
      {
        auto elem = Dc::get(type, ad->items);
        if (elem->start_stop != Item::START_RUNNING) break;
        if (elem->target_num == 0) {
            break;
        }
        if (write_to_file4(elem, event)) {  // last sample, file closed
            elem->start_stop = 0;
        }
      }
		break;
		default:
		break;
    }
}

void Dc::SENSOR_ORIENTATION_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
      case SENSOR_ORIENTATION:
      {
        auto elem = Dc::get(type, ad->items);
        if (elem->start_stop != Item::START_RUNNING) break;
        if (elem->target_num == 0) {
            break;
        }
        if (write_to_file3(elem, event)) {  // last sample, file closed
            elem->start_stop = 0;
        }
      }
		break;
		default:
		break;
    }
}

void Dc::SENSOR_HUMAN_PEDOMETER_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
      case SENSOR_ORIENTATION:
      {
        auto elem = Dc::get(type, ad->items);
        if (elem->start_stop != Item::START_RUNNING) break;
        if (elem->target_num == 0) {
            break;
        }
        bool is_float = false;
        if (write_to_file3(elem, event, is_float)) {  // last sample, file closed
            elem->start_stop = 0;
        }
      }
		break;
		default:
		break;
    }
}

// HRM-Pulse
void Dc::SENSOR_HRM_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
      case SENSOR_HRM:
      {
        auto elem = Dc::get(type, ad->items);
        if (elem->start_stop != Item::START_RUNNING) {
              break;
        }
        if (elem->target_num == 0) {
            break;
        }
        bool is_float = false;
        if (write_to_file(elem, event, is_float)) {  // last sample, file closed
            elem->start_stop = 0;
        }
      }
		break;
		default:
		break;
    }
}

void Dc::SENSOR_HRM_LED_GREEN_cb(sensor_h sensor, sensor_event_s* event, void *user_data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(user_data);
    sensor_type_e type;
    sensor_get_type(sensor, &type);
    switch(type) {
      case SENSOR_HRM_LED_GREEN:
      {
        auto elem = Dc::get(type, ad->items);
        if (elem->start_stop != Item::START_RUNNING) break;
        if (elem->target_num == 0) {
            break;
        }
        bool is_float = false;
        if (write_to_file(elem, event), is_float) {  // last sample, file closed
            elem->start_stop = 0;
        }
      }
		break;
		default:
		break;

    }
}



bool Dc::write_to_file(Item *elem, sensor_event_s* event, bool is_float) {
    if (elem->target_num == 0) return false;
    bool last_sample = false;
    if (elem->infile_num == 0) {
        elem->fp = fopen(elem->filename.c_str(), "w");  // keep file open until "STOP" clicked
        if (elem->fp && ++elem->infile_num) {
            elem->timestamp0 = static_cast<double>(event->timestamp);
            fprintf (elem->fp, "samples,timestamp\n");  // csv file header
            if (is_float) {
                fprintf(elem->fp, "%.6f,%d\n", event->values[0], 0);
            } else {
                fprintf(elem->fp, "%d,%d\n", int(event->values[0]), 0);
            }
        }
    } else {
        if (++elem->infile_num <= elem->target_num && elem->fp) {
            double t0 = elem->timestamp0;
            double curr = static_cast<double>(event->timestamp);  // in usec
            if (is_float) {
                fprintf(elem->fp, "%.6f,%d\n", event->values[0], static_cast<int>((curr - t0)));
            } else {
                fprintf(elem->fp, "%d,%d\n", int(event->values[0]), static_cast<int>((curr - t0)));
            }
            if (elem->infile_num == elem->target_num) {
                fclose(elem->fp);
                elem->fp = nullptr;
                return true;
            }
        }
    }
    return last_sample;
}

bool Dc::write_to_file3(Item *elem, sensor_event_s* event, bool is_float) {
    if (elem->target_num == 0) return false;
    bool last_sample = false;
    if (elem->infile_num == 0) {
        elem->fp = fopen(elem->filename.c_str(), "w");  // keep file open until "STOP" clicked
        if (elem->fp && ++elem->infile_num) {
            elem->timestamp0 = static_cast<double>(event->timestamp);
            fprintf (elem->fp, "x,y,z,timestamp\n");  // csv file header
            if (is_float) {
                fprintf(elem->fp, "%.6f,%.6f,%.6f,%d\n", event->values[0], event->values[1], event->values[2], 0);
            } else {
                fprintf(elem->fp, "%d,%d,%d,%d\n", int(event->values[0]), int(event->values[1]), int(event->values[2]), 0);
            }
        }
    } else {
        if (++elem->infile_num <= elem->target_num && elem->fp) {
            double t0 = elem->timestamp0;
            double curr = static_cast<double>(event->timestamp);  // in usec
            if (is_float) {
                fprintf(elem->fp, "%.6f,%.6f,%.6f,%d\n", event->values[0], event->values[1], event->values[2], static_cast<int>((curr - t0)));
            } else {
                fprintf(elem->fp, "%d,%d,%d,%d\n", int(event->values[0]), int(event->values[1]), int(event->values[2]), static_cast<int>((curr - t0)));
            }
            if (elem->infile_num == elem->target_num) {
                fclose(elem->fp);
                elem->fp = nullptr;
                return true;
            }
        }
    }
    return last_sample;
}

bool Dc::write_to_file4(Item *elem, sensor_event_s* event, bool is_float) {
    if (elem->target_num == 0) return false;
    bool last_sample = false;
    if (elem->infile_num == 0) {
        elem->fp = fopen(elem->filename.c_str(), "w");  // keep file open until "STOP" clicked
        if (elem->fp && ++elem->infile_num) {
            elem->timestamp0 = static_cast<double>(event->timestamp);
            fprintf (elem->fp, "x,y,z,w,timestamp\n");  // csv file header
            if (is_float) {
                fprintf(elem->fp, "%.6f,%.6f,%.6f,%.6f,%d\n", event->values[0], event->values[1], event->values[2], event->values[3], 0);
            } else {
                fprintf(elem->fp, "%d,%d,%d,%d,%d\n", int(event->values[0]), int(event->values[1]), int(event->values[2]), int(event->values[3]), 0);
            }
        }
    } else {
        if (++elem->infile_num <= elem->target_num && elem->fp) {
            double t0 = elem->timestamp0;
            double curr = static_cast<double>(event->timestamp);  // in usec
            if (is_float) {
                fprintf(elem->fp, "%.6f,%.6f,%.6f,%.6f,%d\n", event->values[0], event->values[1], event->values[2], event->values[3], static_cast<int>((curr - t0)));
            } else {
                fprintf(elem->fp, "%d,%d,%d,%d,%d\n",int(event->values[0]), int(event->values[1]), int(event->values[2]), int(event->values[3]), static_cast<int>((curr - t0)));
            }
            if (elem->infile_num == elem->target_num) {
                fclose(elem->fp);
                elem->fp = nullptr;
                return true;
            }
        }
    }
    return last_sample;
}

/**
 * @brief: helper functions to make file name with date time post fix
 */
std::string Dc::create_file_name(std::string name, std::string extension) {
    std::string abs_filename = filepath().append(name);
    abs_filename.append("-");
    abs_filename.append(date_and_time());
    abs_filename.append(".");
    abs_filename.append(extension);
    return abs_filename;
}

std::string Dc::date_and_time() {
	char buffer[81];
	time_t now = time(0);
	tm* ltm = localtime(&now);
	memset(buffer, 0, sizeof(buffer));
	strftime(buffer, sizeof(buffer) - 1, "%Y%m%d-%H%M%S", ltm);
	return std::string(buffer);
}

void Dc::start(void *data) {
    appdata_s* ad = reinterpret_cast<appdata_s*>(data);

	int f = View::get_sample_frequency();

	int ms =  1000 / f;  			// in num_ms
    int N = 1 * 3600 * f;  			//
	PrintF("MLOG: start: +: f=%d, ms=%d\n", f, ms);

	/*
	// The switch-case must match this table (defined in view.cpp)
	const char* View::IDS_sensors_label2[] = {
		"Accelerometer",
		"Linear Acceleration",
		"Gyroscope",
		"Magnetometer"
		"Rotation vector",
		"Orientation",
		"Pedometer",
		"HRM-pulse",
		"HRM-Green",
	};
	*/

	for (int i = 0; i < View::sensors_list_size; i++) {
		PrintF("MLOG: i=%d, checked=%d, sensor=%s\n", i, View::is_sensor_selected(i), View::IDS_sensors_label2[i]);
		if (View::is_sensor_selected(i)) {
			switch (i) {
			case 0:  // Accelerometer
		    	ad->items.push_back(Dc::Item(SENSOR_ACCELEROMETER, ms, std::string("accel"), N, SENSOR_ACCELEROMETER_cb, true));
				break;
			case 1:  // Linear Acceleration
		    	ad->items.push_back(Dc::Item(SENSOR_LINEAR_ACCELERATION, ms, std::string("accel-linear"), N, SENSOR_LINEAR_ACCELERATION_cb, true));
				break;
			case 2:  // Gyroscope
		    	ad->items.push_back(Dc::Item(SENSOR_GYROSCOPE, ms, std::string("gyroscope"), N, SENSOR_GYROSCOPE_cb, true));
				break;
			case 3:  // Magnetometer
		    	ad->items.push_back(Dc::Item(SENSOR_MAGNETIC, ms, std::string("magnetic"), N, SENSOR_MAGNETIC_cb, true));
				break;
			case 4:  // Rotation vector
		    	ad->items.push_back(Dc::Item(SENSOR_ROTATION_VECTOR, ms, std::string("rotation"), N, SENSOR_ROTATION_VECTOR_cb, true));
				break;
			case 5:  // Orientation
		    	ad->items.push_back(Dc::Item(SENSOR_ORIENTATION, ms, std::string("orientation"), N, SENSOR_ORIENTATION_cb, true));
				break;
			case 6:  // Pedometer
		    	ad->items.push_back(Dc::Item(SENSOR_HUMAN_PEDOMETER, ms, std::string("pedometer"), N, SENSOR_HUMAN_PEDOMETER_cb, true));
				break;
			case 7:  // HRM-pulse
		    	ad->items.push_back(Dc::Item(SENSOR_HRM, ms, std::string("hrm-pulse"), N, SENSOR_HRM_cb, true));
				break;
			case 8:  // HRM-Green
		    	ad->items.push_back(Dc::Item(SENSOR_HRM_LED_GREEN, ms, std::string("hrm-green"), N, SENSOR_HRM_LED_GREEN_cb, true));
				break;
			default:
				break;
			}

		}
	}

	// register items for listeners here
    int ok = SENSOR_ERROR_NONE;
    int num_actived = 0;
    for (auto& elem : ad->items) {
		bool supported = false;
		sensor_h sensor;
		sensor_is_supported(elem.id, &supported);
		PrintF("MLOG: elem name=%s, supported=%d\n", elem.filename.c_str(), supported);
		if (supported && sensor_get_default_sensor(elem.id, &sensor) == ok) {
			if ((sensor_create_listener(sensor, &elem.listener) == ok)
				&& (sensor_listener_set_event_cb(elem.listener, elem.num_ms, elem.callback, ad) == ok)
				&& (sensor_listener_set_option(elem.listener, elem.option) == ok)) {
				if (sensor_listener_start(elem.listener) == ok) {
					elem.enabled = true;
					num_actived++;
				} else {
					dlog_print(DLOG_ERROR, LOG_TAG, "SENSOR_ID: %d, Error", elem.id);
					PrintF("MLOG: Error\n");
				}
			} else {
				PrintF("MLOG: Error\n");
			}
		} else {
			PrintF("MLOG: Error\n");
		}
    }
	PrintF("MLOG: start: -: items size=%d, activated=%d \n", ad->items.size(), num_actived);
}

void Dc::stop(void *data) {
	PrintF("MLOG: stop: + \n");
    appdata_s* ad = reinterpret_cast<appdata_s*>(data);
	ad->items.clear();  // close files and stop sensors
	PrintF("MLOG: stop: - \n");
}
// -----------------------------------------------------------------------------

