#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "queue_manager.h"

#define CHECK_WVAL		(uint32_t)(0xAABB4455)
#define CHECK_RVAL		(uint32_t)(0x5544BBAA)

int get_command( const char * cmd_string)
{
	if ((cmd_string[0] >= '0') && (cmd_string[0] <= '9'))
		return (int)(cmd_string[0] - '0');
	else
		return -1;
}

int get_argument( const char * arg_string)
{
	int arg_length = 0;
	char * jumper = (char *)arg_string;
	
	while (*arg_string++ != 0)
		arg_length++;
	
	if (arg_length > MAX_ARG_LENGTH)
		return -1;
	else
	{
		int i, ret = 0;

		for (i = 0; i < arg_length; i++)
		{
			if ((*jumper >= '0') && (*jumper <= '9'))
			{
				ret += ((int)*jumper++ - '0');

					if (i < arg_length - 1)
						ret *= 10;
			}
			else 
				return -2;
		}
		return ret;
	}
}

size_t set_queue_to_file( t_queue q, FILE * f)
{
	/*
	 *	Так как структура t_queue содержит в себе три ссылки на область в памяти,
	 *	а при исполнении программы могут выделяться различные области, то запись
	 *	данных о структуре t_queue переходит в относительные величины по правилу:
	 *	- base = 0 всегда,
	 *	- head = номер относительно base,
	 *	- tail = номер относительно base,
	 *	- size и max_size - без изменений
	 */
	
	uint32_t ch = CHECK_WVAL;
	size_t res = fwrite((const void *)(&ch), sizeof(uint32_t), 1, f);	

	uint32_t data_to_write[5] = { 0, 0, 0, 0 ,0 };
	data_to_write[1] = (uint32_t)(q.head - q.base);
	data_to_write[2] = (uint32_t)(q.tail - q.base);
	data_to_write[3] = (uint32_t)(q.size);
	data_to_write[4] = (uint32_t)(q.max_size);

	res = fwrite( (const void *)data_to_write, sizeof(uint32_t), sizeof(data_to_write) / sizeof(uint32_t), f);
	uint32_t i;
	for (i = 0; i < data_to_write[3]; i++)
	{
		uint32_t val = qpop(&q);
		fwrite( (const void *)(&val), sizeof(val), 1, f);
	}

	return 0;
}

size_t get_queue_from_file( t_queue * q, FILE * f)
{
	/*
	 *	Так как структура t_queue содержит в себе три ссылки на область в памяти,
	 *	а при исполнении программы могут выделяться различные области, то чтение
	 *	данных о структуре t_queue переходит в относительные величины по правилу:
	 *	- base = p_base,
	 *	- head = p_base + номер относительно base,
	 *	- tail = p_base + номер относительно base,
	 *	- size и max_size - без изменений
	 */

	uint32_t val;
	size_t ret = fread_s( (void *)(&val), sizeof(val), sizeof(uint32_t), 1, f);

	if (val != CHECK_WVAL)
		return -1;
	else
	{
		uint32_t data_to_read[5] = { 0, 0, 0, 0 ,0 };
		size_t ret = fread_s((void *)data_to_read, sizeof(data_to_read), sizeof(uint32_t), sizeof(data_to_read) / sizeof(uint32_t), f);

		q->tail = q->head = (size_t *)(q->base) + data_to_read[1];
		q->max_size = data_to_read[4];

		size_t i;
		for (i = 0; i < (size_t)data_to_read[3]; i++)
		{
			fread_s( (void *)(&val), sizeof(val), sizeof(uint32_t), 1, f);
			qpush( q, val);
		}
	}

	return 0;
}