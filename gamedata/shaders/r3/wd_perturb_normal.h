#ifndef WD_PERTURB_NORMAL_H
#define WD_PERTURB_NORMAL_H

Texture2D s_wdnrm;

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

#endif
