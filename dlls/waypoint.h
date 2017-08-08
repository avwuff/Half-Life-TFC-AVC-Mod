//
// HPB_bot - botman's High Ping Bastard bot
//
// (http://planethalflife.com/botman/)
//
// waypoint.h
//

#ifndef WAYPOINT_H
#define WAYPOINT_H

#define MAX_WAYPOINTS 1024
#define MAX_TEAM 4

// defines for waypoint flags field (32 bits are available)
#define W_FL_USE_BUTTON  (1<<0)  /* use a nearby button (lifts, doors, etc.) */
#define W_FL_LIFT        (1<<1)  /* wait for lift to be down before approaching this waypoint */
#define W_FL_CROUCH      (1<<2)  /* must crouch to reach this waypoint */
#define W_FL_FLAG        (1<<3)  /* flag position */
#define W_FL_FLAG_GOAL   (1<<4)  /* flag return position */
#define W_FL_HEALTH      (1<<5)  /* health kit location */
#define W_FL_ARMOR       (1<<6)  /* armor location */
#define W_FL_AMMO        (1<<7)  /* ammo location */

#define W_FL_DELETED     (1<<31) /* used by waypoint allocation code */


#define WAYPOINT_VERSION 2

// define the waypoint file header structure...
typedef struct {
   char filetype[8];  // should be "HPB_bot\0"
   int  waypoint_file_version;
   int  waypoint_file_flags;  // not currently used
   int  number_of_waypoints[MAX_TEAM];
   char mapname[32];  // name of map for these waypoints
} WAYPOINT_HDR;


// define the structure for waypoints...
typedef struct {
   int    flags;    // button, lift, flag, health, ammo, etc.
   Vector origin;   // location
} WAYPOINT;


#define MAX_PATH_INDEX 4

// define the structure for waypoint paths (paths are connections between
// two waypoint nodes that indicates the bot can get from point A to point B.
// note that paths DON'T have to be two-way.  sometimes they are just one-way
// connections between two points.  There is an array called "paths" that
// contains head pointers to these structures for each waypoint index.
typedef struct path {
   short int index[MAX_PATH_INDEX];  // indexes of waypoints (index -1 means not used)
   struct path *next;   // link to next structure
} PATH;


// waypoint function prototypes...
void WaypointInit(void);
//void WaypointAddPath(int team, short int add_index, short int path_index);
void WaypointAdd(edict_t *pEntity);
void WaypointDelete(edict_t *pEntity);
void WaypointCreatePath(edict_t *pEntity, int cmd);
void WaypointRemovePath(edict_t *pEntity, int cmd);
bool WaypointLoad(edict_t *pEntity);
void WaypointSave(void);
bool WaypointReachable(Vector v_srv, Vector v_dest, edict_t *pEntity);
void WaypointThink(edict_t *pEntity);

#endif // WAYPOINT_H
