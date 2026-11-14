#include "common.h"
#include "wd_arc_field.h"

static const float WD_HALO_FLICKER = 0.22;

struct v2p_nixie
{
	float2 tc0 : TEXCOORD0;
	float3 N   : TEXCOORD1;
	float3 P   : TEXCOORD2;
};

float4 main( v2p_nixie I ) : COLOR0
{
	float mask = tex2D(s_base, I.tc0 ).r;

	// same phase as the digit shader so the halo breathes with the digits
	float flicker = 1.0;
#ifdef WD_ARC_FIELD
	flicker *= wd_tube_surge();
#endif

	return float4( WD_HALO_COLOR * flicker * mask, 0.0f );
}
