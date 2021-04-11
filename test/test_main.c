#include <stdio.h>

#include <time.h>

#include <stdlib.h>

#include "conv_133_171_encoder.h"

#include "conv_decoder.h"



int main()
{


	uint8_t encode_register=0;

	uint8_t test_input[50];

	uint8_t test_output[25];

	uint8_t test_message[24];

	uint8_t test_errored_input[50];

	uint8_t detected_wrong_bytes=0;


	struct yarus viterbi_trellis[TRELLIS_LENGTH];
	init_trellis(viterbi_trellis, full_nodes, full_edges);


	printf("TEST#1\n" );

	srand(time(NULL));

	encode_register=0;

	deinit_path(all_paths);

	for(uint8_t i=0; i<TRELLIS_LENGTH;i++)
	{
		deinit_branch_yarus(viterbi_trellis, i);
	}

	for(uint8_t i=0; i<50;i++)
		test_input[i]=0;



	for(uint8_t i=0; i<24;i++)
	{
		test_message[i]=(uint8_t)(rand()%255);
	}

	encode_message_133_171(test_message,test_input, 24, &encode_register);
	

	for(uint8_t i=0; i< 50; i++)
	{
		test_errored_input[i]=0;
		test_errored_input[i]=(test_input[i]);
		uint8_t random_shift=(uint8_t)rand()%8;
		if((i&1)==0)
			test_errored_input[i]= test_errored_input[i]^(1U<<random_shift);
	}

	 // test_errored_input[4]=test_errored_input[4]^24U;

	for(uint8_t i=0; i<25 ; i++)
		test_output[i]=0;

	decode_message(test_errored_input, 50, test_output, viterbi_trellis);

	detected_wrong_bytes=0;

	for(uint8_t i=0; i<24; i++)
	{
		printf("%d_%d\n", test_message[i], test_output[i] );
		if(test_message[i]!=test_output[i])
			detected_wrong_bytes++;
	}
	printf("DETECTED WRONG BYTES: %d\n", detected_wrong_bytes );

	return 0;
}