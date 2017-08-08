// Av's SMOKE GRENADE

/*
		MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pPlayer->pev );
			WRITE_BYTE( state );
			WRITE_BYTE( m_iId );
			WRITE_BYTE( m_iClip );
		MESSAGE_END();

	gmsgCurWeapon = REG_USER_MSG("CurWeapon", 3);

*/

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
#include "smoke.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;



static FILE *fp;

void SmokePrecache()
{
}

void SmokeShowWep( edict_t *pEntity )
{	
	
	// Show the smoke grenade in this guys hand

	entvars_t *pev = VARS( pEntity );
	
	pev->viewmodel = MAKE_STRING("models/v_satchel.mdl");
	pev->weaponanim = 2;

		
	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
		WRITE_BYTE( 2 );						// sequence number
		WRITE_BYTE( 0 );					// weaponmodel bodygroup.
	MESSAGE_END();

	
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pEntity );
		WRITE_BYTE( 1 );		// state
		WRITE_BYTE( 24 );	// id
		WRITE_BYTE( -1 );	// clip
	MESSAGE_END();

}
void SmokeThrow( edict_t *pEntity )
{
	// Throw the smoke grenade.
	MESSAGE_BEGIN( MSG_ONE, SVC_WEAPONANIM, NULL, pEntity );
		WRITE_BYTE( 3 );						// sequence number
		WRITE_BYTE( 0 );					// weaponmodel bodygroup.
	MESSAGE_END();

	ClientPrint( VARS(pEntity), HUD_PRINTTALK, "Throwing bomb...");


}








