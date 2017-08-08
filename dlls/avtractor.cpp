
#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "soundent.h"
#include "externmsg.h"

#include "studio.h"
#include "bot.h"
#include "avdll.h"
#include "sadll.h"
#include "avtractor.h"
#include "avdamage.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;
extern origentdata_t EntData[1025];

//extern int gmsgCurWeapon;

int g_sModelIndexBeam = 0;

extern bool AdminLoggedIn[33];
static FILE *fp;

// keep an array of our snowball charging time.
float tractimes[33];
int tracmode[33];

edict_t *tracindex[33];			// indexes to the beam targets
edict_t *tractarget[33];		// indexes to what we're pointing at

float tracdist[33];
Vector tracorig[33];			// Stores the offset of where we hit to where the center is
Vector tracpos[33];			// Stores the offset of where we hit to where the center is

bool tracfiremode[33];			// 0 = normal mode, 1 = alt mode. (reload toggles), 
float tracsoundtime[33];
float trachitplayer[33];

bool tracturn[33];


int tracdiststep[33];		// distance to move in steps

Vector tracmyangle[33];
Vector tracitangle[33];

Vector tracitorigpos[33][10];
edict_t *tracorigent[33][10];
//int tracnumlevels

int tracall[33];

#define EGON_SOUND_OFF			"weapons/egon_off1.wav"
#define EGON_SOUND_RUN			"weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP		"weapons/egon_windup2.wav"
#define EGON_SOUND_FLIP			"buttons/lever4.wav"

//int tracammo[33];

CBeam *m_pBeam;


void TracPrecache()
{
	PRECACHE_MODEL("models/v_egon.mdl");
	PRECACHE_MODEL("models/p_egon.mdl");

	PRECACHE_SOUND( EGON_SOUND_OFF );
	PRECACHE_SOUND( EGON_SOUND_RUN );
	PRECACHE_SOUND( EGON_SOUND_STARTUP );
	PRECACHE_SOUND( EGON_SOUND_FLIP );

	g_sModelIndexBeam = PRECACHE_MODEL( "sprites/xbeam1.spr" );
	
}

void TracShowWep( edict_t *pEntity )
{	
	
	// Show the smoke grenade in this guys hand

	entvars_t *pev = VARS( pEntity );
	int mynum = ENTINDEX(pEntity);
	if (!AdminLoggedIn[mynum]) return;

	pev->viewmodel = MAKE_STRING("models/v_egon.mdl");
	pev->weaponmodel = MAKE_STRING("models/p_egon.mdl");
	
	pev->weaponanim = 9;
		
	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
		WRITE_BYTE( 9 );						// sequence number
		WRITE_BYTE( 0 );					// weaponmodel bodygroup.
	MESSAGE_END();

	
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pEntity );
		WRITE_BYTE( 1 );		// state
		WRITE_BYTE( 27 );	// id
		WRITE_BYTE( -1 );	// clip
	MESSAGE_END();
	
	tractimes[mynum] = gpGlobals->time + 0.2;
	tracmode[mynum] = 10;

}


