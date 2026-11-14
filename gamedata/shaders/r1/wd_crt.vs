#define half float
#define half2 float2
#define half3 float3
#define half4 float4
#include "common.h"
#include "wd_arc_field.h"

struct	ui_vert_in
{
	float4 	P		: POSITION;
	float4	color	: COLOR0;
	float2 	uv		: TEXCOORD0;
};

struct	ui_vert_out
{
 	float2 	tc0		: TEXCOORD0;
	float2 	panel	: TEXCOORD1;
	float4	cpos	: TEXCOORD2;
	float2	metric	: TEXCOORD3;
	float4	c		: COLOR0;
	float4 	P		: POSITION;
};

#define WD_HUD_NEAR			0.005
#define WD_HUD_DEPTH_MAX	0.02

#if defined(SHADER_BUS) && SHADER_BUS >= 4
uniform float4	bus_cvar_hud_fov;
#endif

ui_vert_out main (ui_vert_in v)
{
	ui_vert_out	O;

	O.tc0		= v.uv;
	O.panel		= v.P.xy;
	O.metric	= v.P.xy * float2( length( mul( (float3x3)m_W, float3( 1, 0, 0 ) ) ), length( mul( (float3x3)m_W, float3( 0, 1, 0 ) ) ) );
	O.c			= v.color;
	O.P			= v.P;
	O.P.w		= 1;
	O.P			= mul( m_WVP, O.P );
#if defined(SHADER_BUS) && SHADER_BUS >= 4
	if ( bus_cvar_hud_fov.x > 0.0 )
		O.P.xy	*= 1.0 / ( tan( radians( bus_cvar_hud_fov.x * 83.0 ) * 0.5 ) * m_P._22 );
#endif
	O.P.z		= WD_HUD_DEPTH_MAX * max( O.P.w - WD_HUD_NEAR, 0.0 );
	O.cpos		= O.P;
	return 		O;
}
