#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "basemonster.h"
#include "entity_state.h"
#include "vector.h"
#include "bd.h"
#include "studio.h"
#include "bot.h"
#include "avdll.h"
//#include "shake.h"


//new declares
#define BD_FOLLOW_MIN 96
#define BD_FOLLOW_MAX 512
#define BD_FOLLOW_SPEED 512

extern bool AdminLoggedIn[33];

float lastPush[33];


extraentinfo_t EntInfo[33];

int precachedmodels[32];
int precachedsounds[32];

float logtimeout;

int ignore_command = 0;
int tx, ty, counter;
char tempText[1024];
edict_t *tEntity;
CBaseEntity *ctEntity;
TraceResult tr;
Vector tVector;
Vector tVec1, tVec2, tVec3;



cvar_t ds_skydiving = {"ds_skydiving","0"};
cvar_t ds_logallcommands = {"ds_logallcommands","0" };
cvar_t ds_logfornext = {"ds_logfornext","1" };
cvar_t ds_adminlevel = {"ds_adminlevel","1" };
cvar_t ds_wenisreflects = {"ds_wenisreflects","10" };

cvar_t ds_superjump = {"ds_superjump","1" };
cvar_t ds_speedburst = {"ds_speedburst","1" };
cvar_t ds_walljump = {"ds_walljump","1" };
cvar_t ds_gravity = {"ds_gravity","1" };
cvar_t ds_ghost = {"ds_ghost","0" };
cvar_t ds_bounce = {"ds_bounce","1" };
cvar_t ds_explode = {"ds_explode","1" };
cvar_t ds_drunk = {"ds_drunk","1" };
cvar_t ds_kaboom = {"ds_kaboom","1" };
cvar_t ds_followmode = {"ds_followmode","1" };
cvar_t ds_dooropen = {"ds_dooropen","1" };
cvar_t ds_jetpack = {"ds_jetpack","1" };
cvar_t ds_trail = {"ds_trail","1" };
cvar_t ds_splat = {"ds_splat","1" };
cvar_t ds_push = {"ds_push","1" };
cvar_t ds_hook = {"ds_hook","1" };
cvar_t ds_martyr = {"ds_martyr","1" };
cvar_t ds_firework = {"ds_firework","1" };
//cvar_t ds_quadme = {"ds_quadme","5000" };
//cvar_t ds_petxentree = {"ds_petxentree","20000" };
cvar_t ds_telepr0t = {"ds_telepr0t","1" };
cvar_t ds_humiliate = {"ds_humiliate","1" };
cvar_t ds_l33tbeam = {"ds_l33tbeam","1" };

//new
//cvar_t ds_freezeray = {"ds_freeze","1" };
cvar_t ds_give =  {"ds_give","1" };



void ExplosionCreate( const Vector &center, const Vector &angles, edict_t *pOwner) {

	KeyValueData	kvd;
	char			buf[128];
    edict_t *pExp;
    pExp = CREATE_NAMED_ENTITY(MAKE_STRING("env_explosion"));
    pExp->v.origin=center;
    pExp->v.angles=angles;

	sprintf( buf, "1200");
	kvd.szKeyName = "iMagnitude";
	kvd.szValue = buf;
	DispatchKeyValue( pExp, &kvd );

	DispatchSpawn(pExp);
	DispatchUse(pExp,NULL);

}


void hextostring(char *hexstring) {

	const char *hexchar = "0123456789abcdef";
    int length = strlen(hexstring);
    int counter;
    int c2;
    // if the string's length is even, somthing is wrong and it's not a proper hex string, so return

    if((!fmod(length,2))||length<3) return;

    for(counter=0;counter<length/2;counter++) {

    	for(c2=0;c2< (int)strlen(hexchar);c2++) {

            if(tolower(hexstring[counter*2+1])==hexchar[c2])

            	hexstring[counter]=strlen(hexchar)*c2;
			}

    	for(c2=0;c2<(int)strlen(hexchar);c2++) {

            if(tolower(hexstring[counter*2+2])==hexchar[c2])

            	hexstring[counter]+=c2;

            }

    }

    hexstring[length/2]=0;

}


//start of Bills commands..
void bd_superjump ( edict_t *pEntity ) {

	if ((pEntity->v.flags & FL_ONGROUND)&&CVAR_GET_FLOAT("ds_superjump")!=0&&(!pEntity->v.deadflag)) {
		
		pEntity->v.velocity.x*=2;
		pEntity->v.velocity.y*=2;
		pEntity->v.velocity.z+=800;

        ignore_command=1;

	} else if (EntInfo[ENTINDEX(pEntity)].level>=1&&(!pEntity->v.deadflag)) {

		pEntity->v.velocity.x*=2;
		pEntity->v.velocity.y*=2;
		pEntity->v.velocity.z+=800;

        ignore_command=1;

	}

}


void bd_speedburst ( edict_t *pEntity ) {

	if(CVAR_GET_FLOAT("ds_speedburst")!=0&&EntInfo[ENTINDEX(pEntity)].speedbursttimer==0&&(!pEntity->v.deadflag)) {
		
		pEntity->v.velocity.x*=4;
		pEntity->v.velocity.y*=4;
        EntInfo[ENTINDEX(pEntity)].speedbursttimer=gpGlobals->time;

        ignore_command=1;

	}

}


void bd_ghost ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_ghost")!=0&&(!pEntity->v.deadflag)) {

		if (pEntity->v.rendermode != kRenderTransAdd) {
    	
			pEntity->v.rendermode = kRenderTransAdd;
			pEntity->v.renderfx = kRenderFxDistort;
			pEntity->v.renderamt = 255;
			ClientPrint(VARS(pEntity), HUD_PRINTTALK, "> ghost on\n");

		} else {

			pEntity->v.rendermode = kRenderNormal;
			pEntity->v.renderfx = kRenderFxNone;
			pEntity->v.renderamt = 0;
			ClientPrint(VARS(pEntity), HUD_PRINTTALK, "> ghost off\n");

		}
	
        ignore_command=1;

	}
	
}


void bd_bounce ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_bounce")!=0) {

    	EntInfo[ENTINDEX(pEntity)].bouncemode=1-EntInfo[ENTINDEX(pEntity)].bouncemode;
      	sprintf(tempText,"> Bounce mode: %s\n", EntInfo[ENTINDEX(pEntity)].bouncemode?"ON":"OFF");
		ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

        ignore_command=1;

	}

}


void bd_explode ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_explode")!=0&&(!pEntity->v.deadflag)) {

    	ExplosionCreate(pEntity->v.origin,pEntity->v.angles,NULL);

		if (EntInfo[ENTINDEX(pEntity)].level<1) 

	        ClientKill(pEntity);

        ignore_command=1;

	}

}

