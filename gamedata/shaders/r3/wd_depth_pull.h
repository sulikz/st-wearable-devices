#define WD_DEPTH_PULL	0.005

float3 wd_pull_toward_eye ( float3 Pe )
{
	return Pe * ( ( Pe.z > 0 ) ? max( 1.0 - WD_DEPTH_PULL / Pe.z, 0.5 ) : 1.0 );
}
