#include <stdio.h>

#include "conv_133_171_encoder.h"

#include "conv_decoder.h"


//Разложение байта на 2-битовые сочетания(определяется скоростью кода 1/2, где n=2)
void parse_byte(uint8_t input, uint8_t* output, uint8_t* pos )
{
	output[*pos]=input >> 6U;
	*pos=*pos+1;
	output[*pos]=(input & 0x3fU) >> 4U;
	*pos=*pos+1;
	output[*pos]=(input & 0x0FU) >> 2U;
	*pos=*pos+1;
	output[*pos]=(input & 0x03U);
	*pos=*pos+1;
}


uint8_t hamming_weigth(uint8_t input_1, uint8_t input_2)
{
	uint8_t ret=0;
	uint8_t temp=input_1^input_2;

	for(uint8_t i=0; i<8;i++)
	{
			if(temp&(1<<i))
				ret++;
	}
	return ret;
}


void update_branch_node(struct yarus* trellis, uint8_t cur_yarus, uint8_t cur_node ,uint8_t codeword)
{
	uint8_t buffer_register = cur_node;

	#ifdef DEBUG_133_171

	uint8_t local_code = (encode_bit_133(0,&buffer_register,1)<<1U);
	local_code=local_code|(encode_bit_171(&buffer_register,1));
	trellis[cur_yarus].yar_nodes[cur_node]->child_edges[0]->ham_weigth=hamming_weigth(local_code,codeword);

	buffer_register = cur_node;
	local_code = ((encode_bit_133(1<<6U,&buffer_register,1))<<1U);
	local_code = local_code|(encode_bit_171(&buffer_register,1));
	trellis[cur_yarus].yar_nodes[cur_node]->child_edges[1]->ham_weigth=hamming_weigth(local_code,codeword);

	#endif

}


void update_branch_yarus(struct yarus* trellis, uint8_t cur_yarus, uint8_t codeword)
{
	for(uint8_t i=0; i<STATES; i++)
	{
		update_branch_node(trellis, cur_yarus, i , codeword);
	}
}

void deinit_branch_node(struct yarus* trellis, uint8_t cur_yarus, uint8_t cur_node)
{
	trellis[cur_yarus].yar_nodes[cur_node]->child_edges[0]->ham_weigth=0;
	trellis[cur_yarus].yar_nodes[cur_node]->child_edges[1]->ham_weigth=0;
}

void deinit_branch_yarus(struct yarus* trellis, uint8_t cur_yarus)
{
	for(uint8_t i=0; i<STATES; i++)
	{
		deinit_branch_node(trellis, cur_yarus, i);
	}
}


void init_trellis(struct yarus* trellis, struct node (* local_node_array)[STATES], struct edge (* local_edge_array)[2*STATES])
{
	for(uint8_t i=0; i<TRELLIS_LENGTH; i++ )  //Кол-во ярусов == ДКО
	{

		for(uint8_t k=0; k<STATES; k++)  // Кол-во состояний регистра
		{
			trellis[i].yar_nodes[k]=&local_node_array[i][k];

			uint8_t buffer_state=k;
			
			if(i<TRELLIS_LENGTH-1)
			{
					//Нулевой путь
					trellis[i].yar_nodes[k]->child_edges[0]=&local_edge_array[i][2*k];
					buffer_state = (buffer_state>>1U)&STATE_LENGTH;
					trellis[i].yar_nodes[k]->child_edges[0]->next=&local_node_array[i+1][buffer_state];

					//Единичный путь
					trellis[i].yar_nodes[k]->child_edges[1]=&local_edge_array[i][2*k+1];
					buffer_state=k;
					buffer_state=((buffer_state>>1U)|(1U<<STATE_SWITCH))&STATE_LENGTH; // TODO:УЗКОЕ МЕСТО!!! нужен макрос для порядка второго сдвига
					trellis[i].yar_nodes[k]->child_edges[1]->next=&local_node_array[i+1][buffer_state];
					// printf("ONE_STATE:%d\n", buffer_state );
			}


			//Связь последнего яруса с первым для образования кольца
			if(i==TRELLIS_LENGTH-1)
			{


					//Нулевой путь
					trellis[i].yar_nodes[k]->child_edges[0]=&local_edge_array[i][2*k];
					buffer_state = (buffer_state>>1U)&STATE_LENGTH;
					trellis[i].yar_nodes[k]->child_edges[0]->next=&local_node_array[0][buffer_state];

					//Единичный путь
					trellis[i].yar_nodes[k]->child_edges[1]=&local_edge_array[i][2*k+1];
					buffer_state=k;
					buffer_state=((buffer_state>>1U)|(1U<<STATE_SWITCH))&STATE_LENGTH;// TODO:УЗКОЕ МЕСТО!!! нужен макрос для порядка второго сдвига
					trellis[i].yar_nodes[k]->child_edges[1]->next=&local_node_array[0][buffer_state];
			}
		}
	}
}


