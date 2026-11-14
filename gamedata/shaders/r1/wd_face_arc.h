#include "common.h"
#include "wd_sample_ui.h"

static const float3 WD_ARC_OK = float3( 0.7765, 0.7765, 0.7765 );
static const float3 WD_ARC_WARN = float3( 1.0, 0.6941, 0.3490 );
static const float3 WD_ARC_CRIT = float3( 1.0, 0.3490, 0.3490 );

#define WD_ARC_LEAD		11.33
#define WD_ARC_TAIL		2.27
#define WD_ARC_QUARTER	1.57079633

float3 get_arc_tone ( float4 c )
{
	return c.b > 0.75 ? WD_ARC_CRIT : ( c.b > 0.25 ? WD_ARC_WARN : WD_ARC_OK );
}

// Polynomial angle approximation avoids SM2 atan2 instruction overflow.
float wd_atan2(float y,float x) {
    float ax=abs(x), ay=abs(y);
    float r=min(ax,ay)/max(max(ax,ay),1e-6);
    float a=0.78539816*r-r*(r-1.0)*(0.2447+0.0663*r);
    a=lerp(a,1.57079633-a,step(ax,ay));
    a=lerp(a,3.14159265-a,step(x,0.0));
    return a*sign(y);
}

float get_arc_position ( float2 panel )
{
	float2 offset	= panel - (PANEL_ORIGIN + 0.5 * PANEL_SIZE);
	float angle	= fmod( wd_atan2( offset.y, offset.x ) + 4.0 * WD_ARC_QUARTER, WD_ARC_QUARTER );
	return (angle - radians( WD_ARC_LEAD )) / radians( 90.0 - WD_ARC_LEAD - WD_ARC_TAIL );
}

float4 wd_arc_sample ( float2 tc, float2 panel, float4 c )
{
	float4 art	= wd_sample_ui(tc);
	float position	= get_arc_position( panel );
	float edge	= 0.01;
	float fill	= lerp( -edge, 1.0 + edge, c.r );
	return float4( get_arc_tone( c ) * art.rgb * c.a, art.a * saturate( (fill - position) / edge + 0.5 ) );
}

#define WD_CRT_SAMPLE(I) wd_arc_sample( I.tc0, I.panel, I.c )
