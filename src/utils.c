#include "utils.h"

float signf(float x)
{
	if (x > 0)
		return 1;
	if (x < 0)
		return -1;
	return 0;
}