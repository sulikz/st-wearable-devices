#ifndef WD_CRT_INPUT_H
#define WD_CRT_INPUT_H
#include "wd_arc_field.h"

struct	ui_vert_out
{
 	float2 	tc0		: TEXCOORD0;
	float2 	panel	: TEXCOORD1;
	float4	cpos	: TEXCOORD2;
	float2	metric	: TEXCOORD3;
	float4	c		: COLOR0;
};

#define CRT_PITCH		float2(0.00013168, 0.00013129)

#endif
