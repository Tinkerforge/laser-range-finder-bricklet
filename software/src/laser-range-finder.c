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
#define I2C_ADDRESS 0x62

#define I2C_HALF_CLOCK_100KHZ  5000  // 10us per clock
#define I2C_READ  1
#define I2C_WRITE 0

#define SIMPLE_UNIT_DISTANCE 0
#define SIMPLE_UNIT_VELOCITY 1

const SimpleMessageProperty smp[] = {
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_VALUE, SIMPLE_DIRECTION_GET}, // FID_GET_DISTANCE_VALUE
	{SIMPLE_UNIT_VELOCITY, SIMPLE_TRANSFER_VALUE, SIMPLE_DIRECTION_GET}, // FID_GET_VELOCITY_VALUE
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_PERIOD, SIMPLE_DIRECTION_SET}, // FID_SET_DISTANCE_CALLBACK_PERIOD
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_PERIOD, SIMPLE_DIRECTION_GET}, // FID_GET_DISTANCE_CALLBACK_PERIOD
	{SIMPLE_UNIT_VELOCITY, SIMPLE_TRANSFER_PERIOD, SIMPLE_DIRECTION_SET}, // FID_SET_VELOCITY_CALLBACK_PERIOD
	{SIMPLE_UNIT_VELOCITY, SIMPLE_TRANSFER_PERIOD, SIMPLE_DIRECTION_GET}, // FID_GET_VELOCITY_CALLBACK_PERIOD
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_THRESHOLD, SIMPLE_DIRECTION_SET}, // FID_SET_DISTANCE_CALLBACK_THRESHOLD
	{SIMPLE_UNIT_DISTANCE, SIMPLE_TRANSFER_THRESHOLD, SIMPLE_DIRECTION_GET}, // FID_GET_DISTANCE_CALLBACK_THRESHOLD
	{SIMPLE_UNIT_VELOCITY, SIMPLE_TRANSFER_THRESHOLD, SIMPLE_DIRECTION_SET}, // FID_SET_VELOCITY_CALLBACK_THRESHOLD
	{SIMPLE_UNIT_VELOCITY, SIMPLE_TRANSFER_THRESHOLD, SIMPLE_DIRECTION_GET}, // FID_GET_VELOCITY_CALLBACK_THRESHOLD
	{0, SIMPLE_TRANSFER_DEBOUNCE, SIMPLE_DIRECTION_SET}, // FID_SET_DEBOUNCE_PERIOD
	{0, SIMPLE_TRANSFER_DEBOUNCE, SIMPLE_DIRECTION_GET}, // FID_GET_DEBOUNCE_PERIOD
};

const SimpleUnitProperty sup[] = {
	{NULL, SIMPLE_SIGNEDNESS_UINT, FID_DISTANCE, FID_DISTANCE_REACHED, SIMPLE_UNIT_DISTANCE}, // distance
	{NULL, SIMPLE_SIGNEDNESS_INT,  FID_VELOCITY, FID_VELOCITY_REACHED, SIMPLE_UNIT_VELOCITY}, // velocity
};

const uint8_t smp_length = sizeof(smp);