void TracReload( edict_t *pEntity )
{
	// Pressed reload.
	// toggle
	int mynum = ENTINDEX(pEntity);
	if (!AdminLoggedIn[mynum]) return;

	if (tractimes[mynum] > gpGlobals->time) return;


	if (tracfiremode[mynum] == 0)
	{
		tracfiremode[mynum] = 1;
		// enable alt fire
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( 2 );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
		tractimes[mynum] = gpGlobals->time + 1.5;
		tracsoundtime[mynum] = gpGlobals->time + 0.6;
		tracmode[mynum] = 11;

		
	}
	else if (tracfiremode[mynum] == 1)
	{
		tracfiremode[mynum] = 0;
		// disable alt fire
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( 4 );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
		
		tractimes[mynum] = gpGlobals->time + 0.6;
		tracsoundtime[mynum] = gpGlobals->time + 0.1;
		tracmode[mynum] = 11;
	}
}
void TracAttackStart( edict_t *pEntity )
{
	// Started holding down the ATTACK button.
	int mynum = ENTINDEX(pEntity);
	
	if (!AdminLoggedIn[mynum]) return;

	// make sure we can fire again
	if (tractimes[mynum] < gpGlobals->time)
	{
		entvars_t *pPev = VARS( pEntity );
		
		// send the weapon animation

		if (tracfiremode[mynum] == 0)
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 5 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 3 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
		}
		// track when we started holding down button
		
		tracmode[mynum] = 1;

		// set the "nextthink"
		tractimes[mynum] = gpGlobals->time;
		tracsoundtime[mynum] = gpGlobals->time + 3.8;

		// make soemthing to point at

		/*
		edict_t *pent = CREATE_ENTITY();
		entvars_t *pev = VARS( pent );
		
		pev->classname = MAKE_STRING("egontarget");
		pev->origin = pPev->origin;
		
		SET_ORIGIN(pent, pev->origin);
		SET_MODEL(pent, "models/shell.mdl");
		pent->v.rendermode = 2;
		pent->v.renderamt = 0;


		tracindex[mynum] = pent;
		*/

		/*
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMENTS );

			WRITE_SHORT ( mynum );
			WRITE_SHORT ( ENTINDEX(pent) );

			WRITE_SHORT ( g_sModelIndexBeam );
					
			WRITE_BYTE( 1 ); 
			WRITE_BYTE( 10 );
			WRITE_BYTE( 255 );  // life
			WRITE_BYTE( 75 );
			WRITE_BYTE( 0 ); 
			
			if (tracfiremode[mynum] == 0)
			{
				WRITE_BYTE( 255 ); 
				WRITE_BYTE( 0 ); 
				WRITE_BYTE( 255 ); 
			}
			else
			{
				WRITE_BYTE( 255 ); 
				WRITE_BYTE( 0 ); 
				WRITE_BYTE( 0 ); 
			}
			
			WRITE_BYTE( 100 );
			WRITE_BYTE( 0 );

		MESSAGE_END();
		*/
		
		tracmyangle[mynum] = pEntity->v.v_angle;


		// Create the beam effect here.
		if (tracfiremode[mynum] == 0)
		{
			// Make the swirley beam.
			tracindex[mynum] = TracMakeBeam( pEntity, 255, 0, 255, 1, 40, 20, 255);
			
			edict_t *temp;
			if (tracturn[mynum] == 0) temp = TracMakeBeam( pEntity, 255, 255, 0, 0, 55, 20, 100);
			if (tracturn[mynum] == 1) temp = TracMakeBeam( pEntity, 255, 255, 255, 0, 55, 20, 100);
			
			tracindex[mynum]->v.euser1 = temp;
		}
		else
		{
			// Make the swirley beam.
			tracindex[mynum] = TracMakeBeam( pEntity, 255, 0, 0, 1, 40, 20, 255);
			edict_t *temp;
			temp = TracMakeBeam( pEntity, 0, 255, 0, 0, 55, 20, 100);
			tracindex[mynum]->v.euser1 = temp;

		}

		// sound
		
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, EGON_SOUND_STARTUP, 0.9, ATTN_NORM, 0, 100 );
	}
}

edict_t *TracMakeBeam( edict_t *pEntity, int r, int g, int b, bool sine, int width, int noise, int brigh)
{
	
	edict_t *pent = CREATE_NAMED_ENTITY(MAKE_STRING("beam"));
	entvars_t *pev = VARS(pent);
			
	//char buf[80];

	KeyValueData	kvd;

	// Set the KEYVALUES here!
	kvd.fHandled = FALSE;
	kvd.szClassName = NULL;
	kvd.szKeyName = "classname";
	kvd.szValue = "beam";
	DispatchKeyValue( pent, &kvd );

	pev->aiment = pent;
	pev->owner = pEntity;

	DispatchSpawn(pent);
	
	// beaminit stuff

	pev->flags |= FL_CUSTOMENTITY;

	pev->renderamt = brigh;
	pev->frame = 0;
	pev->model = MAKE_STRING( "sprites/xbeam1.spr" );
	pev->modelindex = g_sModelIndexBeam;
	pev->scale = width;
	
	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;


	// set type
	pev->rendermode = (pev->rendermode & 0xF0) | (BEAM_ENTPOINT&0x0F);
	
	// start pos
	pev->origin = pEntity->v.origin - gpGlobals->v_up * 50;
	
	// end entity - the player
	pev->skin = (ENTINDEX(pEntity) & 0x0FFF) | ((pev->skin&0xF000)<<12); 
	pev->aiment = pEntity;

	// start and end attachments
	pev->sequence = (pev->sequence & 0x0FFF) | ((0&0xF)<<12);
	pev->skin = (pev->skin & 0x0FFF) | ((1&0xF)<<12);

	// relink the beam

	const Vector &startPos = pev->origin, &endPos = pEntity->v.origin;

	pev->mins.x = min( startPos.x, endPos.x );
	pev->mins.y = min( startPos.y, endPos.y );
	pev->mins.z = min( startPos.z, endPos.z );
	pev->maxs.x = max( startPos.x, endPos.x );
	pev->maxs.y = max( startPos.y, endPos.y );
	pev->maxs.z = max( startPos.z, endPos.z );
	pev->mins = pev->mins - pev->origin;
	pev->maxs = pev->maxs - pev->origin;

	UTIL_SetSize( VARS(pent), pev->mins, pev->maxs );
	UTIL_SetOrigin( VARS(pent), pev->origin );
	
	// setflags
	if (sine) pev->rendermode = (pev->rendermode & 0x0F) | (BEAM_FSINE&0xF0);
	pev->spawnflags |= SF_BEAM_TEMPORARY;
	
	// scroll rate
	pev->animtime = 50;

	//noise
	pev->body = noise;
	//colour
	pev->rendercolor.x = r;
	pev->rendercolor.y = g;
	pev->rendercolor.z = b;

	return pent;

}

