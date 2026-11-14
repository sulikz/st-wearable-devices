#ifndef WD_SAMPLE_UI_H
#define WD_SAMPLE_UI_H
// Four alpha-aware samples for the original no-mipmap font/sprite atlases.
// Explicit gradients avoid divergent-branch derivative artefacts in SM3.
float4 wd_sample_ui(float2 tc)
{
    float2 dx = ddx(tc), dy = ddy(tc);
    float4 a = tex2Dgrad(s_base, tc - 0.125 * dx - 0.375 * dy, dx, dy);
    float4 b = tex2Dgrad(s_base, tc + 0.375 * dx - 0.125 * dy, dx, dy);
    float4 c = tex2Dgrad(s_base, tc + 0.125 * dx + 0.375 * dy, dx, dy);
    float4 d = tex2Dgrad(s_base, tc - 0.375 * dx + 0.125 * dy, dx, dy);
    float alpha = a.a + b.a + c.a + d.a;
    return float4((a.rgb*a.a + b.rgb*b.a + c.rgb*c.a + d.rgb*d.a) / max(alpha, 1e-6), alpha*0.25);
}
#endif
