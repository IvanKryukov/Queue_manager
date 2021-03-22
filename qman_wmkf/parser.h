#if !(defined PARSER_H)
#define PARSER_H

#include "stdint.h"
#include "queue_manager.h"

#define MAX_ARG_LENGTH	(uint32_t)(11)

int	get_command(  const char * cmd_string);
int get_argument( const char * arg_string);

size_t set_queue_to_file( t_queue q, FILE * f);
size_t get_queue_from_file( t_queue * q, FILE * f);

#endif
