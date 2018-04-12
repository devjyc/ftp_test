/******************************************************************************
 * ������� �������� ���, �������� ����������� ����� �.�. ����� 
 * ��� ������� ������� ����� �� ������ ����� (01-01-1970)
 * ��� ������� � ��������� �����
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "userfunc.h"
#include "usart1.h"

/**
 * ������ � ���������� ����
 */
void print_data_hex(void *data, int len)
{
    int i;
    uint8_t byte;
    

    for (i = 0; i < len; i++) {
	byte = *((uint8_t*)data + i);
	if (i % 8 == 0)
	    printf(" ");
	if (i % /*16*/32 == 0) {
	    printf("\r\n");
	}

	printf("%02X ", byte);
    }
    printf("\r\n");
}

/**
 * ���������� MAC ����� - 6 ����
 */
void print_mac_addr(c8* str, uint8_t* addr)
{
 printf("%s %02X-%02X-%02X-%02X-%02X-%02X\n", str,
	addr[0], addr[1], addr[2],
	addr[3], addr[4], addr[5]);
}


/**
 * ���������� IP ����� - 4 ����
 */
void print_ip_addr(c8* str, uint8_t* addr)
{  
  printf("%s %d.%d.%d.%d\n", str, addr[0], addr[1], addr[2], addr[3]);
}