/*
void bd_petxentree ( edict_t *pEntity ) {

    if(EntInfo[ENTINDEX(pEntity)].level>=CVAR_GET_FLOAT("ds_petxentree")&&(!pEntity->v.deadflag)&&(pEntity->v.flags & FL_ONGROUND)) {

    	if(EntInfo[ENTINDEX(pEntity)].xentreeindex==0) {

	   		UTIL_MakeVectors ( pEntity->v.v_angle );
	        tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("xen_tree"));
	        tEntity->v.origin=pEntity->v.origin+(Vector(gpGlobals->v_forward.x,gpGlobals->v_forward.y,0).Normalize()*64);
	        tEntity->v.angles=pEntity->v.angles;
            tEntity->v.spawnflags|=SF_NORESPAWN;
            tEntity->v.movetype = MOVETYPE_PUSH;
	        DispatchSpawn(tEntity);
	        DROP_TO_FLOOR(tEntity);
           	sprintf(tempText,"> You got a xen tree.\n");
		   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);
	        EntInfo[ENTINDEX(pEntity)].xentreeindex = ENTINDEX(tEntity);

        } else {

	   		UTIL_MakeVectors ( pEntity->v.v_angle );
	        ctEntity=NULL;
            tx=0;
    	    ctEntity=UTIL_FindEntityInSphere( ctEntity, INDEXENT(EntInfo[ENTINDEX(pEntity)].xentreeindex)->v.origin, 8192 );

        	while(ctEntity!=NULL) {

	            if(ENTINDEX(ctEntity->pev->owner)==EntInfo[ENTINDEX(pEntity)].xentreeindex) {

//	                INDEXENT(EntInfo[ENTINDEX(pEntity)].xentreeindex)->v.flags |= FL_KILLME;
					ctEntity->pev->origin=pEntity->v.origin+(Vector(gpGlobals->v_forward.x,gpGlobals->v_forward.y,0).Normalize()*64);
			        ctEntity->pev->angles=Vector(0,pEntity->v.angles.y,pEntity->v.angles.z);
                    DROP_TO_FLOOR(ENT(ctEntity->pev));
					INDEXENT(EntInfo[ENTINDEX(pEntity)].xentreeindex)->v.origin=pEntity->v.origin+(Vector(gpGlobals->v_forward.x,gpGlobals->v_forward.y,0).Normalize()*64);
					INDEXENT(EntInfo[ENTINDEX(pEntity)].xentreeindex)->v.angles=Vector(0,pEntity->v.angles.y,pEntity->v.angles.z);
                    DROP_TO_FLOOR(INDEXENT(EntInfo[ENTINDEX(pEntity)].xentreeindex));
	              	sprintf(tempText,"> You moved your xen tree.\n");
				   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);
                    tx=1;

	            }

	    	    ctEntity=UTIL_FindEntityInSphere( ctEntity, INDEXENT(EntInfo[ENTINDEX(pEntity)].xentreeindex)->v.origin, 512 );

            }

            if(tx==0) {

              	sprintf(tempText,"> Your xen tree got screwed up, so now it's stuck.\n");
			   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);
//      	        EntInfo[ENTINDEX(pEntity)].xentreeindex = 0;

            }

        }

        ignore_command=1;

	}

}
*/
/*
void bd_quadme ( edict_t *pEntity ) {

   if(EntInfo[ENTINDEX(pEntity)].level>=CVAR_GET_FLOAT("ds_quadme")&&(!pEntity->v.deadflag)) {

    	if(gpGlobals->time - EntInfo[ENTINDEX(pEntity)].quadtimer > 60) {

        	if(pEntity->v.flags & FL_ONGROUND) {

	            tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("item_artifact_super_damage"));
	            tEntity->v.origin=pEntity->v.origin;
	            tEntity->v.spawnflags|=SF_NORESPAWN;
	            DispatchSpawn(tEntity);
		       	sprintf(tempText,"> Quad damage!\n");
			   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);
	            EntInfo[ENTINDEX(pEntity)].quadtimer = gpGlobals->time;

            } else {

		       	sprintf(tempText,"> You must be on the ground to use this command.\n");
			   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

            }

        } else {

       		sprintf(tempText,"> You must wait %d seconds before using this command.\n", (int)(60 - (gpGlobals->time - EntInfo[ENTINDEX(pEntity)].quadtimer)));
		   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

        }

        ignore_command=1;

	}

}
*/

void bd_l33tbeam ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_l33tbeam")!=0&&(!pEntity->v.deadflag)) {

    	if(gpGlobals->time - EntInfo[ENTINDEX(pEntity)].wenistimer > 20) {

		    UTIL_MakeVectors(pEntity->v.v_angle);
		    tVec1 = GetGunPosition(pEntity);
		    tVec3 = tVec1;
		    tVector = Vector(RANDOM_LONG(0,255), RANDOM_LONG(0,255), RANDOM_LONG(0,255));

		    for(counter=0;counter<CVAR_GET_FLOAT("ds_wenisreflects");counter++) {

			    tVec2 = tVec3;
			    UTIL_TraceReflect(tVec1, tVec1 + gpGlobals->v_forward*8192, dont_ignore_monsters, pEntity, &tr, counter+1);
			    tVec3 = tr.vecEndPos;

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

					WRITE_BYTE( TE_BEAMPOINTS );
	    	        WRITE_COORD(tVec2.x);
		            WRITE_COORD(tVec2.y);
	    	        WRITE_COORD(tVec2.z);
		            WRITE_COORD(tVec3.x);
	        	    WRITE_COORD(tVec3.y);
		            WRITE_COORD(tVec3.z);
		            WRITE_SHORT(precachedmodels[0]);
		            WRITE_BYTE(1);
		            WRITE_BYTE(10);
		            WRITE_BYTE(10);
		            WRITE_BYTE(10);
		            WRITE_BYTE(0);
		            WRITE_BYTE(tVector.x);WRITE_BYTE(tVector.y);WRITE_BYTE(tVector.z);
		            WRITE_BYTE(RANDOM_LONG(127,255));
		            WRITE_BYTE(1);

				MESSAGE_END();
				
				if (EntInfo[ENTINDEX(pEntity)].level<1) 

					EntInfo[ENTINDEX(pEntity)].wenistimer = gpGlobals->time;

		    }

		} else {

       		sprintf(tempText,"> You must wait %d seconds before using this command.\n", (int)(20 - (gpGlobals->time - EntInfo[ENTINDEX(pEntity)].wenistimer)));
		   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

        }

        ignore_command=1;

	}

}


void bd_splat ( edict_t *pEntity ) {

	if(CVAR_GET_FLOAT("ds_splat")!=0&&(!pEntity->v.deadflag)&&(pEntity->v.flags & FL_ONGROUND)) {

		pEntity->v.flags &= ~FL_ONGROUND;
        pEntity->v.health = 1;
    	pEntity->v.velocity.z = -9999999;

        ignore_command=1;

	}

}


void bd_firework ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_firework")!=0&&(!pEntity->v.deadflag)&&(!EntInfo[ENTINDEX(pEntity)].fireworktimer)) {

    	EntInfo[ENTINDEX(pEntity)].fireworktimer = gpGlobals->time;
        EntInfo[ENTINDEX(pEntity)].fireworkspeed = RANDOM_LONG(600,1000);
        pEntity->v.velocity.z = EntInfo[ENTINDEX(pEntity)].fireworkspeed;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	    	WRITE_BYTE( TE_KILLBEAM );
			WRITE_SHORT( ENTINDEX(pEntity) );

		MESSAGE_END();

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMFOLLOW );

			WRITE_SHORT( ENTINDEX(pEntity) );
			WRITE_SHORT( precachedmodels[0] );

			WRITE_BYTE( 40 ); // life
			WRITE_BYTE( 10 );
			WRITE_BYTE( RANDOM_LONG(0,255) ); // r
			WRITE_BYTE( RANDOM_LONG(0,255) ); // g
			WRITE_BYTE( RANDOM_LONG(0,255) ); // b
			WRITE_BYTE( RANDOM_LONG(128,255) ); // brightness

		MESSAGE_END();

        ignore_command=1;

	}

}


