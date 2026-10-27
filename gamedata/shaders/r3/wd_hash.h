#ifndef WD_HASH_H
#define WD_HASH_H

uint wd_hash ( uint v )
{
	uint state	= v * 747796405u + 2891336453u;
	uint word	= ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

#endif
