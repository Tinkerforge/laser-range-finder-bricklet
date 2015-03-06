/* laser-range-finder-bricklet
 * Copyright (C) 2015 Olaf Lüke <olaf@tinkerforge.com>
 *
 * laser-range-finder.c: Implementation of Laser Range Finder Bricklet messages
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

#include "laser-range-finder.h"

#include "bricklib/bricklet/bricklet_communication.h"
#include "bricklib/utility/util_definitions.h"
#include "bricklib/drivers/adc/adc.h"
#include "brickletlib/bricklet_entry.h"
#include "brickletlib/bricklet_simple.h"
#include "config.h"

#define I2C_EEPROM_ADDRESS_HIGH 84

#define I2C_ADDRESS_HIGH 0x62 // 7 bit, first bit = read/write
#define I2C_ADDRESS_LOW  0x62

#define SIMPLE_UNIT_DISTANCE 0

const SimpleMessageProperty smp[] = {
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_VALUE, SIMPLE_DIRECTION_GET}, // TYPE_GET_DISTANCE_VALUE
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_PERIOD, SIMPLE_DIRECTION_SET}, // TYPE_SET_DISTANCE_CALLBACK_PERIOD
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_PERIOD, SIMPLE_DIRECTION_GET}, // TYPE_GET_DISTANCE_CALLBACK_PERIOD
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_THRESHOLD, SIMPLE_DIRECTION_SET}, // TYPE_SET_DISTANCE_CALLBACK_THRESHOLD
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_THRESHOLD, SIMPLE_DIRECTION_GET}, // TYPE_GET_DISTANCE_CALLBACK_THRESHOLD
	{0, SIMPLE_TRANSFER_DEBOUNCE, SIMPLE_DIRECTION_SET}, // TYPE_SET_DEBOUNCE_PERIOD
	{0, SIMPLE_TRANSFER_DEBOUNCE, SIMPLE_DIRECTION_GET}, // TYPE_GET_DEBOUNCE_PERIOD
};

const SimpleUnitProperty sup[] = {
	{distance_from_lidar, SIMPLE_SIGNEDNESS_INT, FID_DISTANCE, FID_DISTANCE_REACHED, SIMPLE_UNIT_DISTANCE}, // distance
};

const uint8_t smp_length = sizeof(smp);

void invocation(const ComType com, const uint8_t *data) {
	switch(((MessageHeader*)data)->fid) {
		case FID_GET_DISTANCE_VALUE:
		case FID_SET_DISTANCE_CALLBACK_PERIOD:
		case FID_GET_DISTANCE_CALLBACK_PERIOD:
		case FID_SET_DISTANCE_CALLBACK_THRESHOLD:
		case FID_GET_DISTANCE_CALLBACK_THRESHOLD:
		case FID_SET_DEBOUNCE_PERIOD:
		case FID_GET_DEBOUNCE_PERIOD: {
			simple_invocation(com, data);
			break;
		}

		case FID_SET_MOVING_AVERAGE: {
			set_moving_average(com, (SetMovingAverage*)data);
			break;
		}

		case FID_GET_MOVING_AVERAGE: {
			get_moving_average(com, (GetMovingAverage*)data);
			break;
		}

		default: {
			BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
			break;
		}
	}
}

void constructor(void) {
	_Static_assert(sizeof(BrickContext) <= BRICKLET_CONTEXT_MAX_SIZE, "BrickContext too big");

	// TODO: Configure LIDAR
	// TODO: Get first distance measurement here to fill moving average

	BC->moving_average_upto = DEFAULT_MOVING_AVERAGE;
	reinitialize_moving_average();

	simple_constructor();
}

void destructor(void) {
	simple_destructor();
}

int32_t distance_from_lidar(const int32_t value) {
	uint16_t distance = i2c_read_distance();

	BC->moving_average_sum = BC->moving_average_sum -
	                         BC->moving_average[BC->moving_average_tick] +
	                         distance;

	BC->moving_average[BC->moving_average_tick] = distance;
	BC->moving_average_tick = (BC->moving_average_tick + 1) % BC->moving_average_upto;

	return (BC->moving_average_sum + BC->moving_average_upto/2)/BC->moving_average_upto;
}

uint16_t i2c_read_distance(void) {
	// TODO: Implement me
	return 0;
}

void reinitialize_moving_average(void) {
	int32_t initial_value = BA->adc_channel_get_data(BS->adc_channel);
	for(uint8_t i = 0; i < BC->moving_average_upto; i++) {
		BC->moving_average[i] = initial_value;
	}
	BC->moving_average_tick = 0;
	BC->moving_average_sum = initial_value*BC->moving_average_upto;
}


void set_moving_average(const ComType com, const SetMovingAverage *data) {
	if(BC->moving_average_upto != data->length) {
		if(data->length < 1) {
			BC->moving_average_upto = 1;
		} else if(data->length > MAX_MOVING_AVERAGE) {
			BC->moving_average_upto = MAX_MOVING_AVERAGE;
		} else {
			BC->moving_average_upto = data->length;
		}

		reinitialize_moving_average();
	}

	BA->com_return_setter(com, data);
}

void get_moving_average(const ComType com, const GetMovingAverage *data) {
	GetMovingAverageReturn gmar;
	gmar.header        = data->header;
	gmar.header.length = sizeof(GetMovingAverageReturn);
	gmar.length        = BC->moving_average_upto;

	BA->send_blocking_with_timeout(&gmar, sizeof(GetMovingAverageReturn), com);
}

void tick(const uint8_t tick_type) {
	simple_tick(tick_type);
}
