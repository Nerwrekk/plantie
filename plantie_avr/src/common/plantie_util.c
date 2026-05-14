#include "plantie_util.h"

#define MAX_SIZE 16

void PUtil_Uint16ToAscii(uint16_t value, char* str, uint8_t size)
{
	if (size > MAX_SIZE)
	{
		str[0] = '\0';
		return;
	}

	char temp[MAX_SIZE] = { 0 };
	uint8_t indx        = 0;
	do
	{
		temp[indx] = (value % 10) + '0';
		value /= 10;
		indx++;
	} while (value > 0);

	for (uint8_t i = 0; i < indx; i++)
	{
		str[i] = temp[indx - i - 1];
	}

	if (indx >= size)
	{
		indx = size;
	}
	str[indx] = '\0';
}