void invocation(const ComType com, const uint8_t *data) {
	switch(((MessageHeader*)data)->fid) {
		case FID_GET_DISTANCE:
		case FID_GET_VELOCITY:
		case FID_SET_DISTANCE_CALLBACK_PERIOD:
		case FID_GET_DISTANCE_CALLBACK_PERIOD:
		case FID_SET_VELOCITY_CALLBACK_PERIOD:
		case FID_GET_VELOCITY_CALLBACK_PERIOD:
		case FID_SET_DISTANCE_CALLBACK_THRESHOLD:
		case FID_GET_DISTANCE_CALLBACK_THRESHOLD:
		case FID_SET_VELOCITY_CALLBACK_THRESHOLD:
		case FID_GET_VELOCITY_CALLBACK_THRESHOLD:
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

		case FID_SET_MODE: {
			set_mode(com, (SetMode*)data);
			break;
		}

		case FID_GET_MODE: {
			get_mode(com, (GetMode*)data);
			break;
		}

		case FID_IS_LASER_ENABLED: {
			is_laser_enabled(com, (IsLaserEnabled*)data);
			break;
		}

		case FID_ENABLE_LASER: {
			BC->laser_enabled = true;
			break;
		}

		case FID_DISABLE_LASER: {
			BC->laser_enabled = false;
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

	PIN_PWR.type = PIO_OUTPUT_0;
	PIN_PWR.attribute = PIO_DEFAULT;
	BA->PIO_Configure(&PIN_PWR, 1);

	PIN_SCL.type = PIO_INPUT;
	PIN_SCL.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_SCL, 1);

	PIN_SDA.type = PIO_INPUT;
	PIN_SDA.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_SCL, 1);

	PIN_PWM.type = PIO_INPUT;
	PIN_PWM.attribute = PIO_PULLUP;
	BA->PIO_Configure(&PIN_PWM, 1);

	SLEEP_MS(100);
	PIN_PWR.type = PIO_OUTPUT_1;
	BA->PIO_Configure(&PIN_PWR, 1);

	simple_constructor();

	BC->laser_enabled = false;
	BC->new_mode = 0;
	BC->moving_average_upto[SIMPLE_UNIT_DISTANCE] = DEFAULT_MOVING_AVERAGE_DISTANCE;
	BC->moving_average_upto[SIMPLE_UNIT_VELOCITY] = DEFAULT_MOVING_AVERAGE_VELOCITY;
	reinitialize_moving_average_distance();
	reinitialize_moving_average_velocity();

	BC->measurement_state = MS_START_ACQUISITION;
	BC->next_measurement_state_counter = 100;
}

void destructor(void) {
	simple_destructor();
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		if(BC->next_measurement_state_counter > 0) {
			BC->next_measurement_state_counter--;
		} else {
			switch(BC->measurement_state) {
				case MS_DISABLED: {
					new_velocity_value(0);
					new_distance_value(0);
					if(BC->update_mode) {
						BC->measurement_state = MS_UPDATE_MODE;
					} else if(BC->laser_enabled) {
						BC->measurement_state = MS_START_ACQUISITION;
					}

					break;
				}

				case MS_UPDATE_MODE: {
					if(BC->new_mode == 0) {
						uint8_t data = 0;
						if(lidar_write_register(REG_AQUISATION_MODE_CONTROL, 1, &data)) {
							BC->update_mode = false;
							if(BC->laser_enabled) {
								BC->measurement_state = MS_START_ACQUISITION;
							} else {
								BC->measurement_state = MS_DISABLED;
							};
						}
					} else {
						uint8_t velocity_resoluition_value = 0xC8;
						switch(BC->new_mode) {
							case 1: velocity_resoluition_value = 0xC8; break;
							case 2: velocity_resoluition_value = 0x50; break;
							case 3: velocity_resoluition_value = 0x28; break;
							case 4: velocity_resoluition_value = 0x14; break;
						}

						if(lidar_write_register(REG_VELOCITY_RESOLUTION, 1, &velocity_resoluition_value)) {
							BC->update_mode = false;
							if(BC->laser_enabled) {
								BC->measurement_state = MS_START_ACQUISITION;
							} else {
								BC->measurement_state = MS_DISABLED;
							}
						}
					}

					break;
				}

				case MS_START_ACQUISITION: {
					if(BC->update_mode) {
						BC->measurement_state = MS_UPDATE_MODE;
					} else if(!BC->laser_enabled) {
						BC->measurement_state = MS_DISABLED;
					} else {
						const uint8_t data = 0x04; // Take acquisition & correlation processing with DC correction
						if(lidar_write_register(REG_CONTROL, 1, &data)) {
							if(BC->new_mode == 0) {
								BC->measurement_state = MS_READ_DISTANCE;
							} else {
								BC->measurement_state = MS_TAKE_VELOCITY_MEASUREMENT;
							}
						}
					}

					break;
				}

				case MS_READ_DISTANCE: {
					uint8_t data[2];
					if(lidar_read_register(REG_DISTANCE, 2, data)) {
						const uint16_t distance = (data[0] << 8) | data[1];
						if((distance & (1 << 15)) != (1 << 15)) { // If the MSB is 1 then the reading is not considered valid
							new_distance_value(distance);
							new_velocity_value(0);
						}
						BC->measurement_state = MS_START_ACQUISITION;
					}

					break;
				}

				case MS_TAKE_VELOCITY_MEASUREMENT: {
					const uint8_t data = 0x80; // Take velocity measurement
					if(lidar_write_register(REG_AQUISATION_MODE_CONTROL, 1, &data)) {
						BC->measurement_state = MS_READ_VELOCITY;
					}

					break;
				}

				case MS_READ_VELOCITY: {
					int8_t velocity;
					if(lidar_read_register(REG_VELOCITY_MEASUREMENT, 1, (uint8_t *)&velocity)) {
						new_velocity_value(velocity);
						new_distance_value(0);
						BC->measurement_state = MS_START_ACQUISITION;
					}

					break;
				}

				default: {
					BC->measurement_state = MS_START_ACQUISITION;

					break;
				}
			}
		}
	}

	simple_tick(tick_type);
}

