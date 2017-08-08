// My Grapple!

#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "soundent.h"

#include "studio.h"
#include "bot.h"
#include "avdll.h"
#include "sadll.h"
#include "avgrapple.h"
#include "avdamage.h"

extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

extern bool AdminLoggedIn[33];

int m_iTrail = 0;
int m_iRope = 0;

static FILE *fp;


void GrapCreateBolt( edict_t *pEntity )
{
	// Create the bolt and return the edict_t to it

	if (pEntity->v.iuser1 > 0) return;
	if (pEntity->v.deadflag != DEAD_NO) return;

	if (AdminLoggedIn[ENTINDEX(pEntity)])
	{
	}
	else
	{
		if (CVAR_GET_FLOAT("sa_allowgrapple") != 1) return;
	}

	edict_t *pent = CREATE_ENTITY();
	
	entvars_t *pev = VARS( pent );
	entvars_t *pPev = VARS( pEntity );

	
	pev->classname = MAKE_STRING("grapple");

	Vector anglesAim = pPev->v_angle + pPev->punchangle;
	UTIL_MakeVectors( anglesAim );

	anglesAim.x		 = -anglesAim.x;
	Vector vecSrc	 = GetGunPosition( pEntity ) - gpGlobals->v_up * 5;
	Vector vecDir	 = gpGlobals->v_forward;

	pev->origin = vecSrc;
	pev->angles = anglesAim;
	pev->owner = pEntity;

	pev->takedamage = DAMAGE_NO;
	pev->health = 20;

	// The bolt doesn't move as fast under water...

	if (pPev->waterlevel == 3)
	{
		pev->velocity = vecDir * 2000;
		pev->speed = 2000;
	}
	else
	{
		pev->velocity = vecDir * 2000;
		pev->speed = 2000;
	}

	pPev->euser4 = pent;


	GrapSpawn( pent );


}
// forgot the name of the crowssbw arrow model... time to find out.

void GrapPrecache()
{

	PRECACHE_MODEL( "models/crossbow_bolt.mdl" );
	PRECACHE_SOUND("weapons/xbow_fire1.wav");
	PRECACHE_SOUND("weapons/xbow_hitbod1.wav");
	PRECACHE_SOUND("weapons/xbow_hitbod2.wav");
	PRECACHE_SOUND("weapons/xbow_hit1.wav");
	m_iTrail = PRECACHE_MODEL("sprites/smoke.spr");
	m_iRope = PRECACHE_MODEL("sprites/rope.spr");

}

void GrapSpawn( edict_t *pent )
{
	
	entvars_t *pev = VARS( pent );
	int red = 0, green = 0, blue = 0;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	
	// make the model invisible
	//pev->rendermode = kRenderTransTexture;
	//pev->renderamt = 0;

	//pev->effects |= EF_LIGHT;
	
	red = RANDOM_FLOAT(0,255);
	green = RANDOM_FLOAT(0,255);
	blue = RANDOM_FLOAT(0,255);

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT( ENTINDEX(pent) );	// entity
		WRITE_SHORT( m_iTrail );	// model
		WRITE_BYTE( 10 ); // life
		WRITE_BYTE( 2 );  // width
		WRITE_BYTE( red );   // r, g, b
		WRITE_BYTE( green );   // r, g, b
		WRITE_BYTE( blue );   // r, g, b
		WRITE_BYTE( 230 );	// brightness

	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	pev->gravity = 0.5;

//	SET_MODEL(pent, "models/shell.mdl");
	SET_MODEL(pent, "models/crossbow_bolt.mdl"); // so i can see it use a big model

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	//SetTouch( BoltTouch );	 // Call this function when something is hit.
	//SetThink( BubbleThink ); // The think function during flight.
	
	pev->iuser1 = 1; // bolt touch
	pev->iuser2 = 1; // bubblethink


		
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMENTS );

		WRITE_SHORT ( ENTINDEX(pent) );
		WRITE_SHORT ( ENTINDEX(pev->owner) );

		WRITE_SHORT ( m_iRope );
				
		WRITE_BYTE( 1 ); 
		WRITE_BYTE( 10 );
		WRITE_BYTE( 255 );  // life
		WRITE_BYTE( 25 );
		WRITE_BYTE( 0 ); 
		
		WRITE_BYTE( 255 ); 
		WRITE_BYTE( 255 ); 
		WRITE_BYTE( 255 ); 
					
		WRITE_BYTE( 255 );
		WRITE_BYTE( 0 );

	MESSAGE_END();

	pev->nextthink = gpGlobals->time + 0.2;

	

}

