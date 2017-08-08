
#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "soundent.h"
#include "engine.h"

#include "studio.h"
#include "bot.h"
#include "avdll.h"
#include "sadll.h"
#include "avdamage.h"
#include "avfox.h"
#include "studio.h"
#include "animation.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

extern bool AdminLoggedIn[33];

#define FOX_WALK			0
#define FOX_IDLE			1
#define FOX_BITE			2
#define FOX_EAT				3
#define FOX_DOWN_TO_EAT		4
#define FOX_UP_FROM_EAT		5
#define FOX_DOWN_TO_SLEEP	6
#define FOX_UP_FROM_SLEEP	7
#define FOX_ROLL_OVER		8
#define FOX_ROLL_BACK		9
#define FOX_SLEEP_CHEST		10
#define FOX_SLEEP_SIDE		11

int FoxModel = 0;

int MaxFrames[12];




struct avplaces_s 
{
	Vector AvLoc;
};

typedef avplaces_s avplaces_t;

avplaces_t AvPlaces[512];
avplaces_t FollowPlaces[512];

int CurrAvPlace[2] ;
int LookingPlace[2];


Vector LastPlaceMe;
int LastPlaceTicks = 0;

// What's the fox doing now:
int FoxMode = 0;

float lastaction;

float m_flFrameRate, m_flGroundSpeed, m_flNumFrames;
bool m_fSequenceFinished;


/*
FOX MODES:

0	- Not spawned.
1	- Standing around, with av.
2	- Following Av
3	- Following target, only to bite.
4	- Following target, to kill
5	- Sleeping on ground, on chest
6	- Lying down onto chest
7	- Getting up from chest-lie
8	- Rolling over onto side
9	- Sleeping on side
10	- Rolling back onto chest
11	- Lowering to eat
12	- Eating
13	- Rasing from eating position.
  */

bool AvWatching = 0;
bool AvControlling = 0;


edict_t *av;
edict_t *fox;
edict_t *following;

int FoxCreate( edict_t *pEntity )
{


	MaxFrames[0] = 256;
	MaxFrames[1] = 256;
	MaxFrames[2] = 256;
	MaxFrames[3] = 256;
	MaxFrames[4] = 200;
	MaxFrames[5] = 200;
	MaxFrames[6] = 200;
	MaxFrames[7] = 200;
	MaxFrames[8] = 200;
	MaxFrames[9] = 200;
	MaxFrames[10] = 256;
	MaxFrames[11] = 256;

	if (AdminLoggedIn[ENTINDEX(pEntity)] == 0) return 0;

	// Create the fox and set it up
	entvars_t *pPev = VARS( pEntity );

	if (pPev->iuser1 > 0) return 0;

	KeyValueData	kvd;
	
	edict_t *tEntity;
	tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("info_target"));

	entvars_t *pRunOnPev;
	pRunOnPev =  VARS(tEntity);
	
	char buf[80];
	sprintf( buf, "%s", "av_pet");
	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = buf;

	DispatchKeyValue( tEntity, &kvd );

	// place this inside the player

	pRunOnPev->origin = pEntity->v.origin;
	pRunOnPev->angles.y = (pPev->v_angle.y) + 180;
	
	SET_MODEL( ENT( pRunOnPev ) , "models/avnewfox3.mdl");
	UTIL_SetSize( pRunOnPev, Vector( -12, -12 ,-34), Vector(12, 12, 1));
	
	pRunOnPev->modelindex = FoxModel;
	pRunOnPev->takedamage = DAMAGE_NO;
	pRunOnPev->health = 40;
	pRunOnPev->euser4 = pEntity;
	
	pRunOnPev->solid = SOLID_BBOX;

	SET_ORIGIN( tEntity , pEntity->v.origin );
	DROP_TO_FLOOR( tEntity );
	
	// Spawn routine
	
	pRunOnPev->movetype = MOVETYPE_FLY;

	pRunOnPev->nextthink = gpGlobals->time;

	pRunOnPev->effects = 0;
	pRunOnPev->frame = 0;
	

	pRunOnPev->fuser1 = gpGlobals->time;
	pRunOnPev->fuser2 = (float) MODEL_FRAMES( pRunOnPev->modelindex ) - 1;
	pRunOnPev->framerate = 1;

	av = pEntity;

	pRunOnPev->sequence = 1;

	FoxSetSequence( FOX_WALK, pRunOnPev );

	FoxMode = 1;
	

	
	AddAvPlace( pEntity );
	LookingPlace[0] = CurrAvPlace[0];

	lastaction = gpGlobals->time;

	fox = tEntity;

	return 1;

}