void TracBeamSetColor( edict_t *pent, int r1, int g1, int b1, int r2, int g2, int b2)
{
	// set colour of the 2 beams
	
	pent->v.rendercolor.x = r1;
	pent->v.rendercolor.y = g1;
	pent->v.rendercolor.z = b1;

	pent->v.euser1->v.rendercolor.x = r2;
	pent->v.euser1->v.rendercolor.y = g2;
	pent->v.euser1->v.rendercolor.z = b2;


}

void TracUndo( edict_t *pEntity )
{
	int mynum = ENTINDEX(pEntity);
	if (!AdminLoggedIn[mynum]) return;

	ALERT(at_console, "TRYING TO UNDO: the pos is %f %f %f\n", tracitorigpos[mynum][0].x, tracitorigpos[mynum][0].y, tracitorigpos[mynum][0].z);

	if (tracorigent[mynum][0] != NULL)
	{
		if (!FNullEnt(tracorigent[mynum][0]))
		{

			SET_ORIGIN(tracorigent[mynum][0], tracitorigpos[mynum][0]);
			tracorigent[mynum][0] = NULL;
		}
	}

	//Shift

	for (int i = 0; i < 9; i++)
	{
		tracitorigpos[mynum][i] = tracitorigpos[mynum][i+1];
		tracorigent[mynum][i] = tracorigent[mynum][i+1];
	}






}


