
#include "conv_1_3_encoder.h"


//Отладочный простейший энкодер (1,3) R=1/2


void clear_left_reg(uint8_t* reg)
{
	*reg=*reg<<1U;
	*reg=*reg & (0x3U);
}



uint8_t encode_bit(uint8_t input, uint8_t* reg)
{
	
	clear_left_reg(reg);

	*reg=*reg|input;

	uint8_t ret = ((*reg&1U)^(*reg&(1U<<1U))>>1U);

	return ret;
}


void encode_message(uint8_t* msg, uint8_t* output_buffer, uint16_t len, uint8_t* reg)
{
	//очистка регистра состояния
	*reg=0;

	uint8_t local_len=0;
	uint8_t shift_iterator=0;

	for(uint8_t i=0; i< len; i++)
	{
		local_len=2*i;
		uint8_t j=0;
		for(;j<4;j++)
		{
			if(msg[i]&(1<<(7-j)))
			{
				output_buffer[local_len]|=1<<(7-(shift_iterator));
				output_buffer[local_len]|=(encode_bit(1,reg))<<(7-(shift_iterator+1));
			}
			else
				output_buffer[local_len]|=(encode_bit(0,reg))<<(7-(shift_iterator+1));
			
			shift_iterator+=2;
		}

		local_len++;
		shift_iterator=0;

		for(;j<8;j++)
		{
			if(msg[i]&(1<<(7-j)))
			{
				output_buffer[local_len]|=1<<(7-(shift_iterator));
				output_buffer[local_len]|=(encode_bit(1,reg))<<(7-(shift_iterator+1));
			}
			else
				output_buffer[local_len]|=(encode_bit(0,reg))<<(7-(shift_iterator+1));
			
			shift_iterator+=2;
		}
	}
}