#ifndef WD_SCREEN_FX_H
#define WD_SCREEN_FX_H
// R2 has no matching shader-bus binding. Keep the screen art unchanged.
float3 apply_screen_fx(float3 rgb, float2 norm, float2 grid, float2 panel, bool roll)
{
    return rgb;
}
#endif
