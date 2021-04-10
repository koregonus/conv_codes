#include "scrambler.h"



void generate_scrambler_sequence(uint8_t* seq_array, uint8_t seq_length)
{
	uint8_t state_reg = 0xFFU;
	uint8_t temp;

	for(uint8_t j=0; j<seq_length; j++)
	{
		seq_array[j]=0;
		for(uint8_t i=0; i<8; i++)
		{
			temp=state_reg&1;
			seq_array[j]=seq_array[j]^(temp<<(7-i));
			state_reg=(state_reg>>1U);
			temp=temp^((state_reg&(1U<<2U))>>2U)^((state_reg&(1U<<4U))>>4U)^((state_reg&(1U<<6U))>>6);
			state_reg=state_reg^(temp<<7U);
		}
	}
}


void scramble_message(uint8_t* msg, uint8_t* scram_seq, uint8_t length)
{
	for(uint8_t i=0; i<length; i++)
		msg[i]=msg[i]^scram_seq[i];
}