#include <stdio.h>
#include <stdlib.h>
#include "queue_manager.h"

#define TRACE_PTRS

void print_queue_info( const t_queue * q)
{
	printf(" - queue info - \n"

#if defined TRACE_PTRS
		   "base: 0x%p\n"
		   "head: 0x%p\n"
		   "tail: 0x%p\n"
#else
		   "head: %zu\n"
		   "tail: %zu\n"
#endif
		   "    size: %zu\n"
		   "max size: %zu\n",

#if defined TRACE_PTRS
		   q->base, q->head, q->tail,
#else
		   *q->head, *q->tail,
#endif
		   q->size, q->max_size
		  );
}

void print_queue_items__all( const t_queue * q)
{
	printf(" - queue data - \n");
	uint32_t i;

	for (i = 0; i < q->max_size; i++)
		printf("%02zu ", q->base[i]);

	printf("\n");
}

void print_queue_items__chosen( const t_queue * q)
{

}

e_qres qinit( t_queue * q, const size_t * p_base, const size_t length_in_items)
{
	if (q == NULL)
		return q_invalid_object;

	if (p_base == NULL)
		return q_invalid_pointer;

	if (length_in_items == 0)
		return q_invalid_length;

	q->base = (size_t *)p_base;
	q->max_size = (size_t)length_in_items;

	q->head = q->tail = (size_t *)p_base;
	q->size = 0;

	return q_ok;
}

bool qempty( const t_queue * q)
{
	return ((q->head == q->tail) && (q->size == 0));
}

size_t qsize( t_queue * q)
{
	if ((q->head == q->tail) && (q->size != 0))
		return q->max_size;

	return (q->head > q->tail) ? (q->max_size - (size_t)(q->head - q->tail)) : (size_t)(q->tail - q->head);
}

e_qres qpush( t_queue * q, const uint32_t val)
{
	size_t current_size = qsize(q);

	if (current_size == q->max_size)
		return q_full;

	*q->tail = val;

	if (q->tail >= q->base + (q->max_size - 1))
		q->tail = q->base;
	else
		q->tail++;

	q->size = ++current_size;

	return q_ok;
}

uint32_t qpop( t_queue * q)
{
	if (qempty(q))
		return (size_t)NULL;

	{
		size_t current_size = qsize(q);
		uint32_t ret = (uint32_t)(*q->head);

		*q->head = 0; ////////////////////////////////////////////////////// TODO!!! удалить эту строку в финальном варианте

		if (q->head >= q->base + (q->max_size - 1))
			(q->head = q->base);
		else
			q->head++;

		q->size = --current_size;

		return ret;
	}
}

e_qres qmerge(t_queue * qnew, t_queue * q1, t_queue * q2, const size_t * p_base, const size_t length_in_items)
{
	if ((q1 == NULL) || (q2 == NULL))
		return q_invalid_object;

	{
		e_qres qret = qinit(qnew, p_base, length_in_items);

		if (qret != q_ok)
			return qret;
		else
		{
			if ((q1->size == 0) && (q2->size == 0))
				return q_ok;
			else
			{				
				t_queue qmax = *q1;
				t_queue qmin = *q2;

				if (q2->size > q1->size)
				{
					qmax = *q2;
					qmin = *q1;
				}

				qnew->head = qnew->tail = qnew->base + (qmax.head - qmax.base);
				const size_t new_size = qmax.size;
				size_t i;

				for (i = 0; i < new_size; i++)
				{
					qpush(qnew, (const uint32_t)qpop(&qmax));

					if (qmin.size != 0)
						qpush(qnew, (const uint32_t)qpop(&qmin));
				}				

				q1->head = q1->tail = (size_t *)q1->base;
				q2->head = q2->tail = (size_t *)q1->base;
				q1->size = q2->size = 0;
			}
		}
	}
	return q_ok;
}
