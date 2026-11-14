#define half float
#define half2 float2
#define half3 float3
#define half4 float4
#include "common.h"
#include "skin.h"


struct v2p_nixie
{
	float2 tc0  : TEXCOORD0;
	float3 N    : TEXCOORD1;
	float3 P    : TEXCOORD2;
	float4 hpos : POSITION;
};

v2p_nixie _main( v_model I )
{
	v2p_nixie O;
	O.hpos = mul( m_WVP, I.pos );
	O.tc0 = I.tc.xy;
	O.N = mul( (float3x3)m_WV, (float3)I.norm );
	O.P = (float3)mul( m_WV, I.pos );
	return O;
}

#define SKIN_VF v2p_nixie
#include "skin_main.h"
