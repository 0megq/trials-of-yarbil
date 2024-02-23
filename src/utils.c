#include "utils.h"

float signf(float x)
{
	if (x > 0)
		return 1;
	if (x < 0)
		return -1;
	return 0;
}

Vector2 signv(Vector2 v)
{
	v.x = signf(v.x);
	v.y = signf(v.y);
	return v;
}

Vector2 absv(Vector2 v)
{
	v.x = v.x / signf(v.x);
	v.y = v.y / signf(v.y);
	return v;
}