#include "common.h"
#include "skin.h"
#include "screenspace_mvectors.h"

struct v2p_nixie
{
	float2 tc0  : TEXCOORD0;
	float3 N    : TEXCOORD1;
	float3 P    : TEXCOORD2;
	float4 hpos : SV_Position;
};

v2p_nixie _main( v_model I )
{
	v2p_nixie O;
	O.hpos = mul( m_WVP, I.P );
	O.hpos.xy = ssfx_taa_jitter(O.hpos);
	O.tc0 = I.tc.xy;
	O.N = mul( (float3x3)m_WV, (float3)I.N );
	O.P = (float3)mul( m_WV, I.P );
	return O;
}

/////////////////////////////////////////////////////////////////////////
#ifdef 	SKIN_NONE
v2p_nixie	main(v_model v) 		{ return _main(v); }
#endif

#ifdef 	SKIN_0
v2p_nixie	main(v_model_skinned_0 v) 	{ return _main(skinning_0(v)); }
#endif

#ifdef	SKIN_1
v2p_nixie	main(v_model_skinned_1 v) 	{ return _main(skinning_1(v)); }
#endif

#ifdef	SKIN_2
v2p_nixie	main(v_model_skinned_2 v) 	{ return _main(skinning_2(v)); }
#endif

#ifdef	SKIN_3
v2p_nixie	main(v_model_skinned_3 v) 	{ return _main(skinning_3(v)); }
#endif

#ifdef	SKIN_4
v2p_nixie	main(v_model_skinned_4 v) 	{ return _main(skinning_4(v)); }
#endif

FXVS;
