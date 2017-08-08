/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef DECALS_H
#define DECALS_H

//
// Dynamic Decals
//
enum decal_e 
{	
	DECAL_GUNSHOT1 = 0, 
	DECAL_GUNSHOT2,
	DECAL_GUNSHOT3,
	DECAL_GUNSHOT4,
	DECAL_GUNSHOT5,
	DECAL_LAMBDA1,
	DECAL_LAMBDA2,
	DECAL_LAMBDA3,
	DECAL_LAMBDA4,
	DECAL_LAMBDA5,
	DECAL_LAMBDA6,
	DECAL_SCORCH1,
	DECAL_SCORCH2,
	DECAL_BLOOD1, 
	DECAL_BLOOD2, 
	DECAL_BLOOD3, 
	DECAL_BLOOD4, 
	DECAL_BLOOD5, 
	DECAL_BLOOD6, 
	DECAL_YBLOOD1, 
	DECAL_YBLOOD2, 
	DECAL_YBLOOD3, 
	DECAL_YBLOOD4, 
	DECAL_YBLOOD5, 
	DECAL_YBLOOD6, 
	DECAL_GLASSBREAK1,
	DECAL_GLASSBREAK2,
	DECAL_GLASSBREAK3,
	DECAL_BIGSHOT1,
	DECAL_BIGSHOT2,
	DECAL_BIGSHOT3,
	DECAL_BIGSHOT4,
	DECAL_BIGSHOT5,
	DECAL_SPIT1,
	DECAL_SPIT2,
	DECAL_BPROOF1,		// Bulletproof glass decal
	DECAL_GARGSTOMP1,	// Gargantua stomp crack
	DECAL_SMALLSCORCH1,	// Small scorch mark
	DECAL_SMALLSCORCH2,	// Small scorch mark
	DECAL_SMALLSCORCH3,	// Small scorch mark
	DECAL_MOMMABIRTH,	// Big momma birth splatter
	DECAL_MOMMASPLAT,
};


typedef struct 
{
	char	*name;
	int		index;
} DLL_DECALLIST;

DLL_DECALLIST gDecals[];

DLL_DECALLIST gDecals[] = {
	{ "{shot1",	0 },		// DECAL_GUNSHOT1 
	{ "{shot2",	0 },		// DECAL_GUNSHOT2
	{ "{shot3",0 },			// DECAL_GUNSHOT3
	{ "{shot4",	0 },		// DECAL_GUNSHOT4
	{ "{shot5",	0 },		// DECAL_GUNSHOT5
	{ "{lambda01", 0 },		// DECAL_LAMBDA1
	{ "{lambda02", 0 },		// DECAL_LAMBDA2
	{ "{lambda03", 0 },		// DECAL_LAMBDA3
	{ "{lambda04", 0 },		// DECAL_LAMBDA4
	{ "{lambda05", 0 },		// DECAL_LAMBDA5
	{ "{lambda06", 0 },		// DECAL_LAMBDA6
	{ "{scorch1", 0 },		// DECAL_SCORCH1
	{ "{scorch2", 0 },		// DECAL_SCORCH2
	{ "{blood1", 0 },		// DECAL_BLOOD1
	{ "{blood2", 0 },		// DECAL_BLOOD2
	{ "{blood3", 0 },		// DECAL_BLOOD3
	{ "{blood4", 0 },		// DECAL_BLOOD4
	{ "{blood5", 0 },		// DECAL_BLOOD5
	{ "{blood6", 0 },		// DECAL_BLOOD6
	{ "{yblood1", 0 },		// DECAL_YBLOOD1
	{ "{yblood2", 0 },		// DECAL_YBLOOD2
	{ "{yblood3", 0 },		// DECAL_YBLOOD3
	{ "{yblood4", 0 },		// DECAL_YBLOOD4
	{ "{yblood5", 0 },		// DECAL_YBLOOD5
	{ "{yblood6", 0 },		// DECAL_YBLOOD6
	{ "{break1", 0 },		// DECAL_GLASSBREAK1
	{ "{break2", 0 },		// DECAL_GLASSBREAK2
	{ "{break3", 0 },		// DECAL_GLASSBREAK3
	{ "{bigshot1", 0 },		// DECAL_BIGSHOT1
	{ "{bigshot2", 0 },		// DECAL_BIGSHOT2
	{ "{bigshot3", 0 },		// DECAL_BIGSHOT3
	{ "{bigshot4", 0 },		// DECAL_BIGSHOT4
	{ "{bigshot5", 0 },		// DECAL_BIGSHOT5
	{ "{spit1", 0 },		// DECAL_SPIT1
	{ "{spit2", 0 },		// DECAL_SPIT2
	{ "{bproof1", 0 },		// DECAL_BPROOF1
	{ "{gargstomp", 0 },	// DECAL_GARGSTOMP1,	// Gargantua stomp crack
	{ "{smscorch1", 0 },	// DECAL_SMALLSCORCH1,	// Small scorch mark
	{ "{smscorch2", 0 },	// DECAL_SMALLSCORCH2,	// Small scorch mark
	{ "{smscorch3", 0 },	// DECAL_SMALLSCORCH3,	// Small scorch mark
	{ "{mommablob", 0 },	// DECAL_MOMMABIRTH		// BM Birth spray
	{ "{mommablob", 0 },	// DECAL_MOMMASPLAT		// BM Mortar spray?? need decal
};
#endif	// DECALS_H