void TracTick( edict_t *pEntity )
{
	int mynum = ENTINDEX(pEntity);
	if (!AdminLoggedIn[mynum]) return;
	
	if (tracsoundtime[mynum] < gpGlobals->time && tracsoundtime[mynum] > 0)
	{
		if (tracmode[mynum] == 11)
		{
			EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, EGON_SOUND_FLIP, 0.9, ATTN_NORM, 0, 100 );
			tracsoundtime[mynum] = 0;
		}
		else
		{
			EMIT_SOUND_DYN2(pEntity, CHAN_STATIC, EGON_SOUND_RUN, 0.9, ATTN_NORM, 0, 100 );
			tracsoundtime[mynum] = 0;
		}
	}

	if (tractimes[mynum] < gpGlobals->time) 
	{

		entvars_t *pPev = VARS( pEntity );
		if (tracmode[mynum] == 1)
			{
			// We are supposed to be doing traces to see what we are aiming at.
			
			TraceResult	tr;	
			Vector anglesAim = pPev->v_angle + pPev->punchangle;
			UTIL_MakeVectors( anglesAim );

			anglesAim.x		 = -anglesAim.x;
			Vector vecSrc	 = GetGunPosition( pEntity );
			Vector vecEnd	 = vecSrc + gpGlobals->v_forward * 8192;

			// Update position
			
			UTIL_TraceLine ( vecSrc, vecEnd , dont_ignore_monsters, pEntity, &tr);

			edict_t *pent = tracindex[mynum];
			entvars_t *pev = VARS(pent);

			SET_ORIGIN(pent, tr.vecEndPos);
			SET_ORIGIN(pent->v.euser1, tr.vecEndPos);

			tractimes[mynum] = gpGlobals->time + 0.1;

			// Let's see if we hit an ent!
			
			bool hityes = 0;
			if (tr.pHit && !FStrEq((char*)STRING(tr.pHit->v.classname), "worldspawn") && AdminLoggedIn[mynum]) hityes = 1;
			
			if (tr.pHit && FStrEq((char*)STRING(tr.pHit->v.classname), "player")) 
			{
				hityes = 1;	
				if (AdminLoggedIn[ENTINDEX(tr.pHit)] && tracall[mynum] != 1) hityes = 0;
			}
			else
			{
				// only touch players
				if (tracfiremode[mynum] == 1) hityes = 0;
			}

			if (hityes)
			{
				// Latch onto this ent
				// Change beam colour
				
				if (tracfiremode[mynum] == 0)
				{
					
					if (tracturn[mynum] == 1 && EntData[ENTINDEX(tr.pHit)].hasorigin) 
					{

						TracBeamSetColor( pent, 255, 128, 64, 0, 0, 255 );
					}
					else
					{
						TracBeamSetColor( pent, 255, 255, 0, 0, 0, 255 );
					}
				}
				else
				{
					TracBeamSetColor( pent, 0, 255, 0, 0, 0, 255 );
				}


				// Store the latch target.
				
				tractarget[mynum] = tr.pHit;
				tracmode[mynum] = 2;
				tracitangle[mynum] = tr.pHit->v.angles;


				// Shift the ones in the current thing down

				int i = 9;

				while (i > 0)
				{
					ALERT(at_console, "SETTING %i TO %i: %f %f %f\n", i, i-1, tracitorigpos[mynum][i].x, tracitorigpos[mynum][i].y, tracitorigpos[mynum][i].z);

					tracitorigpos[mynum][i] = tracitorigpos[mynum][i-1];
					tracorigent[mynum][i] = tracorigent[mynum][i-1];

					i--;
				}


				tracitorigpos[mynum][0] = tr.pHit->v.origin;
				tracorigent[mynum][0] = tr.pHit;

/*
				if (tracfiremode[mynum] == 0)
				{
					tracdist[mynum] = (float)(tr.vecEndPos - pPev->origin).Length();
				}
				else
				{
					tracdist[mynum] = 8192;
				}
*/


				tracdist[mynum] = (float)(tr.vecEndPos - pPev->origin).Length();

				tracorig[mynum] = ((tr.pHit->v.absmin + tr.pHit->v.absmax) * 0.5) - tr.vecEndPos - (gpGlobals->v_forward * 1);

				trachitplayer[mynum] = 0;
				
				if (FStrEq((char*)STRING(tr.pHit->v.classname), "player"))
				{
					trachitplayer[mynum] = tr.pHit->v.maxspeed + 1;
					tr.pHit->v.maxspeed = 1;
					tr.pHit->v.movetype = MOVETYPE_NOCLIP;
					tr.pHit->v.speed = 0;
					tr.pHit->v.velocity = Vector(0,0,0);
				}
			}
		}
		else if (tracmode[mynum] == 2)
		{

			// mode 2 - locked on an ent, so, now move it around.
			// but keep it at the right radius

			//ALERT(at_console, "Trac Dist is %f", tracdist[mynum]);

			TraceResult	tr;	
			Vector anglesAim = pPev->v_angle + pPev->punchangle;
			UTIL_MakeVectors( anglesAim );

			anglesAim.x		 = -anglesAim.x;
			Vector vecSrc	 = GetGunPosition( pEntity ) ;
			Vector vecEnd	 = vecSrc + gpGlobals->v_forward * tracdist[mynum];

			// Update position
			
			UTIL_TraceLine ( vecSrc + gpGlobals->v_forward * 20, vecEnd , dont_ignore_monsters, tractarget[mynum], &tr);

			edict_t *pent = tracindex[mynum];
			edict_t *pentmove = tractarget[mynum];
		
			SET_ORIGIN(pent, tr.vecEndPos);
			SET_ORIGIN(pent->v.euser1, tr.vecEndPos);

			

			// add to the old co-ords
			Vector EndPos = pentmove->v.origin + (tr.vecEndPos - ((pentmove->v.absmin + pentmove->v.absmax) * 0.5)) + tracorig[mynum];

			tracpos[mynum] = pentmove->v.origin;

			//move it in steps
			if (tracdiststep[mynum] == 0) tracdiststep[mynum] = 1;
			
			//EndPos.x = (int)EndPos.x;
			//EndPos.y = (int)EndPos.y;
			//EndPos.z = (int)EndPos.z;
			
			Vector DistMoved = tracorig[mynum] - EndPos;

			// round or something
			DistMoved.x = ((int)DistMoved.x - ((int)DistMoved.x % tracdiststep[mynum]));
			DistMoved.y = ((int)DistMoved.y - ((int)DistMoved.y % tracdiststep[mynum]));
			DistMoved.z = ((int)DistMoved.z - ((int)DistMoved.z % tracdiststep[mynum]));

			EndPos = tracorig[mynum] - DistMoved;


			// turn it
			if (tracturn[mynum] && EntData[ENTINDEX(pentmove)].hasorigin)
			{

				float angturn = tracmyangle[mynum].y - pEntity->v.v_angle.y;
				pentmove->v.angles.y = tracitangle[mynum].y - angturn;
				
			}

			SET_ORIGIN(pentmove, EndPos);

			tractimes[mynum] = gpGlobals->time + 0.1;

		}
		else if (tracmode[mynum] == 10)
		{
			// if its alt fire mode, switch it over
			tracmode[mynum] = 0;

			if (tracfiremode[mynum] == 0)
			{
				MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
					WRITE_BYTE( 0 );						// sequence number
					WRITE_BYTE( 0 );					// weaponmodel bodygroup.
				MESSAGE_END();
			}
			else
			{
				MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
					WRITE_BYTE( 2 );						// sequence number
					WRITE_BYTE( 0 );					// weaponmodel bodygroup.
				MESSAGE_END();

				// dont allow firing for 1.5 seconds
				tractimes[mynum] = gpGlobals->time + 1.5;
			}
		}
	}
}

