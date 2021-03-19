#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "queue_manager.h"

#define QUEUE1_LENGTH (uint32_t)(8)
#define QUEUE2_LENGTH (uint32_t)(5)
#define QUEUE3_LENGTH (uint32_t)(QUEUE1_LENGTH + QUEUE2_LENGTH)

#define QUEUE_MAXCOUNT (2)

static uint32_t mem_space[3][QUEUE3_LENGTH];
static const uint32_t qu_size[2] = { QUEUE1_LENGTH, QUEUE2_LENGTH };


static void print_qret( const e_qres ret)
{
	printf("res: ");

	if (q_ok == ret)
		printf("OK\n");

	else if ( ret == q_empty)
		printf("empty\n");

	else if ( ret == q_full)
		printf("full\n");

	else if (ret == q_invalid_object)
		printf("invalid object\n");

	else if (ret == q_invalid_pointer)
		printf("invalid pointer\n");

	else if (ret == q_invalid_pointer)
		printf("invalid length\n");

	else
		printf("unknown\n");
}

int main( int argc, char * argv)
{
	printf(" *** Queue manager ***\n");

	t_queue qu[3];
	uint8_t qu_count = 0;
	uint32_t i = 1;

	/*
	 * Check if the file with the queues exits
	 * + :	read the info about each queue
	 * - :	create the file and mark up the file space
	 */
	FILE * quf;
	errno_t quf_res = fopen_s( &quf, "./queues.bin", "r");
	bool flag_q_first = false;

	if (NULL == quf)
	{
		quf_res = fopen_s( &quf, "./queues.bin", "w+b");
		flag_q_first = true;
	}
	else
		quf_res = freopen_s( &quf, "./queues.bin", "w+b", quf);
	


	char board_char_0 = getchar();


	if ('1' == board_char_0)
	{
		printf("switched to queue #%c!\n", board_char_0);
		qu_count = 0;
	}

	if ('2' == board_char_0)
	{
		printf("switched to queue #%c!\n", board_char_0);
		qu_count = 1;
	}

	if ('3' == board_char_0)
	{
		printf("switched to queue #%c!\n", board_char_0);
		qu_count = 2;
	}


	if ( 'i' == board_char_0)
	{
		if (qu_count == QUEUE_MAXCOUNT)
			printf("It is enough!\n");
		else
		{
			const e_qres qres = qinit(&qu[qu_count], (size_t *)&mem_space[qu_count][0], qu_size[qu_count]);
			print_qret(qres);

			if (qres == q_ok)
			{
				print_queue_info(&qu[qu_count]);
				print_queue_items__all(&qu[qu_count]);
			}
		}
	}

	if ('u' == board_char_0)
	{
		const e_qres qres = qpush(&qu[qu_count], i++);
		print_qret(qres);
		print_queue_info(&qu[qu_count]);
		print_queue_items__all(&qu[qu_count]);
	}

	if ('o' == board_char_0)
	{
		uint32_t qres = qpop(&qu[qu_count]);
		printf("[%u]\n", qres);
		print_queue_info(&qu[qu_count]);
		print_queue_items__all(&qu[qu_count]);
	}

	if ('m' == board_char_0)
	{
		const e_qres qres = qmerge( &qu[2], &qu[0], &qu[1], (size_t *)&mem_space[2][0], QUEUE3_LENGTH);
		print_qret(qres);

		if (qres == q_ok)
		{
			printf("switched to queue #3!\n");
			qu_count = 2;

			print_queue_info(&qu[2]);
			print_queue_items__all(&qu[2]);
		}
	}

	if ('p' == board_char_0)
	{
		print_queue_info(&qu[qu_count]);
		print_queue_items__all(&qu[qu_count]);
	}

	/*
	if ('q' == board_char_0)
		break; */

	return 0;
}