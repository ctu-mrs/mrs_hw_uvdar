/*
 * baca_protocol.h
 *
 *  Created on: Mar 22, 2019
 *      Author: zaitldav
 */

#ifndef BACA_PROTOCOL_H_
#define BACA_PROTOCOL_H_

//#include "main.h"
#include <stdint.h>

struct baca_payload {
	uint8_t data_correct;
	uint8_t payload_size;
	uint8_t payload[255];
};

uint8_t baca_protocol_encode(uint8_t* buffer, uint8_t* payload, uint8_t payload_size, uint8_t data_id);
uint8_t baca_protocol_encode_uint8(uint8_t* buffer, uint8_t payload, uint8_t data_id);
uint8_t baca_protocol_encode_uint16(uint8_t* buffer, uint16_t payload, uint8_t data_id);
struct baca_payload baca_protocol_decode(uint8_t* buffer, uint8_t buffer_size);

#endif /* BACA_PROTOCOL_H_ */
