#ifndef _GRAPPLE_H
#define _GRAPPLE_H

#include "vector.h"	// Added by ClassView

class CBasePlayer;

class CGrappleBolt : public CBaseEntity
{
	void Spawn( void );
	void Precache( void );
	int  Classify ( void );
	void EXPORT BubbleThink( void );
	void EXPORT BoltTouch( CBaseEntity *pOther );
	void EXPORT DragPlayer(void);
	void EXPORT ReleaseHook(void);
	void EXPORT PlayerStoppedThink(void);

	int m_iTrail;
	Vector draggerDist;
	Vector endOrigin;
	bool dragFinished;
	bool dragWasFinished;
	bool draggerMoving;
	edict_t *dragger;

public:
	static CGrappleBolt *BoltCreate( void );
	void Init(CBasePlayer *plr);
	virtual void Killed( entvars_t *pevAttacker, int iGib );
	bool dragging;
};

#endif