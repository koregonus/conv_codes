#ifndef _CONV_CODES_133_171H
#define _CONV_CODES_133_171H

#include <inttypes.h>



void clear_left_reg_133_171(uint8_t* reg);

uint8_t encode_bit_133(uint8_t input, uint8_t* reg, uint8_t inv);

uint8_t encode_bit_171(uint8_t* reg, uint8_t inv);

void encode_message_133_171(uint8_t* msg, uint8_t* output_buffer, uint16_t len, uint8_t* reg);




#endif