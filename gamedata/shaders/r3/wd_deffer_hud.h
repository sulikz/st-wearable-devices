#undef USE_TDETAIL
#undef USE_TDETAIL_BUMP

#include "sload.h"
#include "check_screenspace.h"
#include "screenspace_mvectors.h"

#ifdef SSFX_RAINDROPS
	#include "screenspace_hud_raindrops.h"
#endif

Texture2D s_wdnrm;

// ---- SSS HUD raindrops ----
#define WD_RAIN_UPFACE   1
#define WD_RAIN_DEADZONE 0.0
#define WD_WET_BUMP      1.0
#define WD_WET_REFRACT   0.0
#define WD_WET_DARKEN    0.0

float3 wd_perturb_normal(float3 N, float3 viewpos, float2 uv, float3 mapN)
{
	float3 dp1  = ddx(viewpos);
	float3 dp2  = ddy(viewpos);
	float2 duv1 = ddx(uv);
	float2 duv2 = ddy(uv);

	float3 dp2perp = cross(dp2, N);
	float3 dp1perp = cross(N, dp1);
	float3 T = dp2perp * duv1.x + dp1perp * duv2.x;
	float3 B = dp2perp * duv1.y + dp1perp * duv2.y;

	float invmax = rsqrt(max(dot(T, T), dot(B, B)));
	return normalize(T * (invmax * mapN.x) + B * (invmax * mapN.y) + N * mapN.z);
}

f_deffer main( p_flat I )
{
	f_deffer O;

	float3 drops    = 0;  // xy = raindrop normal, z = overall strength
	float  drops_w  = 0;  // reflection/bulge strength (weapon: max(drops_1.z, 3.0))
	float2 uv_smear = 0;  // texture-UV refraction shift
	float  wet_gloss = 0; // drops uniforms only exist when SSFX_RAINDROPS is on

#ifdef SSFX_RAINDROPS
	float rainAmount = saturate((ssfx_hud_drops_1.y - WD_RAIN_DEADZONE) / (1.0 - WD_RAIN_DEADZONE));

	if (rainAmount > 0.0)
	{
		drops = ssfx_hud_raindrops(s_hud_rain, I.RDrops.xyz, 1.0f);

#if WD_RAIN_UPFACE == 1
		drops *= saturate(I.RDrops.w);
#endif
		drops *= rainAmount;
		drops_w  = max(ssfx_hud_drops_1.z, 3.0f);
		uv_smear = drops.xy * ssfx_hud_drops_1.w * WD_WET_REFRACT;
	}
	wet_gloss = (ssfx_hud_drops_1.y * ssfx_hud_drops_2.z) + drops.z * ssfx_hud_drops_2.w;
#endif

	float2 uv_s = I.tcdh.xy + uv_smear;
	float4 base = tbase(uv_s);
#ifdef WD_ALPHA_TEST
	clip(base.a - 0.5);
#endif
	float3 D = base.rgb;
	float4 Nu   = s_wdnrm.Sample(smp_base, uv_s);
	float3 mapN = unpack_normal(Nu.wzy);
#ifdef SSFX_ENHANCED_SHADERS
	float  gloss = Nu.x;
#else
	float  gloss = Nu.x * Nu.x;
#endif

	mapN.xy += drops.xy * drops_w * WD_WET_BUMP;
	gloss    = saturate(gloss + wet_gloss);
	D       *= 1.0 - drops.z * WD_WET_DARKEN;

	float3 Ngeo = normalize((float3)I.N.xyz);
	float3 N    = wd_perturb_normal(Ngeo, I.position.xyz, I.tcdh.xy, mapN);

	float ms = xmaterial;
#ifdef USE_LM_HEMI
	float4 lm = s_hemi.Sample(smp_rtlinear, I.lmh);
	float  h  = get_hemi(lm);
# ifdef USE_R2_STATIC_SUN
	       ms = get_sun(lm);
# endif
#else
	float  h  = I.position.w;
# ifdef USE_R2_STATIC_SUN
	       ms = I.tcdh.w;
# endif
#endif

	float4 Ne = float4(N, h);
	O = pack_gbuffer(
		Ne,
		float4(I.position.xyz, ms),
		float4(D, gloss)
	);

	O.velocity = ssfx_mv_calc(I.vel_curr, I.vel_prev, 1, 0);

	return O;
}
