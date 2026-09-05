#include "common.h"

static const float WD_HALO_FLICKER = 0.22;

struct v2p_nixie
{
	float2 tc0 : TEXCOORD0;
	float3 N   : TEXCOORD1;
	float3 P   : TEXCOORD2;
};

float4 main( v2p_nixie I ) : SV_Target
{
	float mask = s_base.Sample( smp_base, I.tc0 ).r;

	// same phase as the digit shader so the halo breathes with the digits
	float flicker = 1.0f - WD_HALO_FLICKER * (0.5f + 0.5f * sin(timers.x * 41.0f) * sin(timers.x * 13.7f));

	return float4( WD_HALO_COLOR * flicker * mask, 0.0f );
}