bool lidar_read_register(const uint8_t reg, const uint8_t length, uint8_t *data) {
	i2c_start();
	if(!i2c_send_byte((I2C_ADDRESS << 1) | I2C_WRITE)) {
		i2c_stop();
		return false;
	}
	if(!i2c_send_byte(length > 1 ? (reg | CONTINOUS_RW) : reg)) {
		i2c_stop();
		return false;
	}
	i2c_stop();
	i2c_start();
	if(!i2c_send_byte((I2C_ADDRESS << 1) | I2C_READ)) {
		i2c_stop();
		return false;
	}

	for(uint8_t i = 0; i < length; i++) {
		data[i] = i2c_recv_byte(i != (length - 1));
	}
	i2c_stop();

	return true;
}

bool lidar_write_register(const uint8_t reg, const uint8_t length, const uint8_t *data) {
	i2c_start();
	if(!i2c_send_byte((I2C_ADDRESS << 1) | I2C_WRITE)) {
		return false;
	}
	if(!i2c_send_byte(length > 1 ? (reg | CONTINOUS_RW) : reg)) {
		return false;
	}
	for(uint8_t i = 0; i < length; i++) {
		if(!i2c_send_byte(data[i])) {
			return false;
		}
	}
	i2c_stop();

	return true;
}

void new_velocity_value(const int8_t velocity) {
	int16_t scaled_velocity = velocity*10; // default = *10
	switch(BC->new_mode) {
		case 1: scaled_velocity = velocity*10; break;
		case 2: scaled_velocity = velocity*25; break;
		case 3: scaled_velocity = velocity*50; break;
		case 4: scaled_velocity = velocity*100; break;
	}

	BC->moving_average_sum[SIMPLE_UNIT_VELOCITY] = BC->moving_average_sum[SIMPLE_UNIT_VELOCITY] -
	                                               BC->moving_average_velocity[BC->moving_average_tick[SIMPLE_UNIT_VELOCITY]] +
	                                               scaled_velocity;

	BC->moving_average_velocity[BC->moving_average_tick[SIMPLE_UNIT_VELOCITY]] = scaled_velocity;
	BC->moving_average_tick[SIMPLE_UNIT_VELOCITY] = (BC->moving_average_tick[SIMPLE_UNIT_VELOCITY] + 1) % BC->moving_average_upto[SIMPLE_UNIT_VELOCITY];

	BC->last_value[SIMPLE_UNIT_VELOCITY] = BC->value[SIMPLE_UNIT_VELOCITY];
	int16_t new_value = (BC->moving_average_sum[SIMPLE_UNIT_VELOCITY] + BC->moving_average_upto[SIMPLE_UNIT_VELOCITY]/2)/BC->moving_average_upto[SIMPLE_UNIT_VELOCITY];
	if(ABS(new_value) < 9) {
		new_value = 0;
	}

	BC->value[SIMPLE_UNIT_VELOCITY] = new_value;
}