void TracAttackFly( edict_t *pEntity )
{
	int mynum = ENTINDEX(pEntity);
	if (!AdminLoggedIn[mynum]) return;

	if (tracmode[mynum] != 0 && tracmode[mynum] != 10)
	{
		// Let go of ATTACK
		
		tracmode[mynum] = 0;

		tracindex[mynum]->v.euser1->v.flags |= FL_KILLME;
		tracindex[mynum]->v.flags |= FL_KILLME;

		if (trachitplayer[mynum] > 0)
		{
			
			ALERT(at_console, "WOULD SET TO: %f\n", trachitplayer[mynum]);

			tractarget[mynum]->v.maxspeed = 500;
			tractarget[mynum]->v.speed = 500;
			tractarget[mynum]->v.movetype = MOVETYPE_WALK;
			
			//tractarget[mynum]->v.renderamt = 10;
			//tractarget[mynum]->v.rendercolor.x = 255;
			//tractarget[mynum]->v.rendercolor.y = 0;
			//tractarget[mynum]->v.rendercolor.z = 255;

			// Figure out how much VELOCITY to give them
			
			edict_t *pentmove = tractarget[mynum];
			Vector NewVel = (pentmove->v.origin) - tracpos[mynum];
			
			tractarget[mynum]->v.velocity = NewVel;
		}
		


		// aTTACk complete

		STOP_SOUND( pEntity, CHAN_STATIC, EGON_SOUND_RUN );
		EMIT_SOUND_DYN2(pEntity, CHAN_WEAPON, EGON_SOUND_OFF, 0.98, ATTN_NORM, 0, 100); 

		tracsoundtime[mynum] = 0;

		if (tracfiremode[mynum] == 0)
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 0 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
			tractimes[mynum] = gpGlobals->time + 0.3;
		}
		else
		{
			MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
				WRITE_BYTE( 4 );						// sequence number
				WRITE_BYTE( 0 );					// weaponmodel bodygroup.
			MESSAGE_END();
			tractimes[mynum] = gpGlobals->time + 0.6;
			tracmode[mynum] = 10;
		}
	}
}

void TracHandleCommand( edict_t *pEntity, const char *pcmd, const char *arg1, const char *arg2)
{
	int mynum = ENTINDEX(pEntity);

	if (FStrEq( pcmd, "tracdist"))
	{
		// move the distance closer/further
		
		int newdist = tracdist[mynum] + atoi(arg1);
		if (newdist < 50) newdist = 50;
		
		tracdist[mynum] = newdist;

	}
	else if (FStrEq( pcmd, "tracstep"))
	{
		// move the distance closer/further
		
		
		tracdiststep[mynum] = atoi(arg1);

	}

	else if (FStrEq( pcmd, "tracturn"))
	{
		// move the distance closer/further
		if (FStrEq(arg1, "on"))
		{
			tracturn[mynum] = 1;
		}
		
		if (FStrEq(arg1, "off"))
		{
			tracturn[mynum] = 0;
		}

	}

	else if (FStrEq( pcmd, "tracall"))
	{
		// move the distance closer/further
		if (FStrEq(arg1, "on"))
		{
			tracall[mynum] = 1;
		}
		
		if (FStrEq(arg1, "off"))
		{
			tracall[mynum] = 0;
		}

	}

}