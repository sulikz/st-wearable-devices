// UI3D hud occlusion against the current frame's g-buffer depth
#define WD_OCCLUDE_BIAS	0.005

void wd_occlude ( float4 cpos )
{
	float2 tc	= cpos.xy / cpos.w * float2( 0.5, -0.5 ) + 0.5;
#ifdef USE_MSAA
	float scene_z	= s_position.Load( int2( tc * screen_res.xy ), 0 ).z;
#else
	float scene_z	= s_position.SampleLevel( smp_nofilter, tc, 0 ).z;
#endif
	if ( scene_z > SKY_EPS && scene_z < cpos.w - WD_OCCLUDE_BIAS )
		discard;
}
