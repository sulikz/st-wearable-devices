#include "common.h"
struct vf {float2 tc0:TEXCOORD0; float3 light:TEXCOORD1;};
float4 main(vf I):COLOR0 {
    float4 base=tex2D(s_base,I.tc0);
#ifdef WD_ALPHA_TEST
    clip(base.a-0.5);
#endif
    return float4(base.rgb*I.light,base.a);
}