void FoxPrecache()
{

	
	FoxModel = PRECACHE_MODEL("models/avnewfox3.mdl");
//	PRECACHE_SOUND("ambience/waterfall3.wav");

}

void AddAvPlace ( edict_t *pEntity )
{

	CurrAvPlace[0]++;
	if (CurrAvPlace[0] > 511) {
		CurrAvPlace[0] = 0;
	}

	if (CurrAvPlace[0] == LookingPlace[0]) { // we came around again and are now looking where we are now?
		LookingPlace[0]++;
	}

	// Memorize

	AvPlaces[CurrAvPlace[0]].AvLoc  = pEntity->v.origin;

	//ALERT(at_console, "Added Av Place: %i\n", CurrAvPlace );
	

}

void AddTrackerPlace ( edict_t *pEntity )
{

	CurrAvPlace[1]++;
	if (CurrAvPlace[1] > 511) {
		CurrAvPlace[1] = 0;
	}

	if (CurrAvPlace[1] == LookingPlace[1]) { // we came around again and are now looking where we are now?
		LookingPlace[1]++;
	}

	// Memorize

	FollowPlaces[CurrAvPlace[1]].AvLoc  = pEntity->v.origin;

	//ALERT(at_console, "Added Av Place: %i\n", CurrAvPlace );
}


