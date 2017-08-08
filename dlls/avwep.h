void WepDisable( edict_t *pEntity ) ;
void WepEnable( edict_t *pEntity ) ;
bool WepCommand( edict_t *pEntity , const char *pcmd, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5, const char *arg6);
void WepPlayerThink1( edict_t *pEntity );
void WepPlayerThink2( edict_t *pEntity, struct clientdata_s *cd ) ;

