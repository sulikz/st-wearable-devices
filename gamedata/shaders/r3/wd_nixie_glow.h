#include "common.h"

static const float WD_NIXIE_FLICKER = 0.14;
static const float WD_NIXIE_LIMB = 1.6;

struct v2p_nixie
{
	float2 tc0 : TEXCOORD0;
	float3 N   : TEXCOORD1;
	float3 P   : TEXCOORD2;
};

float4 main( v2p_nixie I ) : SV_Target
{
	float4 t_base = s_base.Sample( smp_base, I.tc0 );

	float facing = saturate( dot( normalize(I.N), normalize(-I.P) ) );
	float grazing = 1.0f - facing;
	float limb = 1.0f + WD_NIXIE_LIMB * grazing * grazing;
	float3 color = lerp( WD_NIXIE_CORE, WD_NIXIE_EDGE, grazing * grazing ) * limb;

	float flicker = 1.0f - WD_NIXIE_FLICKER * (0.5f + 0.5f * sin(timers.x * 41.0f) * sin(timers.x * 13.7f));

	return float4( color * flicker * t_base.r, 1.0f );
}