void FoxThink ( edict_t *pent )
{
	
	// animate

	




	entvars_t *pev = VARS( pent );
		

	if (gpGlobals->time - pev->fuser3 > 2)
	{
		
		if (CVAR_GET_FLOAT("deathmatch"))
		{
			int hiswon = pfnGetPlayerWONId( av );
			/*
			if (AV_WONID_AV != hiswon)	
			{
				pev->flags |= FL_KILLME;
				AvControlling = 0;
				AvWatching = 0;
			}
			*/
		}
	}	

	int wholok = 0;

	if (!m_fSequenceFinished)
	{
		float flInterval = (gpGlobals->time - pev->animtime);		
		pev->frame += flInterval * m_flFrameRate * pev->framerate;
	}

	pev->animtime = gpGlobals->time;
	if (pev->frame < 0.0 || pev->frame >= MaxFrames[pev->sequence]) 
	{
		
		pev->frame = fmod( pev->frame, 256 );
		//ok, now see if we are switching modes

		if (FoxMode == 6)
		{
			// sleeping
			FoxMode = 5;
			FoxSetSequence( FOX_SLEEP_CHEST, pev );
		}
		else if (FoxMode == 7)
		{
			// ready to follow
			FoxMode = 2;
			FoxSetSequence( FOX_WALK, pev );
		}	
		else if (FoxMode == 8)
		{
			// sleep on side
			FoxMode = 9;
			FoxSetSequence( FOX_SLEEP_SIDE, pev );
		}	
		else if (FoxMode == 10)
		{
			// back onto chest
			FoxMode = 5;
			FoxSetSequence( FOX_SLEEP_CHEST, pev );
		}	
	}

//	ALERT( at_console, "Foxmode: %i, Idle for: %f, Frame %f, FrameRate %f, NumFrames %f\n", FoxMode, gpGlobals->time - lastaction, pev->frame, m_flFrameRate, m_flNumFrames);
	
		
	pev->nextthink		= gpGlobals->time + 0.1;

	// follow av

	Vector vecTarget;
	Vector GoingTo;

	// find the avplace we are going to.
	GoingTo = AvPlaces[LookingPlace[wholok]].AvLoc ;

	vecTarget = GoingTo - pev->origin;
	vecTarget = vecTarget.Normalize( );

	// distance from me
	
	float dist = (float)(pev->origin - GoingTo).Length();

	// move towards him

	int xdist = 20;
	if (LookingPlace[wholok] == CurrAvPlace[wholok]) xdist = 60;


	if (dist > xdist && !AvControlling) {

		if (FoxMode == 9)
		{
			// roll over
	
			FoxMode = 10;
			FoxSetSequence( FOX_ROLL_BACK, pev );

		}
		else if (FoxMode == 5 || FoxMode == 7 || FoxMode == 6)
		{
			// Get up
			FoxMode = 7;
			FoxSetSequence( FOX_UP_FROM_SLEEP, pev );
			lastaction = gpGlobals->time;
		}
		else if (FoxMode != 10)
		{

			// walk to av
			float flSpeed = pev->velocity.Length();
			pev->velocity = pev->velocity * 0.2 + vecTarget * (flSpeed * 0.8 + 400);
			if (pev->velocity.Length() > 400)
			{
				pev->velocity = pev->velocity.Normalize() * 400;
			}



			pev->framerate = 1;

			Vector newangles = UTIL_VecToAngles( pev->velocity );
			pev->angles.y = newangles.y;
			FoxMode = 2;
			FoxSetSequence( FOX_WALK, pev );
			lastaction = gpGlobals->time;
		}

		//pev->velocity = Vector(0,0,0);
	}
	else if (!AvControlling)
	{
		// reached the location

		if (LookingPlace[wholok] < CurrAvPlace[wholok] || LookingPlace[wholok] > CurrAvPlace[wholok])
		{
			// Still not near av. Go to the next place.
			LookingPlace[wholok]++;
			if (LookingPlace[wholok] > 511) LookingPlace[wholok] = 0;

			//ALERT(at_console, "Trying to find Av at: %i\n", LookingPlace );
		}
		else
		{
			
			if (FoxMode == 2) 
			{

				pev->velocity = Vector(0,0,0);
				FoxSetSequence( FOX_IDLE, pev );
				FoxMode = 1;
			}
		}
	}

	// see if we've moved more than 5 spots
	

	if (FoxMode == 2 && !AvControlling)
	{
		if ((LastPlaceMe - pev->origin).Length() < 4) LastPlaceTicks++;
		else LastPlaceTicks = 0;

		if (LastPlaceTicks > 20)
		{

			if (LookingPlace[wholok] < CurrAvPlace[wholok] || LookingPlace[wholok] > CurrAvPlace[wholok])
			{
				// Still not near av, and seem to be stuck. Skip this place.
				SET_ORIGIN ( ENT(pev), AvPlaces[LookingPlace[wholok]].AvLoc );
				
				LookingPlace[wholok]++;
				if (LookingPlace[wholok] > 511) LookingPlace[wholok] = 0;
				//ALERT(at_console, "Skipping and going on to next place: %i\n", LookingPlace );
			}
		}


		LastPlaceMe = pev->origin;
	
	}

	// PERSON TRACKING


	
	// See if av has moved more than 50 units.

	if ((AvPlaces[CurrAvPlace[wholok]].AvLoc - av->v.origin).Length() > 90 && !AvControlling)
	{
		// He has, memorize new location.
		AddAvPlace(av);
	}

	// See if its time to sleep

	if (gpGlobals->time - lastaction > 10 && !AvControlling)
	{
		// Fall asleep
		if (FoxMode == 1)
		{
			FoxMode = 6;
			FoxSetSequence( FOX_DOWN_TO_SLEEP, pev );
			lastaction = gpGlobals->time;
		}
	}
	
	if (gpGlobals->time - lastaction > 30 && !AvControlling)
	{
		// Fall asleep
		if (FoxMode == 5)
		{
			FoxMode = 8;
			FoxSetSequence( FOX_ROLL_OVER, pev );
			lastaction = gpGlobals->time;
		}
	}

	if (AvControlling)
	{
		lastaction = gpGlobals->time;
		
		pev->angles = av->v.v_angle;
		pev->vuser1 = av->v.v_angle;
		pev->nextthink = gpGlobals->time + 0.05;
		av->v.maxspeed = 1;
	}
	else
	{
		pev->angles.z = 0;
		pev->angles.x = 0;

	
	}


}

