bool RadioCreate( edict_t *pEntity );
void RadioSpawn( edict_t *pent );
void RadioPrecache();
void RadioKill( edict_t *pEntity );
void RadioKillBoom( edict_t *pEntity );
void RadioThink ( edict_t *pent );
void RadioUseCheck( edict_t *pEntity );
void RadioGetSong( int num, char *songname );
void RadioGetSongName( int num, char *songname );
void RadioTouch( edict_t *pEntity, edict_t *pTouch );
void RadioUse( edict_t *pentUsed, edict_t *pEntity );
void RadioShowMenu( edict_t *pEntity, edict_t *pRadio, int n );
void RadioHandleMenuItem(edict_t *pEntity, const char *itm);
void RadioSwitchToSong(int n, int entCode);
void RadioChangeSongPitch(int newPitch, edict_t *pEntity);

#define RADLOOP1  "avatar-x/breathaway.wav"
#define RADLOOP2  "avatar-x/free.wav"
#define RADLOOP3  "avatar-x/hamster.wav"
#define RADLOOP4  "avatar-x/pluginbaby.wav"
#define RADLOOP5  "avatar-x/ramp.wav"
#define RADLOOP6  "avatar-x/popcorn2.wav"
#define RADLOOP7  "avatar-x/roboto.wav"
#define RADLOOP8  "avatar-x/saltwater.wav"
#define RADLOOP9  "avatar-x/polka.wav"
#define RADLOOP10 "avatar-x/voodoo.wav"

//define RADLOOP5 "temploop.wav"
//define RADLOOP6 "komodo2.wav"
//define RADLOOP7 "eternity.wav"
//define RADLOOP8 "ppk.wav"

