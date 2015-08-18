/* laser-range-finder-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config.h: Laser Range Finder Bricklet specific configuration
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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib/drivers/board/sam3s/SAM3S.h"

#include "laser-range-finder.h"

#define BRICKLET_FIRMWARE_VERSION_MAJOR 2
#define BRICKLET_FIRMWARE_VERSION_MINOR 0
#define BRICKLET_FIRMWARE_VERSION_REVISION 1

#define BRICKLET_HARDWARE_VERSION_MAJOR 1
#define BRICKLET_HARDWARE_VERSION_MINOR 0
#define BRICKLET_HARDWARE_VERSION_REVISION 0

#define BRICKLET_DEVICE_IDENTIFIER 255

#define BOARD_MCK 64000000

#define PIN_PWR (BS->pin1_ad)
#define PIN_SDA (BS->pin2_da)
#define PIN_SCL (BS->pin3_pwm)
#define PIN_PWM (BS->pin4_io)

#define LOGGING_LEVEL LOGGING_DEBUG
#define DEBUG_BRICKLET 0

#define BRICKLET_VALUE_APPLIED_OUTSIDE
#define BRICKLET_HAS_SIMPLE_SENSOR
#define BRICKLET_NO_OFFSET
#define INVOCATION_IN_BRICKLET_CODE
#define NUM_SIMPLE_VALUES 2

#define MAX_MOVING_AVERAGE 30
#define DEFAULT_MOVING_AVERAGE_DISTANCE 10
#define DEFAULT_MOVING_AVERAGE_VELOCITY 5

typedef enum {
	MS_DISABLED,
	MS_UPDATE_MODE,
	MS_START_ACQUISITION,
	MS_READ_DISTANCE,
	MS_TAKE_VELOCITY_MEASUREMENT,
	MS_READ_VELOCITY,
} MeasurementState;

typedef struct {
	int32_t value[NUM_SIMPLE_VALUES];
	int32_t last_value[NUM_SIMPLE_VALUES];

	uint32_t signal_period[NUM_SIMPLE_VALUES];
	uint32_t signal_period_counter[NUM_SIMPLE_VALUES];

	uint32_t threshold_debounce;
	uint32_t threshold_period_current[NUM_SIMPLE_VALUES];
	int32_t threshold_min[NUM_SIMPLE_VALUES];
	int32_t threshold_max[NUM_SIMPLE_VALUES];

	int32_t threshold_min_save[NUM_SIMPLE_VALUES];
	int32_t threshold_max_save[NUM_SIMPLE_VALUES];

	uint32_t tick;

	char threshold_option_save[NUM_SIMPLE_VALUES];
	char threshold_option[NUM_SIMPLE_VALUES];

	int16_t moving_average_distance[MAX_MOVING_AVERAGE];
	int16_t moving_average_velocity[MAX_MOVING_AVERAGE];
	int32_t moving_average_sum[NUM_SIMPLE_VALUES];
	uint8_t moving_average_tick[NUM_SIMPLE_VALUES];
	uint8_t moving_average_upto[NUM_SIMPLE_VALUES];

	uint8_t new_mode;
	bool update_mode;
	bool laser_enabled;
	MeasurementState measurement_state;
	uint8_t next_measurement_state_counter;
} BrickContext;

#endif