void bd_martyr ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_martyr")!=0&&(!pEntity->v.deadflag)) {

        pEntity->v.effects|=EF_LIGHT;

		if (EntInfo[ENTINDEX(pEntity)].level<1) 

		    	ClientKill(pEntity);

        pEntity->v.velocity=Vector(0,0,100);
		UTIL_MakeVectors ( pEntity->v.v_angle );

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(pEntity->v.origin.x);
            WRITE_COORD(pEntity->v.origin.y);
            WRITE_COORD(pEntity->v.origin.z-32);
            WRITE_COORD(pEntity->v.origin.x);
            WRITE_COORD(pEntity->v.origin.y);
            WRITE_COORD(pEntity->v.origin.z+512);
            WRITE_SHORT(precachedmodels[0]);
            WRITE_BYTE(1);
            WRITE_BYTE(10);
            WRITE_BYTE(80);
            WRITE_BYTE(80);
            WRITE_BYTE(0);
            WRITE_BYTE(255);WRITE_BYTE(255);WRITE_BYTE(255);
            WRITE_BYTE(255);
            WRITE_BYTE(1);

		MESSAGE_END();

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

			WRITE_BYTE( TE_BEAMPOINTS );
            WRITE_COORD(pEntity->v.origin.x-(gpGlobals->v_right*256).x);
            WRITE_COORD(pEntity->v.origin.y-(gpGlobals->v_right*256).y);
            WRITE_COORD(pEntity->v.origin.z+384);
            WRITE_COORD(pEntity->v.origin.x+(gpGlobals->v_right*256).x);
            WRITE_COORD(pEntity->v.origin.y+(gpGlobals->v_right*256).y);
            WRITE_COORD(pEntity->v.origin.z+384);
            WRITE_SHORT(precachedmodels[0]);
            WRITE_BYTE(1);
            WRITE_BYTE(10);
            WRITE_BYTE(80);
            WRITE_BYTE(60);
            WRITE_BYTE(0);
            WRITE_BYTE(255);WRITE_BYTE(255);WRITE_BYTE(255);
            WRITE_BYTE(255);
            WRITE_BYTE(1);

		MESSAGE_END();

		ctEntity=NULL;
        ctEntity=UTIL_FindEntityInSphere(ctEntity,pEntity->v.origin,4096);

        while(ctEntity!=NULL) {

        	if(FStrEq(STRING(ctEntity->pev->classname),"player")&&(ctEntity->entindex()!=ENTINDEX(pEntity)) && (ctEntity->pev->takedamage != DAMAGE_NO)) {

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

					WRITE_BYTE( TE_BEAMPOINTS );
		            WRITE_COORD(pEntity->v.origin.x);
		            WRITE_COORD(pEntity->v.origin.y);
		            WRITE_COORD(pEntity->v.origin.z);
		            WRITE_COORD(ctEntity->pev->origin.x);
		            WRITE_COORD(ctEntity->pev->origin.y);
		            WRITE_COORD(ctEntity->pev->origin.z);
		            WRITE_SHORT(precachedmodels[0]);
		            WRITE_BYTE(1);
		            WRITE_BYTE(10);
		            WRITE_BYTE(10);
		            WRITE_BYTE(20);
		            WRITE_BYTE(0);
		            WRITE_BYTE(255);WRITE_BYTE(0);WRITE_BYTE(0);
		            WRITE_BYTE(255);
		            WRITE_BYTE(1);

				MESSAGE_END();

	            ctEntity->pev->health = 500;
	            ctEntity->pev->armorvalue = 500;
            }

        ctEntity=UTIL_FindEntityInSphere(ctEntity,pEntity->v.origin,512);

        }

        ignore_command=1;

	}

}


void bd_dooropen ( edict_t *pEntity ) {

    if(EntInfo[ENTINDEX(pEntity)].level>=1&&CVAR_GET_FLOAT("ds_dooropen")!=0&&(!pEntity->v.deadflag)) {

   		UTIL_MakeVectors ( pEntity->v.v_angle );
    	tVector = GetGunPosition( pEntity );
		UTIL_TraceLine( tVector, tVector+(gpGlobals->v_forward*8192), dont_ignore_monsters, pEntity, &tr );
        tEntity=tr.pHit;

        if(!FNullEnt(tEntity)) {

        	if(FStrEq(STRING(tEntity->v.classname),"func_door")||FStrEq(STRING(tEntity->v.classname),"func_door_rotating")) {

	          	sprintf(tempText,"> Open sesame!\n");
		    	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);
				DispatchUse( tEntity, pEntity );

            }

        }

        ignore_command=1;

	}

}


void bd_telepr0t ( edict_t *pEntity ) {

	if((pEntity->v.flags & FL_ONGROUND)&&EntInfo[ENTINDEX(pEntity)].level>=1&&CVAR_GET_FLOAT("ds_telepr0t")!=0&&(!pEntity->v.deadflag)) {

   		UTIL_MakeVectors ( pEntity->v.v_angle );
    	tVector = GetGunPosition( pEntity );
		UTIL_TraceLine( tVector, tVector+(gpGlobals->v_forward*8192), dont_ignore_monsters, pEntity, &tr );
        pEntity->v.origin = tr.vecEndPos + (tr.vecPlaneNormal*72);

        ignore_command=1;

	}

}


void bd_push ( edict_t *pEntity ) {

   if (CVAR_GET_FLOAT("ds_push")!=0 && (!pEntity->v.deadflag)) {

   		UTIL_MakeVectors ( pEntity->v.v_angle );
    	tVector = GetGunPosition( pEntity );
		UTIL_TraceLine( tVector, tVector+(gpGlobals->v_forward*64), dont_ignore_monsters, pEntity, &tr );
        tEntity=tr.pHit;

        if (!FNullEnt(tEntity)) {


			if (gpGlobals->time - lastPush[ENTINDEX(pEntity)] < 1)
			{

					//char msg[80];
					//sprintf(msg,"Maximum number of PUSHes per second is 1. You are PUSH SPAMMING.\n");
					//ClientPrint( VARS(pEntity), HUD_PRINTCONSOLE, msg);

					//sprintf(msg, "kick # %i\n", GETPLAYERUSERID(pEntity));
					//SERVER_COMMAND(msg);
					return;

			}


			lastPush[ENTINDEX(pEntity)] = gpGlobals->time;

        	if ( FStrEq(STRING(tEntity->v.classname),"player")) {

				if(EntInfo[ENTINDEX(pEntity)].level >= EntInfo[ENTINDEX(tEntity)].level && !AdminLoggedIn[ENTINDEX(tEntity)])

            		tEntity->v.velocity=tEntity->v.velocity+(gpGlobals->v_forward*(RANDOM_LONG(3000,5000)));

                else

            		pEntity->v.velocity=pEntity->v.velocity+(gpGlobals->v_forward*(RANDOM_LONG(500,4500)));

			} else if(FStrEq(STRING(tEntity->v.classname),"func_pushable")) {

				if(EntInfo[ENTINDEX(pEntity)].level>=1)

					tEntity->v.velocity=tEntity->v.velocity+(gpGlobals->v_forward*(RANDOM_LONG(2000,6000)));

				else

					tEntity->v.velocity=tEntity->v.velocity+(gpGlobals->v_forward*(RANDOM_LONG(1000,3000)));

			}

        }

        ignore_command=1;

	}

}


void bd_drunk ( edict_t *pEntity ) {

   if(CVAR_GET_FLOAT("ds_drunk")!=0) {

    	EntInfo[ENTINDEX(pEntity)].drunkmode=1-EntInfo[ENTINDEX(pEntity)].drunkmode;
      	sprintf(tempText,"> Drunk mode: %s\n", EntInfo[ENTINDEX(pEntity)].drunkmode?"ON":"OFF");
	   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);
        pEntity->v.fov=90;

        ignore_command=1;

	}

}


void bd_followmode ( edict_t *pEntity ) {

    if(EntInfo[ENTINDEX(pEntity)].level>=1&&CVAR_GET_FLOAT("ds_followmode")!=0) {

    	EntInfo[ENTINDEX(pEntity)].followmode=1-EntInfo[ENTINDEX(pEntity)].followmode;
      	sprintf(tempText,"> Follow mode: %s\n", EntInfo[ENTINDEX(pEntity)].followmode?"ON":"OFF");
	   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

        ignore_command=1;

	}

}

void bd_hookon ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_hook")!=0&&(!pEntity->v.deadflag)) {

   		UTIL_MakeVectors ( pEntity->v.v_angle );
    	tVector = GetGunPosition( pEntity );
		UTIL_TraceLine( tVector, tVector+(gpGlobals->v_forward*8192), dont_ignore_monsters, pEntity, &tr );
        tEntity=tr.pHit;
        
		if(!FNullEnt(tEntity)) {

        	if((FStrEq(STRING(tEntity->v.classname),"player") && tEntity->v.takedamage!=DAMAGE_NO)||FStrEq(STRING(tEntity->v.classname),"func_train")||FStrEq(STRING(tEntity->v.classname),"func_tracktrain")) {

				EntInfo[ENTINDEX(pEntity)].hooktarget = ENTINDEX(tEntity);
				EntInfo[ENTINDEX(pEntity)].hooktimer = gpGlobals->time;
				
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		    		WRITE_BYTE( TE_BEAMENTS );
					WRITE_SHORT( ENTINDEX(pEntity) );
					WRITE_SHORT( ENTINDEX(tEntity) );
					WRITE_SHORT( precachedmodels[0] );
					WRITE_BYTE(1);
					WRITE_BYTE(10);
					WRITE_BYTE(40);
					WRITE_BYTE(20);
					WRITE_BYTE(0);
					WRITE_BYTE(0); WRITE_BYTE(0); WRITE_BYTE(255);
					WRITE_BYTE(127);
					WRITE_BYTE(10);
				MESSAGE_END();

			}

        }

        ignore_command=1;

	}

}


