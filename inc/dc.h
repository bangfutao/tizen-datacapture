#ifndef __DC__H__
#define __DC__H__
#include <app_common.h>
#include <storage-expand.h>
#include "sensor.h"
#include <string>
#include <queue>
#include <ctime>
#include <algorithm>
#include <assert.h>

#define USE_DOWNLOADS_PATH  	// option to set file path: Downloads or private app data folder

/**
 * Implementation of a Data Capture module for Tizen app
 *
 * An Item is holding necessary information for saving samples to a file
 * One Item is for one sensor, it supports Start/Stop style data capture
 *
 */
class Dc {
  public:
    struct Item {
        sensor_type_e id;           	// sensor Id enum
        size_t num_ms;              	// interval time
        sensor_option_e option;     	// sensor option enum
        sensor_listener_h listener; 	// sensor event listener
        bool enabled;               	// set if sensor is activated
        FILE* fp;                   	// save sensor samples
        double timestamp0;          	// timestamp of 1st sample
        int start_stop;             	// set when "Start" button is clicked, cleared when samples saved
        std::string filename;           // full filename with DateTime tagged
        size_t target_num;              // set number of samples to be saved
        size_t infile_num;              // keep records of how many samples have been written in file
        sensor_event_cb callback;       // callback function for saving data
        bool save_data;					// save data to file or not
        int samples_count;				// samples count from sensor start
        static const int START_RUNNING = 1;     // start or stop recording flag

        /**
         * @breif: Construct an Item for a sensor, and set options, filename and flags for Data Capture module
         */
        Item(sensor_type_e id, size_t time_interval, std::string id_name,
        	 size_t target_num, sensor_event_cb cb, bool save_data,
			 sensor_option_e option = SENSOR_OPTION_ALWAYS_ON);
        /**
         * Make sure file is properly closed when an item is destroyed
         */
        virtual ~Item();

        private:
            // Disable assignment copy constructor
            Item& operator=(const Item&);
    };  // Item definition

    /**
     * find the item for a sensor id from items list
     */
    static Item* get(sensor_type_e id, std::vector<Item>& v) {
        for ( auto &elem : v ) {
           if (elem.id == id) return &elem;
        }
        return nullptr;
    }

    /**
     * The function return a file path only, not include actual file name
     */
    static std::string filepath() {
        std::string file_path;
        char* path = nullptr;
#ifdef USE_DOWNLOADS_PATH
        storage_get_directory(STORAGE_TYPE_INTERNAL, STORAGE_DIRECTORY_DOWNLOADS, &path);
		file_path = std::string(path) + std::string("/");
#else
		path = app_get_data_path();
		file_path = std::string(path);
#endif
		if(path) {
			free(path);
		} else {
			assert(path);
		}
        return file_path;
    }

    /**
     * return current datetime string in format, eg: 20200717-103000
     */
    static std::string date_and_time();

    /**
     * return full path and name with date time suffix and extension
     */
    static std::string create_file_name(std::string name, std::string extension);

    /**
     * start sensors for user selected ones
     */
    static void start(void *data);

    /**
     * close files and stop sensors
     */
    static void stop(void *data);

  private:

    /**
     * Dc class constructor: Dc object instantiation is not allowed
     */
    Dc() {}

    /**
     * The functions are sensor event callback, one sensor has one callback
     * It manages collecting data, create file, counting samples and close files when done.
     */
    static void SENSOR_ACCELEROMETER_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    static void SENSOR_LINEAR_ACCELERATION_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    static void SENSOR_GYROSCOPE_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    static void SENSOR_MAGNETIC_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    static void SENSOR_ROTATION_VECTOR_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    static void SENSOR_ORIENTATION_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    static void SENSOR_HUMAN_PEDOMETER_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    static void SENSOR_HRM_cb(sensor_h sensor, sensor_event_s* event, void *user_data);  // pulse

    static void SENSOR_HRM_LED_GREEN_cb(sensor_h sensor, sensor_event_s* event, void *user_data);

    /**
     * Save samples to file and close file when reach target
     * return true if it is the last sample
     */
    static bool write_to_file(Item *elem, sensor_event_s* event, bool is_float = true);  	// single value
    static bool write_to_file3(Item *elem, sensor_event_s* event, bool is_float = true);	// 3 values: x, y, z
    static bool write_to_file4(Item *elem, sensor_event_s* event, bool is_float = true);	// 3 values: x, y, z
};
#endif
