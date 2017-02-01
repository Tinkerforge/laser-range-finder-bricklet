/* laser-range-finder-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * laser-range-finder.h: Implementation of Laser Range Finder Bricklet messages
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef LASER_RANGE_FINDER_H
#define LASER_RANGE_FINDER_H

#include <stdint.h>
#include "bricklib/com/com_common.h"

#define FID_GET_DISTANCE 1
#define FID_GET_VELOCITY 2
#define FID_SET_DISTANCE_CALLBACK_PERIOD 3
#define FID_GET_DISTANCE_CALLBACK_PERIOD 4
#define FID_SET_VELOCITY_CALLBACK_PERIOD 5
#define FID_GET_VELOCITY_CALLBACK_PERIOD 6
#define FID_SET_DISTANCE_CALLBACK_THRESHOLD 7
#define FID_GET_DISTANCE_CALLBACK_THRESHOLD 8
#define FID_SET_VELOCITY_CALLBACK_THRESHOLD 9
#define FID_GET_VELOCITY_CALLBACK_THRESHOLD 10
#define FID_SET_DEBOUNCE_PERIOD 11
#define FID_GET_DEBOUNCE_PERIOD 12
#define FID_SET_MOVING_AVERAGE 13
#define FID_GET_MOVING_AVERAGE 14
#define FID_SET_MODE 15
#define FID_GET_MODE 16
#define FID_ENABLE_LASER 17
#define FID_DISABLE_LASER 18
#define FID_IS_LASER_ENABLED 19
#define FID_DISTANCE 20
#define FID_VELOCITY 21
#define FID_DISTANCE_REACHED 22
#define FID_VELOCITY_REACHED 23
#define FID_GET_SENSOR_HARDWARE_VERISON 24
#define FID_SET_CONFIGURATION 25
#define FID_GET_CONFIGURATION 26

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) StandardMessage;

typedef struct {
	MessageHeader header;
	uint8_t distance_average_length;
	uint8_t velocity_average_length;
} __attribute__((__packed__)) SetMovingAverage;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetMovingAverage;

typedef struct {
	MessageHeader header;
	uint8_t distance_average_length;
	uint8_t velocity_average_length;
} __attribute__((__packed__)) GetMovingAverageReturn;

typedef struct {
	MessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) SetMode;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetMode;

typedef struct {
	MessageHeader header;
	uint8_t mode;
} __attribute__((__packed__)) GetModeReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) EnableLaser;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) DisableLaser;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) IsLaserEnabled;

typedef struct {
	MessageHeader header;
	bool laser_enabled;
} __attribute__((__packed__)) IsLaserEnabledReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetSensorHardwareVersion;

typedef struct {
	MessageHeader header;
	uint8_t version;
} __attribute__((__packed__)) GetSensorHardwareVersionReturn;

typedef struct {
	MessageHeader header;
	uint8_t acquisition_count;
	bool enable_quick_termination;
	uint8_t threshold_value;
	uint16_t measurement_frequency;
} __attribute__((__packed__)) SetConfiguration;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetConfiguration;

typedef struct {
	MessageHeader header;
	uint8_t acquisition_count;
	bool enable_quick_termination;
	uint8_t threshold_value;
	uint16_t measurement_frequency;
} __attribute__((__packed__)) GetConfigurationReturn;

void set_moving_average(const ComType com, const SetMovingAverage *data);
void get_moving_average(const ComType com, const GetMovingAverage *data);
void set_mode(const ComType com, const SetMode *data);
void get_mode(const ComType com, const GetMode *data);
void is_laser_enabled(const ComType com, const IsLaserEnabled *data);
void get_sensor_hardware_version(const ComType com, const GetSensorHardwareVersion *data);
void set_configuration(const ComType com, const SetConfiguration *data);
void get_configuration(const ComType com, const GetConfiguration *data);

void reinitialize_moving_average(const uint8_t type);
bool lidar_read_register(const uint8_t reg, const uint8_t length, uint8_t *data);
bool lidar_write_register(const uint8_t reg, const uint8_t length, const uint8_t *data);

bool i2c_scl_value(void);
void i2c_scl_high(void);
void i2c_scl_low(void);
bool i2c_sda_value(void);
void i2c_sda_high(void);
void i2c_sda_low(void);
void i2c_sleep_halfclock(void);
void i2c_stop(void);
void i2c_start(void);
uint8_t i2c_recv_byte(bool ack);
bool i2c_send_byte(const uint8_t value);

void update_moving_average(const uint8_t type, const uint8_t length);
void new_distance_value(const uint16_t distance);
void new_velocity_value(const int8_t velocity);

void invocation(const ComType com, const uint8_t *data);
void constructor(void);
void destructor(void);
void tick(const uint8_t tick_type);


#define CONTINOUS_RW                0x80

// Internal Control Registers
#define REG_CONTROL                  0x00
#define REG_MODE_STATUS              0x01
#define REG_MAX_ACQUISITION_COUNT    0x02
#define REG_CORRELATION_SETTING      0x03
#define REG_ACQUISITION_MODE_CONTROL 0x04
#define REG_THRESHOLD_OFFSET         0x05
#define REG_REFERENCE_DELAY          0x06 // + 0x07
#define REG_REFERENCE_PEAK_VALUE     0x08
#define REG_VELOCITY_MEASUREMENT     0x09
#define REG_SIGNAL_DELAY             0x0A // + 0x0B
#define REG_SIGNAL_PEAK_VALUE        0x0C
#define REG_NOISE_FLOOR              0x0D
#define REG_SIGNAL_STRENGTH          0x0E
#define REG_DISTANCE                 0x0F // + 0x10
#define REG_DC_THRESHOLD             0x11
#define REG_DELAY_ADD                0x12
#define REG_DISTANCE_CALIBRATION     0x13
#define REG_PREV_MEASURED_DISTANCE   0x14 // + 0x15
#define REG_THRESHOLD_BYPASS         0x1C

// External Control Registers
#define REG_MEASURE_DELAY           0x45
#define REG_VELOCITY_RESOLUTION     0x68

#define LIDAR_VERSION_1 1
#define LIDAR_VERSION_3 3

#endif
