#include "common.h"
#include "wd_depth_pull.h"


struct	ui_vert_in
{
	float4 	P		: POSITION;
	float4	color	: COLOR0;
	float2 	uv		: TEXCOORD0;
};

struct	ui_vert_out
{
 	float2 	tc0		: TEXCOORD0;
	float4	cpos	: TEXCOORD1;
	float4	c		: COLOR0;
	float4 	P		: SV_Position;
};


ui_vert_out main (ui_vert_in v)
{
	ui_vert_out	O;

	O.tc0		= v.uv;
	O.c			= v.color.bgra;
	float3	Pe	= mul( m_WV, float4( v.P.xyz, 1 ) );
	O.cpos		= mul( m_P, float4( Pe, 1 ) );
	O.P			= mul( m_P, float4( wd_pull_toward_eye( Pe ), 1 ) );
	return 		O;
}
