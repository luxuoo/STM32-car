#ifndef __COMM_PROTOCOL_H__
#define __COMM_PROTOCOL_H__

#include <stdint.h>

int32_t  packet_check_valid(uint8_t* data, uint32_t len, uint32_t* redundant);
uint32_t packet_length(uint8_t* data, uint32_t len);
int32_t  packet_encode(uint8_t* payload, uint32_t len, uint8_t* packet_buff, uint32_t buff_len);
int32_t  packet_decode(uint8_t* data, uint32_t len, uint8_t* payload_buff, uint32_t buff_len);


#endif

