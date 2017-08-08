//
// HPB bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// engine.cpp
//

#include "extdll.h"
#include "util.h"
#include "studio.h"
#include "cbase.h"
#include "animation.h"
#include "externmsg.h"
#include "avsnow.h"
#include "avflare.h"
#include "avbfg.h"
#include "avsign.h"

#include "bot.h"
#include "engine.h"
#ifdef SZ_DLL
	#include "avdll.h"
	#include "avwritewep.h"
#endif
#include "dll.h"

static FILE *fp;

extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern edict_t *BuildEdict;

extern edict_t *JustMadeEdict;

extern bool breakingnow;
extern signtext_t playerModels[33];


extern bool AdminLoggedIn[33];
extern bool DLLON;

extern edict_t *llama;

extern char callee[256];

bool ignoremsg = 0;

bool sentwep[33];

extern int mod_id;


int debug_engine = 0;
extern bool mapspawn;

extern float detsoontime;

int temode = 0;
int temsg = 0;

void (*botMsgFunction)(void *, int) = NULL;
void (*botMsgEndFunction)(void *, int) = NULL;
int botMsgIndex;

// messages created in RegUserMsg which will be "caught"
// Keep a list of precached models and sounds, so that we dont try to precache when not allowed
int message_VGUI = 0;
int message_ShowMenu = 0;
int message_WeaponList = 0;
int message_CurWeapon = 0;
int message_AmmoX = 0;
int message_AmmoPickup = 0;
int message_Damage = 0;
int message_Money = 0;  // for Counter-Strike
int message_DeathMsg = 0;
int message_TextMsg = 0;
int message_WarmUp = 0;      // for Front Line Force
int message_WinMessage = 0;  // for Front Line Force
int message_ScreenFade = 0;
int message_HLTV = 0; // for Counter-Strike

//static FILE *fp;

struct precached_s 
{
	int preID;
	char preName[128];
};

typedef precached_s precached_t;


precached_t modelPre[512];
precached_t soundPre[512];

int numModel = 0;
int numSound = 0;
char newfile[128];

//char themodel[256];

char *test1 = "ModelTest1";


char *avadd1 = "avatar-x/avadd1.avfil";
char *avadd4 = "avatar-x/avadd4.avfil";
char *avadd5 = "avatar-x/avadd5.avfil";
char *avadd6 = "avatar-x/avadd6.avfil";
char *avadd7 = "avatar-x/avadd7.avfil";
char *avadd8 = "avatar-x/avadd8.avfil";
char *avadd14 = "avatar-x/avadd14.avfil";
char *avadd15 = "avatar-x/avadd15.avfil";


char *mpp1	= "models/p_smallshotgun.mdl";
char *mpp2	= "models/p_crowbar.mdl";
char *mpp3	= "models/p_knife.mdl";
char *mpp4	= "models/p_medkit.mdl";
char *mpp5	= "models/p_spanner.mdl";
char *mpp6	= "models/p_nailgun.mdl";
char *mpp7	= "models/p_snailgun.mdl";
char *mpp8	= "models/p_rpg.mdl";
char *mpp9	= "models/p_rpg.mdl";
char *mpp10	= "models/p_egon2.mdl";
char *mpp11	= "models/p_9mmhandgun.mdl";
char *mpp12	= "models/p_sniper.mdl";
char *mpp13	= "models/p_mini.mdl";
char *mpp14	= "models/p_glauncher.mdl";
char *mpp15	= "models/p_shotgun.mdl";
char *mpp16	= "models/p_grenade.mdl";

/*char *mpp17	= "";
char *mpp18	= "";
char *mpp19	= "";
char *mpp20	= "";
char *mpp21	= "";
char *mpp22	= "";
char *mpp23	= "";
*/

