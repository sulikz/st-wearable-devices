#include "common.h"
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
static const float2 PANEL_TAPS[4] = { float2( -0.125, -0.375 ), float2( 0.375, -0.125 ), float2( 0.125, 0.375 ), float2( -0.375, 0.125 ) };

float2 clamp_footprint ( float2 step, float2 size )
{
	float texels	= length( step * size );
	return texels > PANEL_TAP_REACH ? step * (PANEL_TAP_REACH / texels) : step;
}

float4 sample_panel_texel ( ui_vert_out I )
{
	float2 dx	= ddx( I.tc0 );
	float2 dy	= ddy( I.tc0 );
	uint width, height, levels;
	s_base.GetDimensions( 0, width, height, levels );
	float4 texel;
	[branch]
	if ( levels > 1 )
	{
		texel	= s_base.SampleGrad( smp_base, I.tc0, dx, dy );
	}
	else
	{
		float2 size	= float2( width, height );
		dx		= clamp_footprint( dx, size );
		dy		= clamp_footprint( dy, size );
		float4 sum	= 0.0;
		[unroll]
		for ( uint tap = 0u; tap < 4u; ++tap )
		{
			float4 hit	= s_base.SampleLevel( smp_base, I.tc0 + PANEL_TAPS[tap].x * dx + PANEL_TAPS[tap].y * dy, 0 );
			sum		+= float4( hit.rgb * hit.a, hit.a );
		}
		texel	= float4( sum.rgb / max( sum.a, 1e-6 ), sum.a * 0.25 );
	}
#ifdef WD_GREYSCALE
	texel.rgb	= dot( texel.rgb, float3( 0.299, 0.587, 0.114 ) );
#endif
	return texel * I.c;
}
#define WD_CRT_SAMPLE(I) sample_panel_texel( I )
#endif

#include "wd_crt_field.h"

float4 	main ( ui_vert_out I )	: SV_Target
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