void bd_hookoff ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_hook")!=0) {

    	EntInfo[ENTINDEX(pEntity)].hooktarget = 0;
    	EntInfo[ENTINDEX(pEntity)].hooktimer = 0;

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
	    	WRITE_BYTE( TE_KILLBEAM );
			WRITE_SHORT( ENTINDEX(pEntity) );
		MESSAGE_END();

        ignore_command=1;

	}

}


void bd_jetpackon ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_jetpack")!=0&&(!pEntity->v.deadflag)) {

    	EntInfo[ENTINDEX(pEntity)].jetpack = 1;

        ignore_command=1;

	}

}


void bd_jetpackoff ( edict_t *pEntity ) {

    if(!pEntity->v.deadflag) {

    	EntInfo[ENTINDEX(pEntity)].jetpack = 0;
        ignore_command=1;
	}
}


void bd_walljumpon ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_walljump")!=0&&(!pEntity->v.deadflag)) {

    	EntInfo[ENTINDEX(pEntity)].walljump = 1;

        ignore_command=1;

	}

}


void bd_walljumpoff ( edict_t *pEntity ) {

    if(!pEntity->v.deadflag) {

    	EntInfo[ENTINDEX(pEntity)].walljump = 0;

        ignore_command=1;

	}

}


void bd_kaboom ( edict_t *pEntity ) {

    if(EntInfo[ENTINDEX(pEntity)].level>=1&&CVAR_GET_FLOAT("ds_kaboom")!=0&&(!pEntity->v.deadflag)) {

        for(ty=-1;ty<2;ty++) {

            for(tx=-1;tx<2;tx++) {

                tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("tf_gl_grenade"));
                tEntity->v.origin = pEntity->v.origin + Vector(tx*64, ty*64, 0);
                tEntity->v.velocity = Vector(0,0,20);
                tEntity->v.angles = pEntity->v.angles;
                DispatchSpawn(tEntity);

            }

        }

        // It's a great trick, but you can only do it once.

		if (EntInfo[ENTINDEX(pEntity)].level<1) {

        ClientKill(pEntity);

		}

        ignore_command=1;

	}

}