int pfnPrecacheModel(char* m)
{




	#ifdef SZ_DLL
		if (DLLON)
		{
/*
			//if (numModel > 510 || mapspawn == 0) {

				// we are at max.

				
				sprintf(newfile, "%s", m);

				bool fl2 = 1;

				if		(FStrEq(m, "models/tree.mdl"))			sprintf(newfile, "%s", "avatar-x/avadd1.avfil");
				else if (FStrEq(m, "models/mirv_grenade.mdl")) 	sprintf(newfile, "%s", "avatar-x/avadd4.avfil");
				else if (FStrEq(m, "models/bomblet.mdl"))		sprintf(newfile, "%s", "avatar-x/avadd5.avfil");
				else if (FStrEq(m, "models/w_grenade.mdl"))		sprintf(newfile, "%s", "avatar-x/avadd6.avfil");
				else if (FStrEq(m, "models/sentry1.mdl"))		sprintf(newfile, "%s", "avatar-x/avadd7.avfil");
				else if (FStrEq(m, "models/sentry2.mdl"))		sprintf(newfile, "%s", "avatar-x/avadd14.avfil");
				else if (FStrEq(m, "models/sentry3.mdl"))		sprintf(newfile, "%s", "avatar-x/avadd15.avfil");
				else if (FStrEq(m, "models/base.mdl"))			sprintf(newfile, "%s", "avatar-x/avadd8.avfil");
				else	fl2 = 0;

				if (fl2 == 0)
				{

					for (int i = 0; i < numModel; i++)
					{
						if (FStrEq(modelPre[i].preName, m)) return modelPre[i].preID ;
					}
				}
				else
				{
					for (int i = 0; i < numModel; i++)
					{
						if (FStrEq(modelPre[i].preName, newfile)) return modelPre[i].preID ;
					}
				}

				return modelPre[100].preID;
			}
			else if (m[0] != '*')
			{
*/
				
				//char themodel[256];

				char *themodel = m;


				//sprintf(themodel, "%s", m);

				//bool fl2 = 1;

				if		(FStrEq(m, "models/tree.mdl"))			themodel = avadd1;
				else if (FStrEq(m, "models/mirv_grenade.mdl")) 	themodel = avadd4;
				else if (FStrEq(m, "models/bomblet.mdl"))		themodel = avadd5;
				else if (FStrEq(m, "models/w_grenade.mdl"))		themodel = avadd6;
				else if (FStrEq(m, "models/sentry1.mdl"))		themodel = avadd7;
				else if (FStrEq(m, "models/sentry2.mdl"))		themodel = avadd14;
				else if (FStrEq(m, "models/sentry3.mdl"))		themodel = avadd15;
				else if (FStrEq(m, "models/base.mdl"))			themodel = avadd8;

	

//				if (FStrEq(m, "models/pent.mdl"))		themodel = test1;
				

				// add to list
/*
			
				int value = 0;
				
				if (fl2) 
				{
					value = (*g_engfuncs.pfnPrecacheModel)(newfile);
				}
				else
				{
					value = (*g_engfuncs.pfnPrecacheModel)(m);
				}

				// make sure it isnt already in the list
				bool flag = 1;
				for (int i = 0; i < numModel; i++)
				{
					if (FStrEq(modelPre[i].preName, newfile)) flag = 0;
				}
				if (flag)
				{
					numModel++;
					//fp=fopen("predebug.txt","a");
				
					if (fl2) 
					{
						sprintf(modelPre[numModel - 1].preName, "%s", newfile);
					//	fprintf(fp, "%i: %s\n", numModel, newfile);
					}
					else
					{
						sprintf(modelPre[numModel - 1].preName, "%s", m);
					//	fprintf(fp, "%i: %s\n", numModel, m);
					}
					modelPre[numModel - 1].preID = value;
					//fclose(fp);	

				}

				return value;
			}
			else
			{
				return 1;
			}

*/

			bool docache = 1;

			if (FStrEq(themodel, "models/player/sniper/sniper.mdl"))			docache=0;
			if (FStrEq(themodel, "models/player/sniper/sniper2.mdl"))			docache=0;
			if (FStrEq(themodel, "models/player/soldier/soldier.mdl"))			docache=0;
			if (FStrEq(themodel, "models/player/soldier/soldier2.mdl"))			docache=0;
			if (FStrEq(themodel, "models/player/medic/medic.mdl"))				docache=0;
			if (FStrEq(themodel, "models/player/medic/medic2.mdl"))				docache=0;
			if (FStrEq(themodel, "models/player/demo/demo.mdl"))				docache=0;
			if (FStrEq(themodel, "models/player/demo/demo2.mdl"))				docache=0;
			if (FStrEq(themodel, "models/player/hvyweapon/hvyweapon.mdl"))		docache=0;
			if (FStrEq(themodel, "models/player/hvyweapon/hvyweapon2.mdl"))		docache=0;
			if (FStrEq(themodel, "models/player/pyro/pyro.mdl"))				docache=0;
			if (FStrEq(themodel, "models/player/pyro/pyro2.mdl"))				docache=0;
			if (FStrEq(themodel, "models/player/spy/spy.mdl"))					docache=0;
			if (FStrEq(themodel, "models/player/spy/spy2.mdl"))					docache=0;
			if (FStrEq(themodel, "models/player/engineer/engineer.mdl"))		docache=0;
			if (FStrEq(themodel, "models/player/engineer/engineer2.mdl"))		docache=0;
			//if (FStrEq(themodel, "models/player/scout/scout.mdl"))				docache=0;
			//if (FStrEq(themodel, "models/player/scout/scout2.mdl"))				docache=0;
			if (FStrEq(themodel, "models/player/civilian/civilian.mdl"))		docache=0;
			if (FStrEq(themodel, "models/player/civilian/civilian2.mdl"))		docache=0;
	
			//if (FStrEq(themodel, "models/v_tfc_nailgun.mdl"))		docache=0;
			//if (FStrEq(themodel, "models/v_tfc_supernailgun.mdl"))		docache=0;

			int somenum = 0;

			if (docache)
			{
				somenum =(*g_engfuncs.pfnPrecacheModel)(themodel);
				//somenum =(*g_engfuncs.pfnPrecacheModel)(m);

					/*
				fp = fopen("predebug.txt","a");
				fprintf(fp, "Precaching: %s - %i\n", themodel, somenum);
				fclose(fp);	
				*/

				return somenum;
			}
			else
			{
				return 0;
			}
		}
		else
		{
	#endif
		return (*g_engfuncs.pfnPrecacheModel)(m);
	#ifdef SZ_DLL
		}
	#endif


}
int pfnPrecacheSound(char* m)
{
/*

	#ifdef SZ_DLL
		if (DLLON)
		{

			if (numSound > 510 || mapspawn == 0) {

				// we are at max.

				for (int i = 0; i < numSound; i++)
				{
					if (FStrEq(soundPre[i].preName, m)) return soundPre[i].preID ;
				}

				return soundPre[100].preID;
			}
			else
			{
				int value = (*g_engfuncs.pfnPrecacheSound)(m);

				// make sure it isnt already in the list
				bool flag = 1;
				for (int i = 0; i < numSound; i++)
				{
					if (FStrEq(soundPre[i].preName, newfile)) flag = 0;
				}
				if (flag)
				{
					numSound++;
					sprintf(soundPre[numSound - 1].preName, "%s", newfile);
					soundPre[numSound - 1].preID = value;
				}

				return value;
			}
		}
		else
		{
	#endif
	
	return (*g_engfuncs.pfnPrecacheSound)(m);

	#ifdef SZ_DLL
		}
	#endif
	*/
	return (*g_engfuncs.pfnPrecacheSound)(m);
}
void pfnSetModel(edict_t *e, const char *m)
{
/*
	if (DLLON)
	{

		#ifdef SZ_DLL

		if (JustMadeEdict != NULL && BuildEdict != NULL) 
		{
			if (ENTINDEX(e) == ENTINDEX(JustMadeEdict)) 
			{
				if (FStrEq(m, "models/dispenser.mdl")) {

					e->v.iuser3 = ENTINDEX(BuildEdict);
					BuildEdict = NULL;
					JustMadeEdict = NULL;
				}
			}
		}	

		bool fl2 = 1;
		sprintf(newfile, "%s", m);

		if (FStrEq(m, "models/tree.mdl"))					sprintf(newfile, "%s", "avatar-x/avadd1.avfil");
		else if (FStrEq(m, "models/mirv_grenade.mdl")) 		sprintf(newfile, "%s", "avatar-x/avadd4.avfil");
		else if (FStrEq(m, "models/bomblet.mdl")) 			sprintf(newfile, "%s", "avatar-x/avadd5.avfil");
		else if (FStrEq(m, "models/w_grenade.mdl"))			sprintf(newfile, "%s", "avatar-x/avadd6.avfil");
		else if (FStrEq(m, "models/sentry1.mdl"))			sprintf(newfile, "%s", "avatar-x/avadd7.avfil");
		else if (FStrEq(m, "models/sentry2.mdl"))			sprintf(newfile, "%s", "avatar-x/avadd14.avfil");
		else if (FStrEq(m, "models/sentry3.mdl"))			sprintf(newfile, "%s", "avatar-x/avadd15.avfil");
		else if (FStrEq(m, "models/base.mdl"))				sprintf(newfile, "%s", "avatar-x/avadd8.avfil");
		else if (FStrEq(m, "models/dispenser.mdl")) 		sprintf(newfile, "%s", "avatar-x/avadd2.avfil");
		else if (FStrEq(m, "models/backpack.mdl") && e->v.movetype == MOVETYPE_TOSS) {

			sprintf(newfile, "%s", "avatar-x/avadd3.avfil");
		}
		else
		{
			fl2 = 0;
		}

		bool flag = 0;

		
		if (fl2 == 0) {
			for (int i = 0; i < numModel; i++)
			{
				if (FStrEq(modelPre[i].preName, m)) flag = 1;
			}
		}
		else
		{
			for (int i = 0; i < numModel; i++)
			{
				if (FStrEq(modelPre[i].preName, newfile)) flag = 1;
			}
		}
		
		if (m[0] == '*') flag = 1;

		if (flag)
		{
			if (fl2 == 0) {
				(*g_engfuncs.pfnSetModel)(e, m);
			
			}
			else
			{
				(*g_engfuncs.pfnSetModel)(e, newfile);
				
			}
		}
		else
		{
			(*g_engfuncs.pfnSetModel)(e, modelPre[100].preName);
		}

		#endif

		#ifndef SZ_DLL
			(*g_engfuncs.pfnSetModel)(e, m);
		#endif
	}
	else
	{
		(*g_engfuncs.pfnSetModel)(e, m);
	}
	
  */




	char *themodel = (char*)m;


	if		(FStrEq(m, "models/tree.mdl"))			themodel = avadd1;
	else if (FStrEq(m, "models/mirv_grenade.mdl")) 	themodel = avadd4;
	else if (FStrEq(m, "models/bomblet.mdl"))		themodel = avadd5;
	else if (FStrEq(m, "models/w_grenade.mdl"))		themodel = avadd6;
	else if (FStrEq(m, "models/sentry1.mdl"))		themodel = avadd7;
	else if (FStrEq(m, "models/sentry2.mdl"))		themodel = avadd14;
	else if (FStrEq(m, "models/sentry3.mdl"))		themodel = avadd15;
	else if (FStrEq(m, "models/base.mdl"))			themodel = avadd8;

	
	(*g_engfuncs.pfnSetModel)(e, themodel);
}
int pfnModelIndex(const char *m)
{
   return (*g_engfuncs.pfnModelIndex)(m);
}
int pfnModelFrames(int modelIndex)
{
   return (*g_engfuncs.pfnModelFrames)(modelIndex);
}
void pfnSetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{


	char *pClassname = (char *)STRING(e->v.classname);
	
#ifdef SZ_DLL

	if (FStrEq( pClassname, "xen_ttrigger" )) {

		
		// it's a tree trigger, and we want a different size for it.

		(*g_engfuncs.pfnSetSize)(e, Vector( -48, -48, 0 ), Vector( 48, 48, 128 ));

	}
	else 
	{

#endif

		(*g_engfuncs.pfnSetSize)(e, rgflMin, rgflMax);
#ifdef SZ_DLL
	}
#endif

}
void pfnChangeLevel(char* s1, char* s2)
{
   (*g_engfuncs.pfnChangeLevel)(s1, s2);
}
void pfnGetSpawnParms(edict_t *ent)
{
   (*g_engfuncs.pfnGetSpawnParms)(ent);
}
void pfnSaveSpawnParms(edict_t *ent)
{
   (*g_engfuncs.pfnSaveSpawnParms)(ent);
}
float pfnVecToYaw(const float *rgflVector)
{
   return (*g_engfuncs.pfnVecToYaw)(rgflVector);
}
void pfnVecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
   (*g_engfuncs.pfnVecToAngles)(rgflVectorIn, rgflVectorOut);
}
void pfnMoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
   (*g_engfuncs.pfnMoveToOrigin)(ent, pflGoal, dist, iMoveType);
}
void pfnChangeYaw(edict_t* ent)
{
   (*g_engfuncs.pfnChangeYaw)(ent);
}
void pfnChangePitch(edict_t* ent)
{
   (*g_engfuncs.pfnChangePitch)(ent);
}
edict_t* pfnFindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
	  return (*g_engfuncs.pfnFindEntityByString)(pEdictStartSearchAfter, pszField, pszValue);
}
int pfnGetEntityIllum(edict_t* pEnt)
{
   return (*g_engfuncs.pfnGetEntityIllum)(pEnt);
}
edict_t* pfnFindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
   return (*g_engfuncs.pfnFindEntityInSphere)(pEdictStartSearchAfter, org, rad);
}
edict_t* pfnFindClientInPVS(edict_t *pEdict)
{
   return (*g_engfuncs.pfnFindClientInPVS)(pEdict);
}
edict_t* pfnEntitiesInPVS(edict_t *pplayer)
{
   return (*g_engfuncs.pfnEntitiesInPVS)(pplayer);
}
void pfnMakeVectors(const float *rgflVector)
{
   (*g_engfuncs.pfnMakeVectors)(rgflVector);
}
void pfnAngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
   (*g_engfuncs.pfnAngleVectors)(rgflVector, forward, right, up);
}
edict_t* pfnCreateEntity(void)
{

	edict_t *theone;


	theone = (*g_engfuncs.pfnCreateEntity)();

	JustMadeEdict = theone;


	return theone;

}
void pfnRemoveEntity(edict_t* e)
{
	#ifdef SZ_DLL
	char *pClassname = (char *)STRING(e->v.classname);

	//ALERT(at_console, "Call to remove %i a %s by %s\n", ENTINDEX(e), pClassname, callee);
/*
	if (FStrEq(pClassname, "building_dispenser") && detsoontime > gpGlobals->time)
	{
		
		e->v.health = 100;

	}
	else if (FStrEq(pClassname, "building_sentrygun") && detsoontime > gpGlobals->time)
	{
		e->v.health = 100;
		e->v.nextthink = gpGlobals->time + 0.1;
		e->v.solid = SOLID_BBOX;
		//e->v.damage = DAMAGE_YES;

		CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(e);
		if (pEntity->MyOwnThink) pEntity->m_pfnThink = pEntity->MyOwnThink;


	}
	else if (FStrEq(pClassname, "building_sentrygun_base") && detsoontime > gpGlobals->time)
	{
		e->v.health = 100;
		e->v.solid = SOLID_BBOX;
	}
	*/
	// Prevent deletion of the camera by TFC
	if (FStrEq(pClassname, "building_camera"))
	{


	}
	else if (FStrEq(pClassname, "func_breakable"))
	{	
		// dont delete func_breakables for now
//		ALERT(at_console, "ATTEMPT WAS MADE TO DELETE BREAKABLE %i\n", ENTINDEX(e) );
		//e->v.nextthink = gpGlobals->time + 5;
	}
	else
	{
	#endif
		(*g_engfuncs.pfnRemoveEntity)(e);	
	#ifdef SZ_DLL
	}
#endif
   
}
edict_t* pfnCreateNamedEntity(int className)
{

	char *pClassname = (char *)STRING(className);

	edict_t *theone;

	theone = (*g_engfuncs.pfnCreateNamedEntity)(className);

	return theone;



}
void pfnMakeStatic(edict_t *ent)
{
   (*g_engfuncs.pfnMakeStatic)(ent);
}
int pfnEntIsOnFloor(edict_t *e)
{
   return (*g_engfuncs.pfnEntIsOnFloor)(e);
}
int pfnDropToFloor(edict_t* e)
{
   return (*g_engfuncs.pfnDropToFloor)(e);
}
int pfnWalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
   return (*g_engfuncs.pfnWalkMove)(ent, yaw, dist, iMode);
}
void pfnSetOrigin(edict_t *e, const float *rgflOrigin)
{

	// see if its a bodyque

	if (FStrEq((char*)STRING(e->v.classname), "bodyque"))
	{
		// see if its the llama
		if (e->v.renderamt == ENTINDEX(llama))
		{
			// set to invisible
			e->v.rendermode = 0;
			e->v.renderamt = 0;
			e->v.renderfx = 0;
		}
	}
	(*g_engfuncs.pfnSetOrigin)(e, rgflOrigin);
}
void pfnEmitSound(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch)
{
/*
	if (DLLON)
	{
		#ifdef SZ_DLL
			if (FStrEq( sample, "misc/r_tele3.wav") || FStrEq( sample, "misc/r_tele4.wav")) {

			   (*g_engfuncs.pfnEmitSound)(entity, channel, "misc/r_tele2.wav", volume, attenuation, fFlags, pitch);
			}
			else
			{
		#endif

			   (*g_engfuncs.pfnEmitSound)(entity, channel, sample, volume, attenuation, fFlags, pitch);


		#ifdef SZ_DLL
			}
		#endif
	}
	else
	{
*/
		(*g_engfuncs.pfnEmitSound)(entity, channel, sample, volume, attenuation, fFlags, pitch);
//	}
}
void pfnEmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	
	(*g_engfuncs.pfnEmitAmbientSound)(entity, pos, samp, vol, attenuation, fFlags, pitch);
}
void pfnTraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
   (*g_engfuncs.pfnTraceLine)(v1, v2, fNoMonsters, pentToSkip, ptr);
}
void pfnTraceToss(edict_t* pent, edict_t* pentToIgnore, TraceResult *ptr)
{
   (*g_engfuncs.pfnTraceToss)(pent, pentToIgnore, ptr);
}
int pfnTraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
   return (*g_engfuncs.pfnTraceMonsterHull)(pEdict, v1, v2, fNoMonsters, pentToSkip, ptr);
}
void pfnTraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
   (*g_engfuncs.pfnTraceHull)(v1, v2, fNoMonsters, hullNumber, pentToSkip, ptr);
}
void pfnTraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
   (*g_engfuncs.pfnTraceModel)(v1, v2, hullNumber, pent, ptr);
}
const char *pfnTraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2 )
{
   return (*g_engfuncs.pfnTraceTexture)(pTextureEntity, v1, v2);
}
void pfnTraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
   (*g_engfuncs.pfnTraceSphere)(v1, v2, fNoMonsters, radius, pentToSkip, ptr);
}
void pfnGetAimVector(edict_t* ent, float speed, float *rgflReturn)
{
   (*g_engfuncs.pfnGetAimVector)(ent, speed, rgflReturn);
}
void pfnServerCommand(char* str)
{
   (*g_engfuncs.pfnServerCommand)(str);
}
void pfnServerExecute(void)
{
   (*g_engfuncs.pfnServerExecute)();
}
void pfnClientCommand(edict_t* pEdict, char* szFmt, ...)
{
	va_list argptr;
	va_start (argptr, szFmt);
	(*g_engfuncs.pfnClientCommand)(pEdict, szFmt, argptr);
	va_end (argptr);
}
void pfnParticleEffect(const float *org, const float *dir, float color, float count)
{
   (*g_engfuncs.pfnParticleEffect)(org, dir, color, count);
}
void pfnLightStyle(int style, char* val)
{
   (*g_engfuncs.pfnLightStyle)(style, val);
}
int pfnDecalIndex(const char *name)
{
   return (*g_engfuncs.pfnDecalIndex)(name);
}
int pfnPointContents(const float *rgflVector)
{
   return (*g_engfuncs.pfnPointContents)(rgflVector);
}
void pfnMessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
			
	if (DLLON)
	{

	   #ifdef SZ_DLL
	

			//ALERT(at_console, "Message Arrived %i - %s\n", msg_type, msgtypes[msg_type].phrase  );

			if (msg_type == SVC_TEMPENTITY)
			{
				temode = 1;
			}

			if (msg_type == gmsgWeaponList) {
				
				int mynum = ENTINDEX(ed);
				ignoremsg = 1;
			}
			else if (msg_type == gmsgResetHUD)
			{	
				// person is respawning, give them the tripmine
				entvars_t *pev = VARS( ed );
				pev->weapons |= (1<<25);
				pev->weapons |= (1<<26);
				pev->weapons |= (1<<29);

				// clear fly paper stuff
				pev->vuser4.x = 0;
				pev->vuser4.y = 0;

				int mynum = ENTINDEX(ed);
				//sentwep[mynum] = 0;

				SendWepList( ed );
				//sentwep[mynum] = 1;


				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, ed );
					WRITE_BYTE( 25 );
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, ed );
					WRITE_BYTE( 26 );
				MESSAGE_END();

				MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, ed );
					WRITE_BYTE( 29 );
				MESSAGE_END();

				if (AdminLoggedIn[ENTINDEX(ed)])
				{
					pev->weapons |= (1<<27);
					pev->weapons |= (1<<28);
					

					MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, ed );
						WRITE_BYTE( 27 );
					MESSAGE_END();
					MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, ed );
						WRITE_BYTE( 28 );
					MESSAGE_END();
				}


				SnowSpawn( ed );
				FlareSpawn( ed );
				BFGSpawn( ed );

				// Do here

				if (pev->vuser3.x == 0 && pev->vuser3.y != pev->playerclass && pev->playerclass > 0)
				{

					
					int e = ENTINDEX(ed);
					if (pev->playerclass == 1) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c1"); sprintf( playerModels[e].text , "c1" ); }
					if (pev->playerclass == 2) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c2"); sprintf( playerModels[e].text , "c2" ); }
					if (pev->playerclass == 3) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c3"); sprintf( playerModels[e].text , "c3" ); }
					if (pev->playerclass == 4) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c4"); sprintf( playerModels[e].text , "c4" ); }
					if (pev->playerclass == 5) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c5"); sprintf( playerModels[e].text , "c5" ); }
					if (pev->playerclass == 6) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c6"); sprintf( playerModels[e].text , "c6" ); }
					if (pev->playerclass == 7) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c7"); sprintf( playerModels[e].text , "c7" ); }
					if (pev->playerclass == 8) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c8"); sprintf( playerModels[e].text , "c8" ); }
					if (pev->playerclass == 9) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c9"); sprintf( playerModels[e].text , "c9" ); }
					if (pev->playerclass == 11) {g_engfuncs.pfnSetClientKeyValue( ENTINDEX(ed) , g_engfuncs.pfnGetInfoKeyBuffer( ed ), "model", "c11"); sprintf( playerModels[e].text , "c11" ); }
					
					
					pev->body = 0;
					pev->skin = 0;
					pev->fuser1 = 1;				
					pev->vuser3.y = pev->playerclass;
				
				}

			}

			#ifdef DEBUGLOG 
			fp=fopen("msg.txt","a");
			fprintf(fp, "!!!! Message Begin, %i, %i\n", msg_dest, msg_type);
			fclose(fp);	
			#endif
		#endif
	}

	if (!ignoremsg) (*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
}
void pfnMessageEnd(void)
{

	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "#### Message End\n");
	fclose(fp);	
	#endif

	temode = 0;
	temsg = 0;


   if (!ignoremsg) (*g_engfuncs.pfnMessageEnd)();

   if (ignoremsg) ignoremsg = 0; // stop ignoring after this message ends


}
void pfnWriteByte(int iValue)
{
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "BYTE: Write Byte: %i\n", iValue);
	fclose(fp);	
	#endif

	if (iValue == TE_BREAKMODEL && temode == 1) breakingnow = 1;

	if (temode == 1)
	{
		/*
		temsg = iValue;
		temode = 2;
		char typm[80];
		sprintf( typm, "Unknown: %i", iValue );

		if (iValue == 0) sprintf( typm, "%s", "TE_BEAMPOINTS");
		if (iValue == 1) sprintf( typm, "%s", "TE_BEAMENTPOINT");
		if (iValue == 2) sprintf( typm, "%s", "TE_GUNSHOT");
		if (iValue == 3) sprintf( typm, "%s", "TE_EXPLOSION");
		if (iValue == 4) sprintf( typm, "%s", "TE_TAREXPLOSION");
		if (iValue == 5) sprintf( typm, "%s", "TE_SMOKE");
		if (iValue == 6) sprintf( typm, "%s", "TE_TRACER");
		if (iValue == 7) sprintf( typm, "%s", "TE_LIGHTNING");
		if (iValue == 8) sprintf( typm, "%s", "TE_BEAMENTS");
		if (iValue == 9) sprintf( typm, "%s", "TE_SPARKS");
		if (iValue == 10) sprintf( typm, "%s", "TE_LAVASPLASH");
		if (iValue == 11) sprintf( typm, "%s", "TE_TELEPORT");
		if (iValue == 12) sprintf( typm, "%s", "TE_EXPLOSIVE");
		if (iValue == 13) sprintf( typm, "%s", "TE_BSPDECAL");
		if (iValue == 14) sprintf( typm, "%s", "TE_IMPLOSIVE");
		if (iValue == 15) sprintf( typm, "%s", "TE_SPRITETRAIL");
		if (iValue == 16) sprintf( typm, "%s", "TE_BEAM");
		if (iValue == 17) sprintf( typm, "%s", "TE_SPRITE");
		if (iValue == 18) sprintf( typm, "%s", "TE_BEAMSPRITE");
		if (iValue == 19) sprintf( typm, "%s", "TE_BEAMTORUS");
		if (iValue == 20) sprintf( typm, "%s", "TE_BEAMDISC");
		if (iValue == 21) sprintf( typm, "%s", "TE_BEAMCYLINDER");
		if (iValue == 22) sprintf( typm, "%s", "TE_BEAMFOLLOW");
		if (iValue == 23) sprintf( typm, "%s", "TE_GLOWSPRITE");
		if (iValue == 24) sprintf( typm, "%s", "TE_BEAMRING");
		if (iValue == 25) sprintf( typm, "%s", "TE_STREAK_SPLASH");
		if (iValue == 26) sprintf( typm, "%s", "TE_BEAMHOSE");
		if (iValue == 27) sprintf( typm, "%s", "TE_DLIGHT");
		if (iValue == 28) sprintf( typm, "%s", "TE_ELIGHT");
		if (iValue == 29) sprintf( typm, "%s", "TE_TEXTMESSAGE");
		if (iValue == 30) sprintf( typm, "%s", "TE_LINE");
		if (iValue == 31) sprintf( typm, "%s", "TE_BOX");
		if (iValue == 99) sprintf( typm, "%s", "TE_KILLBEAM");
		if (iValue == 100) sprintf( typm, "%s", "TE_LARGEFUNNEL");
		if (iValue == 101) sprintf( typm, "%s", "TE_BLOODSTREAM");
		if (iValue == 102) sprintf( typm, "%s", "TE_SHOWLINE");
		if (iValue == 103) sprintf( typm, "%s", "TE_BLOOD");
		if (iValue == 104) sprintf( typm, "%s", "TE_DECAL");
		if (iValue == 105) sprintf( typm, "%s", "TE_FIZZ");
		if (iValue == 106) sprintf( typm, "%s", "TE_MODEL");
		if (iValue == 107) sprintf( typm, "%s", "TE_EXPLODEMODEL");
		if (iValue == 108) sprintf( typm, "%s", "TE_BREAKMODEL");
		if (iValue == 109) sprintf( typm, "%s", "TE_GUNSHOTDECAL");
		if (iValue == 110) sprintf( typm, "%s", "TE_SPRITE_SPRAY");
		if (iValue == 111) sprintf( typm, "%s", "TE_ARMOR_RICOCHET");
		if (iValue == 112) sprintf( typm, "%s", "TE_PLAYERDECAL");
		if (iValue == 113) sprintf( typm, "%s", "TE_BUBBLES");
		if (iValue == 114) sprintf( typm, "%s", "TE_BUBBLETRAIL");
		if (iValue == 115) sprintf( typm, "%s", "TE_BLOODSPRITE");
		if (iValue == 116) sprintf( typm, "%s", "TE_WORLDDECAL");
		if (iValue == 117) sprintf( typm, "%s", "TE_WORLDDECALHIGH");
		if (iValue == 118) sprintf( typm, "%s", "TE_DECALHIGH");
		if (iValue == 119) sprintf( typm, "%s", "TE_PROJECTILE");
		if (iValue == 120) sprintf( typm, "%s", "TE_SPRAY");
		if (iValue == 121) sprintf( typm, "%s", "TE_PLAYERSPRITES");
		if (iValue == 122) sprintf( typm, "%s", "TE_PARTICLEBURST");
		if (iValue == 123) sprintf( typm, "%s", "TE_FIREFIELD");
		if (iValue == 124) sprintf( typm, "%s", "TE_PLAYERATTACHMENT");
		if (iValue == 125) sprintf( typm, "%s", "TE_KILLPLAYERATTACHMENTS");
		if (iValue == 126) sprintf( typm, "%s", "TE_MULTIGUNSHOT");
		if (iValue == 127) sprintf( typm, "%s", "TE_USERTRACER");
		
		ALERT(at_console, "Message is %s\n", typm);
*/

	}
	
   if (!ignoremsg) (*g_engfuncs.pfnWriteByte)(iValue);
}
void pfnWriteChar(int iValue)
{
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "CHAR: Write Char: %i\n", iValue);
	fclose(fp);	
	#endif
   if (!ignoremsg) (*g_engfuncs.pfnWriteChar)(iValue);
}
void pfnWriteShort(int iValue)
{
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "SHRT: Write Short: %i\n", iValue);
	fclose(fp);	
	#endif

   if (!ignoremsg) (*g_engfuncs.pfnWriteShort)(iValue);
}
void pfnWriteLong(int iValue)
{
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "LONG: Write Long: %i\n", iValue);
	fclose(fp);	
	#endif
   if (!ignoremsg) (*g_engfuncs.pfnWriteLong)(iValue);
}
void pfnWriteAngle(float flValue)
{
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "ANGL: Write Angle: %d\n", flValue);
	fclose(fp);	
	#endif

	if (!ignoremsg) (*g_engfuncs.pfnWriteAngle)(flValue);
}
void pfnWriteCoord(float flValue)
{
   
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "CORD: Write Coord: %d\n", flValue);
	fclose(fp);	
	#endif
	if (!ignoremsg) (*g_engfuncs.pfnWriteCoord)(flValue);
}
void pfnWriteString(const char *sz)
{
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "STRN: Write String: %s\n", sz);
	fclose(fp);	
	#endif

	if (!ignoremsg) (*g_engfuncs.pfnWriteString)(sz);
}
void pfnWriteEntity(int iValue)
{
	#ifdef DEBUGLOG 
	fp=fopen("msg.txt","a");
	fprintf(fp, "ENTY: Write Entity: %i\n", iValue);
	fclose(fp);	
	#endif

   if (!ignoremsg) (*g_engfuncs.pfnWriteEntity)(iValue);
}
void pfnCVarRegister(cvar_t *pCvar)
{
   (*g_engfuncs.pfnCVarRegister)(pCvar);
}
float pfnCVarGetFloat(const char *szVarName)
{
   return (*g_engfuncs.pfnCVarGetFloat)(szVarName);
}
const char* pfnCVarGetString(const char *szVarName)
{
   return (*g_engfuncs.pfnCVarGetString)(szVarName);
}
void pfnCVarSetFloat(const char *szVarName, float flValue)
{
   (*g_engfuncs.pfnCVarSetFloat)(szVarName, flValue);
}
void pfnCVarSetString(const char *szVarName, const char *szValue)
{
   (*g_engfuncs.pfnCVarSetString)(szVarName, szValue);
}


