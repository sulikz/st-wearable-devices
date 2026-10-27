#ifndef WD_BODY_FILL_H
#define WD_BODY_FILL_H

#include "wd_face_arc.h"
#undef WD_CRT_SAMPLE

#ifndef WD_BODY_HATCH_PITCH
#define WD_BODY_HATCH_PITCH	13.9
#endif
#ifndef WD_BODY_HATCH_LIGHT
#define WD_BODY_HATCH_LIGHT	0.505
#endif

float get_body_hatch ( float2 panel )
{
	float2 art	= (panel - PANEL_ORIGIN) / PANEL_SIZE * PANEL_ART;
	float run	= (art.x + art.y) / WD_BODY_HATCH_PITCH;
	float phase	= frac( run );
	float edge	= max( fwidth( run ), 1e-4 );
	float inside	= phase < WD_BODY_HATCH_LIGHT ? min( phase, WD_BODY_HATCH_LIGHT - phase ) : -min( phase - WD_BODY_HATCH_LIGHT, 1.0 - phase );
	return saturate( inside / edge + 0.5 );
}

float4 wd_body_sample ( float2 tc, float2 panel, float4 c )
{
	float4 art	= s_base.Sample( smp_base, tc );
	float cover	= art.a;
	if ( c.g > 0.5 )
		cover	*= get_body_hatch( panel );
	return float4( get_arc_tone( c ) * art.rgb * c.a, cover );
}

#define WD_CRT_SAMPLE(I) wd_body_sample( I.tc0, I.panel, I.c )

#endif
