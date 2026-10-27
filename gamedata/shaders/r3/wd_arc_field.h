// Shader bus lanes the anomaly effects read, all zero lanes draw the stock device
#ifndef WD_ARC_FIELD_H
#define WD_ARC_FIELD_H

#if defined(SHADER_BUS) && SHADER_BUS >= 7
#define WD_ARC_FIELD
#include "wd_hash.h"

// arc discharges arrive in bursts at this rate
#define WD_ARC_BURST_HZ		24.0
#define WD_ARC_FLARE_ABOVE	0.7
#define WD_ARC_STEADY		0.6
#define WD_DEGAUSS_SECONDS	1.5
#define WD_DEGAUSS_DECAY	3.0
// mains frequency of the degauss coil
#define WD_DEGAUSS_HZ		50.0

// row 0 the magnetic field at the wrist in view space, in units of the Earth's horizontal field, and the mask's residual magnetisation in w
// row 1 the time the last degauss started and the magnetisation it cleared
uniform float4	bus_wd_arc_field[2];

// PCG hash of the burst number to 0 to 1, exact at any game time
float wd_arc_hash ( uint n )
{
	return wd_hash( n ) / 4294967295.0;
}

uint wd_arc_burst_index ()
{
	return (uint)( timers.x * WD_ARC_BURST_HZ );
}

// 1 during a discharge burst, a weaker steady arc between them
float wd_arc_burst ()
{
	return wd_arc_hash( wd_arc_burst_index() ) > WD_ARC_FLARE_ABOVE ? 1.0 : WD_ARC_STEADY;
}

float wd_field_strength ()
{
	return length( bus_wd_arc_field[0].xyz );
}

// each burst induces a spike in a tube's drive current, so the glow dips and flares with the arc
float wd_tube_surge ()
{
	float depth	= saturate( 0.12 * wd_field_strength() );
	return max( 0.0, 1.0 + depth * 1.5 * (wd_arc_hash( wd_arc_burst_index() ^ 0x9e3779b9u ) * 2.0 - 1.0) );
}

// how far a degauss has decayed, 0 before one or once it is over
float wd_degauss_left ()
{
	float age	= timers.x - bus_wd_arc_field[1].x;
	return age >= 0.0 && age < WD_DEGAUSS_SECONDS ? bus_wd_arc_field[1].y * exp( -WD_DEGAUSS_DECAY * age ) : 0.0;
}

// the coil's alternating field, read at the frame rate the way a camera would see it
float wd_degauss_swing ()
{
	return sin( (timers.x - bus_wd_arc_field[1].x) * 6.2831853 * WD_DEGAUSS_HZ );
}
#endif

#endif