// NEW ONES ARE SUPPOSED TO BE RIGHT HERE..


void* pfnPvAllocEntPrivateData(edict_t *pEdict, int32 cb)
{
   return (*g_engfuncs.pfnPvAllocEntPrivateData)(pEdict, cb);
}
void* pfnPvEntPrivateData(edict_t *pEdict)
{
   return (*g_engfuncs.pfnPvEntPrivateData)(pEdict);
}
void pfnFreeEntPrivateData(edict_t *pEdict)
{
   (*g_engfuncs.pfnFreeEntPrivateData)(pEdict);
}
const char* pfnSzFromIndex(int iString)
{
   return (*g_engfuncs.pfnSzFromIndex)(iString);
}
int pfnAllocString(const char *m)
{
		return (*g_engfuncs.pfnAllocString)(m);
}
entvars_t* pfnGetVarsOfEnt(edict_t *pEdict)
{
   return (*g_engfuncs.pfnGetVarsOfEnt)(pEdict);
}
edict_t* pfnPEntityOfEntOffset(int iEntOffset)
{
   return (*g_engfuncs.pfnPEntityOfEntOffset)(iEntOffset);
}
int pfnEntOffsetOfPEntity(const edict_t *pEdict)
{
   return (*g_engfuncs.pfnEntOffsetOfPEntity)(pEdict);
}
int pfnIndexOfEdict(const edict_t *pEdict)
{
   return (*g_engfuncs.pfnIndexOfEdict)(pEdict);
}
edict_t* pfnPEntityOfEntIndex(int iEntIndex)
{
   return (*g_engfuncs.pfnPEntityOfEntIndex)(iEntIndex);
}
edict_t* pfnFindEntityByVars(entvars_t* pvars)
{
   return (*g_engfuncs.pfnFindEntityByVars)(pvars);
}
void* pfnGetModelPtr(edict_t* pEdict)
{
   return (*g_engfuncs.pfnGetModelPtr)(pEdict);
}
int pfnRegUserMsg(const char *pszName, int iSize)
{
   int msg;
   msg = (*g_engfuncs.pfnRegUserMsg)(pszName, iSize);
	
   //sprintf( msgtypes[msg].phrase, "%s", pszName );

	if (FStrEq(pszName, "CurWeapon"))				gmsgCurWeapon = msg;
	else if (FStrEq(pszName, "Health"))				gmsgHealth = msg;
	else if (FStrEq(pszName, "Damage"))				gmsgDamage = msg;
	else if (FStrEq(pszName, "HudText"))			gmsgHudText = msg;
	else if (FStrEq(pszName, "SayText"))			gmsgSayText = msg;
	else if (FStrEq(pszName, "TextMsg"))			gmsgTextMsg = msg;
	else if (FStrEq(pszName, "WeaponList"))			gmsgWeaponList = msg;
	else if (FStrEq(pszName, "DeathMsg"))			gmsgDeathMsg = msg;
	else if (FStrEq(pszName, "ScoreInfo"))			gmsgScoreInfo = msg;
	else if (FStrEq(pszName, "MOTD"))				gmsgMOTD = msg;
	else if (FStrEq(pszName, "WeapPickup"))			gmsgWeapPickup = msg;
	else if (FStrEq(pszName, "HideWeapon"))			gmsgHideWeapon = msg;
	else if (FStrEq(pszName, "SetFOV"))				gmsgSetFOV = msg;
	else if (FStrEq(pszName, "ShowMenu"))			gmsgShowMenu = msg;
	else if (FStrEq(pszName, "ResetHUD"))			gmsgResetHUD = msg;
	else if (FStrEq(pszName, "AmmoX"))				gmsgAmmoX = msg;




	#ifdef DEBUGLOG 
	fp=fopen("reg.txt","a");
	fprintf(fp, "REGMSG: Name: %s   Value: %i  Size: %i\n", pszName, msg, iSize);
	fclose(fp);	
	#endif

   return msg;
}
void pfnAnimationAutomove(const edict_t* pEdict, float flTime)
{
   (*g_engfuncs.pfnAnimationAutomove)(pEdict, flTime);
}
void pfnGetBonePosition(const edict_t* pEdict, int iBone, float *rgflOrigin, float *rgflAngles )
{
   (*g_engfuncs.pfnGetBonePosition)(pEdict, iBone, rgflOrigin, rgflAngles);
}
uint32 pfnFunctionFromName( const char *pName )
{
   return (*g_engfuncs.pfnFunctionFromName)(pName);
}
const char *pfnNameForFunction( uint32 function )
{
   return (*g_engfuncs.pfnNameForFunction)(function);
}
void pfnClientPrintf( edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg )
{
   (*g_engfuncs.pfnClientPrintf)(pEdict, ptype, szMsg);
}
void pfnServerPrint( const char *szMsg )
{
   (*g_engfuncs.pfnServerPrint)(szMsg);
}
void pfnGetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles )
{
   (*g_engfuncs.pfnGetAttachment)(pEdict, iAttachment, rgflOrigin, rgflAngles);
}
void pfnCRC32_Init(CRC32_t *pulCRC)
{
   (*g_engfuncs.pfnCRC32_Init)(pulCRC);
}
void pfnCRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
   (*g_engfuncs.pfnCRC32_ProcessBuffer)(pulCRC, p, len);
}
void pfnCRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
   (*g_engfuncs.pfnCRC32_ProcessByte)(pulCRC, ch);
}
CRC32_t pfnCRC32_Final(CRC32_t pulCRC)
{
   return (*g_engfuncs.pfnCRC32_Final)(pulCRC);
}
int32 pfnRandomLong(int32 lLow, int32 lHigh)
{
//   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRandomLong: lLow=%d lHigh=%d\n",lLow,lHigh); fclose(fp); }
   return (*g_engfuncs.pfnRandomLong)(lLow, lHigh);
}
float pfnRandomFloat(float flLow, float flHigh)
{
   return (*g_engfuncs.pfnRandomFloat)(flLow, flHigh);
}
void pfnSetView(const edict_t *pClient, const edict_t *pViewent )
{
   (*g_engfuncs.pfnSetView)(pClient, pViewent);
}
float pfnTime( void )
{
   return (*g_engfuncs.pfnTime)();
}
void pfnCrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
   (*g_engfuncs.pfnCrosshairAngle)(pClient, pitch, yaw);
}
byte *pfnLoadFileForMe(char *filename, int *pLength)
{
   return (*g_engfuncs.pfnLoadFileForMe)(filename, pLength);
}
void pfnFreeFile(void *buffer)
{
   (*g_engfuncs.pfnFreeFile)(buffer);
}
void pfnEndSection(const char *pszSectionName)
{
   (*g_engfuncs.pfnEndSection)(pszSectionName);
}
int pfnCompareFileTime(char *filename1, char *filename2, int *iCompare)
{
   return (*g_engfuncs.pfnCompareFileTime)(filename1, filename2, iCompare);
}
void pfnGetGameDir(char *szGetGameDir)
{
   (*g_engfuncs.pfnGetGameDir)(szGetGameDir);
}
void pfnCvar_RegisterVariable(cvar_t *variable)
{
   (*g_engfuncs.pfnCvar_RegisterVariable)(variable);
}
void pfnFadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
   (*g_engfuncs.pfnFadeClientVolume)(pEdict, fadePercent, fadeOutSeconds, holdTime, fadeInSeconds);
}
void pfnSetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed)
{
   (*g_engfuncs.pfnSetClientMaxspeed)(pEdict, fNewMaxspeed);
}
edict_t * pfnCreateFakeClient(const char *netname)
{
   return (*g_engfuncs.pfnCreateFakeClient)(netname);
}
void pfnRunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec )
{
   (*g_engfuncs.pfnRunPlayerMove)(fakeclient, viewangles, forwardmove, sidemove, upmove, buttons, impulse, msec);
}
int pfnNumberOfEntities(void)
{
   return (*g_engfuncs.pfnNumberOfEntities)();
}
char* pfnGetInfoKeyBuffer(edict_t *e)
{
   return (*g_engfuncs.pfnGetInfoKeyBuffer)(e);
}
char* pfnInfoKeyValue(char *infobuffer, char *key)
{
   return (*g_engfuncs.pfnInfoKeyValue)(infobuffer, key);
}
void pfnSetKeyValue(char *infobuffer, char *key, char *value)
{
   (*g_engfuncs.pfnSetKeyValue)(infobuffer, key, value);
}
void pfnSetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value)
{
	if (FStrEq(key, "model"))
	{

		// DISABLE ALL MODEL COMMANDS

		// see if we are allowed to do this
//		if ( INDEXENT(clientIndex)->v.fuser1 != 1 )
		//{
			//(*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, value);

		//(*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, "barney");
		//}
	}
	else
	{
		(*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, value);
	}

}
int pfnIsMapValid(char *filename)
{
   return (*g_engfuncs.pfnIsMapValid)(filename);
}
void pfnStaticDecal( const float *origin, int decalIndex, int entityIndex, int modelIndex )
{
   (*g_engfuncs.pfnStaticDecal)(origin, decalIndex, entityIndex, modelIndex);
}
int pfnPrecacheGeneric(char* s)
{
   return (*g_engfuncs.pfnPrecacheGeneric)(s);
}
int pfnGetPlayerUserId(edict_t *e )
{
   return (*g_engfuncs.pfnGetPlayerUserId)(e);
}
void pfnBuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
   (*g_engfuncs.pfnBuildSoundMsg)(entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
}
int pfnIsDedicatedServer(void)
{
   return (*g_engfuncs.pfnIsDedicatedServer)();
}
cvar_t* pfnCVarGetPointer(const char *szVarName)
{
   return (*g_engfuncs.pfnCVarGetPointer)(szVarName);
}
unsigned int pfnGetPlayerWONId(edict_t *e)
{
   return (*g_engfuncs.pfnGetPlayerWONId)(e);
}


