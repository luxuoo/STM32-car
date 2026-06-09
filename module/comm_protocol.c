
#include "comm_protocol.h"

#include <stdio.h>
#include <string.h>

/*
 * 协议数据格式：
 * 帧头(0xAA) + 数据域长度 + 数据域 + 长度及数据域数据和校验 + 帧尾(0x55)
*/

#define HEAD 0xAA
#define TAIL 0x55

/*
 * 计算校验和
 * 返回值: 所计算的和校验
 */
static uint8_t check_sum(uint8_t* data, uint32_t len)
{
	uint8_t check_sum = 0x00;
	
	for(uint32_t i = 0; i < len; i++){
		check_sum += data[i];
	}
	
	return check_sum;
}

/*
 * 检查数据格式是否有效
 * 返回值: -1 -- 参数错误 -2 -- 长度不够 -3 -- 包格式错误
 */
int32_t packet_check_valid(uint8_t* data, uint32_t len, uint32_t* redundant)
{
	if((data == NULL) || (len == 0) || (redundant == NULL)){
		return -1;
	}
	
	uint32_t index = 0;
	for(index = 0; index < len; index++){
		if(data[index] != HEAD){
			index++;
		}
		else{
			break;
		}
	}
	*redundant = index;
	
	if((len-index) < 3){
		return -2;
	}
	
	uint16_t payload_len = 0x00;
	memcpy(&payload_len, &data[index+1], 2);
	if((len - index) < (payload_len + 5)){
		return -2;
	}
	
	if((data[index+3+payload_len+1] != TAIL) || 
	   (check_sum(&data[index+1], 2+payload_len) != data[index+3+payload_len]))
	{
		return -3;
	}
	
	return 0;
}

/*
 * 取得有效数据包长度
 * 返回值: 有效数据包长度，错误时返回零
 */
uint32_t packet_length(uint8_t* data, uint32_t len)
{
	if((data == NULL) || (data[0] != HEAD) || (len < 5)){
		return 0;
	}
	
	uint16_t payload_len = 0;
	memcpy(&payload_len, &data[1], 2);
	
	return (3+payload_len+2);
}


/*
 * 编码数据负载部分，添加帧头/帧尾/校验等部分
 * 返回值:编码后的数据长度
 */
int32_t packet_encode(uint8_t* payload, uint32_t len, uint8_t* packet_buff, uint32_t buff_len)
{
	if((payload == NULL) || (packet_buff == NULL) || ((len + 5) > buff_len)){
		return -1;
	}
	
	uint16_t payload_len = len;
	uint32_t index = 0;
	
	packet_buff[index++] = HEAD;
	memcpy(&packet_buff[index], &payload_len, 2);
	index += 2;
	
	memcpy(&packet_buff[index], payload, len);
	index += len;
	
	uint8_t check = check_sum(&packet_buff[1], 2+len);
	packet_buff[index++] = check;
	packet_buff[index++] = TAIL;
	
	return index;
}

/*
 * 解码出数据负载部分
 * 返回值： 负载内容长度，错误时返回-1
 */
int32_t packet_decode(uint8_t* data, uint32_t len, uint8_t* payload_buff, uint32_t buff_len)
{
	if((data == NULL) || (data[0] != HEAD) || (len < 5) || (payload_buff == NULL)){
		return -1;
	}
	uint16_t payload_len = 0x00;
	memcpy(&payload_len, &data[1], 2);
	if((len < (payload_len+5)) || (buff_len < payload_len)){
		return -1;
	}
	
	memcpy(payload_buff, &data[3], payload_len);
	return payload_len;
}
