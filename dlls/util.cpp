/***
*
*  Copyright (c) 1999, Valve LLC. All rights reserved.
*
*  This product contains software technology licensed from Id 
*  Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*  All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// util.cpp
//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "externmsg.h"
#include "decals.h"
#include "studio.h"

#ifdef SZ_DLL

	#include "avflare.h"
	extern int	g_sModelIndexBloodSpray;
	extern int	g_sModelIndexBloodDrop;

#endif


#include "bot.h"

extern int mod_id;


Vector UTIL_VecToAngles( const Vector &vec )
{
   float rgflVecOut[3];
   VEC_TO_ANGLES(vec, rgflVecOut);
   return Vector(rgflVecOut);
}

static unsigned short FixedUnsigned16( float value, float scale )
{
	int output;

	output = value * scale;
	if ( output < 0 )
		output = 0;
	if ( output > 0xFFFF )
		output = 0xFFFF;

	return (unsigned short)output;
}

static short FixedSigned16( float value, float scale )
{
	int output;

	output = value * scale;

	if ( output > 32767 )
		output = 32767;

	if ( output < -32768 )
		output = -32768;

	return (short)output;
}
void UTIL_HudMessage( CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage )
{
	if ( !pEntity || !pEntity->IsNetClient() )
		return;

	MESSAGE_BEGIN( MSG_ONE, SVC_TEMPENTITY, NULL, pEntity->edict() );
		WRITE_BYTE( TE_TEXTMESSAGE );
		WRITE_BYTE( textparms.channel & 0xFF );

		WRITE_SHORT( FixedSigned16( textparms.x, 1<<13 ) );
		WRITE_SHORT( FixedSigned16( textparms.y, 1<<13 ) );
		WRITE_BYTE( textparms.effect );

		WRITE_BYTE( textparms.r1 );
		WRITE_BYTE( textparms.g1 );
		WRITE_BYTE( textparms.b1 );
		WRITE_BYTE( textparms.a1 );

		WRITE_BYTE( textparms.r2 );
		WRITE_BYTE( textparms.g2 );
		WRITE_BYTE( textparms.b2 );
		WRITE_BYTE( textparms.a2 );

		WRITE_SHORT( FixedUnsigned16( textparms.fadeinTime, 1<<8 ) );
		WRITE_SHORT( FixedUnsigned16( textparms.fadeoutTime, 1<<8 ) );
		WRITE_SHORT( FixedUnsigned16( textparms.holdTime, 1<<8 ) );

		if ( textparms.effect == 2 )
			WRITE_SHORT( FixedUnsigned16( textparms.fxTime, 1<<8 ) );
		
		if ( strlen( pMessage ) < 512 )
		{
			WRITE_STRING( pMessage );
		}
		else
		{
			char tmp[512];
			strncpy( tmp, pMessage, 511 );
			tmp[511] = 0;
			WRITE_STRING( tmp );
		}
	MESSAGE_END();
}

void UTIL_HudMessageAll( const hudtextparms_t &textparms, const char *pMessage )
{
	int			i;

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
		if ( pPlayer )
			UTIL_HudMessage( pPlayer, textparms, pMessage );
	}
}


void UTIL_HudMessageOne( edict_t *pEntity, const char *pMessage )
{

	//ClientPrint( VARS(pEntity), HUD_PRINTCENTER, pMessage);

	
	CBaseEntity *pPlayer = CBaseEntity::Instance( pEntity );
	if ( pPlayer )
	{

		hudtextparms_t m_textParms;

		m_textParms.r1 = 255;
		m_textParms.g1 = 150;
		m_textParms.b1 = 150;
		m_textParms.a1 = 0;

		m_textParms.r2 = 0;
		m_textParms.g2 = 0;
		m_textParms.b2 = 255;
		m_textParms.a2 = 0;

		m_textParms.fadeinTime = 0.001;
		m_textParms.fadeoutTime = 0.001;
		m_textParms.holdTime = 2;
		m_textParms.fxTime = 0.001;
		m_textParms.x = -1;
		m_textParms.y = 0.60;

		m_textParms.effect= 2;
		m_textParms.channel= 1;

		UTIL_HudMessage( pPlayer, m_textParms, pMessage );

	}

}




void UTIL_HudMessageColAll( const char *pMessage )
{

	//ClientPrint( VARS(pEntity), HUD_PRINTCENTER, pMessage);

	int i;
	edict_t *pPlayerEdict;
	
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		pPlayerEdict = INDEXENT( i );
		if ( pPlayerEdict && !pPlayerEdict->free )
		{

			if (FStrEq((char*)STRING(pPlayerEdict->v.classname), "player")) // make sure its actually a player.
			{



				CBaseEntity *pPlayer = CBaseEntity::Instance( pPlayerEdict );
				if ( pPlayer )
				{

					hudtextparms_t m_textParms;

					m_textParms.r1 = 255;
					m_textParms.g1 = 255;
					m_textParms.b1 = 0;
					m_textParms.a1 = 0;

					m_textParms.r2 = 255;
					m_textParms.g2 = 0;
					m_textParms.b2 = 255;
					m_textParms.a2 = 0;

					m_textParms.fadeinTime = 0.01;
					m_textParms.fadeoutTime = 0.2;
					m_textParms.holdTime = 3;
					m_textParms.fxTime = 0.3;
					m_textParms.x = -1;
					m_textParms.y = 0.30;

					m_textParms.effect= 2;
					m_textParms.channel= 1;

					UTIL_HudMessage( pPlayer, m_textParms, pMessage );

				}
			}
		}
	}
}

// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass?0x100:0), pentIgnore, ptr );
}


void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
   TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), pentIgnore, ptr );
}


void UTIL_MakeVectors( const Vector &vecAngles )
{
   MAKE_VECTORS( vecAngles );
}


edict_t *UTIL_FindEntityInSphere( edict_t *pentStart, const Vector &vecCenter, float flRadius )
{
   edict_t  *pentEntity;
   pentEntity = FIND_ENTITY_IN_SPHERE( pentStart, vecCenter, flRadius);

   if (!FNullEnt(pentEntity))
      return pentEntity;

   return NULL;
}


CBaseEntity *UTIL_FindEntityInSphere( CBaseEntity *pStartEntity, const Vector &vecCenter, float flRadius )
{
	edict_t	*pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_IN_SPHERE( pentEntity, vecCenter, flRadius);

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);
	return NULL;
}


CBaseEntity	*UTIL_PlayerByIndex( int playerIndex )
{
	CBaseEntity *pPlayer = NULL;

	if ( playerIndex > 0 && playerIndex <= gpGlobals->maxClients )
	{
		edict_t *pPlayerEdict = INDEXENT( playerIndex );
		if ( pPlayerEdict && !pPlayerEdict->free )
		{
			pPlayer = CBaseEntity::Instance( pPlayerEdict );
		}
	}
	
	return pPlayer;
}

int UTIL_PointContents( const Vector &vec )
{
   return POINT_CONTENTS(vec);
}


void UTIL_SetSize( entvars_t *pev, const Vector &vecMin, const Vector &vecMax )
{
   SET_SIZE( ENT(pev), vecMin, vecMax );
}


void UTIL_SetOrigin( entvars_t *pev, const Vector &vecOrigin )
{
   SET_ORIGIN(ENT(pev), vecOrigin );
}


void ClientPrint( entvars_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 )
{

	if (!FStrEq((char*)STRING(client->classname), "player")) return;


   MESSAGE_BEGIN( MSG_ONE, gmsgTextMsg, NULL, client );

   WRITE_BYTE( msg_dest );
   WRITE_STRING( msg_name );

   if ( param1 )
      WRITE_STRING( param1 );
   if ( param2 )
      WRITE_STRING( param2 );
   if ( param3 )
      WRITE_STRING( param3 );
   if ( param4 )
      WRITE_STRING( param4 );

   MESSAGE_END();
}

void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 )
{
   //if (!FStrEq((char*)STRING(client->classname), "player")) return;


   MESSAGE_BEGIN( MSG_ALL, gmsgTextMsg );
      WRITE_BYTE( msg_dest );
      WRITE_STRING( msg_name );

      if ( param1 )
         WRITE_STRING( param1 );
      if ( param2 )
         WRITE_STRING( param2 );
      if ( param3 )
         WRITE_STRING( param3 );
      if ( param4 )
         WRITE_STRING( param4 );

   MESSAGE_END();
}

void UTIL_SayText( const char *pText, edict_t *pEdict )
{

   MESSAGE_BEGIN( MSG_ONE, gmsgSayText, NULL, pEdict );

   WRITE_BYTE( ENTINDEX(pEdict) );
   WRITE_STRING( pText );

   MESSAGE_END();
}

#ifdef	DEBUG
edict_t *DBG_EntOfVars( const entvars_t *pev )
{
	if (pev->pContainingEntity != NULL)
		return pev->pContainingEntity;
	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine");
	edict_t* pent = (*g_engfuncs.pfnFindEntityByVars)((entvars_t*)pev);
	if (pent == NULL)
		ALERT(at_console, "DAMN!  Even the engine couldn't FindEntityByVars!");
	((entvars_t *)pev)->pContainingEntity = pent;
	return pent;
}
#endif //DEBUG


// return team number 0 through 3 based what MOD uses for team numbers
int UTIL_GetTeam(edict_t *pEntity)
{
   if (mod_id == TFC_DLL)
   {
      return pEntity->v.team - 1;  // TFC teams are 1-4
   }
   else if (mod_id == CSTRIKE_DLL)
   {
      char *infobuffer;
      char model_name[32];

      infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)( pEntity );
      strcpy(model_name, (g_engfuncs.pfnInfoKeyValue(infobuffer, "model")));

      if ((strcmp(model_name, "terror") == 0) ||  // Phoenix Connektion
          (strcmp(model_name, "arab") == 0) ||    // L337 Krew
          (strcmp(model_name, "artic") == 0) ||   // Artic Avenger
          (strcmp(model_name, "guerilla") == 0))  // Gorilla Warfare
      {
         return 0;
      }
      else if ((strcmp(model_name, "urban") == 0) ||  // Seal Team 6
               (strcmp(model_name, "gsg9") == 0) ||   // German GSG-9
               (strcmp(model_name, "sas") == 0) ||    // UK SAS
               (strcmp(model_name, "gign") == 0))     // French GIGN
      {
         return 1;
      }

      return 0;  // return zero if team is unknown
   }
   else
   {
      int team = pEntity->v.team;

      if ((team < 0) || (team > 3))
         team = 0;

      return team;
   }
}



bool IsAlive(edict_t *pEdict)
{
   return (pEdict->v.deadflag == DEAD_NO) && (pEdict->v.health > 0);
}


bool FInViewCone(Vector *pOrigin, edict_t *pEdict)
{
   Vector2D vec2LOS;
   float    flDot;

   UTIL_MakeVectors ( pEdict->v.angles );

   vec2LOS = ( *pOrigin - pEdict->v.origin ).Make2D();
   vec2LOS = vec2LOS.Normalize();

   flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

   if ( flDot > 0.50 )  // 60 degree field of view 
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


bool FVisible( const Vector &vecOrigin, edict_t *pEdict )
{
   TraceResult tr;
   Vector      vecLookerOrigin;

   // look through caller's eyes
   vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;

   UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, pEdict, &tr);

   if (tr.flFraction != 1.0)
   {
      return FALSE;// Line of sight is not established
   }
   else
   {
      return TRUE;// line of sight is valid.
   }
}


Vector GetGunPosition(edict_t *pEdict)
{
   return (pEdict->v.origin + pEdict->v.view_ofs);
}



Vector VecBModelOrigin(edict_t *pEdict)
{
   return pEdict->v.absmin + (pEdict->v.size * 0.5);
}


bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer)
{
   float distance;
   static bool check_footstep_sounds = TRUE;
   static float footstep_sounds_on;
   float sensitivity = 1.0;
   float volume;

   // update sounds made by this player, alert bots if they are nearby...

   if (check_footstep_sounds)
   {
      check_footstep_sounds = FALSE;
      footstep_sounds_on = CVAR_GET_FLOAT("mp_footsteps");
   }

   if (footstep_sounds_on > 0.0)
   {
      // check if this player is moving fast enough to make sounds...
      if (pPlayer->v.velocity.Length2D() > 220.0)
      {
         volume = 500.0;  // volume of sound being made (just pick something)

         Vector v_sound = pPlayer->v.origin - pEdict->v.origin;

         distance = v_sound.Length();

         // is the bot close enough to hear this sound?
         if (distance < (volume * sensitivity))
         {
            Vector bot_angles = UTIL_VecToAngles( v_sound );

            pEdict->v.ideal_yaw = bot_angles.y;


            return TRUE;
         }
      }
   }

   return FALSE;
}

void ShowMenu (int bitsValidSlots, int nDisplayTime, BOOL fNeedMore, char pszText[1024])
{

	MESSAGE_BEGIN( MSG_ALL, gmsgShowMenu);

	WRITE_SHORT( bitsValidSlots);
	WRITE_CHAR( nDisplayTime );
	WRITE_BYTE( fNeedMore );
	WRITE_STRING (pszText);

	MESSAGE_END();




  
}


void UTIL_LogPrintf( char *fmt, ... )
{
	va_list			argptr;
	static char		string[1024];
	
	va_start ( argptr, fmt );
	vsprintf ( string, fmt, argptr );
	va_end   ( argptr );

	// Print to server console
	ALERT( at_logged, "%s", string );
}

void UTIL_EmitAmbientSound( edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch )
{
	float rgfl[3];
	vecOrigin.CopyToArray(rgfl);

	EMIT_AMBIENT_SOUND(entity, rgfl, samp, vol, attenuation, fFlags, pitch);
}

Vector UTIL_ClampVectorToBox( const Vector &input, const Vector &clampSize )
{
	Vector sourceVector = input;

	if ( sourceVector.x > clampSize.x )
		sourceVector.x -= clampSize.x;
	else if ( sourceVector.x < -clampSize.x )
		sourceVector.x += clampSize.x;
	else
		sourceVector.x = 0;

	if ( sourceVector.y > clampSize.y )
		sourceVector.y -= clampSize.y;
	else if ( sourceVector.y < -clampSize.y )
		sourceVector.y += clampSize.y;
	else
		sourceVector.y = 0;
	
	if ( sourceVector.z > clampSize.z )
		sourceVector.z -= clampSize.z;
	else if ( sourceVector.z < -clampSize.z )
		sourceVector.z += clampSize.z;
	else
		sourceVector.z = 0;

	return sourceVector.Normalize();
}

CBaseEntity *UTIL_FindEntityByString( CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue )
{
	edict_t	*pentEntity;

	if (pStartEntity)
		pentEntity = pStartEntity->edict();
	else
		pentEntity = NULL;

	pentEntity = FIND_ENTITY_BY_STRING( pentEntity, szKeyword, szValue );

	if (!FNullEnt(pentEntity))
		return CBaseEntity::Instance(pentEntity);
	return NULL;
}

CBaseEntity *UTIL_FindEntityByClassname( CBaseEntity *pStartEntity, const char *szName )
{
	return UTIL_FindEntityByString( pStartEntity, "classname", szName );
}

void UTIL_MakeAimVectors( const Vector &vecAngles )
{
	float rgflVec[3];
	vecAngles.CopyToArray(rgflVec);
	rgflVec[0] = -rgflVec[0];
	MAKE_VECTORS(rgflVec);
}
void UTIL_BloodDecalTrace( TraceResult *pTrace, int bloodColor )
{

	//if ( UTIL_ShouldShowBlood( bloodColor ) )
	//{
		//if ( bloodColor == BLOOD_COLOR_RED )
			//UTIL_DecalTrace( pTrace, DECAL_BLOOD1 + RANDOM_LONG(0,5) );
		//else

			UTIL_DecalTrace( pTrace, DECAL_BLOOD1 + RANDOM_LONG(0,5) );
	//}
	
}

void UTIL_DecalTrace( TraceResult *pTrace, int decalNumber )
{
	short entityIndex;
	int index;
	int message;

	if ( decalNumber < 0 )
		return;

	index = gDecals[ decalNumber ].index;

	if ( index < 0 )
		return;

	if (pTrace->flFraction == 1.0)
		return;

	// Only decal BSP models
	if ( pTrace->pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( pTrace->pHit );
		if ( pEntity && !pEntity->IsBSPModel() )
			return;
		entityIndex = ENTINDEX( pTrace->pHit );
	}
	else 
		entityIndex = 0;

	message = TE_DECAL;
	if ( entityIndex != 0 )
	{
		if ( index > 255 )
		{
			message = TE_DECALHIGH;
			index -= 256;
		}
	}
	else
	{
		message = TE_WORLDDECAL;
		if ( index > 255 )
		{
			message = TE_WORLDDECALHIGH;
			index -= 256;
		}
	}
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( message );
		WRITE_COORD( pTrace->vecEndPos.x );
		WRITE_COORD( pTrace->vecEndPos.y );
		WRITE_COORD( pTrace->vecEndPos.z );
		WRITE_BYTE( index );
		if ( entityIndex )
			WRITE_SHORT( entityIndex );
	MESSAGE_END();
}


void UTIL_Sparks( const Vector &position )
{
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, position );
		WRITE_BYTE( TE_SPARKS );
		WRITE_COORD( position.x );
		WRITE_COORD( position.y );
		WRITE_COORD( position.z );
	MESSAGE_END();
}
CBaseEntity *UTIL_FindEntityByTargetname( CBaseEntity *pStartEntity, const char *szName )
{
	return UTIL_FindEntityByString( pStartEntity, "targetname", szName );
}
void UTIL_Remove( CBaseEntity *pEntity )
{
	if ( !pEntity )
		return;

	//pEntity->UpdateOnRemove();
	pEntity->pev->flags |= FL_KILLME;
	pEntity->pev->targetname = 0;
}
Vector VecBModelOrigin( entvars_t* pevBModel )
{
	return pevBModel->absmin + ( pevBModel->size * 0.5 );
}

void UTIL_TraceReflect(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr, int Reflect)
{
	Vector tVector, tStart, tEnd;
    TraceResult ttr;
    int counter;

    tStart = vecStart;
    tEnd = vecEnd;
    ttr = *ptr;

    for(counter = 0;counter<Reflect;counter++)
    {
    	UTIL_TraceLine(tStart, tEnd, igmon, pentIgnore, &ttr);
		tEnd = ((tEnd-tStart).Length())*((2*DotProduct(-(tEnd-tStart).Normalize(),ttr.vecPlaneNormal)*ttr.vecPlaneNormal)+(tEnd-tStart).Normalize());
        tStart = ttr.vecEndPos;
   		if (ttr.flFraction >= 1.0)
        	break;
    }

    *ptr = ttr;
}

void UTIL_ParticleEffect( const Vector &vecOrigin, const Vector &vecDirection, ULONG ulColor, ULONG ulCount )
{
	PARTICLE_EFFECT( vecOrigin, vecDirection, (float)ulColor, (float)ulCount );
}
#ifdef SZ_DLL

void UTIL_BloodDrips( const Vector &origin, const Vector &direction, int color, int amount )
{

	if ( color == DONT_BLEED || amount == 0 )
		return;

		amount *= 2;
	if ( amount > 255 )
		amount = 255;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, origin );
		WRITE_BYTE( TE_BLOODSPRITE );
		WRITE_COORD( origin.x);								// pos
		WRITE_COORD( origin.y);
		WRITE_COORD( origin.z);
		WRITE_SHORT( g_sModelIndexBloodSpray );				// initial sprite model
		WRITE_SHORT( g_sModelIndexBloodDrop );				// droplet sprite models
		WRITE_BYTE( color );								// color index into host_basepal
		WRITE_BYTE( min( max( 3, amount / 10 ), 16 ) );		// size
	MESSAGE_END();
}	
			
#endif



Vector UTIL_StringToVector( const char *pString )
{

	Vector retVec;

	char *pstr, *pfront, tempString[128];
	int	j;

	strcpy( tempString, pString );
	pstr = pfront = tempString;

	for ( j = 0; j < 3; j++ )			// lifted from pr_edict.c
	{
		if (j == 0) retVec.x = atof( pfront );
		if (j == 1) retVec.y = atof( pfront );
		if (j == 2) retVec.z = atof( pfront );


		while ( *pstr && *pstr != ' ' )
			pstr++;
		if (!*pstr)
			break;
		pstr++;
		pfront = pstr;
	}

	return retVec;
}