// new stuff for SDK 2.0

void pfnInfo_RemoveKey(char *s, const char *key)
{
   (*g_engfuncs.pfnInfo_RemoveKey)(s, key);
}
const char *pfnGetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
   return (*g_engfuncs.pfnGetPhysicsKeyValue)(pClient, key);
}
void pfnSetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
   (*g_engfuncs.pfnSetPhysicsKeyValue)(pClient, key, value);
}
const char *pfnGetPhysicsInfoString(const edict_t *pClient)
{
   return (*g_engfuncs.pfnGetPhysicsInfoString)(pClient);
}
unsigned short pfnPrecacheEvent(int type, const char *psz)
{
   return (*g_engfuncs.pfnPrecacheEvent)(type, psz);
}
void pfnPlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay,
   float *origin, float *angles, float fparam1,float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
   (*g_engfuncs.pfnPlaybackEvent)(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}
unsigned char *pfnSetFatPVS(float *org)
{
   return (*g_engfuncs.pfnSetFatPVS)(org);
}
unsigned char *pfnSetFatPAS(float *org)
{
   return (*g_engfuncs.pfnSetFatPAS)(org);
}
int pfnCheckVisibility(const edict_t *entity, unsigned char *pset)
{
   return (*g_engfuncs.pfnCheckVisibility)(entity, pset);
}
void pfnDeltaSetField(struct delta_s *pFields, const char *fieldname)
{
   (*g_engfuncs.pfnDeltaSetField)(pFields, fieldname);
}
void pfnDeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
   (*g_engfuncs.pfnDeltaUnsetField)(pFields, fieldname);
}
void pfnDeltaAddEncoder(char *name, void (*conditionalencode)( struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
   (*g_engfuncs.pfnDeltaAddEncoder)(name, conditionalencode);
}
int pfnGetCurrentPlayer(void)
{
   return (*g_engfuncs.pfnGetCurrentPlayer)();
}
int pfnCanSkipPlayer(const edict_t *player)
{
   return (*g_engfuncs.pfnCanSkipPlayer)(player);
}
int pfnDeltaFindField(struct delta_s *pFields, const char *fieldname)
{
   return (*g_engfuncs.pfnDeltaFindField)(pFields, fieldname);
}
void pfnDeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
   (*g_engfuncs.pfnDeltaSetFieldByIndex)(pFields, fieldNumber);
}
void pfnDeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
   (*g_engfuncs.pfnDeltaUnsetFieldByIndex)(pFields, fieldNumber);
}
void pfnSetGroupMask(int mask, int op)
{
   (*g_engfuncs.pfnSetGroupMask)(mask, op);
}
int pfnCreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
   return (*g_engfuncs.pfnCreateInstancedBaseline)(classname, baseline);
}
void pfnCvar_DirectSet(struct cvar_s *var, char *value)
{
   (*g_engfuncs.pfnCvar_DirectSet)(var, value);
}
void pfnForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
   (*g_engfuncs.pfnForceUnmodified)(type, mins, maxs, filename);
}
void pfnGetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
   (*g_engfuncs.pfnGetPlayerStats)(pClient, ping, packet_loss);
}

