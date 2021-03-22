#if !(defined QUEUE_MANAGER_H)
#define QUEUE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
	size_t * base;
	size_t * head;
	size_t * tail;
	size_t	 size;
	size_t	 max_size;
} t_queue;

typedef enum
{
	q_ok = 0,
	q_empty,
	q_full,
	q_invalid_object,
	q_invalid_pointer,
	q_invalid_length
} e_qres;


void print_queue_info( const t_queue * q);
void print_queue_items__all( const t_queue * q);
void print_queue_items__chosen( const t_queue * q, const uint32_t bit_one_number);

e_qres		qinit(	t_queue * q, const size_t * p_base, const size_t length_in_items);
bool		qempty(	const t_queue * q);
size_t		qsize(	t_queue * q);
uint32_t 	qpop(	t_queue * q);
e_qres		qpush(	t_queue * q, const uint32_t val);
e_qres		qmerge( t_queue * qres, t_queue * q1, t_queue * q2, const size_t * p_base, const size_t length_in_items);

#endif
