// Wearable Devices' screen vertex shader, tagging each triangle corner so a bent picture stays inside its own art
#include "common.h"
#include "wd_arc_field.h"

struct	ui_vert_in
{
	float4 	P		: POSITION;
	float4	color	: COLOR0;
	float2 	uv		: TEXCOORD0;
#ifdef WD_ARC_FIELD
	uint	id		: SV_VertexID;
#endif
};

struct	ui_vert_out
{
 	float2 	tc0		: TEXCOORD0;
	float2 	panel	: TEXCOORD1;
	float4	cpos	: TEXCOORD2;
	float2	metric	: TEXCOORD3;
	float4	c		: COLOR0;
#ifdef WD_ARC_FIELD
	float4	corner_a	: TEXCOORD4;
	float4	corner_b	: TEXCOORD5;
#endif
	float4 	P		: SV_Position;
};

ui_vert_out main (ui_vert_in v)
{
	ui_vert_out	O;

	O.tc0		= v.uv;
	O.panel		= v.P.xy;
	O.metric	= v.P.xy * float2( length( mul( (float3x3)m_W, float3( 1, 0, 0 ) ) ), length( mul( (float3x3)m_W, float3( 0, 1, 0 ) ) ) );
	O.c			= v.color.bgra;
	O.P			= v.P;
	O.P.w		= 1;
	O.P			= mul( m_WVP, O.P );
	O.cpos		= O.P;
#ifdef WD_ARC_FIELD
	// the UI draws every triangle from three fresh vertices, so the id's remainder tells its corners apart
	uint corner	= v.id % 3u;
	float3 pick	= float3( corner == 0u, corner == 1u, corner == 2u );
	O.corner_a	= float4( pick.x * v.uv, pick.y * v.uv );
	O.corner_b	= float4( pick.z * v.uv, pick.x, pick.y );
#endif
	return 		O;
}
