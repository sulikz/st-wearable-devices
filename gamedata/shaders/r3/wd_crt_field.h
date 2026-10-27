#ifndef WD_CRT_FIELD_H
#define WD_CRT_FIELD_H

#ifdef WD_ARC_FIELD
// picture shift per Earth field as a share of the panel width, picture turn in radians per Earth field along the tube
#define WD_FIELD_SHIFT		0.01
#define WD_FIELD_TURN		0.02
// colour purity error in radians of hue per Earth field at the panel edge, the three guns' spread around the green one
#define WD_FIELD_PURITY		0.25
#define WD_GUN_SPREAD		0.25
// the field where a beam starts grazing the tube's neck and bends no further
#define WD_FIELD_SATURATE	6.0
// how far the field swings in radians as the nearest arc jumps, how far a scan line drifts per Earth field
#define WD_FIELD_SWING		0.6
#define WD_FIELD_RIPPLE		0.006
#define WD_DEGAUSS_TURN		0.05
// arc bursts knock groups of this many scan lines out of sync, over 63 percent of the screen at this field
#define WD_BAND_LINES		9.0
#define WD_BAND_REACH		1.5
// the widest tear and the widest red and blue misconvergence in panel widths in a torn band
#define WD_BAND_TEAR		0.02
#define WD_BAND_CONVERGE	0.008
// the largest hue error in radians, the saturation a torn band gains and how far each gun's drive strays
#define WD_BAND_HUE			2.5
#define WD_BAND_CHROMA		0.8
#define WD_BAND_GUNS		0.6
// loose patches in CRT pixels, the field where they spread over 63 percent of their most and the most they ever cover
#define WD_PATCH_CELL		float2( 56.0, 20.0 )
#define WD_PATCH_REACH		3.0
#define WD_PATCH_SHARE		0.45
// the furthest a patch jumps in panel fractions and how far it stretches or squeezes
#define WD_PATCH_SHIFT		0.02
#define WD_PATCH_WARP		0.6
#define WD_BACKDROP_SHARE	0.8
#define WD_GRID_OFFSET		8192.0

// the picture's shift in panel fractions and its hue error, from the field the beams cross on the way to the screen
float3 wd_field_warp ( float2 norm, float2 panel )
{
	float4 field	= bus_wd_arc_field[0];
	float3 ax		= normalize( mul( m_WV, float4( 1.0, 0.0, 0.0, 0.0 ) ).xyz );
	float3 ay		= normalize( mul( m_WV, float4( 0.0, 1.0, 0.0, 0.0 ) ).xyz );
	float3 normal	= normalize( cross( ax, ay ) );
	normal			= dot( normal, mul( m_WV, float4( panel, 0.0, 1.0 ) ).xyz ) > 0.0 ? -normal : normal;

	float3 bent		= field.xyz * wd_arc_burst();
	bent			/= 1.0 + length( bent ) / WD_FIELD_SATURATE;
	float felt		= length( bent );
	// near the edge the nearest arc jumps round the anomaly from burst to burst and the field swings with it
	uint burst		= wd_arc_burst_index();
	float swing		= WD_FIELD_SWING * saturate( felt / 4.0 ) * (wd_arc_hash( burst ^ 0x68e31da4u ) * 2.0 - 1.0);
	bent			= bent * cos( swing ) + cross( normal, bent ) * sin( swing ) + normal * dot( normal, bent ) * (1.0 - cos( swing ));

	// the beam flies towards the eye and the Lorentz force pushes its electrons across the field
	float3 push		= -cross( normal, bent );
	float2 across	= float2( dot( push, ax ), dot( push, ay ) );
	float2 aspect	= float2( 1.0, PANEL_SIZE.x / PANEL_SIZE.y );
	float2 shift	= WD_FIELD_SHIFT * across * aspect;
	float2 lean		= dot( across, across ) > 0.0 ? normalize( across ) : float2( 0.0, 0.0 );
	// the arc current flickers while the beam scans down the screen, so each line lands a little off the last
	shift			+= WD_FIELD_RIPPLE * felt * sin( norm.y * 37.0 + timers.x * 61.0 + wd_arc_hash( burst ) * 6.2831853 ) * lean * aspect;

	// the field along the tube turns the picture, the degauss coil shakes it as it fades
	float degauss	= wd_degauss_left();
	float turn		= WD_FIELD_TURN * dot( bent, normal ) + WD_DEGAUSS_TURN * degauss * wd_degauss_swing();
	float2 arm		= (norm - 0.5) * PANEL_SIZE;
	float2 turned	= float2( arm.x * cos( turn ) - arm.y * sin( turn ), arm.x * sin( turn ) + arm.y * cos( turn ) );
	shift			+= (turned - arm) / PANEL_SIZE;

	// beams land at a steeper angle towards the edges and hit the next phosphor over
	float hue		= WD_FIELD_PURITY * length( across ) * dot( (norm - 0.5) * 2.0, lean );
	// a magnetised mask leaves blotches in the corners until a degauss clears them
	float corner	= length( norm - 0.5 ) * 2.0;
	hue				+= (field.w + degauss) * corner * corner * cos( 2.0 * atan2( norm.y - 0.5, norm.x - 0.5 ) + 1.0 );
	return float3( shift, hue );
}

