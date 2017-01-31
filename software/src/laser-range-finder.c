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
#define I2C_HALF_CLOCK_400KHZ  1250  // 2.5us per clock
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
	BrickContext *bc = BC;
	BrickletAPI  *ba = BA;

	uint32_t ret_data[4]; // Biggest packet has 10 bytes
	*((MessageHeader*)ret_data) = *((MessageHeader*)data);
	((MessageHeader*)ret_data)->length = 0;

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
			return;
		}

		case FID_SET_MOVING_AVERAGE: {
			update_moving_average(SIMPLE_UNIT_DISTANCE, ((SetMovingAverage*)data)->distance_average_length);
			update_moving_average(SIMPLE_UNIT_VELOCITY, ((SetMovingAverage*)data)->velocity_average_length);

			break;
		}

		case FID_GET_MOVING_AVERAGE: {
			((GetMovingAverageReturn*)ret_data)->header.length = sizeof(GetMovingAverageReturn);
			((GetMovingAverageReturn*)ret_data)->distance_average_length = bc->moving_average_upto[SIMPLE_UNIT_DISTANCE];
			((GetMovingAverageReturn*)ret_data)->velocity_average_length = bc->moving_average_upto[SIMPLE_UNIT_VELOCITY];

			break;
		}

		case FID_SET_MODE: {
			if((bc->lidar_version == LIDAR_VERSION_1 && ((SetMode*)data)->mode > 4) ||
			   (bc->lidar_version == LIDAR_VERSION_3 && (((SetMode*)data)->mode < 5 || ((SetMode*)data)->mode > 10))) {
				ba->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
				return;
			}
			bc->new_mode = ((SetMode*)data)->mode;
			bc->update_mode = true;

			break;
		}

		case FID_GET_MODE: {
			((GetModeReturn*)ret_data)->header.length = sizeof(GetModeReturn);
			((GetModeReturn*)ret_data)->mode = bc->new_mode;
			break;
		}

		case FID_IS_LASER_ENABLED: {
			((IsLaserEnabledReturn*)ret_data)->header.length = sizeof(IsLaserEnabledReturn);
			((IsLaserEnabledReturn*)ret_data)->laser_enabled = bc->laser_enabled;
			break;
		}

		case FID_ENABLE_LASER: {
			bc->laser_enabled = true;
			break;
		}

		case FID_DISABLE_LASER: {
			bc->laser_enabled = false;
			break;
		}

		case FID_GET_SENSOR_HARDWARE_VERISON: {
			((GetSensorHardwareVersionReturn*)ret_data)->header.length = sizeof(GetSensorHardwareVersionReturn);
			((GetSensorHardwareVersionReturn*)ret_data)->version = bc->lidar_version;
			break;
		}

		default: {
			ba->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com);
			return;
		}
	}

	if(((MessageHeader*)ret_data)->length == 0) {
		ba->com_return_setter(com, data);
	} else {
		ba->send_blocking_with_timeout(ret_data, ((MessageHeader*)ret_data)->length, com);
	}
}

void constructor(void) {
	_Static_assert(sizeof(BrickContext) <= BRICKLET_CONTEXT_MAX_SIZE, "BrickContext too big");
	BrickContext *bc = BC;
	BrickletAPI  *ba = BA;

	PIN_PWR.type = PIO_OUTPUT_0;
	PIN_PWR.attribute = PIO_DEFAULT;
	ba->PIO_Configure(&PIN_PWR, 1);

	PIN_SCL.type = PIO_INPUT;
	PIN_SCL.attribute = PIO_PULLUP;
	ba->PIO_Configure(&PIN_SCL, 1);

	PIN_SDA.type = PIO_INPUT;
	PIN_SDA.attribute = PIO_PULLUP;
	ba->PIO_Configure(&PIN_SCL, 1);

	PIN_PWM.type = PIO_INPUT;
	PIN_PWM.attribute = PIO_PULLUP;
	ba->PIO_Configure(&PIN_PWM, 1);

	SLEEP_MS(100);
	PIN_PWR.type = PIO_OUTPUT_1;
	ba->PIO_Configure(&PIN_PWR, 1);

	simple_constructor();

	bc->laser_enabled = false;
	bc->moving_average_upto[SIMPLE_UNIT_DISTANCE] = DEFAULT_MOVING_AVERAGE_DISTANCE;
	bc->moving_average_upto[SIMPLE_UNIT_VELOCITY] = DEFAULT_MOVING_AVERAGE_VELOCITY;
	reinitialize_moving_average(SIMPLE_UNIT_DISTANCE);
	reinitialize_moving_average(SIMPLE_UNIT_VELOCITY);

	bc->next_measurement_state_counter = 100;

	// Reset FPGA of LIDAR lite
	SLEEP_MS(200);
	uint8_t data = 0;
	lidar_write_register(REG_CONTROL, 1, &data);
	SLEEP_MS(200);

	// After reset we read the acquisition mode, since it has a different default
	// value in v1 and v3
	lidar_read_register(REG_ACQUISITION_MODE_CONTROL, 1, &data);
	if(data == 0) {
		bc->lidar_version = LIDAR_VERSION_1;
		bc->new_mode = 0;
		bc->measurement_state = MS_START_ACQUISITION;
	} else {
		bc->lidar_version = LIDAR_VERSION_3;
		bc->new_mode = 5;
		bc->update_mode = true;
		bc->measurement_state = MS_V3_READ_VELOCITY;

		uint8_t value = 0xFF; // continues measurement
		lidar_write_register(REG_DC_THRESHOLD, 1, &value);
		value = 0x04; // start measurement
		lidar_write_register(REG_CONTROL, 1, &value);
	}
}

