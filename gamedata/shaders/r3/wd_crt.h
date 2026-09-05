#include "common.h"
#include "wd_occlude.h"

struct	ui_vert_out
{
 	float2 	tc0		: TEXCOORD0;
	float2 	panel	: TEXCOORD1;
	float4	cpos	: TEXCOORD2;
	float4	c		: COLOR0;
};

float3 crt_pattern ( float2 grid )
{
	float2 fw	= fwidth( grid );
	float scan_k	= SCAN_STRENGTH * (1.0 - smoothstep( 0.80, 3.75, fw.y ));
	float mask_k	= MASK_STRENGTH * (1.0 - smoothstep( 0.75, 3.35, fw.x ));

	float scan	= sin( grid.y * 3.14159265 ) * 0.5 + 0.5;
	scan		= lerp( 1.0, scan, scan_k );

	float2 cell	= floor( grid );
	float idx	= fmod( cell.y * 2.0 + cell.x + 8192.0, 4.0 );
	float3 mask	= float3( idx < 0.5, idx >= 0.5 && idx < 1.5, idx >= 1.5 && idx < 2.5 );
	mask		= lerp( float3(1.0, 1.0, 1.0), mask, mask_k );

	float boost	= min( BRIGHT_CAP, 1.0 / ((1.0 - mask_k * 0.75) * (1.0 - scan_k * 0.5)) );

	return scan * mask * boost;
}

float4 	main ( ui_vert_out I )	: SV_Target
{
	wd_occlude( I.cpos );
	float4 r	= s_base.Sample( smp_base, I.tc0 ) * I.c;

	float2 norm	= saturate( (I.panel - PANEL_ORIGIN) / PANEL_SIZE );
	r.rgb		*= crt_pattern( norm * CRT_RES );

	float vig	= pow( saturate( 16.0 * norm.x * (1.0 - norm.x) * norm.y * (1.0 - norm.y) ), 0.25 );
	r.rgb		*= lerp( 1.0, vig, EDGE_FADE );

	return r;
}