// each arc burst knocks groups of scan lines out of sync, and closer in loose patches of the picture too
// xy the shift in panel fractions, z how hard the burst hits, w the colour error from -1 to 1, torn 1 where the picture lost sync
float4 wd_field_band ( float2 grid, float2 crt_res, out float torn )
{
	float raw		= wd_field_strength() * wd_arc_burst();
	float felt		= raw / (1.0 + raw / WD_FIELD_SATURATE);
	uint burst		= wd_arc_burst_index();
	float2 cells	= grid + WD_GRID_OFFSET;
	float4 hit		= 0.0;

	// the groups start on a fresh line every burst, so the torn bands never sit still
	float reach		= 1.0 - exp( -felt / WD_BAND_REACH );
	float scan		= cells.y + wd_arc_hash( burst ^ 0x1b873593u ) * WD_BAND_LINES;
	uint band		= (uint)( scan / WD_BAND_LINES ) * 0x9e3779b9u ^ burst;
	float pick		= wd_arc_hash( band ^ 0x85ebca6bu );
	bool lines		= wd_arc_hash( band ) < reach;
	if ( lines )
		hit			= float4( WD_BAND_TEAR * (pick * 2.0 - 1.0) * reach, 0.0, 0.5 + 0.5 * pick, wd_arc_hash( band ^ 0xc2b2ae35u ) * 2.0 - 1.0 );

	// a patch jumps out of place and stretches or squeezes about its own middle
	float spread	= 1.0 - exp( -felt / WD_PATCH_REACH );
	float2 offset	= float2( wd_arc_hash( burst ^ 0x2f6b9a1du ), wd_arc_hash( burst ^ 0x5bd1e995u ) ) * WD_PATCH_CELL;
	float2 cell		= floor( (cells + offset) / WD_PATCH_CELL );
	uint patch		= (uint)cell.x * 0x27d4eb2fu ^ (uint)cell.y * 0x165667b1u ^ burst * 0x9e3779b9u;
	bool loose		= wd_arc_hash( patch ) < spread * WD_PATCH_SHARE;
	torn			= lines || loose ? 1.0 : 0.0;
	if ( loose )
	{
		float2 middle	= (cell + 0.5) * WD_PATCH_CELL - offset;
		float2 jolt		= float2( wd_arc_hash( patch ^ 0x85ebca6bu ), wd_arc_hash( patch ^ 0xc2b2ae35u ) ) * 2.0 - 1.0;
		float squeeze	= wd_arc_hash( patch ^ 0x27d4eb2fu ) * 2.0 - 1.0;
		hit.xy			= jolt * WD_PATCH_SHIFT * spread + (cells - middle) / crt_res * WD_PATCH_WARP * squeeze;
		hit.z			= 0.5 + 0.5 * wd_arc_hash( patch ^ 0x165667b1u );
		hit.w			= wd_arc_hash( patch ^ 0x1b873593u ) * 2.0 - 1.0;
	}
	return hit;
}

// a shift in panel fractions as a step in texture coordinates
float2 wd_panel_step ( float2 shift, float2 nx, float2 ny, float2 tx, float2 ty, float det )
{
	float2 pixels	= float2( ny.y * shift.x - ny.x * shift.y, nx.x * shift.y - nx.y * shift.x ) / det;
	return tx * pixels.x + ty * pixels.y;
}

// turns a colour round the grey axis
float3 wd_hue_turn ( float3 rgb, float angle )
{
	float3 axis		= 0.57735027;
	float cs		= cos( angle );
	return rgb * cs + cross( axis, rgb ) * sin( angle ) + axis * dot( axis, rgb ) * (1.0 - cs);
}

// the texture rect of the quad's own art, rebuilt from the corners of the triangle the pixel lies in
// a corner's interpolated weight and weighted coordinate share one ratio at every pixel and in every derivative
float4 wd_picture_rect ( ui_vert_out I )
{
	float3 weight		= float3( I.corner_b.zw, 1.0 - I.corner_b.z - I.corner_b.w );
	float2 scaled[3]	= { I.corner_a.xy, I.corner_a.zw, I.corner_b.xy };
	float2 lo			= 1e6;
	float2 hi			= -1e6;
	[unroll]
	for ( uint corner = 0u; corner < 3u; ++corner )
	{
		float dx	= ddx( weight[corner] );
		float dy	= ddy( weight[corner] );
		float power	= weight[corner] * weight[corner] + dx * dx + dy * dy;
		float2 uv	= (scaled[corner] * weight[corner] + ddx( scaled[corner] ) * dx + ddy( scaled[corner] ) * dy) / max( power, 1e-20 );
		lo			= power > 1e-12 ? min( lo, uv ) : lo;
		hi			= power > 1e-12 ? max( hi, uv ) : hi;
	}
	return float4( lo, hi );
}

