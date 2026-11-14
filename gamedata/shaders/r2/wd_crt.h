#include "common.h"
#include "wd_sample_ui.h"
#include "wd_occlude.h"
#include "wd_crt_input.h"
#include "wd_screen_fx.h"

#define SCAN_FADE_START	0.5
#define SCAN_FADE_END	0.75
#define MASK_FADE_START	0.45
#define MASK_FADE_END	0.65
#define PANEL_TAP_REACH	4.0

float3 crt_pattern ( float2 grid )
{
	float2 gx	= ddx( grid );
	float2 gy	= ddy( grid );
	float2 fw	= sqrt( gx * gx + gy * gy );
	float scan_k	= SCAN_STRENGTH * (1.0 - smoothstep( SCAN_FADE_START, SCAN_FADE_END, fw.y ));
	float mask_k	= MASK_STRENGTH * (1.0 - smoothstep( MASK_FADE_START, MASK_FADE_END, max( fw.x, fw.y ) ));

	float scan	= sin( grid.y * 3.14159265 ) * 0.5 + 0.5;
	scan		= lerp( 1.0, scan, scan_k );

	float2 cell	= floor( grid );
	float idx	= fmod( cell.y * 2.0 + cell.x + 8192.0, 4.0 );
	float3 mask	= float3( idx < 0.5, idx >= 0.5 && idx < 1.5, idx >= 1.5 && idx < 2.5 );
	mask		= lerp( float3(1.0, 1.0, 1.0), mask, mask_k );

	float boost	= min( BRIGHT_CAP, 1.0 / ((1.0 - mask_k * 0.75) * (1.0 - scan_k * 0.5)) );

	return scan * mask * boost;
}

#ifndef WD_CRT_SAMPLE
// SM3 has no GetDimensions. Hardware gradients select available mipmaps.
float4 sample_panel_texel(ui_vert_out I)
{
    float4 texel = wd_sample_ui(I.tc0);
#ifdef WD_GREYSCALE
    texel.rgb = dot(texel.rgb, float3(0.299, 0.587, 0.114));
#endif
    return texel * I.c;
}
#define WD_CRT_SAMPLE(I) sample_panel_texel( I )
#endif

float4 	main ( ui_vert_out I )	: COLOR0
{
	wd_occlude( I.cpos );
	float2 raw	= (I.panel - PANEL_ORIGIN) / PANEL_SIZE;
	float2 norm	= saturate( raw );
	float2 grid	= I.metric / CRT_PITCH;
#ifdef WD_ARC_FIELD
	float4 r	= wd_field_sample( I, raw, norm, grid );
#else
	float4 r	= WD_CRT_SAMPLE( I );
#endif
	r.rgb		*= crt_pattern( grid );

	float vig	= pow( saturate( 16.0 * norm.x * (1.0 - norm.x) * norm.y * (1.0 - norm.y) ), 0.25 );
	r.rgb		*= lerp( 1.0, vig, EDGE_FADE );

	r.rgb		= apply_screen_fx( r.rgb, norm, grid, I.panel, true );
	return r;
}
