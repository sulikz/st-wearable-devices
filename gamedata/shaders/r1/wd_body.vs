#define half float
#define half2 float2
#define half3 float3
#define half4 float4
#include "common.h"
#include "skin.h"
struct vf {
    float4 hpos:POSITION;
    float2 tc0:TEXCOORD0;
    float3 light:TEXCOORD1;
};
vf _main(v_model v) {
    vf o;
    o.hpos=mul(m_WVP,v.pos);
    o.tc0=v.tc.xy;
    o.light=2.0*calc_model_lq_lighting(normalize(mul((float3x3)m_W,v.norm)));
    return o;
}
#define SKIN_VF vf
#include "skin_main.h"
