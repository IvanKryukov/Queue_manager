#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "queue_manager.h"
#include "parser.h"

//#define TRACE_RES

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
static const size_t qu_size[3] = { QUEUE1_LENGTH, QUEUE2_LENGTH, QUEUE3_LENGTH };

#if (defined TRACE_RES)
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
#endif

size_t get_queues( t_queue * q1, t_queue * q2, t_queue* q3, FILE * f)
{
	size_t ret = get_queue_from_file( q1, f);
	if (0 != ret)
		return 1;
	
	ret = get_queue_from_file( q2, f);
	if (0 != ret)
		return 2;

	ret = get_queue_from_file( q3, f);
	if (0 != ret)
		return 3;

	return 0;
}

void set_queues( t_queue q1, t_queue q2, t_queue q3, FILE * f)
{
	set_queue_to_file( q1, f);
	set_queue_to_file( q2, f);
	set_queue_to_file( q3, f);
}



int main( int argc, char ** argv)
{	
	argc--;

	if ((argc == 0) || (argc > 3))
		return 0;

	/**************************************************
	 *** Read the input info: command and arguments ***
	 **************************************************/
	// Read the command
	int command = get_command((const char *)*++argv);
	if ((command < (int)cmd_push) && (command > (int)cmd_merge))
	{
		printf("wrong command\n");
		return -1;
	}

	// Read the arguments
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

	/**************************************************************
	 *** Check if the command and related arguments are correct ***
	 **************************************************************/
	if ((command == (int)cmd_merge) && (argc != 1))
	{
		printf("unexpected token\n");
		return -3;
	}

	if (((command == (int)cmd_push) || (command == (int)cmd_print_qdata_bits)) && (argc != 3))
	{
		printf("argument needed\n");
		return -4;
	}

	if (((command >= (int)cmd_pop) && (command <= (int)cmd_print_qdata_all)) && (argc != 2))
	{
		printf("unexpected token\n");
		return -5;
	}

	if ((command >= (int)cmd_push) && (command <= (int)cmd_merge) &&
		((argument[0] < 1) || (argument[0] > 3))
		)
	{
		printf("wrong argument\n");
		return -6;
	}

	if ((command > (int)cmd_pop) && (command < (int)cmd_merge) &&
		((argument[0] < 1) || (argument[0] > 3))
		)
	{
		printf("wrong argument\n");
		return -7;
	}

	if ((command > (int)cmd_print_qdata_bits) && ( (argument[1] < 0) || (argument[1] > 31) ))
	{
		printf("wrong argument 2\n");
		return -8;
	}

	/******************************************
	 *** Initialization of queue structures ***
	 ******************************************/
	t_queue qu[3];
	uint8_t qu_count = 0;

	e_qres qres = qinit(&qu[0], (size_t *)&mem_space[0][0], qu_size[0]);
#if (defined TRACE_RES)
	print_qret(qres);
#endif

	qres = qinit(&qu[1], (size_t *)&mem_space[1][0], qu_size[1]);
#if (defined TRACE_RES)
	print_qret(qres);
#endif

	qres = qinit(&qu[2], (size_t *)&mem_space[2][0], qu_size[2]);
#if (defined TRACE_RES)
	print_qret(qres);
#endif

	/*******************************************************
	 *** Read the queue info and data if the file exists ***
	 *******************************************************/
	FILE * quf;
	errno_t quf_res = fopen_s( &quf, "./queues.bin", "rb+");
	if (NULL != quf)
	{
		size_t getq_ret = get_queues(&qu[0], &qu[1], &qu[2], quf);
		fclose(quf);
	}

	/****************************************************************************
	 *** Execute the command with related arguments after checking everything ***
	 ****************************************************************************/
	if ((int)cmd_push == command)
	{
		const e_qres qres = qpush(&qu[argument[0] - 1], argument[1]);
#if (defined TRACE_RES)
		print_qret(qres);
		print_queue_info(&qu[argument[0] - 1]);
		print_queue_items__all(&qu[argument[0] - 1]);
#endif
	}

	if ((int)cmd_pop == command)
	{
		if (qempty(&qu[argument[0] - 1]))
			printf("empty\n");
		else
		{
			uint32_t qres = qpop(&qu[argument[0] - 1]);
			printf("%u\n", qres);
		}
#if (defined TRACE_RES)
		print_queue_info(&qu[argument[0] - 1]);
		print_queue_items__all(&qu[argument[0] - 1]);
#endif
	}

	if ((int)cmd_merge == command)
	{
		const e_qres qres = qmerge( &qu[2], &qu[0], &qu[1], (size_t *)&mem_space[2][0], QUEUE3_LENGTH);
#if (defined TRACE_RES)
		print_qret(qres);
		if (qres == q_ok)
		{
			qu_count = 2;

			print_queue_info(&qu[2]);
			print_queue_items__all(&qu[2]);
		}
#endif
	}

	if ((int)cmd_print_qinfo == command)
	{
		print_queue_info(&qu[argument[0] - 1]);
		print_queue_items__all(&qu[argument[0] - 1]);
	}

	if ((int)cmd_print_qdata_all == command)
	{
		print_queue_items__all(&qu[argument[0] - 1]);
	}

	if ((int)cmd_print_qdata_bits == command)
	{
		print_queue_items__chosen(&qu[argument[0] - 1], argument[1]);
	}

	/**********************************************
	 *** Write the info and data of every queue ***
	 **********************************************/
	quf_res = fopen_s(&quf, "./queues.bin", "wb+");
	set_queues(qu[0], qu[1], qu[2], quf);
	fclose(quf);
	
	return 0;
}