void bd_szcommands ( edict_t *pEntity ) {

   	sprintf(tempText,"> The following console commands are unique to this server:\n");
   	ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, tempText);

		//admin only commands
	if (EntInfo[ENTINDEX(pEntity)].level>=1) {

		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> ADMIN ONLY COMMANDS:\n");

		if (CVAR_GET_FLOAT("ds_dooropen")!=0)
			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> DOOROPEN\n");

		if (CVAR_GET_FLOAT("ds_telepr0t")!=0)
			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> TELEPR0T\n");

		if (CVAR_GET_FLOAT("ds_give")!=0)
			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> GIVE <weapon name>\n");

		if (CVAR_GET_FLOAT("ds_kaboom")!=0)
			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> KABOOM\n");

		if (CVAR_GET_FLOAT("ds_followmode")!=0)
			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> FOLLOWMODE\n");

//		if (CVAR_GET_FLOAT("ds_freeze")!=0)
//			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> FREEZE\n");
		
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "\n");

	
	}

	sprintf(tempText,"> COMMANDS:\n");
	ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, tempText);

	if (CVAR_GET_FLOAT("ds_superjump")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> SUPERJUMP\n");

	if (CVAR_GET_FLOAT("ds_walljump")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> +WALLJUMP\n");

	if (CVAR_GET_FLOAT("ds_splat")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> SPLAT\n");

	if (CVAR_GET_FLOAT("ds_explode")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> EXPLODE\n");

	if (CVAR_GET_FLOAT("ds_speedburst")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> SPEEDBURST\n");

	if (CVAR_GET_FLOAT("ds_firework")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> FIREWORK\n");

	if (CVAR_GET_FLOAT("ds_hook")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> +HOOK\n");

	if (CVAR_GET_FLOAT("ds_drunk")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> DRUNK\n");

	if (CVAR_GET_FLOAT("ds_bounce")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> BOUNCE\n");

	if (CVAR_GET_FLOAT("ds_ghost")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> GHOST\n");

	if (CVAR_GET_FLOAT("ds_push")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> PUSH\n");

	if (CVAR_GET_FLOAT("ds_martyr")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> MARTYR\n");

	if (CVAR_GET_FLOAT("ds_gravity")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> GRAVITY <percent>\n");

	if (CVAR_GET_FLOAT("ds_jetpack")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> +JETPACK\n");

	if (CVAR_GET_FLOAT("ds_trail")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> TRAIL <color>\n");

	if (CVAR_GET_FLOAT("ds_l33tbeam")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> L33TBEAM\n");

	if (CVAR_GET_FLOAT("ds_humiliate")!=0)
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> HUMILIATE\n");

	ignore_command=1;

}


void bd_humiliate ( edict_t *pEntity ) {

    if(CVAR_GET_FLOAT("ds_humiliate") != 0 && (!pEntity->v.deadflag)) 
	{

    	if(gpGlobals->time - EntInfo[ENTINDEX(pEntity)].humiltimer > 10) 
		{

	   		UTIL_MakeVectors ( pEntity->v.v_angle );
    		tVector = GetGunPosition( pEntity );
			UTIL_TraceLine( tVector, tVector+(gpGlobals->v_forward*8192), dont_ignore_monsters, pEntity, &tr );
	        tEntity=tr.pHit;

	        if(!FNullEnt(tEntity)) 
			{

	        	if (FStrEq(STRING(tEntity->v.classname),"player") && strlen(CMD_ARGV(1)) < 20) 
				{

                   	MESSAGE_BEGIN( MSG_ALL, REG_USER_MSG( "DeathMsg", -1 ) );
						WRITE_BYTE( ENTINDEX(pEntity) );		// the killer
						WRITE_BYTE( ENTINDEX(tEntity) );		// the victim
						WRITE_STRING( CMD_ARGV(1) );		// what they were killed by (should this be a string?)
					MESSAGE_END();

				if (EntInfo[ENTINDEX(pEntity)].level<1)		EntInfo[ENTINDEX(pEntity)].humiltimer = gpGlobals->time;

	            }

	        }

        } 
		else 
		{

       		sprintf(tempText,"> You must wait %d seconds before using this command.\n", (int)(10 - (gpGlobals->time - EntInfo[ENTINDEX(pEntity)].humiltimer)));
		   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

        }

        ignore_command=1;
	}

}


//disco's additional commands

void ds_givenameditem( edict_t *pEntity, const char *pszName ) {

	if(EntInfo[ENTINDEX(pEntity)].level>=1&&CVAR_GET_FLOAT("ds_give")!=0) {

		if ( stristr(pszName, "tf_weapon") !=0) {

			int istr = MAKE_STRING(pszName);

			edict_t	*pent;
	
			pent = CREATE_NAMED_ENTITY(istr);

			if ( FNullEnt( pent ) )	{

				ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> invalid item to give\n");
		
			} else {

				VARS( pent )->origin = pEntity->v.origin;
				pent->v.spawnflags |= SF_NORESPAWN;

				DispatchSpawn( pent );
				DispatchTouch( pent, ENT( pEntity ) );
	
			}

		} else {

			ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "> you did not specify a weapon\n");

		}

	}

}


void ds_grav ( edict_t *pEntity, const char *iGravAmt ) {

	if(CVAR_GET_FLOAT("ds_gravity")!=0) {

		if (FStrEq(iGravAmt, "0") || FStrEq(iGravAmt, "100")) {

			ClientPrint(VARS(pEntity), HUD_PRINTTALK, "> gravity set to 100%%\n");
			pEntity->v.gravity = 0;
	
		} else {

			float fGravity = (float)atoi(iGravAmt) / (float)100;

			pEntity->v.gravity = fGravity;

			sprintf(tempText,"> gravity set to %i %%\n", atoi(iGravAmt));
			ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

		}

        ignore_command=1;

    }

}


void ds_level( edict_t *pEntity, const char* iLevel ) {

	if (AdminLoggedIn[ENTINDEX(pEntity)])
	{

	
		EntInfo[ENTINDEX(pEntity)].level = (int)atof(iLevel);
		sprintf(tempText,"> %s's admin level is %i\n",STRING(pEntity->v.netname), EntInfo[ENTINDEX(pEntity)].level);
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, tempText );
	}
	else
	{
		ClientPrint(VARS(pEntity), HUD_PRINTCONSOLE, "Sorry, this is Not for you!\n" );
	}
}


void ds_trailme ( edict_t *pEntity, int mRed, int mGreen, int mBlue ) {

	if(CVAR_GET_FLOAT("ds_trail")!=0&&(!pEntity->v.deadflag)) {

		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	    	WRITE_BYTE( TE_KILLBEAM );
			WRITE_SHORT( ENTINDEX(pEntity) );

		MESSAGE_END();

		int iRed = mRed;
		int iGreen = mGreen;
		int iBlue = mBlue;
	
		
		if (iRed < 0)
		    iRed = 0;
		if (iRed > 255)
		    iRed = 255;
		if (iGreen < 0)
			iGreen = 0;
		if (iGreen > 255)
		    iGreen = 255;
		if (iBlue < 0)
		    iBlue = 0;
		if (iBlue > 255)
		    iBlue = 255;
		

		if(iRed != 0 || iGreen != 0 || iBlue != 0) {

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY ); //here we start a message which draws a beam behind a player

				WRITE_BYTE( TE_BEAMFOLLOW ); //sets the beam to follow the entity(player)
				WRITE_SHORT( ENTINDEX(pEntity) ); // entity
				WRITE_SHORT( precachedmodels[0] ); //model/sprite
				WRITE_BYTE( 20 ); //life
				WRITE_BYTE( 10 ); //width
				WRITE_BYTE( iRed ); //red
				WRITE_BYTE( iGreen ); //green
				WRITE_BYTE( iBlue ); //blue
				WRITE_BYTE( 255 ); //brightness

			MESSAGE_END(); //ends the message we started above

		} else if(iRed == 255 && iGreen == 255 && iBlue == 255) {

			//do nothing...
		
		} else {

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY ); //here we start a message which draws a beam behind a player

				WRITE_BYTE( TE_BEAMFOLLOW ); //sets the beam to follow the entity(player)
				WRITE_SHORT( ENTINDEX(pEntity) ); // entity
				WRITE_SHORT( precachedmodels[0] ); //model/sprite
				WRITE_BYTE( 20 ); //life
				WRITE_BYTE( 10 ); //width
				WRITE_BYTE( RANDOM_LONG(10, 250) ); //red
				WRITE_BYTE( RANDOM_LONG(10, 250) ); //green
				WRITE_BYTE( RANDOM_LONG(10, 250) ); //blue
				WRITE_BYTE( 255 ); //brightness

			MESSAGE_END(); //ends the message we started above

	       ignore_command=1;

		}

	}

}

/*
void ds_freeze( edict_t *pEntity ) {
  
    if(EntInfo[ENTINDEX(pEntity)].level>=1&&CVAR_GET_FLOAT("ds_freeze")!=0&&(!pEntity->v.deadflag)) {

    	if(gpGlobals->time - EntInfo[ENTINDEX(pEntity)].freeze > 10) {

	   		UTIL_MakeVectors ( pEntity->v.v_angle );
    		tVector = GetGunPosition( pEntity );
			UTIL_TraceLine( tVector, tVector+(gpGlobals->v_forward*8192), dont_ignore_monsters, pEntity, &tr );
	        tEntity=tr.pHit;

	        if(!FNullEnt(tEntity)) {

	        	if(FStrEq(STRING(tEntity->v.classname),"player")) {
    
					int m_vFreezeAngle = 0;
					float m_attackEndTime = 0;
					int m_flFreezeTime = gpGlobals->time + 10.0;
					
					MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, tEntity->v.origin );

						WRITE_BYTE( TE_BEAMCYLINDER );
						WRITE_COORD( tEntity->v.origin.x);//center of effect on x axis
						WRITE_COORD( tEntity->v.origin.y);//center of effect on y axis
						WRITE_COORD( tEntity->v.origin.z);//center of effect on z axis
						WRITE_COORD( tEntity->v.origin.x);//axis of effect on x axis
						WRITE_COORD( tEntity->v.origin.y + 20);//axis of effect on y axis
						WRITE_COORD( tEntity->v.origin.z + 420 ); // z axis and Radius of effect
						WRITE_SHORT( precachedmodels[9] );
						WRITE_BYTE( 0 ); // startframe
						WRITE_BYTE( 0 ); //framerate in 0.1''s
						WRITE_BYTE( 20 ); //Life in 0.1''s
						WRITE_BYTE( 32 ); //Line Width in .1 units
						WRITE_BYTE( 0 ); //Noise Amplitude in 0.01''s
						WRITE_BYTE( 43 );//r, g, b
						WRITE_BYTE( 49 );//r, g, b
						WRITE_BYTE( 217 );//r, g, b
						WRITE_BYTE( 200 ); // brightness
						WRITE_BYTE( 0 ); // speed

					MESSAGE_END();

					MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tEntity->v.origin );

						WRITE_BYTE( TE_EXPLOSION ); // This makes a dynamic light and the explosion sprites/sound
						WRITE_COORD( tEntity->v.origin.x ); // Send to PAS because of the sound
						WRITE_COORD( tEntity->v.origin.y );
						WRITE_COORD( tEntity->v.origin.z );
						WRITE_SHORT( precachedmodels[9] );
						WRITE_BYTE( (tEntity->v.dmg) * .60 ); // scale * 10
						WRITE_BYTE( 15 ); // framerate
						WRITE_BYTE( TE_EXPLFLAG_NOSOUND );

					MESSAGE_END();


					//UTIL_ScreenFade( (CBaseEntity *)tEntity, Vector(0,0,255), 2.0, 1.0, 128, FFADE_IN );
			
					//UTIL_EmitAmbientSound(ENT(0), tEntity->v.origin, "weapons/emp_1.wav", 1.0, ATTN_NORM, 0, 100);
			
					tEntity->v.rendermode = kRenderNormal;
					tEntity->v.renderfx = kRenderFxGlowShell;
					tEntity->v.rendercolor.x = 0;  // red
					tEntity->v.rendercolor.y = 0;  // green
					tEntity->v.rendercolor.z = 200; // blue
					tEntity->v.renderamt = 10;  // glow shell distance from entity
					tEntity->v.movetype = MOVETYPE_NONE;

					if (m_flFreezeTime > 0) {

						if (m_flFreezeTime <= gpGlobals->time) {

							tEntity->v.rendermode = kRenderNormal;
							tEntity->v.renderfx = kRenderFxNone;
							tEntity->v.renderamt = 0;
							tEntity->v.movetype = MOVETYPE_WALK;
							m_flFreezeTime = 0;

						}

					}

					if (EntInfo[ENTINDEX(pEntity)].level<1) 

						EntInfo[ENTINDEX(pEntity)].freezetimer = gpGlobals->time;

	            }

	        }

        } else {

       		sprintf(tempText,"> You must wait %d seconds before using this command.\n", (int)(10 - (gpGlobals->time - EntInfo[ENTINDEX(pEntity)].humiltimer)));
		   	ClientPrint(VARS(pEntity), HUD_PRINTTALK, tempText);

        }

        ignore_command=1;

	}

}
*/


void BDGameDLLInit( void ) {

	int counter;

	logtimeout = 0;

	CVAR_REGISTER(&ds_skydiving);
    CVAR_REGISTER(&ds_logallcommands);
    CVAR_REGISTER(&ds_logfornext);
	CVAR_REGISTER(&ds_adminlevel);
    CVAR_REGISTER(&ds_wenisreflects);

    CVAR_REGISTER(&ds_superjump);
    CVAR_REGISTER(&ds_speedburst);
    CVAR_REGISTER(&ds_walljump);
    CVAR_REGISTER(&ds_gravity);
	CVAR_REGISTER(&ds_ghost);
    CVAR_REGISTER(&ds_bounce);
    CVAR_REGISTER(&ds_explode);
    CVAR_REGISTER(&ds_drunk);
    CVAR_REGISTER(&ds_kaboom);
    CVAR_REGISTER(&ds_followmode);
    CVAR_REGISTER(&ds_dooropen);
    CVAR_REGISTER(&ds_jetpack);
	CVAR_REGISTER(&ds_trail);
    CVAR_REGISTER(&ds_splat);
    CVAR_REGISTER(&ds_push);
    CVAR_REGISTER(&ds_hook);
    CVAR_REGISTER(&ds_martyr);
    CVAR_REGISTER(&ds_firework);
    CVAR_REGISTER(&ds_humiliate);
	//CVAR_REGISTER(&ds_freezeray);
    CVAR_REGISTER(&ds_l33tbeam);
	CVAR_REGISTER(&ds_telepr0t);

	CVAR_REGISTER(&ds_give);
	

	for(counter=0; counter <= 32; counter++) {

		EntInfo[counter].lastinfoupdate=0;
		EntInfo[counter].level=0;
		EntInfo[counter].speedbursttimer=0;
		EntInfo[counter].bouncemode=0;
		EntInfo[counter].drunkmode=0;
		EntInfo[counter].followmode=0;
		EntInfo[counter].logging=0;
		EntInfo[counter].jetpack=0;
		EntInfo[counter].fireworktimer=0;
		EntInfo[counter].fireworkspeed=0;
		//EntInfo[counter].quadtimer=0;
		//EntInfo[counter].xentreeindex=0;
		EntInfo[counter].hooktarget=0;
		EntInfo[counter].hooktimer=0;
	    EntInfo[counter].walljump=0;
		EntInfo[counter].humiltimer=0;
		EntInfo[counter].wenistimer=0;
		//EntInfo[counter].freezetimer=0;

    }

    //NeoTFGameDLLInit();
}

void BDDispatchSpawn( edict_t *pent )
{

	int counter;
	//EntInfo[ENTINDEX(pent)].lastinfoupdate=0;

	char *pClassname = (char *)STRING(pent->v.classname);

	if (strcmp(pClassname, "worldspawn") == 0) {

		for(counter=0; counter <= 32; counter++) {

		    EntInfo[counter].lastinfoupdate=0;
			EntInfo[counter].level=0;
		    EntInfo[counter].speedbursttimer=0;
		    EntInfo[counter].bouncemode=0;
		    EntInfo[counter].drunkmode=0;
		    EntInfo[counter].followmode=0;
		    EntInfo[counter].logging=0;
		    EntInfo[counter].jetpack=0;
		    EntInfo[counter].fireworktimer=0;
		    EntInfo[counter].fireworkspeed=0;
		    //EntInfo[counter].quadtimer=0;
		    //EntInfo[counter].xentreeindex=0;
		    EntInfo[counter].hooktarget=0;
		    EntInfo[counter].hooktimer=0;
		    EntInfo[counter].walljump=0;
		    EntInfo[counter].humiltimer=0;
		    EntInfo[counter].wenistimer=0;
			//EntInfo[counter].freezetimer=0;

		}

   		precachedmodels[0] = PRECACHE_MODEL("sprites/laserbeam.spr"); //Beam effects sprite
   		precachedmodels[1] = PRECACHE_MODEL("models/presentsm.mdl"); //Firework model
   		precachedmodels[2] = PRECACHE_MODEL("models/presentsm.mdl"); //Firework model
   		precachedmodels[3] = PRECACHE_MODEL("models/presentsm.mdl"); //Firework model
   		precachedmodels[4] = PRECACHE_MODEL("models/presentsm.mdl"); //Firework model
   		precachedmodels[5] = PRECACHE_MODEL("models/presentsm.mdl"); //Quad damage
   		precachedmodels[6] = PRECACHE_MODEL("models/presentsm.mdl"); //xen_tree
   		precachedmodels[7] = PRECACHE_MODEL("models/presentsm.mdl"); //xen_tree
		precachedmodels[8] = PRECACHE_MODEL("sprites/xbeam1.spr");
		//precachedmodels[9] = PRECACHE_MODEL("sprites/zerogxplode.spr");// freeze sprite

        precachedsounds[0] = PRECACHE_SOUND("zombie/claw_strike1.wav");
        precachedsounds[1] = PRECACHE_SOUND("zombie/claw_strike2.wav");
        precachedsounds[2] = PRECACHE_SOUND("zombie/claw_strike3.wav");
        precachedsounds[3] = PRECACHE_SOUND("zombie/claw_miss1.wav");
        precachedsounds[4] = PRECACHE_SOUND("zombie/claw_miss2.wav");
		precachedsounds[5] = PRECACHE_SOUND("weapons/emp_1.wav");
		

//		AvRunPrecache();
    }


    if(FStrEq(STRING(pent->v.classname),"player")) {

   		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

	    	WRITE_BYTE( TE_KILLBEAM );
			WRITE_SHORT( ENTINDEX(pent) );

		MESSAGE_END();
    }

}


void BDClientDisconnect ( edict_t *pEntity ) {

	EntInfo[ENTINDEX(pEntity)].lastinfoupdate=0;
	EntInfo[ENTINDEX(pEntity)].level=0;
    EntInfo[ENTINDEX(pEntity)].speedbursttimer=0;
    EntInfo[ENTINDEX(pEntity)].bouncemode=0;
    EntInfo[ENTINDEX(pEntity)].drunkmode=0;
    EntInfo[ENTINDEX(pEntity)].followmode=0;
    EntInfo[ENTINDEX(pEntity)].logging=0;
    EntInfo[ENTINDEX(pEntity)].jetpack=0;
    EntInfo[ENTINDEX(pEntity)].fireworktimer=0;
    EntInfo[ENTINDEX(pEntity)].fireworkspeed=0;
    //EntInfo[ENTINDEX(pEntity)].quadtimer=0;
    //EntInfo[ENTINDEX(pEntity)].xentreeindex=0;
    EntInfo[ENTINDEX(pEntity)].hooktarget=0;
    EntInfo[ENTINDEX(pEntity)].hooktimer=0;
    EntInfo[ENTINDEX(pEntity)].walljump=0;
    EntInfo[ENTINDEX(pEntity)].humiltimer=0;
    EntInfo[ENTINDEX(pEntity)].wenistimer=0;
	//EntInfo[ENTINDEX(pEntity)].freezetimer=0;

}


void BDClientPutInServer( edict_t *pEntity ) {

    EntInfo[ENTINDEX(pEntity)].lastinfoupdate=0;
	EntInfo[ENTINDEX(pEntity)].level=0;
    EntInfo[ENTINDEX(pEntity)].speedbursttimer=0;
    EntInfo[ENTINDEX(pEntity)].bouncemode=0;
    EntInfo[ENTINDEX(pEntity)].drunkmode=0;
    EntInfo[ENTINDEX(pEntity)].followmode=0;
    EntInfo[ENTINDEX(pEntity)].logging=0;
    EntInfo[ENTINDEX(pEntity)].jetpack=0;
    EntInfo[ENTINDEX(pEntity)].fireworktimer=0;
    EntInfo[ENTINDEX(pEntity)].fireworkspeed=0;
    //EntInfo[ENTINDEX(pEntity)].quadtimer=0;
    //EntInfo[ENTINDEX(pEntity)].xentreeindex=0;
    EntInfo[ENTINDEX(pEntity)].hooktarget=0;
    EntInfo[ENTINDEX(pEntity)].hooktimer=0;
    EntInfo[ENTINDEX(pEntity)].walljump=0;
    EntInfo[ENTINDEX(pEntity)].humiltimer=0;
    EntInfo[ENTINDEX(pEntity)].wenistimer=0;
	//EntInfo[ENTINDEX(pEntity)].freezetimer=0;

}




void BDPlayerPreThink( edict_t *pEntity ) {

//	char tempText[8192];
	edict_t *tEntity;
	CBaseEntity *ctEntity;
	TraceResult tr;
	Vector tVector;
	int	counter;
	//Check to see if this player's ID and points have just been registered by the SA
/*
	if((gpGlobals->time*5-EntInfo[ENTINDEX(pEntity)].lastinfoupdate)>=1) {

	    if(CVAR_GET_FLOAT("sv_skydiving")!=0)

    	    if((CVAR_GET_FLOAT("sv_skydiving")<pEntity->v.flFallVelocity)&&pEntity->v.health>1)

        		pEntity->v.health = 1;

		if((int)CVAR_GET_FLOAT("sv_registerid")>0) {

    	    if((int)CVAR_GET_FLOAT("sv_registerid")==GETPLAYERUSERID(pEntity)) {

                if(FStrEq(CVAR_GET_STRING("sv_registercommand"),"NULL")) {

                	//EntInfo[ENTINDEX(pEntity)].level=(int)CVAR_GET_FLOAT("sv_registerpoints");
                    //pEntity->v.iuser4 = (int)CVAR_GET_FLOAT("sv_registerpoints");
	        		CVAR_SET_FLOAT("sv_registerid",0);
	        		CVAR_SET_FLOAT("sv_registerpoints",0);

				} else {

                    sprintf(tempText,"%s",CVAR_GET_STRING("sv_registercommand"));

                    if(tempText[0]==*"&")

                    	hextostring(tempText);

                    strcat(tempText,";");

                    // Check if it's to be sent to all clients
                    if(tempText[0]=='*') {

 						ctEntity=NULL;

	    				while(ctEntity = UTIL_FindEntityByClassname(ctEntity, "player")) {

	                        if(CVAR_GET_FLOAT("sv_adminlevel")>EntInfo[ENTINDEX(ENT(ctEntity->pev))].level) {

		                        CLIENT_COMMAND(ENT(ctEntity->pev),&tempText[1]);
	                        }
				    	}

		        		CVAR_SET_FLOAT("sv_registerid",0);
	                    CVAR_SET_STRING("sv_registercommand","NULL");

                    } else {

	                    CLIENT_COMMAND(pEntity,tempText);
		        		CVAR_SET_FLOAT("sv_registerid",0);
	                    CVAR_SET_STRING("sv_registercommand","NULL");

                    }
				}
			}
		}

        EntInfo[ENTINDEX(pEntity)].lastinfoupdate=gpGlobals->time*5;

    }
*/
    if(EntInfo[ENTINDEX(pEntity)].speedbursttimer>0) {

        if(gpGlobals->time-EntInfo[ENTINDEX(pEntity)].speedbursttimer>1.0) {

        	EntInfo[ENTINDEX(pEntity)].speedbursttimer=0;

		}

	}

    if(EntInfo[ENTINDEX(pEntity)].bouncemode)

        if(pEntity->v.flags & FL_ONGROUND) {

        	pEntity->v.velocity=Vector(RANDOM_LONG(-480,480), RANDOM_LONG(-480,480), 480);

		}

    if(EntInfo[ENTINDEX(pEntity)].drunkmode)

    	pEntity->v.fov=50+(sin(gpGlobals->time)+1)*40;

    if((EntInfo[ENTINDEX(pEntity)].jetpack)&&(!pEntity->v.deadflag)) {

    	pEntity->v.velocity.z=256;

		if (EntInfo[ENTINDEX(pEntity)].level<1) {
        
			if(gpGlobals->time-EntInfo[ENTINDEX(pEntity)].jetpack>=0.2) {

				if((--pEntity->v.health)==0)

        			ClientKill(pEntity);

			}

			UTIL_ParticleEffect(pEntity->v.origin, Vector(0,0,-1024), 108, 128);
			EntInfo[ENTINDEX(pEntity)].jetpack=gpGlobals->time;

		}
	}

    if((EntInfo[ENTINDEX(pEntity)].walljump)&&(!pEntity->v.deadflag)&&(!(pEntity->v.flags & FL_ONGROUND))&&(!(pEntity->v.flags & FL_DUCKING))) {

		//UTIL_MakeVectors ( pEntity->v.v_angle );
    	tVector = GetGunPosition( pEntity )+Vector(0,0,-32);
		UTIL_TraceLine( tVector, tVector+(Vector(pEntity->v.velocity.Normalize().x,pEntity->v.velocity.Normalize().y,0)*64), dont_ignore_monsters, pEntity, &tr );
		//pEntity->v.origin = tr.vecEndPos + (tr.vecPlaneNormal*72);
		//v=|u|(2((u/|u|).n)n+(u/|u|))

   		if (tr.flFraction != 1.0) {

			tVector=(pEntity->v.velocity.Length()*1.5)*((2*DotProduct(-pEntity->v.velocity.Normalize(),tr.vecPlaneNormal)*tr.vecPlaneNormal)+pEntity->v.velocity.Normalize());
            tVector.z = 4 * abs(tVector.z);
            pEntity->v.velocity = tVector;
        }

    }

    if(EntInfo[ENTINDEX(pEntity)].fireworktimer) {

    	pEntity->v.velocity.z=EntInfo[ENTINDEX(pEntity)].fireworkspeed;
		tVector = pEntity->v.origin;
		UTIL_TraceLine( tVector, tVector+(pEntity->v.velocity.Normalize()*64), dont_ignore_monsters, pEntity, &tr );

 		if (tr.flFraction != 1.0) {

			// TE_EXPLODEMODEL		107		spherical shower of models, picks from set
			// coord, coord, coord (origin)
			// coord (velocity)
			// short (model index)
			// short (count)
			// byte (life in 0.1's)

			for(counter=1;counter<=4;counter++) {

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			    	WRITE_BYTE( TE_EXPLODEMODEL );
					WRITE_COORD( pEntity->v.origin.x );
					WRITE_COORD( pEntity->v.origin.y );
					WRITE_COORD( pEntity->v.origin.z - 64 );
	                WRITE_COORD( 100 );
	                WRITE_SHORT(precachedmodels[counter]);
	                WRITE_SHORT(4);
	                WRITE_BYTE(50);
				MESSAGE_END();

            }

	    	ExplosionCreate(pEntity->v.origin,pEntity->v.angles,NULL);
			ClientKill(pEntity);
			
			/*
            tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("item_artifact_super_damage"));
            tEntity->v.origin=pEntity->v.origin;
            tEntity->v.spawnflags|=SF_NORESPAWN;
            DispatchSpawn(tEntity);
            DROP_TO_FLOOR(tEntity);
			*/

       		EntInfo[ENTINDEX(pEntity)].fireworktimer=0;

 		}

        if(gpGlobals->time-EntInfo[ENTINDEX(pEntity)].fireworktimer>=4) {

			for(counter=1;counter<=4;counter++) {

				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			    	WRITE_BYTE( TE_EXPLODEMODEL );
					WRITE_COORD( pEntity->v.origin.x );
					WRITE_COORD( pEntity->v.origin.y );
					WRITE_COORD( pEntity->v.origin.z - 64 );
	                WRITE_COORD( 100 );
	                WRITE_SHORT(precachedmodels[counter]);
	                WRITE_SHORT(4);
	                WRITE_BYTE(50);
				MESSAGE_END();

            }

	    	ExplosionCreate(pEntity->v.origin,pEntity->v.angles,NULL);
			ClientKill(pEntity);

			/*
            tEntity = CREATE_NAMED_ENTITY(MAKE_STRING("item_artifact_super_damage"));
            tEntity->v.origin=pEntity->v.origin;
            tEntity->v.spawnflags|=SF_NORESPAWN;
            DispatchSpawn(tEntity);
            DROP_TO_FLOOR(tEntity);
			*/

   			EntInfo[ENTINDEX(pEntity)].fireworktimer=0;

        }

    }

    if(EntInfo[ENTINDEX(pEntity)].followmode) {

		ctEntity=NULL;
        ctEntity=UTIL_FindEntityInSphere(ctEntity,pEntity->v.origin,BD_FOLLOW_MAX);

        while(ctEntity!=NULL) {

        	if(FStrEq(STRING(ctEntity->pev->classname),"player")&&(ctEntity->entindex()!=ENTINDEX(pEntity))) {

        		if((ctEntity->pev->origin-pEntity->v.origin).Length()>BD_FOLLOW_MIN) {

					ctEntity->pev->velocity = ( pEntity->v.origin - ctEntity->pev->origin ).Normalize() * BD_FOLLOW_SPEED;

				} else {

					//ctEntity->pev->velocity=g_vecZero;

	            }

            }

        ctEntity=UTIL_FindEntityInSphere(ctEntity,pEntity->v.origin,BD_FOLLOW_MAX);

        }

    }

    if(EntInfo[ENTINDEX(pEntity)].hooktarget&&(!pEntity->v.deadflag)) {

	    tEntity=INDEXENT(EntInfo[ENTINDEX(pEntity)].hooktarget);
	    
		if((tEntity->v.origin-pEntity->v.origin).Length()>128) {

        	pEntity->v.velocity =(tEntity->v.origin-pEntity->v.origin).Normalize()*400;

        } else {

        	//pEntity->v.velocity=g_vecZero;

        }

        if(gpGlobals->time-EntInfo[ENTINDEX(pEntity)].hooktimer>=4) {

			MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		    	WRITE_BYTE( TE_BEAMENTS );
				WRITE_SHORT( ENTINDEX(pEntity) );
				WRITE_SHORT( ENTINDEX(tEntity) );
				WRITE_SHORT( precachedmodels[0] );
	            WRITE_BYTE(1);
	            WRITE_BYTE(10);
	            WRITE_BYTE(40);
	            WRITE_BYTE(20);
                WRITE_BYTE(0);
                WRITE_BYTE(0); WRITE_BYTE(0); WRITE_BYTE(255);
                WRITE_BYTE(127);
                WRITE_BYTE(10);
            MESSAGE_END();

            EntInfo[ENTINDEX(pEntity)].hooktimer=gpGlobals->time;

    	}

    }

}


void BDClientCommand( edict_t *pEntity, const char *arg0, const char *arg1, const char *arg2, const char *arg3, const char *arg4 ) {

	if ( FStrEq(arg0, "superjump" )) {

		bd_superjump (pEntity);

	} else if ( FStrEq(arg0, "speedburst" )) {

		bd_speedburst (pEntity);

	} else if ( FStrEq(arg0, "ghost" )) {

		bd_ghost (pEntity);

	} else if ( FStrEq(arg0, "bounce" )) {

		bd_bounce (pEntity);

	} else if ( FStrEq(arg0, "explode" )) {

		bd_explode (pEntity);
/*
	} else if ( FStrEq(arg0, "petxentree" )) {

		bd_petxentree (pEntity);
*/
/*	} else if ( FStrEq(arg0, "quadme" )) {

		bd_quadme (pEntity);
*/
	} else if ( FStrEq(arg0, "l33tbeam" )) {

		bd_l33tbeam (pEntity);

	} else if ( FStrEq(arg0, "splat" )) {

		bd_splat (pEntity);

	} else if ( FStrEq(arg0, "firework" )) {

		bd_firework (pEntity);

	} else if ( FStrEq(arg0, "martyr" )) {

		bd_martyr (pEntity);

	} else if ( FStrEq(arg0, "trail" )) {

		int cRGB = ds_colors( pEntity, arg1 );
		int mRed,mGreen,mBlue;

		mRed = ((((cRGB - (cRGB % 255)) / 255) - (((cRGB - (cRGB % 255)) / 255) % 255)) / 255);
		mGreen = ((cRGB - (cRGB % 255)) / 255) % 255;
		mBlue = cRGB % 255 % 255;
				
		 if (FStrEq(arg1,"")) {

			ds_trailme ( pEntity, mRed, mGreen, mBlue );

		} else {

			ds_trailme ( pEntity, mRed, mGreen, mBlue );
		}
			
	} else if ( FStrEq(arg0, "dooropen" )) {

		bd_dooropen (pEntity);

	} else if ( FStrEq(arg0, "telepr0t" )) {

		bd_telepr0t (pEntity);

	} else if ( FStrEq(arg0, "push" )) {

		bd_push (pEntity);

	} else if ( FStrEq(arg0, "drunk" )) {

		bd_drunk (pEntity);

	} else if ( FStrEq(arg0, "followmode" )) {

		bd_followmode (pEntity);

	} else if ( FStrEq(arg0, "+hook" )) {

		bd_hookon (pEntity);

	} else if ( FStrEq(arg0, "-hook" )) {

		bd_hookoff (pEntity);

	} else if ( FStrEq(arg0, "+jetpack" )) {

		bd_jetpackon (pEntity);

	} else if ( FStrEq(arg0, "-jetpack" )) {

		bd_jetpackoff (pEntity);

	} else if ( FStrEq(arg0, "+walljump" )) {

		bd_walljumpon (pEntity);

	} else if ( FStrEq(arg0, "-walljump" )) {

		bd_walljumpoff (pEntity);

	} else if ( FStrEq(arg0, "kaboom" )) {

		bd_kaboom (pEntity);

	} else if ( FStrEq(arg0, "szcommands" )) {

		bd_szcommands (pEntity);

	} else if ( FStrEq(arg0, "humiliate" )) {

		bd_humiliate (pEntity);

	} else if ( FStrEq(arg0, "gravity" )) {

		ds_grav (pEntity, arg1);

	}/* else if ( FStrEq(arg0, "freeze" )) {

		ds_freeze (pEntity);

	}*/ else if (FStrEq(arg0, "give" )) {

		ds_givenameditem(pEntity, arg1);
	

	} else if (FStrEq(arg0, "ds_level" )) {

		ds_level(pEntity, arg1);
	

	}

}



int ds_colors( edict_t *pEntity, const char *arg1 ) {

	int mRed;
	int mGreen;
	int mBlue;
	int cRGB;
	int iGoodColor = 1;
				
	if(FStrEq(arg1,"red")) {

		mRed = 250;
		mGreen = 10;
		mBlue = 10;

	} else if(FStrEq(arg1,"blue")) {

		mRed = 10;
		mGreen = 235;
		mBlue = 250;

	} else if(FStrEq(arg1,"pink")) {

		mRed = 10;
		mGreen = 235;
		mBlue = 250;


	} else if(FStrEq(arg1,"salmon")) {

		mRed = 250;
		mGreen = 150;
		mBlue = 125;

	} else if(FStrEq(arg1,"gold")) {

		mRed = 250;
		mGreen = 230;
		mBlue = 10;

	} else if(FStrEq(arg1,"turquoise")) {

		mRed = 10;
		mGreen = 190;
		mBlue = 175;

	} else if(FStrEq(arg1,"magenta")  || FStrEq(arg1,"violet")) {

		mRed = 230;
		mGreen = 10;
		mBlue = 120;

	} else if(FStrEq(arg1,"orange")) {

		mRed = 250;
		mGreen = 150;
		mBlue = 10;

	} else if(FStrEq(arg1,"green")) {

		mRed = 10;
		mGreen = 250;
		mBlue = 10;

	} else if(FStrEq(arg1,"white")) {

		mRed = 254;
		mGreen = 254;
		mBlue = 254;

	} else if(FStrEq(arg1,"yellow")) {
		
		mRed = 250;
		mGreen = 250;
		mBlue = 10;

	} else if(FStrEq(arg1,"purple")) {

		mRed = 250;
		mGreen = 10;
		mBlue = 250;

	} else if(FStrEq(arg1,"random") || FStrEq(arg1,"")) {

		mRed = 0;
		mGreen = 0;
		mBlue = 0;

	} else {

		iGoodColor = 0;
		ClientPrint(VARS(pEntity), HUD_PRINTTALK, "> %s is not a recognized color\n", arg1);

		cRGB = (255 * 255 * 255) + (255 * 255) + 255;
		return cRGB;

	}

	if (FStrEq(arg1,"") || FStrEq(arg1,"")) {

		//ClientPrint(VARS(pEntity), HUD_PRINTTALK, "> you have a random trail\n");

	} else {

		ClientPrint(VARS(pEntity), HUD_PRINTTALK, "> you have a %s trail\n", arg1);

	}

	cRGB = (mRed * 255 * 255) + (mGreen * 255) + mBlue;

	return cRGB;

}