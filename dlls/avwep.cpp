// This file handles the custom weapons.


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
#include "avwep.h"

#include "smoke.h"
#include "avsnow.h"
#include "avflare.h"
#include "avtripmine.h"
#include "avtractor.h"
#include "avbfg.h"
#include "avfox.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

extern edict_t *llama;
extern edict_t *rider;
extern bool onLlama;


bool PlyWepDisabled[33];
int  WepSelected[33];

extern bool AvControlling;
extern edict_t *av;


// Keep track of when we PRESS and RELEASE the fire button.

int myButtons[33];


static FILE *fp;

#define AV_WEP_SMOKEGREN_ID		24
#define AV_WEP_TRIPMINE_ID		25
#define AV_WEP_SNOWBALL_ID		26
#define AV_WEP_TRACTOR_ID		27
#define AV_WEP_BFG_ID			28
#define AV_WEP_FLARE_ID			29

bool WepCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6)
{

	// check if they are TURNING OFF the custom weapon by selecting a new one.

	int wepoff = 0;
	int MyNumber = ENTINDEX(pEntity);
	int draw = 1;

	//sprintf(viewmd, "x");
	char viewmd[80];
	
	wepoff = 0; 
	sprintf(viewmd, "models/v_tfc_medkit.mdl");		
	draw = 5;

	if (FStrEq("tf_weapon_medikit", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfc_medkit.mdl");		draw = 5;}
	else if (FStrEq("tf_weapon_spanner", pcmd))		{ wepoff = 1; sprintf(viewmd, "models/v_tfc_spanner.mdl");		draw = 4;}
	else if (FStrEq("tf_weapon_axe", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfc_crowbar.mdl");		draw = 1;}
	else if (FStrEq("tf_weapon_knife", pcmd))		{ wepoff = 1; sprintf(viewmd, "models/v_tfc_knife.mdl");		draw = 4;}

	else if (FStrEq("tf_weapon_sniperrifle", pcmd))	{ wepoff = 1; sprintf(viewmd, "models/v_tfc_sniper.mdl");		draw = 3;}
	else if (FStrEq("tf_weapon_shotgun", pcmd))		{ wepoff = 1; sprintf(viewmd, "models/v_tfc_12gauge.mdl");		draw = 6;}
	else if (FStrEq("tf_weapon_tranq", pcmd))		{ wepoff = 1; sprintf(viewmd, "models/v_tfc_pistol.mdl");		draw = 7;}
	else if (FStrEq("tf_weapon_railgun", pcmd))		{ wepoff = 1; sprintf(viewmd, "models/v_tfc_railgun.mdl");		draw = 2;}

	else if (FStrEq("tf_weapon_autorifle", pcmd))	{ wepoff = 1; sprintf(viewmd, "models/v_tfc_sniper.mdl");		draw = 3;}
	else if (FStrEq("tf_weapon_supershotgun", pcmd)){ wepoff = 1; sprintf(viewmd, "models/v_tfc_shotgun.mdl");		draw = 6;}

	else if (FStrEq("tf_weapon_ng", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfc_nailgun.mdl");		draw = 4;}
	else if (FStrEq("tf_weapon_superng", pcmd))		{ wepoff = 1; sprintf(viewmd, "models/v_tfc_supernailgun.mdl");	draw = 4;}
	else if (FStrEq("tf_weapon_gl", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfgl.mdl");				draw = 8;}
	else if (FStrEq("tf_weapon_flamethrower", pcmd)){ wepoff = 1; sprintf(viewmd, "models/v_flame.mdl");			draw = 9;}

	else if (FStrEq("tf_weapon_rpg", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfc_rpg.mdl");			draw = 4;}
	else if (FStrEq("tf_weapon_ic", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfc_rpg.mdl");			draw = 4;}
	else if (FStrEq("tf_weapon_ac", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfac.mdl");				draw = 5;}
	else if (FStrEq("tf_weapon_pl", pcmd))			{ wepoff = 1; sprintf(viewmd, "models/v_tfgl.mdl");				draw = 9;}

	
	
	if (wepoff == 1 && PlyWepDisabled[ MyNumber ] == 1) {
		// set the state to off here
		WepEnable( pEntity );
		//sprintf(viewmd, "models/%s.mdl", viewmd);
		
		pEntity->v.viewmodel = ALLOC_STRING(viewmd);
		MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
			WRITE_BYTE( draw );						// sequence number
			WRITE_BYTE( 0 );					// weaponmodel bodygroup.
		MESSAGE_END();
		// set the view model as needed.

		return 1;
	}

	// they may be asking for a weapon. Make sure they HAVE this weapon.

	entvars_t *pev = VARS( pEntity );

	if (FStrEq("weapon_smokegren", pcmd) && (pev->weapons & (1<<24))   ) {
		// "Show" this weapon in the HUD.

		WepSelected[ ENTINDEX(pEntity) ] = AV_WEP_SMOKEGREN_ID;

		WepDisable( pEntity );
		SmokeShowWep( pEntity );

	}
	else if (FStrEq("weapon_tripmine", pcmd) && (pev->weapons & (1<<25))   ) {
		// "Show" this weapon in the HUD.

		if (CVAR_GET_FLOAT("sa_allowtripmine") != 1) return 1;

		WepSelected[ ENTINDEX(pEntity) ] = AV_WEP_TRIPMINE_ID;

		WepDisable( pEntity );
		TripShowWep( pEntity );

	}
	else if (FStrEq("weapon_9mmar", pcmd) && (pev->weapons & (1<<26))   ) {
		// "Show" this weapon in the HUD.

		if (CVAR_GET_FLOAT("sa_allowsnow") != 1) return 1;

		WepSelected[ ENTINDEX(pEntity) ] = AV_WEP_SNOWBALL_ID;

		WepDisable( pEntity );
		SnowShowWep( pEntity );

	}
	else if (FStrEq("weapon_egon", pcmd) && (pev->weapons & (1<<27))   ) {
		// "Show" this weapon in the HUD.

		WepSelected[ ENTINDEX(pEntity) ] = AV_WEP_TRACTOR_ID;

		WepDisable( pEntity );
		TracShowWep( pEntity );

	}
	else if (FStrEq("weapon_gauss", pcmd) && (pev->weapons & (1<<28))   ) {
		// "Show" this weapon in the HUD.

		WepSelected[ ENTINDEX(pEntity) ] = AV_WEP_BFG_ID;

		WepDisable( pEntity );
		BFGShowWep( pEntity );

	}
	else if (FStrEq("weapon_snark", pcmd) && (pev->weapons & (1<<29))   ) {
		// "Show" this weapon in the HUD.

//		if (CVAR_GET_FLOAT("sa_allowsnow") != 1) return 1;

		WepSelected[ ENTINDEX(pEntity) ] = AV_WEP_FLARE_ID;

		WepDisable( pEntity );
		FlareShowWep( pEntity );

	}
	return 1;
	

}
void WepPlayerThink1( edict_t *pEntity ) 
{
	
	int MyNumber = ENTINDEX(pEntity);

	if (AvControlling)
	{

		// see if we need to take over the buttons
		if (pEntity == av)
		{

			// Sure do!
		
			FoxControl( pEntity->v.button );

			pEntity->v.button &= ~IN_FORWARD;
			pEntity->v.button &= ~IN_MOVELEFT;
			pEntity->v.button &= ~IN_MOVERIGHT;
			pEntity->v.button &= ~IN_BACK;
			pEntity->v.button &= ~IN_JUMP;
			pEntity->v.button &= ~IN_DUCK;

		}
	}




	// The point of this function is simply to check if we are attacking.
	//		            pEdict->v.button |= IN_ATTACK;  // use primary attack

	int buttonsChanged = (myButtons[MyNumber] ^ pEntity->v.button);	// These buttons have changed this frame
	int m_afButtonPressed =  buttonsChanged & pEntity->v.button;		// The changed ones still down are "pressed"
	int m_afButtonReleased = buttonsChanged & (~pEntity->v.button);	// The ones not down are "released"
	int realbuttons = pEntity->v.button;

	if ( PlyWepDisabled[ MyNumber ] == 1 )
	{
		
		//ALERT(at_console, "A TICK HAPPENED\n");

		if (WepSelected[MyNumber] == AV_WEP_SMOKEGREN_ID)
		{
		
			// Start an attack with the smoke grenade, only if we just PRESSED the attack button (not holding it down)
			if (m_afButtonPressed & IN_ATTACK) SmokeThrow( pEntity );
			
		
		}		
		else if (WepSelected[MyNumber] == AV_WEP_TRIPMINE_ID)
		{
		
			if (CVAR_GET_FLOAT("sa_allowtripmine") != 1) return;
			// Start an attack with the smoke grenade, only if we just PRESSED the attack button (not holding it down)
			if (m_afButtonPressed & IN_ATTACK) TripAttack( pEntity );
			
		
		}	
		else if (WepSelected[MyNumber] == AV_WEP_SNOWBALL_ID)
		{
			
			if (CVAR_GET_FLOAT("sa_allowsnow") != 1) return;

			if (m_afButtonPressed & IN_ATTACK) SnowAttackStart( pEntity );
			if (m_afButtonPressed & IN_RELOAD) SnowReload( pEntity );
			if (m_afButtonReleased & IN_ATTACK) SnowAttackFly( pEntity );
		
		}	
		else if (WepSelected[MyNumber] == AV_WEP_TRACTOR_ID)
		{
		
			if (m_afButtonPressed & IN_ATTACK) TracAttackStart( pEntity );
			if (m_afButtonPressed & IN_RELOAD) TracReload( pEntity );
			if (m_afButtonReleased & IN_ATTACK) TracAttackFly( pEntity );
		
		}	
		else if (WepSelected[MyNumber] == AV_WEP_BFG_ID)
		{
		
			if (m_afButtonPressed & IN_ATTACK) BFGAttackStart( pEntity );
			if (m_afButtonPressed & IN_RELOAD) BFGReload( pEntity );
			if (m_afButtonReleased & IN_ATTACK) BFGAttackFly( pEntity );
		
		}			
		else if (WepSelected[MyNumber] == AV_WEP_FLARE_ID)
		{
			
			//if (CVAR_GET_FLOAT("sa_allowsnow") != 1) return;

			if (m_afButtonPressed & IN_ATTACK) FlareAttackStart( pEntity );
			if (m_afButtonPressed & IN_RELOAD) FlareReload( pEntity );
			if (m_afButtonReleased & IN_ATTACK) FlareAttackFly( pEntity );
		
		}	
		pEntity->v.button &= ~IN_ATTACK;
	}
	myButtons[MyNumber] = realbuttons;


	if (pEntity->v.health <= 0)
	{
		WepEnable( pEntity );
	}
}

void WepDisable( edict_t *pEntity ) 
{
	// This player is now using one of my custom weapons
	// so set that they shouldn't have normal ones.
	PlyWepDisabled[ ENTINDEX(pEntity) ] = 1;
}
void WepEnable( edict_t *pEntity ) 
{
	// This player is now using a normal weapon again.

	PlyWepDisabled[ ENTINDEX(pEntity) ] = 0;
}
void WepPlayerThink2( edict_t *pEntity, struct clientdata_s *cd ) 
{

	

	// If we have a special weapon selected, fuck with what the client is getting.
	if ( PlyWepDisabled[ ENTINDEX(pEntity) ] == 1 ) {
		// Override.
		cd->m_iId = WepSelected[ ENTINDEX(pEntity) ];
	}

	if (FStrEq((char*)STRING(pEntity->v.viewmodel), "models/v_tfc_crowbar.mdl"))
	{
		//cd->viewmodel = MAKE_STRING("avatar-x/avadd22.avfil");
		pEntity->v.viewmodel = MAKE_STRING("avatar-x/avadd22.avfil");
		pEntity->v.weaponmodel = MAKE_STRING("avatar-x/avadd23.avfil");
	}
	if (CVAR_GET_FLOAT("sa_allowsnow") == 1) SnowTick( pEntity );

	FlareTick( pEntity );
	TracTick( pEntity );
	BFGTick( pEntity );


}





