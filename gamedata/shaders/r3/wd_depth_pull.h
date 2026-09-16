#define WD_DEPTH_PULL	0.005

float3 wd_pull_toward_eye ( float3 Pe )
{
	return Pe * ( max( Pe.z - WD_DEPTH_PULL, 0.001 ) / Pe.z );
}
