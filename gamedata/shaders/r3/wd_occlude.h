// SSS24 upscaling UI3D hud occlusion
Texture2D s_wdpos;

#define WD_OCCLUDE_BIAS	0.005

void wd_occlude ( float4 cpos )
{
	float2 tc	= cpos.xy / cpos.w * float2( 0.5, -0.5 ) + 0.5;
	float scene_z	= s_wdpos.SampleLevel( smp_nofilter, tc, 0 ).z;
	if ( scene_z > SKY_EPS && scene_z < cpos.w - WD_OCCLUDE_BIAS )
		discard;
}
