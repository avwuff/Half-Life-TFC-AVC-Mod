// Tell the player only about the weapons he has and not all the others!
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
#include "avwritewep.h"


extern GETENTITYAPI other_GetEntityAPI;
extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;
extern DLL_FUNCTIONS other_gFunctionTable;
extern const Vector g_vecZero;

static FILE *fp;

// Keep track of which weapons we've sent

bool wepids[32];

void SendWepList( edict_t *ed )
{

	//for (int i=0;i<32;i++) wepids[i] = 0;
	
	int plycls = ed->v.playerclass;

	if (plycls == 1)
	{
		SendWeapon(	ed,	5,	0,	1	); // crowbar
		SendWeapon(	ed, 8,	1,	1	); // single
		SendWeapon(	ed,	10,	3,	1	); // nailgun
	}
	else if (plycls == 2)
	{
		SendWeapon(	ed,	5,	0,	1	); 
		SendWeapon(	ed,	6,	1,	1	);
		SendWeapon(	ed,	7,	2,	1	);
		SendWeapon(	ed,	10,	3,	1	);

	}
	else if (plycls == 3)
	{
		SendWeapon(	ed,	5,	0,	1	); 
		SendWeapon(	ed,	8,	1,	1	);
		SendWeapon(	ed,	9,	2,	1	);
		SendWeapon(	ed,	14,	4,	1	);

	}
	else if (plycls == 4)
	{
		SendWeapon(	ed,	5,	0,	1	); 
		SendWeapon(	ed,	8,	1,	1	);
		SendWeapon(	ed,	12,	3,	1	);
		SendWeapon(	ed,	22,	4,	1	);

	}
	else if (plycls == 5)
	{
		SendWeapon(	ed,	3,	0,	1	); 
		SendWeapon(	ed,	8,	1,	1	);
		SendWeapon(	ed,	9,	2,	1	);
		SendWeapon(	ed,	11,	3,	1	);

	}
	else if (plycls == 6)
	{
		SendWeapon(	ed,	5,	0,	1	); 
		SendWeapon(	ed,	8,	1,	1	);
		SendWeapon(	ed,	9,	2,	1	);
		SendWeapon(	ed,	17,	4,	1	);

	}
	else if (plycls == 7)
	{
		SendWeapon(	ed,	5,	0,	1	); 
		SendWeapon(	ed,	8,	1,	1	);
		SendWeapon(	ed,	13,	3,	1	);
		SendWeapon(	ed,	15,	4,	1	);

	}
	else if (plycls == 8)
	{
		SendWeapon(	ed, 23,	0,	1	); 
		SendWeapon(	ed,	20,	1,	1	);
		SendWeapon(	ed,	9,	2,	1	);
		SendWeapon(	ed,	10,	3,	1	);

	}
	else if (plycls == 9)
	{
		SendWeapon(	ed,	4,	0,	1	); 
		SendWeapon(	ed,	21,	1,	1	);
		SendWeapon(	ed,	9,	2,	1	);

	}
	else if (plycls == 11)
	{
		SendWeapon(	ed,	5,	0,	1	);  // civ has only axe
	}

	// Now give everyone else the other weapons they have

	SendWeapon(	ed,	25,	4,	2	);		// mine in slot 5, position 2
	SendWeapon(	ed,	26,	5,	1	);		// snowball in slot 6, position 1
	SendWeapon(	ed,	27,	4,	3	);		// tractor in 5, 3
	SendWeapon(	ed,	28,	4,	4	);		// BFG in 5, 4
	SendWeapon(	ed,	29,	3,	4	);		// Flare in slot 6, position 2

	// Finally tell them the remaining weapons are in the same slots
	
	for (int i=0;i<32;i++) 
	{
		if (!wepids[i])
		{
			SendWeapon(	ed,	i,	0,	4	);
		}
		wepids[i] = 0;
	}



}

	