// a beam bent off its own art lands on nothing, and filtering stays half a texel inside the art
float4 wd_bent_sample ( ui_vert_out J, float2 tc, float4 rect, float2 texel )
{
	bool inside	= all( tc >= rect.xy - 0.01 * texel ) && all( tc <= rect.zw + 0.01 * texel );
	J.tc0		= clamp( tc, rect.xy + 0.5 * texel, rect.zw - 0.5 * texel );
	float4 art	= WD_CRT_SAMPLE( J );
	return inside ? art : 0.0;
}

float4 wd_field_sample ( ui_vert_out I, float2 raw, float2 norm, float2 grid )
{
	// a step in panel fractions as a step in texture coordinates, from the quad's own derivatives
	float2 nx		= ddx( raw );
	float2 ny		= ddy( raw );
	float2 tx		= ddx( I.tc0 );
	float2 ty		= ddy( I.tc0 );
	float2 gx		= ddx( grid );
	float2 gy		= ddy( grid );
	float det		= nx.x * ny.y - ny.x * nx.y;
	det				= abs( det ) < 1e-12 ? 1e-12 : det;
	float2 crt_res	= float2( length( float2( gx.x, gy.x ) ), length( float2( gx.y, gy.y ) ) )
					/ max( float2( length( float2( nx.x, ny.x ) ), length( float2( nx.y, ny.y ) ) ), 1e-12 );

	// the lanes are uniform, so the whole draw takes one side and the samples keep their gradients
	float4 picture;
	[branch]
	if ( wd_field_strength() + bus_wd_arc_field[0].w + wd_degauss_left() <= 0.0 )
	{
		picture		= WD_CRT_SAMPLE( I );
	}
	else
	{
		float4 rect		= wd_picture_rect( I );
		float2 texel;
		s_base.GetDimensions( texel.x, texel.y );
		texel			= 1.0 / texel;
		float3 warp		= wd_field_warp( norm, I.panel );
		float torn;
		float4 band		= wd_field_band( grid, crt_res, torn );
		warp.xy			+= band.xy;
		// the backdrop filling the panel holds still and only takes the colour errors, the items on it bend
		float tex_det	= tx.x * ty.y - ty.x * tx.y;
		float2 extent	= rect.zw - rect.xy;
		float covers	= abs( extent.x * extent.y * det / (abs( tex_det ) < 1e-20 ? 1e-20 : tex_det) );
		warp.xy			*= covers > WD_BACKDROP_SHARE ? 0.0 : 1.0;
		float2 step		= wd_panel_step( warp.xy, nx, ny, tx, ty, det );

		// the three guns sit apart in the neck, so the field bends each beam by a different amount
		// and a torn group throws the red and blue beams off the green one across the line
		float2 converge	= float2( WD_BAND_CONVERGE * band.w, 0.0 );
		float2 miss		= wd_panel_step( converge, nx, ny, tx, ty, det );
		ui_vert_out J	= I;
		J.panel			= I.panel - (warp.xy * (1.0 + WD_GUN_SPREAD) + converge) * PANEL_SIZE;
		float red		= wd_bent_sample( J, I.tc0 - step * (1.0 + WD_GUN_SPREAD) - miss, rect, texel ).r;
		J.panel			= I.panel - (warp.xy * (1.0 - WD_GUN_SPREAD) - converge) * PANEL_SIZE;
		float blue		= wd_bent_sample( J, I.tc0 - step * (1.0 - WD_GUN_SPREAD) + miss, rect, texel ).b;
		J.panel			= I.panel - warp.xy * PANEL_SIZE;
		picture			= wd_bent_sample( J, I.tc0 - step, rect, texel );

		// a torn group lands its beams on the wrong phosphors, so its colours turn and run hot, and it flares or dims with the burst
		float hue		= warp.z + WD_BAND_HUE * band.w;
		picture.rgb		= wd_hue_turn( float3( red, picture.g, blue ), hue );
		float grey		= dot( picture.rgb, float3( 0.299, 0.587, 0.114 ) );
		float chroma	= 1.0 + WD_BAND_CHROMA * band.z;
		float flare		= 1.0 + 2.0 * (band.z - 0.75) * torn;
		// the burst also upsets each gun's drive, which tints even the grey art
		float drive		= band.w * 3.14159265;
		float3 guns		= 1.0 + WD_BAND_GUNS * torn * float3( cos( drive ), cos( drive - 2.0943951 ), cos( drive + 2.0943951 ) );
		picture.rgb		= max( 0.0, lerp( grey, picture.rgb, chroma ) * guns * flare );
	}
	return picture;
}
#endif

#endif
