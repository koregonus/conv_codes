#ifndef CONV_DECODER_H
#define CONV_DECODER_H


#include <inttypes.h>



#define DEBUG_133_171

#ifdef DEBUG_1_3

#define STATES 2

#define TOTAL_PATHS 4

#define PATH_LENGTH 2

#define STATE_LENGTH 1

#define STATE_SWITCH 1U

#define TRELLIS_LENGTH 3

#endif



#ifdef DEBUG_5_7

#define STATES 4

#define TOTAL_PATHS 8

#define PATH_LENGTH 3

#define STATE_SWITCH 1U

#define STATE_LENGTH 0x3U

#define TRELLIS_LENGTH 5

#endif


#ifdef DEBUG_15_17

#define STATES 8

#define TOTAL_PATHS 16

#define PATH_LENGTH 4

#define STATE_SWITCH 2U

#define STATE_LENGTH 0x7U

#define TRELLIS_LENGTH 6

#define SWITCH_ON 1

#define SWITCH_OFF 0


#endif


#ifdef DEBUG_133_171

#define STATES 64

#define TOTAL_PATHS 512

#define PATH_LENGTH 9

#define STATE_SWITCH 5U

#define STATE_LENGTH 0x3F

#define TRELLIS_LENGTH 9

#endif




struct path
{
	uint8_t path_weight;
};


struct node
{
	struct edge* child_edges[2];
};


struct edge
{
	
	uint8_t ham_weigth;
	struct node* next;
};

struct yarus
{
	struct node* yar_nodes[STATES];
	struct edge* yar_edges[2*STATES];
};

struct edge full_edges[TRELLIS_LENGTH][2*STATES];

struct node full_nodes[TRELLIS_LENGTH][STATES];

struct path all_paths[TOTAL_PATHS];

uint8_t hamming_weigth(uint8_t input_1, uint8_t input_2);

void init_trellis(struct yarus* trellis, struct node (* local_node_array)[STATES], struct edge (* local_edge_array)[2*STATES]);

void update_branch_node(struct yarus* trellis, uint8_t cur_yarus, uint8_t cur_node ,uint8_t codeword);

void update_branch_yarus(struct yarus* trellis, uint8_t cur_yarus, uint8_t codeword);

uint8_t find_path(struct node* current_start_node, struct path* local_path_array);

void deinit_branch_node(struct yarus* trellis, uint8_t cur_yarus, uint8_t cur_node);

void deinit_branch_yarus(struct yarus* trellis, uint8_t cur_yarus);

void deinit_path(struct path* local_path_array);

void parse_byte(uint8_t input, uint8_t* output, uint8_t* pos );

void decode_message(uint8_t* input, uint8_t input_length, uint8_t* output, struct yarus* trellis);
#endif