void FoxTouch(edict_t *pent, edict_t *pentTouch)
{
	// Make it so when I touch something, it's as if Av touched it.
	//ispatchTouch( av, pentTouch);
	(*other_gFunctionTable.pfnTouch)(av, pentTouch);

}

void FoxTouch2(edict_t *pent, edict_t *pentTouch)
{
	// Make it so when I touch something, it's as if Av touched it.
	//ispatchTouch( av, pentTouch);
	(*other_gFunctionTable.pfnTouch)(pent, av);

}

void GetSequenceInfo( void *pmodel, entvars_t *pev, float *pflFrameRate, float *pflGroundSpeed )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return;

	mstudioseqdesc_t	*pseqdesc;

	if (pev->sequence >= pstudiohdr->numseq)
	{
		*pflFrameRate = 0.0;
		*pflGroundSpeed = 0.0;
		return;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	if (pseqdesc->numframes > 1)
	{
		m_flNumFrames = pseqdesc->numframes;
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrt( pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0]+ pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1]+ pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}

void FoxSetSequence(int seq, entvars_t *pev)
{
	if (pev->sequence != seq)
	{

		pev->sequence = seq;
		pev->frame = 0;
		void *pmodel = GET_MODEL_PTR( ENT(pev) );
		GetSequenceInfo( pmodel, pev, &m_flFrameRate, &m_flGroundSpeed );

		pev->animtime = gpGlobals->time;
		pev->framerate = 1.0;
		m_fSequenceFinished = 0;
	}
}

void FoxSee( edict_t *pEntity )
{
	// See out of the fox
	if (pEntity != av) return;
	SET_VIEW( pEntity, fox );
	
	AvWatching = 1;
}


void FoxStopSee( edict_t *pEntity )
{
	if (pEntity != av) return;
	// See out of the fox
	SET_VIEW( pEntity, pEntity );
	AvWatching = 0;
}

void FoxControl( int button )
{

	// change our velocity based on what av is pressing
	
	entvars_t *pev = VARS( fox );
	UTIL_MakeVectors ( pev->angles );
	
	int vel = 400;
	//pev->velocity = Vector(0,0,0);


	bool moving = 0;

	if (button & IN_FORWARD)
	{
		pev->velocity = pev->velocity + gpGlobals->v_forward * vel;
		moving =1;
	}

	if (button & IN_MOVELEFT)
	{
		pev->velocity = pev->velocity - gpGlobals->v_right * vel;
		moving =1;
	}

	if (button & IN_MOVERIGHT)
	{
		pev->velocity = pev->velocity + gpGlobals->v_right * vel;
		moving =1;
	}

	if (button & IN_BACK)
	{
		pev->velocity = pev->velocity - gpGlobals->v_forward * vel;
		moving =1;
	}

	if (button & IN_JUMP)
	{
		pev->velocity = pev->velocity + gpGlobals->v_up * vel;
		moving =1;
	}
	
	if (button & IN_DUCK)
	{
		pev->velocity = pev->velocity - gpGlobals->v_up * vel;
		moving =1;
	}

	if (moving) {
		pev->velocity = pev->velocity.Normalize() * vel;
		FoxSetSequence ( FOX_WALK, VARS(fox));
		FoxMode = 2;
	}
	else
	{
		FoxSetSequence ( FOX_IDLE, VARS(fox));
		FoxMode = 1;
	}
}

void FoxStartControl( edict_t *pEntity )
{
	if (pEntity != av) return;
	FoxSee( pEntity );
	AvControlling = 1;
	fox->v.movetype = MOVETYPE_STEP;
	av->v.maxspeed = 1;

}
void FoxStopControl( edict_t *pEntity )
{
	if (pEntity != av) return;
	FoxStopSee( pEntity );
	fox->v.movetype = MOVETYPE_FLY;
	AvControlling = 0;
	av->v.maxspeed = 500;
}