void GrapTouch ( edict_t *pent, edict_t *pentTouch )
{


	entvars_t *pev = VARS( pent );

	CBaseEntity *pOther = NULL;
	pOther = CBaseEntity::Instance( pentTouch );


	if (pev->iuser1 == 1) 
	{
		pev->iuser1 = 0;


		
		pev->nextthink = gpGlobals->time + 0.1;
		
		/*
		 * Play different sounds depending on what is hit.
		 */

		entvars_t *pevOwner = VARS( pev->owner );
		
		if (!(FStrEq((char *)STRING(pentTouch->v.classname), "worldspawn") || FStrEq((char *)STRING(pentTouch->v.classname), "func_wall"))) // av's NEW check-if-player function! ph34r!
		{


			if (FStrEq((char *)STRING(pentTouch->v.classname), "func_button") || FStrEq((char *)STRING(pentTouch->v.classname), "func_rot_button"))
			{
				// hit a button, activate it
				DispatchUse( pentTouch, pev->owner );
			}
			else if (FStrEq((char *)STRING(pentTouch->v.classname), "func_breakable"))
			{
				
				// hit a breakable, break it
				if (pentTouch->v.health > 10)
				{
					pentTouch->v.health = pentTouch->v.health - 10;
					EMIT_SOUND_DYN2(ENT(pev), CHAN_WEAPON, "weapons/xbow_hit1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 98 + RANDOM_LONG(0,7));
				}
				else
				{
					DispatchUse( pentTouch, pev->owner );
				}
			}
			else if (FStrEq((char *)STRING(pentTouch->v.classname), "player"))
			{
				if (pevOwner->team != pentTouch->v.team)
				{
				
					AvTakeDamage( VARS(pentTouch), pevOwner, VARS(pent), 5, DMG_CLUB);
				}
			
			}


			// we hit something that isnt worldspawn, get rid of it

			GrapRelease( pent );
			return;


			/*
			switch( RANDOM_LONG(0,1) )
			{
			case 0:
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hitbod1.wav", 1, ATTN_NORM); break;
			case 1:
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hitbod2.wav", 1, ATTN_NORM); break;
			}
			*/

			
			// we hit a player


		}


		/*
	//	else if (0) // something to check if the ent is moving here
	//	{
			// Hit something that moves, like a door. Release the hook.

	//		GrapRelease( pent );
/
//			return;
//		}

  */

		else
		{
			EMIT_SOUND_DYN2(ENT(pev), CHAN_WEAPON, "weapons/xbow_hit1.wav", RANDOM_FLOAT(0.95, 1.0), ATTN_NORM, 0, 98 + RANDOM_LONG(0,7));

			pev->iuser2 = 3; // DragPlayer

			/*
			 * If the grapple hit something that is moving we need to store a
			 * reference to that entity.
			 */
			//dragger = pOther->edict();
		}

		
		// Set up the position for the bolt.
		Vector vecDir = pev->velocity.Normalize( );
		UTIL_SetOrigin( pev, pev->origin - vecDir * 12 );
		
		/*
		 * The distance between the bolt and the dragger so we can calculate if
		 * the dragger has moved between "thinks".
		 */
		
		pev->angles = UTIL_VecToAngles( vecDir );
		pev->solid = SOLID_NOT;
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = Vector( 0, 0, 0 );
		pev->avelocity.z = 0;
		pev->angles.z = RANDOM_LONG(0,360);
		
		// No sparks under water.
		if (UTIL_PointContents(pev->origin) != CONTENTS_WATER)
		{
//			UTIL_Sparks( pev->origin );
		}

		// Start the beam rope so it looks like a rope is pulling the player

		//#define TE_KILLBEAM			99		// kill all beams attached to entity
		// short (entity)

	}
}


void GrapThink( edict_t *pent )
{

	entvars_t *pev = VARS( pent );

	// Make sure the player is still valid
		
	if (pent->v.owner->v.deadflag != DEAD_NO || pent->v.owner->v.iuser3 != 0)
	{
		// remove the grapple
		GrapRelease( pent );
		return;
	}



	if (pev->iuser2 == 1) // BubbleThink
	{

		pev->nextthink = gpGlobals->time + 0.1;

		// Spawn bubbles here if underwater

	}
	else if (pev->iuser2 == 2) // PlayerStoppedThink - meaning player has reached end but yet is still holding the button
	{

		entvars_t *pevOwner = VARS( pev->owner );
		//CBasePlayer *plr = GetClassPtr((CBasePlayer *)pevOwner);

		Vector vec_temp;

		//plr->m_afPhysicsFlags |= PFLAG_ONTRAIN; // Turn on this flag so the game thinks you're on a train
												// and it can check the movement keys while hanging.

		// Put something in here that prevents the player from using movement keys


		
		//plr->IsHanging = true;

		pevOwner->gravity = -1;// Turn off gravity

		pevOwner->movetype = MOVETYPE_NONE;// Only way to make player 100% stable

		pevOwner->origin = pev->vuser1;// Hold this origin

		pevOwner->velocity = Vector ( 0, 0, 0 );// Drop all velocity
		pevOwner->speed = 0;
		pev->iuser3 = pevOwner->maxspeed;
		pevOwner->maxspeed = 0;



		//UTIL_SetOrigin( pevOwner, endOrigin );

		//pev->effects &= ~EF_LIGHT;// Turn off flashing light once hanging

		pev->nextthink = gpGlobals->time;		// i think this turns off thinking


	}
	else if (pev->iuser2 == 3) // DragPlayer - pulling player along.
	{

		pev->nextthink = gpGlobals->time + 0.1;
		if ( pev->owner )
		{
			entvars_t *pevOwner = VARS( pev->owner );
			//CBasePlayer *plr = GetClassPtr((CBasePlayer *)pevOwner);
			
			//pevOwner->gravity = -1;
		
			Vector prevOrigin = pev->origin;
			
			Vector dist = (pev->origin - pevOwner->origin);

			if(dist.Length() > 50)
			{

				Vector addamt = dist.Normalize() * 500;
							
				//if (pevOwner->flags & FL_ONGROUND) addamt.z = 0;
				
				pevOwner->velocity = (pevOwner->velocity * .90) + addamt;

				if (pevOwner->velocity.Length() > 800) {
					pevOwner->velocity = pevOwner->velocity / (pevOwner->velocity.Length() / 800);
				}
				pevOwner->speed = 500;
			}
			else
			{
				pevOwner->velocity = Vector ( 0, 0, 0 );
				pevOwner->speed = 0;
				pev->vuser1 = pevOwner->origin;
				pev->iuser2 = 2; // playerstoppedthink

			}
		}
	}
	else if (pev->iuser2 == 4) // HangPlayer - Let the player hang from his rope here.
	{

		pev->nextthink = gpGlobals->time + 0.1;
		if ( pev->owner )
		{
			entvars_t *pevOwner = VARS( pev->owner );
			//CBasePlayer *plr = GetClassPtr((CBasePlayer *)pevOwner);
			
			//pevOwner->gravity = -1;
		
			Vector prevOrigin = pev->origin;
			
			Vector dist = (pev->origin - pevOwner->origin);

			// hang the player here

			if(dist.Length() > pev->iuser4)
			{
				pevOwner->velocity = dist.Normalize() * 500;
				pevOwner->speed = 500;
			}
			else
			{
				pevOwner->velocity = Vector ( 0, 0, 0 );
				pevOwner->speed = 0;
			}
		}
	}
	
}

void GrapHang ( edict_t *pent )
{
	// Let the player HANG here!
	entvars_t *pevOwner = VARS( pent->v.owner );
	Vector dist = (pent->v.origin - pevOwner->origin);

	pent->v.iuser4 = (int)dist.Length();
	
	pent->v.iuser2 = 4;
}

void GrapRelease( edict_t *pent )
{

	
	entvars_t *pev = NULL;
	pev = VARS( pent->v.owner );

	//CBasePlayer *plr = GetClassPtr((CBasePlayer *)pev);

	//	plr->m_afPhysicsFlags &= ~PFLAG_ONTRAIN;

	//Turn off movement key bullshit here

	
	pev->gravity = 1;
	pev->movetype = MOVETYPE_WALK;
	pev->maxspeed = pent->v.iuser3;


	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_KILLBEAM );
		WRITE_SHORT ( ENTINDEX(pent->v.owner) );

	MESSAGE_END();	

	//SetBits( pent->v.flags, FL_KILLME );

	REMOVE_ENTITY( pent );
	pev->euser4 = NULL;

}


