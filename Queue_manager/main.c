#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "queue_manager.h"
#include "parser.h"

#define QUEUE1_LENGTH (uint32_t)(8)
#define QUEUE2_LENGTH (uint32_t)(5)
#define QUEUE3_LENGTH (uint32_t)(QUEUE1_LENGTH + QUEUE2_LENGTH)

#define QUEUE_MAXCOUNT (2)

typedef enum
{
	cmd_push = 1,
	cmd_pop,
	cmd_print_qinfo,
	cmd_print_qdata_all,
	cmd_print_qdata_bits,
	cmd_merge
} e_cmd_code;

static size_t mem_space[3][QUEUE3_LENGTH];
static const size_t qu_size[2] = { QUEUE1_LENGTH, QUEUE2_LENGTH };


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


size_t get_queues( t_queue * q1, t_queue * q2, FILE * f)
{
	size_t ret = get_queue_from_file( q1, f);
	if (0 != ret)
		return 1;
	
	ret = get_queue_from_file( q2, f);
	if (0 != ret)
		return 2;

	return 0;
}

void set_queues( t_queue q1, t_queue q2, FILE * f)
{
	set_queue_to_file( q1, f);
	set_queue_to_file( q2, f);
}



int main( int argc, char ** argv)
{	
	argc--;

	if ((argc == 0) || (argc > 3))
		return 0;

	printf(" *** Queue manager ***\n");

	int command = get_command((const char *)*++argv);
	if ((command < (int)cmd_push) && (command > (int)cmd_merge))
	{
		printf("wrong command\n");
		return -1;
	}

	char tmp_line[128];
	int argument[2] = { 0, 0 };
	if (argc >= 2)
	{
		strcpy_s(tmp_line, MAX_ARG_LENGTH, (const char *)*++argv);
		argument[0] = get_argument( (const char *)tmp_line);
		
		if (argument[0] < 0)
		{
			printf("wrong argument 1\n");
			return -2;
		}
	}
	if (argc == 3)
	{
		strcpy_s(tmp_line, MAX_ARG_LENGTH, (const char *)*++argv);
		argument[1] = get_argument((const char *)tmp_line);

		if (argument[1] < 0)
		{
			printf("wrong argument 2\n");
			return -2;
		}
	}

	if (command == (int)cmd_merge)
	{
		if (argc != 1)
		{
			printf("unexpected token\n");
			return -3;
		}
	}

	if ((command == (int)cmd_push) && (argc != 3))
	{
		printf("argument needed\n");
		return -4;
	}

	if (((command >= (int)cmd_pop) && (command <= (int)cmd_print_qdata_bits)) && (argc != 2))
	{
		printf("unexpected token\n");
		return -5;
	}

	if ((command >= (int)cmd_push) && (command <= (int)cmd_print_qdata_bits) &&
		((argument[0] < 1) || (argument[0] > 2))
		)
	{
		printf("wrong argument\n");
		return -6;
	}


	t_queue qu[3];
	uint8_t qu_count = 0;
	uint32_t i = 1;

	e_qres qres = qinit(&qu[0], (size_t*)&mem_space[0][0], qu_size[0]);
	print_qret(qres);

	qres = qinit(&qu[1], (size_t*)&mem_space[1][0], qu_size[1]);
	print_qret(qres);

	/*
	 *	Check if the file with the queues exits
	 *	+ :	read the info about each queue
	 *	- :	create the file and mark up the file space
	 */
	FILE * quf;
	errno_t quf_res = fopen_s( &quf, "./queues.bin", "rb");
	bool flag_q_first = false;

	if (NULL != quf)
	{
		fclose(quf);
		quf_res = fopen_s( &quf, "./queues.bin", "rb");
		size_t getq_ret = get_queues(&qu[0], &qu[1], quf);
		fclose(quf);
	}

	quf_res = fopen_s( &quf, "./queues.bin", "wb+");



	if ((int)cmd_push == command)
	{
		const e_qres qres = qpush(&qu[argument[0] - 1], argument[1]);
		print_qret(qres);
		print_queue_info(&qu[argument[0] - 1]);
		print_queue_items__all(&qu[argument[0] - 1]);
	}

	if ((int)cmd_pop == command)
	{
		uint32_t qres = qpop(&qu[argument[0] - 1]);
		printf("[%u]\n", qres);
		print_queue_info(&qu[argument[0] - 1]);
		print_queue_items__all(&qu[argument[0] - 1]);
	}

	if ((int)cmd_merge == command)
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

	if ((int)cmd_print_qinfo == command)
	{
		print_queue_info(&qu[argument[0] - 1]);
		print_queue_items__all(&qu[argument[0] - 1]);
	}
	
	set_queues(qu[0], qu[1], quf);
	fclose(quf);
	
	return 0;
}