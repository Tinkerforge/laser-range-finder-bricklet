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

#define FID_GET_DISTANCE_VALUE 1
#define FID_SET_DISTANCE_CALLBACK_PERIOD 2
#define FID_GET_DISTANCE_CALLBACK_PERIOD 3
#define FID_SET_DISTANCE_CALLBACK_THRESHOLD 4
#define FID_GET_DISTANCE_CALLBACK_THRESHOLD 5
#define FID_SET_DEBOUNCE_PERIOD 6
#define FID_GET_DEBOUNCE_PERIOD 7
#define FID_SET_MOVING_AVERAGE 8
#define FID_GET_MOVING_AVERAGE 9
#define FID_DISTANCE 10
#define FID_DISTANCE_REACHED 11

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) StandardMessage;

typedef struct {
	MessageHeader header;
	uint8_t length;
} __attribute__((__packed__)) SetMovingAverage;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetMovingAverage;

typedef struct {
	MessageHeader header;
	uint8_t length;
} __attribute__((__packed__)) GetMovingAverageReturn;

void set_moving_average(const ComType com, const SetMovingAverage *data);
void get_moving_average(const ComType com, const GetMovingAverage *data);

int32_t distance_from_lidar(const int32_t value);
uint16_t i2c_read_distance(void);
void reinitialize_moving_average(void);

void invocation(const ComType com, const uint8_t *data);
void constructor(void);
void destructor(void);
void tick(const uint8_t tick_type);


// Internal Control Registers
#define REG_CONTROL                 0x00
#define REG_MODE_STATUS             0x01
#define REG_MAX_AQUISITION_COUNT    0x02
#define REG_CORRELATION_SETTING     0x03
#define REG_AQUISATION_MODE_CONTROL 0x04
#define REG_THRESHOLD_OFFSET        0x05
#define REG_REFERENCE_DELAY         0x06 // + 0x07
#define REG_REFERENCE_PEAK_VALUE    0x08
#define REG_VELOCITY_MEASUREMENT    0x09
#define REG_SIGNAL_DELAY            0x0A // + 0x0B
#define REG_SIGNAL_PEAK_VALUE       0x0C
#define REG_NOISE_FLOOR             0x0D
#define REG_SIGNAL_STRENGTH         0x0E
#define REG_DISTANCE                0x0F // + 0x10
#define REG_DC_THRESHOLD            0x11
#define REG_DELAY_ADD               0x12
#define REG_DISTANCE_CALIBRATION    0x13
#define REG_PREV_MEASURED_DISTANCE  0x14 // + 0x15


#endif
