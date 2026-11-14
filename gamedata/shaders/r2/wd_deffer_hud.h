// DX9 SM3 G-buffer: position, normal/hemi, diffuse/gloss.
#undef USE_TDETAIL
#undef USE_TDETAIL_BUMP
#include "common.h"
#include "wd_perturb_normal.h"

f_deffer main(p_flat I)
{
    f_deffer O;
    float4 base = tex2D(s_base, I.tcdh.xy);
#ifdef WD_ALPHA_TEST
    clip(base.a - 0.5);
#endif
    float4 Nu = tex2D(s_wdnrm, I.tcdh.xy);
    float3 Ngeo = normalize(I.N);
    float3 N = wd_perturb_normal(Ngeo, I.position.xyz, I.tcdh.xy, unpack_normal(Nu.wzy));
    float ms = xmaterial;
#ifdef USE_LM_HEMI
    float4 lm = tex2D(s_hemi, I.lmh);
    float h = get_hemi(lm);
#ifdef USE_R2_STATIC_SUN
    ms = get_sun(lm);
#endif
#else
    float h = I.position.w;
#ifdef USE_R2_STATIC_SUN
    ms = I.tcdh.w;
#endif
#endif
    O.Ne = float4(N, h);
    O.position = float4(I.position.xyz, ms);
    O.C = float4(base.rgb, Nu.x * Nu.x);
    return O;
}