uint8_t find_path(struct node* current_start_node, struct path* local_path_array)
{
struct node* buffer_node;

	deinit_path(local_path_array);

	for(uint16_t i=0; i<TOTAL_PATHS; i++)
	{
		buffer_node = current_start_node;
		for(uint8_t j=0; j<PATH_LENGTH; j++)
		{
			if(i&(1<<j))
			{
				local_path_array[i].path_weight+=buffer_node->child_edges[1]->ham_weigth;
				buffer_node=buffer_node->child_edges[1]->next;
			}
			else
			{
				local_path_array[i].path_weight+=buffer_node->child_edges[0]->ham_weigth;
				buffer_node=buffer_node->child_edges[0]->next;
			}
		}
	}

	uint8_t ret=0;

	uint8_t max_temp=local_path_array[0].path_weight;

	for(uint16_t i=0; i<TOTAL_PATHS; i++)
	{
		if(local_path_array[i].path_weight<max_temp)
		{
			ret=i;
			max_temp=local_path_array[i].path_weight;
		}
	}
	return ret;
}


void deinit_path(struct path* local_path_array)
{
	for(uint16_t i=0; i<TOTAL_PATHS; i++)
	{
		local_path_array[i].path_weight=0;
	}
}


void decode_message(uint8_t* input, uint8_t input_length, uint8_t* output, struct yarus* trellis)
{
	//TODO:уточнить относ. "хвоста" декодирования

	uint8_t position=0;

	uint8_t decode_buffer[4*input_length];

	for(uint8_t i=0; i<input_length; i++)
	{
		parse_byte(input[i], decode_buffer, &position);
	}

	uint8_t index_of_min_path = 0;

	uint8_t current_update_start=0;

	uint8_t current_decode_yarus_start=0;

	uint8_t current_state=0;

	uint8_t last_decoded_bit=0;

	uint8_t decode_data_it=0;

	uint8_t shift_data=7;

	//Инициализация решётки
	for(uint8_t i=0; i<PATH_LENGTH-1; i++)
	{
		update_branch_yarus(trellis, current_update_start,decode_buffer[i]);
		current_update_start++;
	}

	for(uint8_t j=PATH_LENGTH-1; j<input_length*4; j++)
	{

		if(current_update_start>=TRELLIS_LENGTH)
			current_update_start=0;
		if(current_decode_yarus_start>=TRELLIS_LENGTH)
			current_decode_yarus_start=0;

		update_branch_yarus(trellis,current_update_start,decode_buffer[j]);
		current_update_start++;

		index_of_min_path	= find_path(trellis[current_decode_yarus_start].yar_nodes[current_state],all_paths);//TODO:перевести all_paths на аргумент функции 

		last_decoded_bit=index_of_min_path&1;

		current_state = (current_state>>1)|(last_decoded_bit<<STATE_SWITCH);

		output[decode_data_it/8]|=(last_decoded_bit<<shift_data);

		decode_data_it++;
		
		if(shift_data==0)
			shift_data=8;

		shift_data--;

		index_of_min_path=0;

		deinit_branch_yarus(trellis, current_decode_yarus_start);
		current_decode_yarus_start++;
	}

}

