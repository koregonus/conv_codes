#include "conv_15_17_encoder.h"


// Промежуточный рабочий энкодер (15,17) R=1/2


void clear_left_reg_15_17(uint8_t* reg)
{
	*reg=*reg<<1;
	*reg=*reg&(0xFU);
}




uint8_t encode_bit_15(uint8_t input, uint8_t* reg, uint8_t inv)
{
	*reg=*reg|input;

	uint8_t ret=0;

	if(inv==0)
		ret = ((*reg&1U)^((*reg&(1U<<1U))>>1U)^((*reg&(1U<<3U))>>3U));
	else
		ret = ((*reg&1U)^((*reg&(1U<<2U))>>2U)^((*reg&(1U<<3U))>>3U));
	return ret;
}


uint8_t encode_bit_17(uint8_t* reg)
{
	uint8_t ret = ((*reg&1U)^((*reg&(1U<<1U))>>1U)^((*reg&(1U<<2U))>>2U)^((*reg&(1U<<3U))>>3U));
	return ret;
}



void encode_message_15_17(uint8_t* msg, uint8_t* output_buffer, uint16_t len, uint8_t* reg)
{
	//очистка регистра состояния перед кодировкой сообщения
	*reg=0;

	uint8_t local_len=0;
	uint8_t shift_iterator=0;

	for(uint8_t i=0; i< len; i++)
	{
		shift_iterator=0;
		local_len=2*i;
		uint8_t j=0;

		for( ;j<4;j++)
		{
			if(msg[i]&(1<<(7-j)))
			{
				clear_left_reg_15_17(reg);
				output_buffer[local_len]|=(encode_bit_15(1,reg,0)<<(7-shift_iterator));
				output_buffer[local_len]|=(encode_bit_17(reg))<<(7-(shift_iterator+1));
			}
			else
			{
				clear_left_reg_15_17(reg);
				output_buffer[local_len]|=(encode_bit_15(0,reg,0)<<(7-shift_iterator));
				output_buffer[local_len]|=(encode_bit_17(reg))<<(7-(shift_iterator+1));
			}

			shift_iterator+=2;
		}

		local_len++;
		shift_iterator=0;
		
		for(;j<8;j++)
		{
			if(msg[i]&(1<<(7-j)))
			{
				clear_left_reg_15_17(reg);
				output_buffer[local_len]|=(encode_bit_15(1,reg,0)<<(7-shift_iterator));
				output_buffer[local_len]|=(encode_bit_17(reg))<<(7-(shift_iterator+1));
			}
			else
			{
				clear_left_reg_15_17(reg);
				output_buffer[local_len]|=(encode_bit_15(0,reg,0)<<(7-shift_iterator));
				output_buffer[local_len]|=(encode_bit_17(reg))<<(7-(shift_iterator+1));
			}

			shift_iterator+=2;
		}
	}
}