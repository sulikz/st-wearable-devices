#ifndef WD_SCREEN_FX_H
#define WD_SCREEN_FX_H

#if defined(SHADER_BUS) && SHADER_BUS >= 7
#include "wd_hash.h"

// x interference level, written by wd_screen_fx.script
uniform float4	bus_wd_screen_interference;
uniform float4	bus_engine_sun_los;

#define WD_GLARE_F0		0.04
#define WD_GLARE_HAZE	0.05
#define WD_GLARE_GLOSS	64.0

// roll adds the hum bar and the torn rows, which need the screen's own panel frame
float3 apply_interference ( float3 rgb, float level, float2 norm, float2 grid, bool roll )
{
	uint2 cell	= (uint2)( floor( grid ) + 8192.0 );
	uint row	= wd_hash( cell.y ^ wd_hash( (uint)( timers.x * 30.0 ) ) );
	if ( roll )
	{
		float phase	= frac( norm.y - timers.x * 0.27 ) - 0.5;
		float bar	= exp( -phase * phase * 60.0 );
		float tear	= (row >> 16u) / 65535.0 > 1.0 - 0.03 * level ? 0.45 : 1.0;
		rgb		*= (1.0 - 0.4 * level * bar) * lerp( 1.0, tear, level );
	}
	float snow	= (wd_hash( cell.x ^ row ) & 65535u) / 65535.0 - 0.5;
	return max( 0.0, rgb + snow * 0.3 * level );
}

float3 get_sun_glare ( float2 panel )
{
	float3 view	= normalize( mul( m_WV, float4( panel, 0.0, 1.0 ) ) );
	float3 n	= normalize( cross( mul( m_WV, float4( 1.0, 0.0, 0.0, 0.0 ) ), mul( m_WV, float4( 0.0, 1.0, 0.0, 0.0 ) ) ) );
	n		= dot( n, view ) > 0.0 ? -n : n;
	float3 to_sun	= -L_sun_dir_e;
	float lit	= saturate( dot( n, to_sun ) );
	float3 glare	= 0.0;
	[branch] if ( lit > 0.0 )
	{
		float facing	= saturate( dot( n, -view ) );
		float fresnel	= WD_GLARE_F0 + (1.0 - WD_GLARE_F0) * pow( 1.0 - facing, 5.0 );
		float lobe	= pow( saturate( dot( reflect( view, n ), to_sun ) ), WD_GLARE_GLOSS ) * (WD_GLARE_GLOSS + 2.0) / 6.2831853;
		glare		= L_sun_color * lit * (WD_GLARE_HAZE + fresnel * lobe);
	}
	return glare;
}

float3 apply_screen_fx ( float3 rgb, float2 norm, float2 grid, float2 panel, bool roll )
{
	float level	= bus_wd_screen_interference.x;
	[branch] if ( level > 0.0 )
		rgb		= apply_interference( rgb, level, norm, grid, roll );
	[branch] if ( bus_engine_sun_los.x > 0.5 )
		rgb		+= get_sun_glare( panel );
	return rgb;
}
#else
float3 apply_screen_fx ( float3 rgb, float2 norm, float2 grid, float2 panel, bool roll )
{
	return rgb;
}
#endif

#endif