void new_distance_value(const uint16_t distance) {
	BC->moving_average_sum[SIMPLE_UNIT_DISTANCE] = BC->moving_average_sum[SIMPLE_UNIT_DISTANCE] -
	                                               BC->moving_average_distance[BC->moving_average_tick[SIMPLE_UNIT_DISTANCE]] +
	                                               distance;

	BC->moving_average_distance[BC->moving_average_tick[SIMPLE_UNIT_DISTANCE]] = distance;
	BC->moving_average_tick[SIMPLE_UNIT_DISTANCE] = (BC->moving_average_tick[SIMPLE_UNIT_DISTANCE] + 1) % BC->moving_average_upto[SIMPLE_UNIT_DISTANCE];

	BC->last_value[SIMPLE_UNIT_DISTANCE] = BC->value[SIMPLE_UNIT_DISTANCE];
	BC->value[SIMPLE_UNIT_DISTANCE] =  (BC->moving_average_sum[SIMPLE_UNIT_DISTANCE] + BC->moving_average_upto[SIMPLE_UNIT_DISTANCE]/2)/BC->moving_average_upto[SIMPLE_UNIT_DISTANCE];
}

void reinitialize_moving_average_distance(void) {
	int32_t initial_value = BC->value[SIMPLE_UNIT_DISTANCE];
	for(uint8_t i = 0; i < BC->moving_average_upto[SIMPLE_UNIT_DISTANCE]; i++) {
		BC->moving_average_distance[i] = initial_value;
	}
	BC->moving_average_tick[SIMPLE_UNIT_DISTANCE] = 0;
	BC->moving_average_sum[SIMPLE_UNIT_DISTANCE] = initial_value*BC->moving_average_upto[SIMPLE_UNIT_DISTANCE];
}

void reinitialize_moving_average_velocity(void) {
	int32_t initial_value = BC->value[SIMPLE_UNIT_VELOCITY];
	for(uint8_t i = 0; i < BC->moving_average_upto[SIMPLE_UNIT_VELOCITY]; i++) {
		BC->moving_average_velocity[i] = initial_value;
	}
	BC->moving_average_tick[SIMPLE_UNIT_VELOCITY] = 0;
	BC->moving_average_sum[SIMPLE_UNIT_VELOCITY] = initial_value*BC->moving_average_upto[SIMPLE_UNIT_VELOCITY];
}

void set_moving_average(const ComType com, const SetMovingAverage *data) {
	if(BC->moving_average_upto[SIMPLE_UNIT_DISTANCE] != data->distance_average_length) {
		if(data->distance_average_length < 1) {
			BC->moving_average_upto[SIMPLE_UNIT_DISTANCE] = 1;
		} else if(data->distance_average_length > MAX_MOVING_AVERAGE) {
			BC->moving_average_upto[SIMPLE_UNIT_DISTANCE] = MAX_MOVING_AVERAGE;
		} else {
			BC->moving_average_upto[SIMPLE_UNIT_DISTANCE] = data->distance_average_length;
		}

		reinitialize_moving_average_distance();
	}

	if(BC->moving_average_upto[SIMPLE_UNIT_VELOCITY] != data->velocity_average_length) {
		if(data->velocity_average_length < 1) {
			BC->moving_average_upto[SIMPLE_UNIT_VELOCITY] = 1;
		} else if(data->velocity_average_length > MAX_MOVING_AVERAGE) {
			BC->moving_average_upto[SIMPLE_UNIT_VELOCITY] = MAX_MOVING_AVERAGE;
		} else {
			BC->moving_average_upto[SIMPLE_UNIT_VELOCITY] = data->velocity_average_length;
		}

		reinitialize_moving_average_velocity();
	}

	BA->com_return_setter(com, data);
}