void SendWeapon(edict_t *ed, int id, int slot, int pos)
{

	wepids[id] = 1;

	int ammo1 = -1;
	int ammo2 = -1;
	int ammo3 = -1;
	int ammo4 = -1;
	bool found = 0;

	char name[80];

	if (id == 3)
	{
		sprintf(name, "tf_weapon_medikit");
		found = 1;
	}
	else if (id == 4)
	{
		sprintf(name, "tf_weapon_spanner");
		ammo1 = 2;
		found = 1;
	}
	else if (id == 5)
	{
		sprintf(name, "tf_weapon_axe");
		found = 1;
	}
	else if (id == 23)
	{
		sprintf(name, "tf_weapon_knife");
		found = 1;
	}
	else if (id == 6)
	{
		sprintf(name, "tf_weapon_sniperrifle");
		found = 1;
		ammo1 = 1;
		ammo2 = 75;
	}
	else if (id == 21)
	{
		found = 1;
		sprintf(name, "tf_weapon_railgun");
		ammo1 = 3;
		ammo2 = 50;
	}
	else if (id == 8)
	{
		found = 1;
		sprintf(name, "tf_weapon_shotgun");
		ammo1 = 1;
		ammo2 = 200;
	}
	else if (id == 20)
	{
		found = 1;
		sprintf(name, "tf_weapon_tranq");
		ammo1 = 1;
		ammo2 = 40;
	}
	else if (id == 7)
	{
		found = 1;
		sprintf(name, "tf_weapon_autorifle");
		ammo1 = 1;
		ammo2 = 75;
	}
	else if (id == 9)
	{
		found = 1;
		sprintf(name, "tf_weapon_supershotgun");
		ammo1 = 1;
		ammo2 = 200;
	}
	else if (id == 11)
	{
		found = 1;
		sprintf(name, "tf_weapon_superng");
		ammo1 = 3;
		ammo2 = 150;
	}
	else if (id == 13)
	{
		found = 1;
		sprintf(name, "tf_weapon_flamethrower");
		ammo1 = 2;
		ammo2 = 200;
	}
	else if (id == 12)
	{
		found = 1;
		sprintf(name, "tf_weapon_gl");
		ammo1 = 4;
		ammo2 = 50;
	}
	else if (id == 10)
	{
		found = 1;
		sprintf(name, "tf_weapon_ng");
		ammo1 = 3;
		ammo2 = 200;
	}
	else if (id == 14)
	{
		found = 1;
		sprintf(name, "tf_weapon_rpg");
		ammo1 = 4;
		ammo2 = 50;
	}
	else if (id == 15)
	{
		found = 1;
		sprintf(name, "tf_weapon_ic");
		ammo1 = 4;
		ammo2 = 20;
	}
	else if (id == 17)
	{
		found = 1;
		sprintf(name, "tf_weapon_ac");
		ammo1 = 1;
		ammo2 = 200;
	}
	else if (id == 22)
	{
		found = 1;
		sprintf(name, "tf_weapon_pl");
		ammo1 = 4;
		ammo2 = 50;
	}
	
	// my weapons
	else if (id == 25)
	{
		sprintf(name, "weapon_tripmine");
		found = 1;
		ammo1 = -1;
		ammo2 = -1;
	}
	else if (id == 26)
	{
		sprintf(name, "weapon_9mmar");
		ammo1 = 16;
		found = 1;
		ammo2 = 5;
		ammo3 = 17;
		ammo4 = 1;

	}
	else if (id == 27)
	{
		sprintf(name, "weapon_egon");
		found = 1;
		ammo1 = -1;
		ammo2 = -1;
	}
	else if (id == 28)
	{
		sprintf(name, "weapon_gauss");
		ammo1 = -1;
		found = 1;
		ammo2 = -1;
	}
	else if (id == 29)
	{
		sprintf(name, "weapon_snark");
		ammo1 = 18;
		found = 1;
		ammo2 = 5;
		ammo3 = 19;
		ammo4 = 1;
	}
	
	if (found)
	{

		MESSAGE_BEGIN( MSG_ONE, gmsgWeaponList, NULL, ed);  
			WRITE_STRING(name);			// string	weapon name
			WRITE_BYTE(ammo1);	// byte		Ammo Type
			WRITE_BYTE(ammo2);				// byte     Max Ammo 1
			WRITE_BYTE(ammo3);	// byte		Ammo2 Type
			WRITE_BYTE(ammo4); 			// byte     Max Ammo 2
			WRITE_BYTE(slot);					// byte		bucket
			WRITE_BYTE(pos);				// byte		bucket pos
			WRITE_BYTE(id);						// byte		id (bit index into pev->weapons)
			WRITE_BYTE(0);					// byte		Flags
		MESSAGE_END();	
	}
}
