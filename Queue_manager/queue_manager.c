#include <stdio.h>
#include <stdlib.h>
#include "queue_manager.h"


void print_queue_info( const t_queue * q)
{
	printf(" - queue info - \n"
		   "base: %p\n"
		   "head: %u\n"
		   "tail: %u\n"
		   "size: %u\n"
		   "max size: %u\n",
		   q->base, q->head, q->tail, q->size, q->max_size
		  );
}

void print_queue_items__all( const t_queue * q)
{
	printf(" - queue data - \n");
	uint32_t i;

	for (i = 0; i < q->max_size; i++)
		printf("%02u ", q->base[i]);

	printf("\n");
}

void print_queue_items__chosen( const t_queue * q)
{

}

e_qres qinit( t_queue * q, const uint32_t * p_base, const int32_t length_in_items)
{
	if (q == NULL)
		return q_invalid_object;

	if (p_base == NULL)
		return q_invalid_pointer;

	if (length_in_items == 0)
		return q_invalid_length;

	q->base = (uint32_t*)p_base;
	q->max_size = (int32_t)length_in_items;

	q->head = q->tail = 0;
	q->size = 0;

	return q_ok;
}

bool qempty( const t_queue * q)
{
	return ((q->head == q->tail) && (q->size == 0));
}

uint32_t qsize( t_queue * q)
{
	if ((q->head == q->tail) && (q->size != 0))
		return q->max_size;

	{
		uint32_t ret_size;

		ret_size = (q->head > q->tail) ? (q->max_size - q->head + q->tail) : (q->tail - q->head);
		return ret_size;
	}
}

e_qres qpush( t_queue * q, const uint32_t val)
{
	uint32_t current_size = qsize(q);

	if (current_size == q->max_size)
		return q_full;

	q->base[q->tail] = val;
	q->tail = (q->tail + 1) % q->max_size;

	q->size = ++current_size;

	return q_ok;
}

uint32_t qpop( t_queue * q)
{
	if (qempty(q))
		return (uint32_t)NULL;

	{
		uint32_t current_size = qsize(q);
		uint32_t ret = q->base[q->head];

		q->base[q->head] = 0; ////////////////////////////////////////////////////// TODO!!! удалить эту строку в финальном варианте
		q->head = (q->head + 1) % q->max_size;

		q->size = --current_size;

		return ret;
	}
}

e_qres qmerge(t_queue * qnew, t_queue * q1, t_queue * q2, const uint32_t * p_base, const int32_t length_in_items)
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
				{
					t_queue qmax = *q1;
					t_queue qmin = *q2;

					if (q2->size > q1->size)
					{
						qmax = *q2;
						qmin = *q1;
					}

					qnew->head = qnew->tail = qmax.head;
					const int new_size = qmax.size;
					int i;

					for (i = 0; i < new_size; i++)
					{
						qpush(qnew, (const uint32_t)qpop(&qmax));

						if (qmin.size != 0)
							qpush(qnew, (const uint32_t)qpop(&qmin));
					}
				}

				q1->head = q1->tail = q1->size = 0;
				q2->head = q2->tail = q2->size = 0;
			}
		}
	}
	return q_ok;
}