void pfnAddServerCommand( char *cmd_name, void (*function) (void) )
{
   (*g_engfuncs.pfnAddServerCommand)(cmd_name, function);
}


qboolean	pfnVoice_GetClientListening(int iReceiver, int iSender)
{
	return (*g_engfuncs.pfnVoice_GetClientListening)(iReceiver, iSender);
}

qboolean	pfnVoice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
{
	return (*g_engfuncs.pfnVoice_SetClientListening)(iReceiver, iSender, bListen);
}

const char *(pfnGetPlayerAuthId)               ( edict_t *e )
{
	return (*g_engfuncs.pfnGetPlayerAuthId)(e);
}
void* pfnSequenceGet( const char* fileName, const char* entryName )
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSequenceGet:\n"); fclose(fp); }
   return (*g_engfuncs.pfnSequenceGet)(fileName, entryName);
}
void* pfnSequencePickSentence( const char* groupName, int pickMethod, int *picked )
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnSequencePickSentence:\n"); fclose(fp); }
   return (*g_engfuncs.pfnSequencePickSentence)(groupName, pickMethod, picked);
}
int pfnGetFileSize( char *filename )
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetFileSize:\n"); fclose(fp); }
   return (*g_engfuncs.pfnGetFileSize)(filename);
}
unsigned int pfnGetApproxWavePlayLen(const char *filepath)
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetApproxWavePlayLen:\n"); fclose(fp); }
   return (*g_engfuncs.pfnGetApproxWavePlayLen)(filepath);
}
int pfnIsCareerMatch( void )
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnIsCareerMatch:\n"); fclose(fp); }
   return (*g_engfuncs.pfnIsCareerMatch)();
}
int pfnGetLocalizedStringLength(const char *label)
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetLocalizedStringLength:\n"); fclose(fp); }
   return (*g_engfuncs.pfnGetLocalizedStringLength)(label);
}
void pfnRegisterTutorMessageShown(int mid)
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnRegisterTutorMessageShown:\n"); fclose(fp); }
   (*g_engfuncs.pfnRegisterTutorMessageShown)(mid);
}
int pfnGetTimesTutorMessageShown(int mid)
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnGetTimesTutorMessageShown:\n"); fclose(fp); }
   return (*g_engfuncs.pfnGetTimesTutorMessageShown)(mid);
}
void pfnProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnProcessTutorMessageDecayBuffer:\n"); fclose(fp); }
   (*g_engfuncs.pfnProcessTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnConstructTutorMessageDecayBuffer:\n"); fclose(fp); }
   (*g_engfuncs.pfnConstructTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnResetTutorMessageDecayData( void )
{
   if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnResetTutorMessageDecayData:\n"); fclose(fp); }
   (*g_engfuncs.pfnResetTutorMessageDecayData)();
}