void get_moving_average(const ComType com, const GetMovingAverage *data) {
	GetMovingAverageReturn gmar;
	gmar.header                  = data->header;
	gmar.header.length           = sizeof(GetMovingAverageReturn);
	gmar.distance_average_length = BC->moving_average_upto[SIMPLE_UNIT_DISTANCE];
	gmar.velocity_average_length = BC->moving_average_upto[SIMPLE_UNIT_VELOCITY];

	BA->send_blocking_with_timeout(&gmar, sizeof(GetMovingAverageReturn), com);
}

void set_mode(const ComType com, const SetMode *data) {
	if(data->mode > 4) {
		BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	BC->new_mode = data->mode;
	BC->update_mode = true;

	BA->com_return_setter(com, data);
}

void get_mode(const ComType com, const GetMode *data) {
	GetModeReturn gvcr;
	gvcr.header        = data->header;
	gvcr.header.length = sizeof(GetModeReturn);
	gvcr.mode          = BC->new_mode;

	BA->send_blocking_with_timeout(&gvcr, sizeof(GetModeReturn), com);
}

void is_laser_enabled(const ComType com, const IsLaserEnabled *data) {
	IsLaserEnabledReturn iler;
	iler.header        = data->header;
	iler.header.length = sizeof(IsLaserEnabledReturn);
	iler.laser_enabled = BC->laser_enabled;

	BA->send_blocking_with_timeout(&iler, sizeof(IsLaserEnabledReturn), com);
}

bool i2c_scl_value(void) {
	return PIN_SCL.pio->PIO_PDSR & PIN_SCL.mask;
}

void i2c_scl_high(void) {
	PIN_SCL.pio->PIO_ODR = PIN_SCL.mask;
	while(!i2c_scl_value()); // allow slave to clock-stretch
}

void i2c_scl_low(void) {
	PIN_SCL.pio->PIO_OER = PIN_SCL.mask;
}

bool i2c_sda_value(void) {
	return PIN_SDA.pio->PIO_PDSR & PIN_SDA.mask;
}

void i2c_sda_high(void) {
	PIN_SDA.pio->PIO_ODR = PIN_SDA.mask;
}

void i2c_sda_low(void) {
	PIN_SDA.pio->PIO_OER = PIN_SDA.mask;
}

void i2c_sleep_halfclock(void) {
	SLEEP_NS(I2C_HALF_CLOCK_100KHZ);
}

void i2c_stop(void) {
	i2c_scl_low();
	i2c_sda_low();
	i2c_sleep_halfclock();
	i2c_scl_high();
	i2c_sleep_halfclock();
	i2c_sda_high();
	i2c_sleep_halfclock();
}

void i2c_start(void) {
	i2c_scl_high();
	i2c_sleep_halfclock();
	i2c_sda_low();
	i2c_sleep_halfclock();
}

uint8_t i2c_recv_byte(bool ack) {
	uint8_t value = 0;

	for(int8_t i = 7; i >= 0; i--) {
		i2c_scl_low();
		i2c_sda_high(); // allow slave to read
		i2c_sleep_halfclock();
		i2c_scl_high();
		if(i2c_sda_value()) {
			value |= (1 << i);
		}
		i2c_sleep_halfclock();
	}

	// ACK
	i2c_scl_low();
	if(ack) {
		i2c_sda_low();
	} else {
		i2c_sda_high();
	}
	i2c_sleep_halfclock();
	i2c_scl_high();
	i2c_sleep_halfclock();

	return value;
}

bool i2c_send_byte(const uint8_t value) {
	for(int8_t i = 7; i >= 0; i--) {
		i2c_scl_low();
		if((value >> i) & 1) {
			i2c_sda_high();
		} else {
			i2c_sda_low();
		}
		i2c_sleep_halfclock();
		i2c_scl_high();
		i2c_sleep_halfclock();
	}

	i2c_sda_high(); // Make sure SDA is always released

	// Wait for ACK
	bool ret = false;

	i2c_scl_low();
	i2c_sleep_halfclock();
	i2c_scl_high();
	if(!i2c_sda_value()) {
		ret = true;
	}

	i2c_sleep_halfclock();

	return ret;
}
