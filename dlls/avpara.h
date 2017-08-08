//Parachute header file
void ParaPrecache();
edict_t *ParaSpawnItem( edict_t *pent );
void ParaThink( edict_t *pent );
void ParaTouch( edict_t *pent, edict_t *pEntity );
void ParaGiveChute( edict_t *pEntity );
void ParaEnable( edict_t *pEntity );
void ParaPrePlayerThink( edict_t *pEntity );
void ParaJoinGame( edict_t *pEntity );