void tick(const uint8_t tick_type) {
	BrickContext *bc = BC;

	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		if(bc->lidar_version == LIDAR_VERSION_1) {
			if(bc->next_measurement_state_counter > 0) {
				bc->next_measurement_state_counter--;
			} else {
				switch(bc->measurement_state) {
					case MS_DISABLED: {
						new_velocity_value(0);
						new_distance_value(0);
						if(bc->update_mode) {
							bc->measurement_state = MS_UPDATE_MODE;
						} else if(bc->laser_enabled) {
							bc->measurement_state = MS_START_ACQUISITION;
						}

						break;
					}

					case MS_UPDATE_MODE: {
						if(bc->new_mode == 0) {
							uint8_t data = 0;
							if(lidar_write_register(REG_ACQUISITION_MODE_CONTROL, 1, &data)) {
								bc->update_mode = false;
								if(bc->laser_enabled) {
									bc->measurement_state = MS_START_ACQUISITION;
								} else {
									bc->measurement_state = MS_DISABLED;
								};
							}
						} else {
							uint8_t velocity_resolution_value = 0xC8;
							switch(BC->new_mode) {
								case 1: velocity_resolution_value = 0xC8; break;
								case 2: velocity_resolution_value = 0x50; break;
								case 3: velocity_resolution_value = 0x28; break;
								case 4: velocity_resolution_value = 0x14; break;
							}

							if(lidar_write_register(REG_VELOCITY_RESOLUTION, 1, &velocity_resolution_value)) {
								bc->update_mode = false;
								if(bc->laser_enabled) {
									bc->measurement_state = MS_START_ACQUISITION;
								} else {
									bc->measurement_state = MS_DISABLED;
								}
							}
						}

						break;
					}

					case MS_START_ACQUISITION: {
						if(bc->update_mode) {
							bc->measurement_state = MS_UPDATE_MODE;
						} else if(!bc->laser_enabled) {
							bc->measurement_state = MS_DISABLED;
						} else {
							const uint8_t data = 0x04; // Take acquisition & correlation processing with DC correction
							if(lidar_write_register(REG_CONTROL, 1, &data)) {
								if(bc->new_mode == 0) {
									bc->measurement_state = MS_READ_DISTANCE;
								} else {
									bc->measurement_state = MS_TAKE_VELOCITY_MEASUREMENT;
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
						if(lidar_write_register(REG_ACQUISITION_MODE_CONTROL, 1, &data)) {
							bc->measurement_state = MS_READ_VELOCITY;
						}

						break;
					}

					case MS_READ_VELOCITY: {
						int8_t velocity;
						if(lidar_read_register(REG_VELOCITY_MEASUREMENT, 1, (uint8_t *)&velocity)) {
							new_velocity_value(velocity);
							new_distance_value(0);
							bc->measurement_state = MS_START_ACQUISITION;
						}

						break;
					}

					default: {
						bc->measurement_state = MS_START_ACQUISITION;

						break;
					}
				}
			}
		} else {
			if(bc->update_mode) {
				uint8_t acq_max_value = 0x80;
				uint8_t acq_mode_value = 0x08;
				uint8_t threshold_bypass_value = 0x00;

				switch(bc->new_mode) {
					case 5:                                 break;
					case 6:  acq_max_value          = 0x1D; break;
					case 7:  acq_mode_value         = 0x00; break;
					case 8:  acq_max_value          = 0xFF; break;
					case 9:  threshold_bypass_value = 0x80; break;
					case 10: threshold_bypass_value = 0xB0; break;
				}

				lidar_write_register(REG_MAX_ACQUISITION_COUNT,    1, &acq_max_value);
				lidar_write_register(REG_ACQUISITION_MODE_CONTROL, 1, &acq_mode_value);
				lidar_write_register(REG_THRESHOLD_BYPASS,         1, &threshold_bypass_value);
				bc->update_mode = false;
			} else {
				if(bc->laser_enabled) {
					switch(bc->measurement_state) {
						case MS_V3_READ_DISTANCE: {
							uint8_t data[2];
							if(lidar_read_register(REG_DISTANCE, 2, data)) {
								const uint16_t distance = (data[0] << 8) | data[1];
								if((distance & (1 << 15)) != (1 << 15)) { // If the MSB is 1 then the reading is not considered valid
									new_distance_value(distance);
								}
								bc->measurement_state = MS_V3_READ_VELOCITY;
							}

							break;
						}

						case MS_V3_READ_VELOCITY: {
							int8_t velocity;
							if(lidar_read_register(REG_VELOCITY_MEASUREMENT, 1, (uint8_t *)&velocity)) {
								new_velocity_value(velocity);
								bc->measurement_state = MS_V3_READ_DISTANCE;
							}
							break;
						}

						default: {
							bc->measurement_state = MS_V3_READ_DISTANCE;
							break;
						}
					}
				} else {
					new_velocity_value(0);
					new_distance_value(0);
				}
			}
		}
	}

	simple_tick(tick_type);
}

bool lidar_read_register(const uint8_t reg, const uint8_t length, uint8_t *data) {
	__disable_irq();
	i2c_start();
	if(!i2c_send_byte((I2C_ADDRESS << 1) | I2C_WRITE)) {
		i2c_stop();
		__enable_irq();
		return false;
	}
	if(!i2c_send_byte(length > 1 ? (reg | CONTINOUS_RW) : reg)) {
		i2c_stop();
		__enable_irq();
		return false;
	}
	i2c_stop();
	i2c_start();
	if(!i2c_send_byte((I2C_ADDRESS << 1) | I2C_READ)) {
		i2c_stop();
		__enable_irq();
		return false;
	}

	for(uint8_t i = 0; i < length; i++) {
		data[i] = i2c_recv_byte(i != (length - 1));
	}
	i2c_stop();
	__enable_irq();

	return true;
}

bool lidar_write_register(const uint8_t reg, const uint8_t length, const uint8_t *data) {
	__disable_irq();
	i2c_start();
	if(!i2c_send_byte((I2C_ADDRESS << 1) | I2C_WRITE)) {
		i2c_stop();
		__enable_irq();
		return false;
	}
	if(!i2c_send_byte(length > 1 ? (reg | CONTINOUS_RW) : reg)) {
		i2c_stop();
		__enable_irq();
		return false;
	}
	for(uint8_t i = 0; i < length; i++) {
		if(!i2c_send_byte(data[i])) {
			i2c_stop();
			__enable_irq();
			return false;
		}
	}
	i2c_stop();
	__enable_irq();

	return true;
}

void new_value(const int32_t value, const uint8_t type) {
	BrickContext *bc = BC;
	bc->moving_average_sum[type] = bc->moving_average_sum[type] -
	                               bc->moving_average_value[type][bc->moving_average_tick[type]] +
								   value;

	bc->moving_average_value[type][BC->moving_average_tick[type]] = value;
	bc->moving_average_tick[type] = (bc->moving_average_tick[type] + 1) % bc->moving_average_upto[type];

	bc->last_value[type] = bc->value[type];
}

void new_velocity_value(const int8_t velocity) {
	BrickContext *bc = BC;

	int16_t scaled_velocity = velocity*10; // default = *10
	switch(bc->new_mode) {
		case 1: scaled_velocity = velocity*10; break;
		case 2: scaled_velocity = velocity*25; break;
		case 3: scaled_velocity = velocity*50; break;
		case 4: scaled_velocity = velocity*100; break;
	}

	new_value(scaled_velocity, SIMPLE_UNIT_VELOCITY);
	int16_t new_value = (bc->moving_average_sum[SIMPLE_UNIT_VELOCITY] + bc->moving_average_upto[SIMPLE_UNIT_VELOCITY]/2)/bc->moving_average_upto[SIMPLE_UNIT_VELOCITY];
	if(ABS(new_value) < 9) {
		new_value = 0;
	}

	bc->value[SIMPLE_UNIT_VELOCITY] = new_value;
}

void new_distance_value(const uint16_t distance) {
	BrickContext *bc = BC;

	new_value(distance, SIMPLE_UNIT_DISTANCE);
	bc->value[SIMPLE_UNIT_DISTANCE] =  (bc->moving_average_sum[SIMPLE_UNIT_DISTANCE] + bc->moving_average_upto[SIMPLE_UNIT_DISTANCE]/2)/bc->moving_average_upto[SIMPLE_UNIT_DISTANCE];
}

void reinitialize_moving_average(const uint8_t type) {
	BrickContext *bc = BC;

	int32_t initial_value = bc->value[type];
	for(uint8_t i = 0; i < bc->moving_average_upto[type]; i++) {
		bc->moving_average_value[type][i] = initial_value;
	}
	bc->moving_average_tick[type] = 0;
	bc->moving_average_sum[type] = initial_value*bc->moving_average_upto[type];
}

void update_moving_average(const uint8_t type, const uint8_t length) {
	BrickContext *bc = BC;

	if(bc->moving_average_upto[type] != length) {
		if(length < 1) {
			bc->moving_average_upto[type] = 1;
		} else if(length > MAX_MOVING_AVERAGE) {
			bc->moving_average_upto[type] = MAX_MOVING_AVERAGE;
		} else {
			bc->moving_average_upto[type] = length;
		}

		reinitialize_moving_average(type);
	}
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
	if(BC->lidar_version == 1) {
		SLEEP_NS(I2C_HALF_CLOCK_100KHZ);
	} else {
		SLEEP_NS(I2C_HALF_CLOCK_400KHZ);
	}
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
