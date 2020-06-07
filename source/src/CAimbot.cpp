// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include <stdlib.h>

#ifndef min
#define min( x, y ) ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#endif
#ifndef max
#define max( x, y ) ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )
#endif

CAimbot Aimbot;

extern void SnapVectorTowards( vec3_t v, vec3_t to );

// There !!!HAS TO BE!!! a corresponding string for each select type (except SORT_MAX)
const char *sortTypeText[SORT_MAX] =
{
	"Off",
	"Distance",
	"Attacker",
	"Crosshair",
	"K/D ratio",
	"Accuracy",
	"Threat",
};

const char *selfPredictText[SPR_MAX] =
{
	"Off",
	"Manual",
	"Ping",
	"L337",
};

const char *headTraceTypeText[HEAD_MAX] =
{
	"Center",
	"Static Surface",
	"X Trace",
};

const char *bodyTraceTypeText[BODY_MAX] =
{
	"Center",
	"Static Surface",
	"X Trace",
};

const char *hitboxText[HITBOX_MAX] =
{
	"Off",
	"Custom",
};

const char *priorityTypeText[AP_MAX] =
{
	"Body Only",
	"Head Only",
	"Body - Head",
	"Head - Body",
	"Head priority",
};

const char *aimTypeText[AIM_MAX] =
{
	"Off",
	"On Fire",
	"On Button",
	"Always",
};

const char *aimModeText[AIMMODE_MAX] =
{
	"Aimbot Off",
	"Normal Aimbot",
};

const char *riflePredictText[RF_PREDICT_MAX] =
{
	"Off",
	"Linear",
	"Linear/2",
	"Average",
	"Smart",
};

CAimbot::CAimbot(void)
{
	senslocked = GrenadeFireOK = false;
	numFrameTargets = 0;
	target = NULL;
	grenadeTarget = NULL;
	lastTarget = NULL;
	lastTargetValid = false;
	atkBtn = NULL;
	firing = false;
	stopAutoTargets = false;
	this->lastShotTime = 0;
}

void CAimbot::SetSelf( int clientNum ){ this->self = &eth32.cg.players[clientNum]; }

void CAimbot::PreFrame(void)
{	
	numFrameTargets = 0;
	target = NULL;
	lastTargetValid = false;

	if (lastTarget && (!eth32.settings.lockTarget || IS_DEAD(lastTarget->clientNum) || !IS_INFOVALID(lastTarget->clientNum)))
		lastTarget = NULL;
}

void CAimbot::PostFrame(void)
{	
	// important default actions, assume neither are set unless explicitly told to do so (safer)
	// these actions are very fast anyway
	LockSensitivity(false);
	
	CheckReload();

	if (!this->attackPressed)
		Autofire(false);

	// master aim mode
	if (eth32.settings.aimMode == AIMMODE_OFF)
		return;

	// No need for aimbot on those conditions
	if (IS_SPECTATOR(eth32.cg.snap->ps.clientNum) || IS_DEAD(eth32.cg.clientNum)){
		grenadeFireTime = 0;
		Autocrouch(false, true);
		return;
	}

	if (eth32.cg.currentWeapon->attribs & WA_USER_DEFINED){
		Aimbot.DoBulletBot();
		if (!target)
			Autocrouch(false, false);
	} else
		Autocrouch(false, false);

	if ((eth32.cg.currentWeapon->attribs & WA_BALLISTIC) && (eth32.settings.grenadeBot || eth32.settings.rifleBot)){
		if (eth32.settings.autoGrenTargets && !stopAutoTargets) {
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortCrosshair);
			if (numFrameTargets)
				grenadeTarget = frameTargets[0];
			else
				grenadeTarget = NULL;
		} else {
			if (!grenadeTarget)
				selectGrenadeTarget(0);
		}

		if (GetAsyncKeyState(XK_Left) & 1){		//	'<'
			selectGrenadeTarget(-1);
			stopAutoTargets = true;
		} else if (GetAsyncKeyState(XK_Right) & 1) {	//	'>'
			selectGrenadeTarget(1);
			stopAutoTargets = true;
		}

		if (grenadeTarget && (!IS_INFOVALID(grenadeTarget->clientNum) || IS_FRIENDLY(grenadeTarget->clientNum) || IS_DEAD(grenadeTarget->clientNum)))
			grenadeTarget = NULL;

		if (grenadeTarget) {
			bool solution = false;
			float flytime;
			vec3_t point;

			VectorCopy(grenadeTarget->lerpOrigin, point);
			Aimbot.grenadeAimbot(point, &flytime, &solution, !eth32.settings.ballisticPredict);

			// if prediction is enabled, need another pass because flytime isnt known beforehand
			if (!RifleMultiBounce && eth32.settings.ballisticPredict && solution){
				PredictPlayer(grenadeTarget, flytime, point, -1);
				Aimbot.grenadeAimbot(point, &flytime, &solution, true);
			}
		}
	}
}

// Reset some stuff when reloading
void CAimbot::CheckReload(void) {	
	if (eth32.cg.snap->ps.weaponstate == WEAPON_RELOADING) {
		if (this->lastShotTime)
			this->lastShotTime = 0;
		else
			return;
	}		
}

// respawn so reset the aimbot
void CAimbot::Respawn(void)
{
#ifdef ETH32_DEBUG
	Debug.Log("Aimbot Respawn()");
#endif
	Autofire(false);
	LockSensitivity(false);
	Autocrouch(false, true);
	lastTarget = target = grenadeTarget = NULL;
	lastTargetValid = false;
	stopAutoTargets = false;
	GrenadeTicking = false;
	grenadeFireTime = 0;
	this->lastShotTime = 0;

	// reset histories
	for (int i=0; i<MAX_CLIENTS; i++) {
		player_t *p = &eth32.cg.players[i];
		memset(p->history, 0, sizeof(p->history));
	}
}

void CAimbot::AddTarget(player_t *player)
{	
	if (player->friendly || player->invulnerable)
		return;	

	// - solcrushr: fov check added before adding to our target array
	if ((eth32.cg.currentWeapon->attribs & WA_USER_DEFINED) && player->distance < eth32.cg.currentWeapon->range && CheckFov(player->orHead.origin)) {
		// sol: if last target is valid, then don't add him to normal targetlist
		//      we can check him independently
		if (lastTarget && player == lastTarget) {
			lastTargetValid = true;
			return;

		}

		frameTargets[numFrameTargets] = player;
		numFrameTargets++;
		// sol: weapdefs look safe, but to prevent overwriting array
		return;
    }


	if (eth32.cg.currentWeapon->attribs & WA_BALLISTIC){
		frameTargets[numFrameTargets] = player;
		numFrameTargets++;
	}
}

// these are callbacks for qsort, not part of aimbot object
int distance_test(const void *elm1, const void *elm2)
{
	const trace_point *a = (trace_point *) elm1;
	const trace_point *b = (trace_point *) elm2;

	if( a->d > b->d )
		return 1;
	else if( a->d < b->d )
		return -1;
	else
		return 0;
}

int angle_test(const void *elm1, const void *elm2)
{
	const trace_point *a = (trace_point *) elm1;
	const trace_point *b = (trace_point *) elm2;

	// elements are dotproducts of normalized vecs so bigger value, smaller angle
	if( a->d > b->d )
		return -1;
	else if( a->d < b->d )
		return 1;
	else
		return 0;
}


void CAimbot::DrawBodyBox( int clientNum )
{
	player_t	*player = &eth32.cg.players[clientNum];
	vec3_t		size;
	vec3_t		boxOrigin;

	// i suppose body box size does not vary much (if at all)
	size[0] = size[1] = eth32.settings.bodybox;
	size[2] = 24.0;

	VectorCopy( player->lerpOrigin, boxOrigin );

	if (player->currentState->eFlags & EF_PRONE)
		size[2] += PRONE_VIEWHEIGHT+12.0;
	else if (player->currentState->eFlags & EF_CROUCHING)
		size[2] += CROUCH_VIEWHEIGHT+8.0;
	else
		size[2] += DEFAULT_VIEWHEIGHT-4.0;

	boxOrigin[2] += -24.0 + size[2]*0.5;

	vec3_t min,max;
	VectorCopy( boxOrigin, min );
	VectorCopy( boxOrigin, max );

	VectorMA( boxOrigin, -size[0]*0.5, xAxis, min );
	VectorMA( min, -size[1]*0.5, yAxis, min );
	VectorMA( min, -size[2]*0.5, zAxis, min );

	VectorMA( boxOrigin, size[0]*0.5, xAxis, max );
	VectorMA( max, size[1]*0.5, yAxis, max );
	VectorMA( max, size[2]*0.5, zAxis, max );

	Engine.MakeRailTrail( min, max, true, eth32.settings.colorBodyHitbox, eth32.settings.bodyRailTime );
}

void CAimbot::DrawHeadBox( int clientNum, bool axes )
{
	vec3_t		cv;
	vec3_t		p;
	hitbox_t 	*hbox;
	bool		moving;
	int 		eFlags;
	vec3_t		vel;
	vec3_t		size;
	float		speed;

	if( eth32.settings.hitboxType == HITBOX_CUSTOM )
		hbox = &customHitbox;
	else
		hbox = &head_hitboxes[eth32.settings.hitboxType];

	eFlags = eth32.cg.players[clientNum].currentState->eFlags;

	VectorCopy( eth32.cg.players[clientNum].currentState->pos.trDelta, vel );
	/* this is not really movement detector, but animation detector
		only use pos.trDelta since that handles user-intended velocities */
	if( VectorCompare(vel, vec3_origin) )
		moving = false;
	else
		moving = true;

	if( (eFlags & EF_PRONE) || (eFlags & EF_PRONE_MOVING) )
		VectorCopy( hbox->prone_offset, cv );
	else {
		if( !moving ){
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset, cv );
			else
				VectorCopy( hbox->stand_offset, cv );
		} else {
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset_moving, cv );
			else
				VectorCopy( hbox->stand_offset_moving, cv );
		}
	}

	VectorCopy( hbox->size, size );

	/* Dynamic Hitbox - adjust X,Y,Z size based on speed perpendicular to our viewdirection
		This is gives the aimbot 'fastness' of aim when guy corners */
	if (eth32.settings.dynamicHitboxScale > 0){
		speed = VectorLength(vel) - fabs(DotProduct(vel,eth32.cg.refdef->viewaxis[0]));

		if( speed > 0 ){
			size[0] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->ps.speed);		// ps.speed is g_speed
			size[1] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->ps.speed);		// ps.speed is g_speed
		}
	}

	orientation_t *head = &eth32.cg.players[clientNum].orHead;
	// rotate hitbox offset vector with tag (hitboxes themselves dont rotate)
	VectorMA( head->origin, cv[2], head->axis[2], p );
	VectorMA( p, cv[1], head->axis[1], p );
	VectorMA( p, cv[0], head->axis[0], p );

	vec3_t min,max;

	VectorMA( p, -0.5* size[0], xAxis, min );
	VectorMA( min, -0.5* size[1], yAxis, min );
	VectorMA( min, -0.5* size[2], zAxis, min );

	VectorMA( p, 0.5* size[0], xAxis, max );
	VectorMA( max, 0.5* size[1], yAxis, max );
	VectorMA( max, 0.5* size[2], zAxis, max );

	Engine.MakeRailTrail( min, max, true, eth32.settings.colorHeadHitbox, eth32.settings.headRailTime );
	if (axes){
		vec3_t ex1,ex2,ey1,ey2,ez1,ez2;
		VectorMA( p, 0, head->axis[0], ex1 );
		VectorMA( p, 25, head->axis[0], ex2 );

		VectorMA( p, 0, head->axis[1], ey1 );
		VectorMA( p, 25, head->axis[1], ey2 );

		VectorMA( p, 0, head->axis[2], ez1 );
		VectorMA( p, 25, head->axis[2], ez2 );

		Engine.MakeRailTrail( ex1, ex2, false, eth32.settings.colorXAxis, eth32.settings.headRailTime );
		Engine.MakeRailTrail( ey1, ey2, false, eth32.settings.colorYAxis, eth32.settings.headRailTime );
		Engine.MakeRailTrail( ez1, ez2, false, eth32.settings.colorZAxis, eth32.settings.headRailTime );
	}
}

/* trace a userdefined box, set the visible vector, and return visibility
	size is world coordinates, X*Y*Z */
bool CAimbot::traceBodyBox( vec3_t boxOrigin, float dist, vec3_t size, vec3_t trOrigin, int skipEnt, player_t *player, bodyBoxtrace_t trType,  vec3_t visible, int maxTraces )
{
	trace_point *p;
	VectorCopy( vec3_origin, visible );
	float boxVolume = size[0]*size[1]*size[2];
	// need this, because if we are modifying the origin with prediction
	// false points (0,0,0) will seem valid after prediction is applied
	if (VectorCompare(boxOrigin, vec3_origin))
		return false;
   
	VectorMA(boxOrigin, eth32.settings.predTarget, player->currentState->pos.trDelta, boxOrigin);
      	
    // Maximum generated points
    if (trType == BODY_STATIC_SURFACE)
		maxTraces = 56;
	else if (trType == BODY_XTRACE)
		maxTraces = 113;

	// Don't allow center trace for body in this fashion
	if (trType != BODY_CENTER) {
		if (Engine.IsVisible(trOrigin, boxOrigin, skipEnt)) {
			VectorCopy( boxOrigin, visible );
			return true;
		}
	} else
		return false;
		
	// Check the same number of points generated.
	if (eth32.cg.currentWeapon->attribs & WA_USER_DEFINED)
		eth32.cg.currentWeapon->bodyTraces = maxTraces;

	// check for a too small volume
	if( boxVolume < 1.0 )
		return false;

	int k = 0;
	p = (trace_point *)malloc(sizeof(trace_point)*maxTraces);
	memset(p, 0, sizeof(trace_point)*maxTraces);

	switch( trType ){
		// Alexplay: rsHook-like trace, 2 intersecting planes full of points.	
		case BODY_XTRACE: {
			float mainpath = 0.125f; // 45º
			float height = 0.5f;	// Initial height of center points
			// Box hypotenuse/2 = Imaginary circle radius touching hitbox's four vertexes (from top)
			float radius = sqrt(size[0]*size[0] + size[1]*size[1]);
			float path = 1.0f / 8.0f;	
			float phi;
			
			// Planes
			for (k=0; k<maxTraces; k++) { // -7: Main column points
				/*
				 * Initially: 45º
				 * Each point is rotated 90º from the center of the boxOrigin, so:
				 *
				 * 45º -> 135º -> 225º -> 315º
				 */
				
				phi = 2.0f * M_PI * mainpath; 
				
				p[k].pt[0] = cosf(phi)*path*radius;
				p[k].pt[1] = sinf(phi)*path*radius;
				p[k].pt[2] = height*size[2];
				
				VectorAdd( boxOrigin, p[k].pt, p[k].pt );
						
				// Each 4 points reduce the perimeter of the circle
				if (k && !(k % 4)) {
					path += 1.0f / 8.0f;
				}
				
				// Each 16 points proceed to the next row of points, also reset the perimeter
				if (k && !(k % 16)) {
					path = 1.0f / 8.0f;
					height -= 1.0f / 6.0f;
				}	
				
				mainpath += 0.25f; // + 90º
			}				

			CalcTrPointsDist(boxOrigin, p, maxTraces);
			qsort( p, maxTraces, sizeof(trace_point), angle_test );
			break;
		}
		// Hitbox's faces filled with points
		case BODY_STATIC_SURFACE: {
			float height = 0.5f;
			float sizex = size[0] / 2;
			float hyp = size[0]*M_SQRT1_2;
			float factz;
			
			vec3_t d1 = {M_SQRT1_2, M_SQRT1_2, 0};
			vec3_t d2 = {-M_SQRT1_2, M_SQRT1_2, 0};
				
			for (int j=0; j<7; j++) {
				factz = size[2] * height;
				
				VectorMA(boxOrigin, sizex, xAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -sizex, xAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, sizex, yAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -sizex, yAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, hyp, d1, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -hyp, d1, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, hyp, d2, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -hyp, d2, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				height -= 1.f / 6.f;
			}
				
			CalcTrPointsDist(boxOrigin, p, maxTraces);
			qsort( p, maxTraces, sizeof(trace_point), angle_test );
			break;
		}
		default:
			/* unknown trace type */
			free(p);
			return false;
	}
	
	// Debug Points
	if (eth32.settings.debugPoints)
		ShowAimpoints(p, maxTraces, trOrigin, skipEnt);

	/* trace them all */
	for( k=0; k<maxTraces; k++ ){
		if(Engine.IsVisible(trOrigin, p[k].pt, skipEnt)){
			VectorCopy( p[k].pt, visible );
			free(p);
			return true;
		}
	}

	free(p);
	return false;
}

// Helper function to calculate trace points distance to our viewdir based on angular distance from boxOrigin center
void CAimbot::CalcTrPointsDist(vec3_t boxOrigin, trace_point *p, int maxTraces)
{
	vec3_t dir, dr;
	
	VectorSubtract(eth32.cg.refdef->vieworg, boxOrigin, dir);
	VectorNormalizeFast( dir );
	
	for (int k=0; k<maxTraces; k++) {
		VectorSubtract(eth32.cg.refdef->vieworg, p[k].pt, dr);
		VectorNormalizeFast(dr);				
		p[k].d = DotProduct(dr, dir);
	}
}

/* trace a userdefined box, set the visible vector, and return visibility
	size is world coordinates, X*Y*Z */
bool CAimbot::traceHeadBox( vec3_t boxOrigin, float dist, vec3_t size, vec3_t trOrigin, int skipEnt, player_t *player, headBoxtrace_t trType,  vec3_t visible, int maxTraces )
{
	trace_point *p;
	VectorCopy( vec3_origin, visible );
	float boxVolume = size[0]*size[1]*size[2];
	// need this, because if we are modifying the origin with prediction
	// false points (0,0,0) will seem valid after prediction is applied
	if (VectorCompare(boxOrigin, vec3_origin)) 
		return false;

	VectorMA(boxOrigin, eth32.settings.predTarget, player->currentState->pos.trDelta, boxOrigin);
      	
    // Maximum generated points
	if (trType == HEAD_STATIC_SURFACE)
		maxTraces = 24;
	else if (trType == HEAD_XTRACE)
		maxTraces = 61;

	if (Engine.IsVisible(trOrigin, boxOrigin, skipEnt)) {
		VectorCopy( boxOrigin, visible );
		return true;
	}
		
	// Check the same number of points generated.
	if (eth32.cg.currentWeapon->attribs & WA_USER_DEFINED)
		eth32.cg.currentWeapon->headTraces = maxTraces;

	// check for a too small volume
	if( boxVolume < 1.0 )
		return false;

	int k = 0;
	p = (trace_point *)malloc(sizeof(trace_point)*maxTraces);
	memset(p, 0, sizeof(trace_point)*maxTraces);

	switch( trType ){
		// Alexplay: rsHook-like trace, same as body Xtrace but with a lot less points.
		case HEAD_XTRACE: {
			float mainpath = 0.125f; // 45º
			float height = 0.5f;	// Initial height of center points
			// Box hypotenuse/2 = Imaginary circle radius touching hitbox's four vertexes (from top)
			float radius = sqrt(size[0]*size[0] + size[1]*size[1]);
			float path = 1.0f / 6.0f;	
			float phi;
			
			// Planes
			for (k=0; k<maxTraces; k++) { // -5: Main column points
				/*
				 * Initially: 45º
				 * Each points is rotated 90º from the center of the boxOrigin, so:
				 *
				 * 45º -> 135º -> 225º -> 315º
				 */
				
				phi = 2.0f * M_PI * mainpath; 
				
				p[k].pt[0] = cosf(phi)*path*radius;
				p[k].pt[1] = sinf(phi)*path*radius;
				p[k].pt[2] = height*size[2];
				
				VectorAdd( boxOrigin, p[k].pt, p[k].pt );
						
				// Each 4 points reduce the perimeter of the circle
				if (k && !(k % 4)) {
					path += 1.0f / 6.0f;
				}
				
				// Each 12 points proceed to the next row of points, also reset the perimeter
				if (k && !(k % 12)) {
					path = 1.0f / 6.0f;
					height -= 1.0f / 4.0f;
				}	
				
				mainpath += 0.25f; // + 90º
			}				
			CalcTrPointsDist(boxOrigin, p, maxTraces);
			qsort( p, maxTraces, sizeof(trace_point), angle_test );

			break;
		}
		// Hitbox's faces filled with points
		case HEAD_STATIC_SURFACE: {
			float height = 0.5f;
			float sizex = size[0] / 2;
			float hyp = size[0]*M_SQRT1_2;
			float factz;
			
			vec3_t d1 = {M_SQRT1_2, M_SQRT1_2, 0};
			vec3_t d2 = {-M_SQRT1_2, M_SQRT1_2, 0};
				
			for (int j=0; j<3; j++) {
				factz = size[2] * height;
				
				VectorMA(boxOrigin, sizex, xAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -sizex, xAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, sizex, yAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -sizex, yAxis, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, hyp, d1, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -hyp, d1, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, hyp, d2, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				VectorMA(boxOrigin, -hyp, d2, p[k].pt);
				VectorMA(p[k].pt, factz, zAxis, p[k].pt);
				k++;
				
				height -= 0.5f;
			}
				
			CalcTrPointsDist(boxOrigin, p, maxTraces);
			qsort( p, maxTraces, sizeof(trace_point), angle_test );
			break;
		}
		default:
			/* unknown trace type */
			free(p);
			return false;
	}
	
	// Debug Points
	if (eth32.settings.debugPoints)
		ShowAimpoints(p, maxTraces, trOrigin, skipEnt);

	/* trace them all */
	for( k=0; k<maxTraces; k++ ){
		if(Engine.IsVisible(trOrigin, p[k].pt, skipEnt)){
			VectorCopy( p[k].pt, visible );			
			free(p);			
			return true;
		}
	}

	free(p);
	return false;
}

void CAimbot::ShowAimpoints(trace_point *p, int maxTraces, vec3_t trOrigin, int skipEnt)
{
	if (!eth32.settings.drawHackVisuals || Gui.IsLimboPanelUp())
		return;
	
	vec3_t ptstart, ptend;
	
	for (int k=0; k<maxTraces; k++) {		
		VectorCopy(p[k].pt, ptstart);
		VectorCopy(p[k].pt, ptend);
		
		ptstart[2] += 0.3;
		ptend[2] -= 0.3;
		
		if (Engine.IsVisible(trOrigin, p[k].pt, skipEnt))
			Engine.MakeRailTrail(ptstart, ptend, false, colorGreen, 100);
		else
			Engine.MakeRailTrail(ptstart, ptend, false, colorYellow, 100);
	}
}

bool CAimbot::traceHead( orientation_t *head, vec3_t hitPoint, int clientNum )
{
	return this->traceHead(head, hitPoint, eth32.cg.muzzle, clientNum);
}

bool CAimbot::traceHead( orientation_t *head, vec3_t hitPoint, vec3_t start, int clientNum )
{
	player_t	*player = &eth32.cg.players[clientNum];
	vec3_t		cv;
	vec3_t		p;
	hitbox_t 	*hbox;
	bool		moving;
	int 		eFlags;
	vec3_t		vel;
	vec3_t		size;
	float		speed;

	// allow users to disable head traces all together
	if (!eth32.cg.currentWeapon->headTraces)
		return false;

	if( eth32.settings.hitboxType == HITBOX_CUSTOM )
		hbox = &customHitbox;
	else
		hbox = &head_hitboxes[eth32.settings.hitboxType];

	eFlags = player->currentState->eFlags;

	VectorCopy( player->currentState->pos.trDelta, vel );

	/* this is not really movement detector, but animation detector
		only use pos.trDelta since that handles user-intended velocities */
	if( VectorCompare(vel, vec3_origin) )
		moving = false;
	else
		moving = true;

	if( (eFlags & EF_PRONE) || (eFlags & EF_PRONE_MOVING) )
		VectorCopy( hbox->prone_offset, cv );
	else {
		if( !moving ){
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset, cv );
			else
				VectorCopy( hbox->stand_offset, cv );
		} else {
			if( eFlags & EF_CROUCHING )
				VectorCopy( hbox->crouch_offset_moving, cv );
			else
				VectorCopy( hbox->stand_offset_moving, cv );
		}
	}
	
	// Head Hitbox Size	
	hbox->size[0] = hbox->size[1] = hbox->size[2] = eth32.settings.headBoxSize;
	VectorCopy( hbox->size, size );
	
	AdjustHitboxes(player, hbox);

	/* Dynamic Hitbox - adjust X,Y,Z size based on speed perpendicular to our viewdirection
		This is gives the aimbot 'fastness' of aim when guy corners */
	if (eth32.settings.dynamicHitboxScale > 0){
		speed = VectorLength(vel) - fabs(DotProduct(vel,eth32.cg.refdef->viewaxis[0]));

		if( speed > 0 ){
			size[0] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->ps.speed);		// ps.speed is g_speed
			size[1] *= (1.0+eth32.settings.dynamicHitboxScale*speed/eth32.cg.snap->ps.speed);		// ps.speed is g_speed
		}
	}

	// rotate hitbox offset vector with tag (hitboxes themselves dont rotate)
	VectorMA( head->origin, cv[2], head->axis[2], p );
	VectorMA( p, cv[1], head->axis[1], p );
	VectorMA( p, cv[0], head->axis[0], p );
	
	return traceHeadBox(p, player->distance, size, start, clientNum, player, (headBoxtrace_t)eth32.settings.headTraceType, hitPoint, eth32.cg.currentWeapon->headTraces);
}

// Adjust Z vecs based on distance to increase chances of hitting at large distances
void CAimbot::AdjustHitboxes(player_t *player, hitbox_t *hbox)
{
	float factor = 0.001f * ((int)player->distance - 1000);	//6-3 = 3; 4000 - 1000 = 3000; 3/3000 = 0.001f
	factor = factor > 3 ? 3 : factor;
	factor = factor < 0 ? 0 : factor;
	
	// X Vecs
	hbox->stand_offset[0] = eth32.settings.vecStand[0];
	hbox->stand_offset_moving[0] = eth32.settings.vecRun[0];
	hbox->crouch_offset[0] = eth32.settings.vecCrouch[0];
 	hbox->crouch_offset_moving[0] = eth32.settings.vecCrawl[0];
 	hbox->prone_offset[0] = eth32.settings.vecProne[0];
	// Y vecs
	hbox->stand_offset[1] = eth32.settings.vecStand[1];
	hbox->stand_offset_moving[1] = eth32.settings.vecRun[1];
	hbox->crouch_offset[1] = eth32.settings.vecCrouch[1];
 	hbox->crouch_offset_moving[1] = eth32.settings.vecCrawl[1];
 	hbox->prone_offset[1] = eth32.settings.vecProne[1];
 	
 	// Z Vecs based on distance
	hbox->stand_offset[2] = eth32.settings.vecStand[2] - factor;
	hbox->stand_offset_moving[2] = eth32.settings.vecRun[2] - factor;
	hbox->crouch_offset[2] = eth32.settings.vecCrouch[2] - factor;
	hbox->crouch_offset_moving[2] = eth32.settings.vecCrawl[2] - factor;
	hbox->prone_offset[2] = eth32.settings.vecProne[2];
}

void CAimbot::AdjustDelay(player_t *player)
{
	if (!eth32.settings.autoDelay || !(eth32.cg.currentWeapon->attribs & WA_USER_DEFINED))
		return;	

	if (player->distance < 2000.f)
		eth32.cg.currentWeapon->delay = eth32.settings.delayClose;
	else
		eth32.cg.currentWeapon->delay = eth32.settings.delayFar;
}

bool CAimbot::traceBody( vec3_t hitPoint, int clientNum )
{
	player_t	*player = &eth32.cg.players[clientNum];
	vec3_t		size;
	vec3_t		boxOrigin;
	vec3_t		vel;
	float		speed;

	// allow user to turn off body traces all together
	if (!eth32.cg.currentWeapon->bodyTraces)
		return false;

	// i suppose body box size does not vary much (if at all)
	size[0] = size[1] = eth32.settings.bodybox;
	size[2] = 24.0; 

	VectorCopy( player->lerpOrigin, boxOrigin );

	if (player->currentState->eFlags & EF_PRONE)
		size[2] += PRONE_VIEWHEIGHT+12.0;
	else if (player->currentState->eFlags & EF_CROUCHING)
		size[2] += CROUCH_VIEWHEIGHT+8.0;
	else
		size[2] += DEFAULT_VIEWHEIGHT-4.0;

	boxOrigin[2] += -24.0 + size[2]*0.5;
	
	return traceBodyBox( boxOrigin, eth32.cg.players[clientNum].distance, size, eth32.cg.muzzle, clientNum, player, (bodyBoxtrace_t)eth32.settings.bodyTraceType, hitPoint, eth32.cg.currentWeapon->bodyTraces );
}

int SortDistance(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->distance < player2->distance)
		return -1;
	else
		return 1;
}

int SortThreat(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->stats.threat == player2->stats.threat)
		return SortDistance( p1, p2 );

	if (player1->stats.threat > player2->stats.threat)
		return -1;
	else
		return 1;
}

int SortAccuracy(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->stats.accuracy > player2->stats.accuracy || (player1->stats.accuracy == player2->stats.accuracy && player1->distance < player2->distance))
		return -1;
	else
		return 1;
}

int SortKdRatio(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (player1->stats.kdRatio == player2->stats.kdRatio)
		return SortDistance( p1, p2 );

	if (player1->stats.kdRatio > player2->stats.kdRatio)
		return -1;
	else
		return 1;
}

int SortAttacker(const void *p1, const void *p2) 
{
	const player_t *player1 = *(const player_t**)p1;
    const player_t *player2 = *(const player_t**)p2;

	if (player1->clientNum == eth32.cg.snap->ps.persistant[PERS_ATTACKER])
		return -1;
	else if (player2->clientNum == eth32.cg.snap->ps.persistant[PERS_ATTACKER])
		return 1;
	else
		return SortDistance( p1, p2 );
}

int SortCrosshair(const void *p1, const void *p2)
{
	const player_t *player1 = *(const player_t**)p1;
	const player_t *player2 = *(const player_t**)p2;

	if (Aimbot.CrosshairDistance(player1) < Aimbot.CrosshairDistance(player2))
		return -1;
	else
		return 1;
}

bool CAimbot::IsSingleShotWeapon(int weapon)
{
	//int weapon = eth32.cg.snap->ps.weapon;
	switch(weapon) {
		case WP_K43:
		case WP_GARAND:
		case WP_CARBINE:
		case WP_KAR98:
		case WP_LUGER:
		case WP_SILENCER:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
		case WP_COLT:
		case WP_SILENCED_COLT:
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
			return true;
		default:
			return false;
	}
}

bool CAimbot::IsAimableWeapon(int weapon)
{
	//int weapon = eth32.cg.snap->ps.weapon;
	switch (weapon) {
		case WP_KNIFE:
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_BINOCULARS:
		case WP_PANZERFAUST:
		case WP_MEDIC_SYRINGE:
		case WP_MEDIC_ADRENALINE:
		case WP_AMMO:
		case WP_MEDKIT:
		case WP_ARTY:
		case WP_DYNAMITE:
		case WP_MORTAR:
		case WP_MORTAR_SET:
		case WP_SATCHEL:
		case WP_LANDMINE:
			return false;
		default:
			return true;
	}
}

// normal bullet aimbot at full efficiency
void CAimbot::DoBulletBot(void)
{
	// autofire
	autoMode = (eth32.cg.currentWeapon->autofire && eth32.settings.autofire && eth32.settings.aimType != AIM_ON_FIRE);
	
	// validate target
	autoMode |= (eth32.settings.aimType == AIM_ON_FIRE && eth32.settings.atkValidate);

	// Do not aim under these conditions
	if (
		// Weapon parameters not known or weapon set to not aim
		!eth32.cg.currentWeapon || !eth32.settings.aimSort || !(eth32.cg.currentWeapon->attribs & WA_USER_DEFINED) ||
		// global aim is off or set for on fire without fire button down
		eth32.settings.aimType == AIM_OFF || 
		(eth32.settings.aimType == AIM_ON_FIRE && !attackPressed) ||
		(eth32.settings.aimType == AIM_ON_BUTTON && !aimkeyPressed) ||
		// Spectating/following a player
		eth32.cg.clientNum != eth32.cg.snap->ps.clientNum ||
		eth32.cg.cgTime == eth32.cg.snap->serverTime ||
		// Reloading
		eth32.cg.snap->ps.weaponstate == WEAPON_RELOADING ||
		// Clip is empty
		!eth32.cg.snap->ps.ammoclip[orig_BG_FindClipForWeapon(eth32.cg.snap->ps.weapon)] ||
		// Current weapon is knife
		eth32.cg.snap->ps.weapon == WP_KNIFE ||
		// Limbo panel is up
		Gui.IsLimboPanelUp()
	)
		return;		

	player_t *player;

	// sol: lock target check here
	if (eth32.settings.lockTarget && lastTarget && lastTargetValid) { // redundant :P
		player = lastTarget;

		if (eth32.settings.headbody == HEAD_PRIORITY || eth32.settings.headbody == HEAD_BODY) {
			if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
				VectorCopy(player->headPt, player->aimPt);
				target = player;
			} else if (traceBody(player->aimPt, player->clientNum ))
				target = player;
		} else if (eth32.settings.headbody == BODY_HEAD) {
			if (traceBody(player->aimPt, player->clientNum ))
				target = player;
		  	else if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
				VectorCopy(player->headPt, player->aimPt);
				target = player;
			}
		} else if (eth32.settings.headbody == HEAD_ONLY) {
			if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
				VectorCopy(player->headPt, player->aimPt);
				target = player;
			}
		} else if (eth32.settings.headbody == BODY_ONLY) {
			if (traceBody(player->aimPt, player->clientNum ))
				target = player;
		}
	}

	// sol: locked target wasn't valid or isn't visible, check normal target list
	if (!target) {
		// run sort based on selection type
		if (eth32.settings.aimSort == SORT_DISTANCE)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortDistance);
		else if (eth32.settings.aimSort == SORT_ATTACKER)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortAttacker);
		else if (eth32.settings.aimSort == SORT_CROSSHAIR)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortCrosshair);
		else if (eth32.settings.aimSort == SORT_KDRATIO)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortKdRatio);
		else if (eth32.settings.aimSort == SORT_ACCURACY)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortAccuracy);
		else if (eth32.settings.aimSort == SORT_THREAT)
			qsort(frameTargets, numFrameTargets, sizeof(player_t*), SortThreat);
		else
			return;	// should never get here

		// sol: modified to allow user to choose priority, may need a seperate function to keep this clean
		if (eth32.settings.headbody == HEAD_PRIORITY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}
			}

			if (!target){
				for (int i=0 ; i<numFrameTargets ; i++)
				{
					player = frameTargets[i];

					if (traceBody(player->aimPt, player->clientNum )) {
						target = player;
						break;
					}
				}
			}
		}
		else if (eth32.settings.headbody == HEAD_BODY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}
				if (!target) {
					if (traceBody(player->aimPt, player->clientNum )) {
						target = player;
						break;
					}
				}
			}
		}
		else if (eth32.settings.headbody == BODY_HEAD) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceBody(player->aimPt, player->clientNum )) {
					target = player;
					break;
				}

				if (!target) {
					if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
						VectorCopy(player->headPt, player->aimPt);
						target = player;
						break;
					}
				}
			}
		}
		else if (eth32.settings.headbody == HEAD_ONLY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceHead(&player->orHead, player->headPt, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}
			}
		}
		else if (eth32.settings.headbody == BODY_ONLY) {
			for (int i=0 ; i<numFrameTargets ; i++)
			{
				player = frameTargets[i];

				if (traceBody(player->aimPt, player->clientNum )) {
					target = player;
					break;
				}
			}
		}
	}

	// set lastTarget here, we don't want preaim targets to lock
	lastTarget = (target) ? target : NULL;

	// do preaim and preshoot stuff if we do not have a valid target yet
	if (!target && (eth32.settings.preAim || eth32.settings.preShoot)) {
		vec3_t ppos, spos;
		
		VectorMA(eth32.cg.muzzle, eth32.settings.preAimTime*0.001, eth32.cg.predictedps->velocity, ppos);
		VectorMA(eth32.cg.muzzle, eth32.settings.preShootTime*0.001, eth32.cg.predictedps->velocity, spos);

		if (eth32.settings.preAim) {
			for (int i=0 ; i<numFrameTargets ; i++) {
				player = frameTargets[i];

				// use head "origin", somewhere in the backpack. This distance from this
				// point to a future aimpoint is smallest, thus minimizing spread
				VectorMA(player->orHead.origin, eth32.settings.preAimTime*0.001, player->cent->currentState.pos.trDelta, player->headPt);

				// point and lock, but do not fire!
				if (Engine.IsVisible(ppos, player->headPt, player->clientNum)) {
					target = player;
					VectorCopy(target->orHead.origin, target->aimPt);
					LockSensitivity(true);
					Point(eth32.cg.muzzle);
					return;
				}
			}
		}
		
		if (eth32.settings.preShoot) {
			for (int i=0 ; i<numFrameTargets ; i++) {
				player = frameTargets[i];

				VectorMA(player->orHead.origin, eth32.settings.preShootTime*0.001, player->cent->currentState.pos.trDelta, player->orHead.origin);
				if (traceHead(&player->orHead, player->headPt, spos, player->clientNum)) {
					VectorCopy(player->headPt, player->aimPt);
					target = player;
					break;
				}
			}
		}		
	}
	
	// we have a valid (i.e. real) target - fire away
	if (target) {
		applySelfPrediction();
		AdjustDelay(target);
		eth32.settings.predTarget = target->omniBot ? eth32.settings.predbot : eth32.settings.pred;
		LockSensitivity(true);
		
		// sol: added check on target->lf, CG_SetLerpFrameAnimationRate not defined for all mods :P
		if (target->lf && (target->lf->frame == 1947 || target->lf->frame == 1998)) {
			VectorMA(target->aimPt, eth32.settings.animCorrection, zAxis, target->aimPt);
		} 
		Autocrouch(true, false);
		
		// we can improve acc even more, when first bullet fires and AC is on,
		// locally we think we're standing but server already crouched, so adjust
		if (acFirstFrame) {
			eth32.cg.muzzle[2] -= 24.0f;
			Point(eth32.cg.muzzle);
		} else
			Point(eth32.cg.muzzle);				
		
		// Speedshooting for single shot weapons
		if (IsSingleShotWeapon(eth32.cg.snap->ps.weapon)) {
			if  (eth32.cg.cgTime - this->lastShotTime >= 30) {
				Autofire(true);
				this->lastShotTime = eth32.cg.cgTime;
			} else
				Autofire(false);
		} else {
			if (eth32.cg.currentWeapon->delay) {
				if (eth32.cg.cgTime - this->lastShotTime > eth32.cg.currentWeapon->delay) {
					Autofire(true);
					this->lastShotTime = eth32.cg.cgTime;					
				} else
					Autofire(false);
					
				return;
			}
			// Do not autofire if we're jumping/falling
			/*if (eth32.cg.snap->ps.velocity[2] > 100.f || eth32.cg.snap->ps.velocity[2] < -100.f)
				Autofire(false);
			else*/
			Autofire(true);
		}
	} else {
		if (eth32.settings.atkValidate) // Allows us to shoot more than 1 bullet manually if autofire is enabled
			Autofire(false);
	}
}

inline void CAimbot::applySelfPrediction()
{
	vec3_t vs;

	if (eth32.cg.predictedps)
		VectorCopy(eth32.cg.predictedps->velocity, vs);
	else
		VectorCopy(eth32.cg.snap->ps.velocity, vs);

	switch (eth32.settings.predSelfType)
	{
		case SPR_OFF:
			return;
		case SPR_MANUAL:
			VectorMA(target->aimPt, eth32.settings.predSelf, vs, target->aimPt);
			return;
		case SPR_PING:
			VectorMA(target->aimPt, (eth32.cg.snap->ping * -0.0001f), vs, target->aimPt);
			return;
		case SPR_LEET: {
			vec3_t displ, v1, vp, p;
			float dt = -1.0f * Engine.FrameCorrectionTime(-1);
			
			VectorSubtract(target->aimPt, eth32.cg.muzzle, p);
			VectorNormalizeFast(p);
		
			VectorScale(p, DotProduct(vs, p), v1);
			VectorSubtract(vs, v1, vp);
			VectorScale(vp, dt, displ);
		
			VectorAdd(target->aimPt, displ, target->aimPt);
			return;
		} default:
			return;
	}
}

void CAimbot::Point(vec3_t vieworg)
{
	vec3_t org, ang;

	if (eth32.settings.drawBulletRail && eth32.cg.snap->ps.weaponTime < 40)
		Engine.MakeRailTrail( eth32.cg.muzzle, target->aimPt, false, eth32.settings.colorBulletRail, 40);

	// Get the vector difference and calc the angle form it
	VectorSubtract(target->aimPt, vieworg, org);
	Tools.VectorAngles(org, ang);
	
	ANG_CLIP(ang[YAW]);
	ANG_CLIP(ang[PITCH]);
	
	ang[YAW] -= eth32.cg.refdefViewAngles[YAW];
	ang[PITCH] = -ang[PITCH] - eth32.cg.refdefViewAngles[PITCH]; // Somehow pitch angles are inverted so we must make then negative first

	// Add the angle difference to the view (i.e aim at the target)
	*(float *)eth32.game.viewAngleX += ang[YAW];
	*(float *)eth32.game.viewAngleY += ang[PITCH];
}

// best of both worlds ;)
void CAimbot::LockSensitivity(bool lock){ senslocked = lock; }
bool CAimbot::SensitivityLocked(){ return senslocked; }
void CAimbot::SetAttackButton( void *ptr ) { atkBtn = (kbutton_t *)ptr; }
void CAimbot::SetCrouchButton( void *ptr ) { crhBtn = (kbutton_t *)ptr; }

void CAimbot::Autocrouch(bool enable, bool force)
{
	static bool autoCrouching = false;
	acFirstFrame = false;

	// force release of crouch, else tapout wont work :(
	if (!enable && force) {
		crhBtn->active = crhBtn->down[0] = crhBtn->down[1] = 0;
		autoCrouching = false;
		return;
	}

	// dont bother if user is manually crouching
	if ((userCrouching && !autoCrouching) || !eth32.settings.autoCrouch){
		autoCrouching = false;
		return;
	}

	if (!autoCrouching && enable){
		// first check aimpt is still visible after crouch to prevent oscilation
		eth32.cg.muzzle[2] -= 24.0f;
		if (Engine.IsVisible(eth32.cg.muzzle, target->aimPt, target->clientNum)){
			crhBtn->active = crhBtn->wasPressed = 1;
			crhBtn->down[0] = -1;
			crhBtn->downtime = *eth32.game.com_frameTime;
			autoCrouching = true;
			acFirstFrame = true;
		}
		eth32.cg.muzzle[2] += 24.0f;
	} else if (autoCrouching && !enable) {
		crhBtn->active = crhBtn->down[0] = crhBtn->down[1] = 0;
		autoCrouching = false;
	}
}

void CAimbot::Autofire(bool enable)
{
	// don't mess with attack if not in control
	if (!autoMode)
		return;

	if (atkBtn->wasPressed || autoMode) {
		// check for heat
		if (enable && (eth32.cg.currentWeapon->attribs & WA_OVERHEAT) && eth32.cg.snap->ps.curWeapHeat > 200)
			enable = false;

		if (enable)
			atkBtn->active = 1;			// set fire
		else
			atkBtn->active = 0;
	}
}

void CAimbot::SetUserCrouchStatus(bool crouched)  { userCrouching = crouched; }
void CAimbot::SetUserAttackStatus(bool atkPressed){ attackPressed = atkPressed; }

float CAimbot::CrosshairDistance(const player_t *player)
{
	int x_dif=0, y_dif=0;

	// Calculate obtuse & adjacent
	player->screenX > eth32.cg.centerX ? x_dif = player->screenX - eth32.cg.centerX : x_dif = eth32.cg.centerX - player->screenX;
	player->screenY > eth32.cg.centerY ? y_dif = player->screenY - eth32.cg.centerY : y_dif = eth32.cg.centerY - player->screenY;

	// Return hypotenuse
	float h2 = ((x_dif*x_dif)+(y_dif*y_dif));

	return h2;
}

int CAimbot::CheckFov(vec3_t origin)
{
	if (AIM_FOV_VALUE > 359.f)
		return 1;

	vec3_t dir;
	float ang;
	VectorSubtract( origin, eth32.cg.refdef->vieworg, dir );
	VectorNormalizeFast( dir );
	ang = 57.295779f*acos(DotProduct( dir, eth32.cg.refdef->viewaxis[0] ));

	if (ang <= AIM_FOV_VALUE)
		return 1;
	else
		return 0;
}

void CAimbot::History(player_t *player)
{
	if (!player)
		return;

	if (eth32.settings.ballisticPredict == RF_PREDICT_OFF)
		return;

	if (!(eth32.cg.currentWeapon->attribs & WA_BALLISTIC))
		return;

	int cidx = player->history_idx - 1;
	if (cidx < 0)
		cidx = MAX_HISTORY;

	if (eth32.cg.time - (int)player->history[cidx].t < 15)
		return;

	VectorCopy(player->lerpOrigin, player->history[player->history_idx].p);
	player->history[player->history_idx].t = (float) eth32.cg.time;

	if (player->history_idx == MAX_HISTORY)
		player->history_idx = 0;
	else
		player->history_idx++;
}

void CAimbot::selectGrenadeTarget(int direction)
{
	player_t *player;
	player_t *targets[MAX_CLIENTS];

	memset(targets, 0, MAX_CLIENTS*sizeof(player_t *));

	// get list of valid targets
	int n = 0;
	for(int i=0; i<numFrameTargets; i++){
		player = frameTargets[i];

		if (!player || player == grenadeTarget)
			continue;

		if (Draw.worldToScreen(player->lerpOrigin, &player->screenX, &player->screenY))
			targets[n++] = player;
	}

	// select
	float x, x0 = 1000;
	int m = -1;

	//set the one closest horizontally to crosshair
	for(int i=0; i<n; i++){
		if (!grenadeTarget)
			x = targets[i]->screenX < 0 ? -targets[i]->screenX : targets[i]->screenX;
		else if (direction > 0)
			x = targets[i]->screenX - grenadeTarget->screenX;
		else
			x = grenadeTarget->screenX - targets[i]->screenX;

		if (x < x0 && x >= 0){
			x0 = x;
			m = i;
		}
	}

	if (m < 0)
		grenadeTarget = NULL;
	else
		grenadeTarget = targets[m];
}

void CAimbot::DrawGrenadelauncherTrace()
{
	vec3_t angles, forward, viewpos, lastPos;
	bool once = false;

	if (!eth32.settings.rifleTracer)
		return;

	if (!(eth32.cg.currentWeapon->attribs & WA_RIFLE_GRENADE))
		return;

	if (eth32.cg.snap->ps.weaponTime != 0 )
		return;

	if (Engine.forward == NULL)
		return;

	trajectory_t rifleTrajectory;
	rifleTrajectory.trType = TR_GRAVITY;
	rifleTrajectory.trTime = eth32.cg.time;
	VectorCopy(eth32.cg.muzzle, rifleTrajectory.trBase);
	VectorCopy(rifleTrajectory.trBase, viewpos);

	VectorCopy(eth32.cg.refdefViewAngles, angles);
	VectorCopy(Engine.forward, forward);
	VectorMA(viewpos, 32, forward, viewpos);
	VectorScale(forward, 2000, forward);

	VectorCopy(forward, rifleTrajectory.trDelta);
	SnapVector(rifleTrajectory.trDelta);

	// Calculate rifle impact
	int timeOffset = 0;
	trace_t rifleTrace;
	vec3_t rifleImpact;
	VectorCopy(rifleTrajectory.trBase, rifleImpact);
	#define TIME_STEPP 50
	int maxTime = 3250;
	int totalFly = 0;

	while (timeOffset < maxTime) {
		vec3_t nextPos;
		timeOffset += TIME_STEPP;
		totalFly += TIME_STEPP;

		BG_EvaluateTrajectory(&rifleTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&rifleTrace, rifleImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);

		// check for hit
		if (rifleTrace.startsolid || rifleTrace.fraction != 1) {
			// When a nade flies for over 750ms and hits somnexing, it'll explode
			if (totalFly > 750)
				break;

			// calculate reflect angle (forward axis)
			int hitTime = eth32.cg.time + totalFly - TIME_STEPP + TIME_STEPP * rifleTrace.fraction;
			BG_EvaluateTrajectoryDelta(&rifleTrajectory, hitTime, rifleTrajectory.trDelta, qfalse, 0);
			float dot = DotProduct(rifleTrajectory.trDelta, rifleTrace.plane.normal);
			VectorMA(rifleTrajectory.trDelta, -2*dot, rifleTrace.plane.normal, rifleTrajectory.trDelta);

			VectorScale(rifleTrajectory.trDelta, 0.35, rifleTrajectory.trDelta);

			if (rifleTrace.surfaceFlags == 0)
				VectorScale(rifleTrajectory.trDelta, 0.5, rifleTrajectory.trDelta);

			// calc new max time and reset trTime
			maxTime -= timeOffset;
			timeOffset = 0;
			rifleTrajectory.trTime = eth32.cg.time;

			// new base origins
			VectorCopy(rifleTrace.endpos, rifleTrajectory.trBase);

			SnapVector(rifleTrajectory.trDelta);
			SnapVector(rifleTrajectory.trBase);
		} else {
			VectorCopy(nextPos, rifleImpact);
       		if(!once)
			{
				VectorCopy(nextPos, lastPos);
				once = true;
			}
			if(Engine.IsVisible(eth32.cg.refdef->vieworg, nextPos, 0))
       			Engine.MakeRailTrail( lastPos, nextPos, false, colorGreen, eth32.cg.frametime*3 );
			else
				Engine.MakeRailTrail( lastPos, nextPos, false, colorMagenta, eth32.cg.frametime*4 );

			VectorCopy(nextPos, lastPos);
		}
	}

	// copy the results for the cams
	VectorCopy( lastPos, lastImpact );
	Aimbot.flyTime = totalFly;
}

void CAimbot::DrawGrenadeTrace()
{
	vec3_t forward, tosspos, viewpos, lastPos, right;

	float pitch, upangle;
	bool once = false;

	if (!eth32.settings.grenadeTracer)
		return;

	if (!(eth32.cg.currentWeapon->attribs & WA_GRENADE))
		return;

	if (Engine.forward == NULL)
		return;

	trajectory_t grenadeTrajectory;
	grenadeTrajectory.trType = TR_GRAVITY;
	grenadeTrajectory.trTime = eth32.cg.time;


	AngleVectors(eth32.cg.refdefViewAngles, NULL, right, NULL);
	VectorCopy(Engine.forward, forward);
	VectorMA(eth32.cg.muzzle, 8, forward, tosspos);
	VectorMA(tosspos, 20, right, tosspos);
	tosspos[2] -= 8;
	SnapVector( tosspos );

	pitch = eth32.cg.refdefViewAngles[0];
	if( pitch >= 0 ) {
		forward[2] += 0.5f;
		pitch = 1.3f;
	}
	else {
		pitch = -pitch;
		pitch = min( pitch, 30 );
		pitch /= 30.f;
		pitch = 1 - pitch;
		forward[2] += (pitch * 0.5f);
		pitch *= 0.3f;
		pitch += 1.f;
	}

	upangle = -eth32.cg.refdefViewAngles[0];
	upangle = min(upangle, 50);
	upangle = max(upangle, -50);
	upangle = upangle/100.0f;
	upangle += 0.5f;

	if(upangle < .1)
		upangle = .1;
	upangle *= 900.0f*pitch;

	VectorNormalizeFast( forward );

	// check for valid start spot (so you don't throw through or get stuck in a wall)
	VectorCopy( self->lerpOrigin, viewpos );
	viewpos[2] += eth32.cg.snap->ps.viewheight;

	trace_t tr;
	orig_CG_Trace(&tr, viewpos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), tosspos, self->currentState->number, MASK_MISSILESHOT);

	if( tr.startsolid ) {
		VectorCopy( forward, viewpos );
		VectorMA( self->lerpOrigin, -24.f, viewpos, viewpos );

		orig_CG_Trace(&tr, viewpos, tv(-4.f, -4.f, 0.f), tv(4.f, 4.f, 6.f), tosspos, self->currentState->number, MASK_MISSILESHOT);
		VectorCopy( tr.endpos, tosspos );
	} else if( tr.fraction < 1 ) {	// oops, bad launch spot
		VectorCopy( tr.endpos, tosspos );
		SnapVectorTowards( tosspos, viewpos );
	}

	VectorScale(forward, upangle, forward);
	VectorCopy(tosspos, grenadeTrajectory.trBase);
	VectorCopy(forward, grenadeTrajectory.trDelta);
	SnapVector(grenadeTrajectory.trDelta);

	// Calculate grenade impact
	int timeOffset = 0;
	trace_t grenadeTrace;
	vec3_t grenadeImpact;
	VectorCopy(grenadeTrajectory.trBase, grenadeImpact);
	#define TIME_STEPP 50
	int maxTime = 4000;
	int totalFly = 0;
	int bounces = 0;

	while (timeOffset < maxTime) {
		vec3_t nextPos;
		timeOffset += TIME_STEPP;
		totalFly += TIME_STEPP;

		BG_EvaluateTrajectory(&grenadeTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		orig_CG_Trace(&grenadeTrace, grenadeImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);

		// check for hit
		if (grenadeTrace.startsolid || grenadeTrace.fraction != 1) {
			// calculate reflect angle (forward axis)
			int hitTime = eth32.cg.time + totalFly - TIME_STEPP + TIME_STEPP * grenadeTrace.fraction;
			BG_EvaluateTrajectoryDelta(&grenadeTrajectory, hitTime, grenadeTrajectory.trDelta, qfalse, 0);
			float dot = DotProduct(grenadeTrajectory.trDelta, grenadeTrace.plane.normal);
			VectorMA(grenadeTrajectory.trDelta, -2*dot, grenadeTrace.plane.normal, grenadeTrajectory.trDelta);

			bounces++;
			VectorScale(grenadeTrajectory.trDelta, powf(0.35, bounces), grenadeTrajectory.trDelta);

			if (VectorLength(grenadeTrajectory.trDelta) < 30.0f)
				return;

			if (grenadeTrace.surfaceFlags == 0)
				VectorScale(grenadeTrajectory.trDelta, 0.5, grenadeTrajectory.trDelta);

			// calc new max time and reset trTime
			maxTime -= timeOffset;
			timeOffset = 0;
			grenadeTrajectory.trTime = eth32.cg.time;

			// new base origins
			VectorCopy(grenadeTrace.endpos, grenadeTrajectory.trBase);

			SnapVector(grenadeTrajectory.trDelta);
			SnapVector(grenadeTrajectory.trBase);
		} else {
			VectorCopy(nextPos, grenadeImpact);
       		if(!once)
			{
				VectorCopy(nextPos, lastPos);
				once = true;
			}
			if(Engine.IsVisible(eth32.cg.refdef->vieworg, nextPos, 0))
       			Engine.MakeRailTrail( lastPos, nextPos, false, colorGreen, eth32.cg.frametime*3 );
			else
				Engine.MakeRailTrail( lastPos, nextPos, false, colorMagenta, eth32.cg.frametime*4 );

			VectorCopy(nextPos, lastPos);
		}
	}

	// copy the results for the cams
	VectorCopy( lastPos, lastImpact );
	Aimbot.flyTime = totalFly;
}

/*					grenade trajectory explanation

given fixed endpos, startpos, and velocity, the pitch angle can be calculated
in closed form,

	g*s*tan(angle) = v^2 - sqrt(v^4 - 2*z*g*v^2 - s*s*g^2);

where g is gravity, v the velocity, z the vertical distance, and s the horizontal distance.
However, this solution is only valid in the case of fixed velocity. Grenades are
thrown with a velocity that depends on the pitch angle, which is the one we're
trying to find, with the result that the angle cannot be calculated in closed form anymore.

Since the velocity derivative is small compared to angular dependency and horizontal distance,
we can just iterate a few tries with sufficient accuracy

*/
bool CAimbot::grenadePitchAngle(vec3_t start, vec3_t endpos, float maxTime, float *flytime, float *angle, bool longOnly)
{
	float Di,s, z, zc, p, t, v, ez;
	float p0, p1, rpitch;
	const float g = 800.0f;
	vec3_t D;
	bool firstTry = true;

	// max error in end position
	ez = 10.0;

	// get the horizontal and vertical distance
	VectorSubtract(endpos, start, D);
	s = sqrtf(D[0]*D[0]+D[1]*D[1]);		// horizontal
	z = D[2];							// vertical

	// multiplication constant
	p = 1.0f/(g*s);

	// initial guess for pitch angle
	p0 = 0.78;

	for (int i=0;i <100; i++) {
		this->grenadePitchCorrection(p0, &rpitch);
		v = speedForPitch(rpitch);

		Di = v*v*v*v - 2.0f*z*g*v*v - g*g*s*s;

		if (Di < 0) {
			if (firstTry) {
				firstTry = false;
				i = -1;
				p0 = 0.78;
				continue;
			}
			return false;
		} else if (Di == 0) {
			*angle = atanf(v*v*p);
			return true;
		} else {
			if (firstTry && !longOnly)
				p1 = atanf(v*v*p-sqrtf(Di)*p);
			else
				p1 = atanf(v*v*p+sqrtf(Di)*p);
		}

		// smoothly converge to prevent premature jumping to other solution
		p0 += (p1-p0)*0.025;
	}

	t = s/(v*cosf(p0));
	rifleGrenadeTime = t;

	if (t > 0 && t < maxTime) {
		// make sure our speed & pitch do indeed solve the equation
		zc = v*sinf(p0)*t - 0.5*g*t*t;
		zc -= z;
		zc = zc < 0.0f ? -1.0f*zc : zc;
		if (zc < ez){
			*angle = p0;
			*flytime = t;
			return true;
		}
	}

	return false;
}

// Grenades aren't launched with a synced forward direction
// use a pre-calculated lookup table to translate grenade pitch angle to view pitch angle
void CAimbot::grenadePitchCorrection(float pitch, float *z)
{
	float gp;
	gp = -pitch*57.2957795131;

	if (gp < -30.0 || gp > 87.74824){
		*z = pitch;
		return;
	}

	float f = gp*2 + 60.0;
	int m = (int)f;
	f -= (float)m;

	*z = pitchLUT[m] + f*(pitchLUT[m+1]-pitchLUT[m]);
	*z *= -0.01745329252;
}

// pieced together from SDK - dont blame me for this mess!
float CAimbot::speedForPitch(float pitch)
{
	pitch *= -57.29578;

	if (pitch > 40.0)
		return 117.0;

	if (pitch < -50.0)
		return 900.0;

	float f = (pitch + 50.0)*2.0;
	int m = (int)f;
	f -= (float)m;

	float speed = speedLUT[m] + f*(speedLUT[m+1]-speedLUT[m]);
	return speed;
}

// calculates the pitch angle for non powered flight given startpos and endpos
bool CAimbot::ballisticPitchAngle(vec3_t start, vec3_t endpos, float v, float maxTime, float *flytime, float *angle)
{
	float pitch, Di, s, z, zc, p, t, ez;
	const float g = 800.0f;
	vec3_t D;

	// set tolerance in calculated endpos
	ez = 5.0f;

	VectorSubtract(endpos, start, D);
	s = sqrtf(D[0]*D[0]+D[1]*D[1]);
	z = D[2];

	Di = v*v*v*v - 2.0f*z*g*v*v - g*g*s*s;
	p = 1.0f/(g*s);
	if (Di < 0) {
		// no solution
		return false;

	} else if (Di == 0) {
		*angle = atanf(v*v*p);
		return true;
	} else {
		pitch = atanf(v*v*p-sqrtf(Di)*p);

		t = s/(v*cosf(pitch));
		rifleGrenadeTime = t;

		if (t > 0.1 && t < maxTime) {
			zc = v*sinf(pitch)*t - 0.5*g*t*t;
			zc -= z;
			zc = zc < 0.0f ? -1.0f*zc : zc;
			if (zc < ez){
				*angle = pitch;
				if (flytime)
					*flytime = t;
				return true;
			}
		}
	}
	return false;
}

// see if we dont hit anything along the way
bool CAimbot::ballisticTrajectoryValid(vec3_t start, vec3_t end, float pitch, float flytime, float v)
{
	float t, dt, vs, vz;
	const float g = 800.0;
	vec3_t d0, p0, p1;
	trace_t tr;

	vs = v*cosf(pitch);
	vz = v*sinf(pitch);

	VectorSubtract(end, start, d0);
	d0[2] = 0;
	VectorNormalizeFast(d0);

	// about 20 traces
	dt = flytime/20.0;

	VectorCopy(start, p0);
	for (t=dt; t<flytime; t+=dt) {
		VectorMA(start, t*vs, d0, p1);
		p1[2] += vz*t - 0.5*g*t*t;
		orig_CG_Trace(&tr, p0, 0, 0, p1, grenadeTarget->clientNum, MASK_MISSILESHOT);

		if (tr.fraction < 1.0f || tr.allsolid){
			// we hit something :(
			return false;
		}

		VectorCopy(p1, p0);
	}

	return true;
}

bool CAimbot::rifleEndPos(vec3_t forward, vec3_t impact, float *flytime)
{
	vec3_t viewpos, lastPos;
	bool once = false;

	trajectory_t rifleTrajectory;
	rifleTrajectory.trType = TR_GRAVITY;
	rifleTrajectory.trTime = eth32.cg.time;
	VectorCopy(eth32.cg.muzzle, rifleTrajectory.trBase);
	VectorCopy(rifleTrajectory.trBase, viewpos);

	VectorMA(viewpos, 32, forward, viewpos);
	VectorScale(forward, 2000, forward);

	VectorCopy(forward, rifleTrajectory.trDelta);
	SnapVector(rifleTrajectory.trDelta);

	// Calculate rifle impact
	int timeOffset = 0;
	trace_t rifleTrace;
	vec3_t rifleImpact;
	VectorCopy(rifleTrajectory.trBase, rifleImpact);
	#define TIME_STEPP 50
	int maxTime = 3250;
	int totalFly = 0;

	while (timeOffset < maxTime) {
		vec3_t nextPos;
		timeOffset += TIME_STEPP;
		totalFly += TIME_STEPP;

		BG_EvaluateTrajectory(&rifleTrajectory, eth32.cg.time + timeOffset, nextPos, qfalse, 0);
		//orig_CG_Trace(&rifleTrace, rifleImpact, 0, 0, nextPos, eth32.cg.snap->ps.clientNum, MASK_MISSILESHOT);
		orig_syscall(CG_CM_BOXTRACE, &rifleTrace, rifleImpact, nextPos, 0, 0, 0, MASK_MISSILESHOT);

		// check for hit
		if (rifleTrace.startsolid || rifleTrace.fraction != 1) {
			// When a nade flies for over 750ms and hits something, it'll explode
			if (totalFly > 750)
				break;

			// calculate reflect angle (forward axis)
			int hitTime = eth32.cg.time + totalFly - TIME_STEPP + TIME_STEPP * rifleTrace.fraction;
			BG_EvaluateTrajectoryDelta(&rifleTrajectory, hitTime, rifleTrajectory.trDelta, qfalse, 0);
			float dot = DotProduct(rifleTrajectory.trDelta, rifleTrace.plane.normal);
			VectorMA(rifleTrajectory.trDelta, -2*dot, rifleTrace.plane.normal, rifleTrajectory.trDelta);

			VectorScale(rifleTrajectory.trDelta, 0.35, rifleTrajectory.trDelta);

			if (rifleTrace.surfaceFlags == 0)
				VectorScale(rifleTrajectory.trDelta, 0.5, rifleTrajectory.trDelta);

			// calc new max time and reset trTime
			maxTime -= timeOffset;
			timeOffset = 0;
			rifleTrajectory.trTime = eth32.cg.time;

			// new base origins
			VectorCopy(rifleTrace.endpos, rifleTrajectory.trBase);

			SnapVector(rifleTrajectory.trDelta);
			SnapVector(rifleTrajectory.trBase);
		} else {
			VectorCopy(nextPos, rifleImpact);
       		if(!once)
			{
				VectorCopy(nextPos, lastPos);
				once = true;
			}

			VectorCopy(nextPos, lastPos);
		}
	}

	VectorCopy( lastPos, impact );
	*flytime = totalFly;
}

// multi bounce is somewhat different. The only way is to try a bunch 
// of guesses near the player and see where it ends up
bool CAimbot::multibounceAimbot(vec3_t point, float *fltime, float *ptch)
{
	// perpendicular vecs px, pz
	vec3_t p, px, pz, ds, dv, t, forward;
	VectorSubtract(point, eth32.cg.muzzle, ds);
	
	pz[0] = -ds[1];
	pz[1] = ds[0];
	pz[2] = 0.0;
	
	px[0] = -ds[2]*ds[0];
	px[1] = -ds[2]*ds[1];
	px[2] = ds[0]*ds[0]+ds[1]*ds[1];
	
	VectorNormalizeFast(pz);
	VectorNormalizeFast(px);
	
	float m = 0.785398; // 2*pi/8
	float pitch, bestPitch;
	float best = 40000.0;
	
	// check trajectories on expanding ring from target center
	for (int z=1; z<6; z++) {
		for (int a=0; a<8; a++) {
			VectorMA(point, 150.0*(float)z*sinf((float)a*m), px, p);
			VectorMA(p, 150.0*(float)z*cosf((float)a*m), pz, p);
			
			// get angle
			if (!ballisticPitchAngle(eth32.cg.muzzle, p, 2000.0, 3.0, NULL, &pitch))
				continue;
			
			// get launch dir
			VectorSubtract(p, eth32.cg.muzzle, ds);
			dv[0] = ds[0]; dv[1] = ds[1]; dv[2] = 0;
			VectorNormalizeFast(dv);
			dv[0] = dv[0]*cosf(pitch);
			dv[1] = dv[1]*cosf(pitch);
			dv[2] = sinf(pitch);
			
			// determine if we "hit" target
			rifleEndPos(dv, t, fltime);
			
			VectorSubtract(t, point, ds);
			if (DotProduct(ds, ds) < best) {
				best = DotProduct(ds, ds);
				bestPitch = pitch;
				VectorCopy(p, grenadeTarget->aimPt);
			}
		}
	}
	
	if (best < 40000.0) {
		*ptch = bestPitch;
		return true;
	} else	
		return false;
}

void CAimbot::grenadeAimbot(vec3_t point, float *fltime, bool *solution, bool pass)
{
	float pitch, flytime;
	bool doAim = false;

	RifleMultiBounce = false;

	// needed for prediction
	*solution = false;

	// grenades need senselock a *little* longer
	if (eth32.cg.time < (grenadeFireTime+300))
		LockSensitivity(true);

	if (eth32.cg.currentWeapon->attribs & WA_GRENADE) {
		flytime = 5.0;
		point[2] += eth32.settings.grenadeZ;

		// for validation of trajectory for grenades, we need an extra pass because more solutions are possible
		*solution = GrenadeFireOK = grenadePitchAngle(eth32.cg.muzzle, point, 5.0, &flytime, &pitch, false);
		if (eth32.settings.valGrenTrajectory && !ballisticTrajectoryValid(eth32.cg.muzzle, grenadeTarget->lerpOrigin, pitch, flytime, speedForPitch(pitch))){
				*solution = GrenadeFireOK = grenadePitchAngle(eth32.cg.muzzle, point, 5.0, &flytime, &pitch, true);
				if (!ballisticTrajectoryValid(eth32.cg.muzzle, grenadeTarget->lerpOrigin, pitch, flytime, speedForPitch(pitch)))
					GrenadeFireOK = false;
		}

		// fire control logic not needed for first prediction pass
		if (!pass) {
			*fltime = flytime;
			return;
		}

		// select conditions to base further actions on
		if (eth32.settings.grenadeAutoFire) {
			if (GrenadeTicking) {
				int trigger = eth32.settings.grenadeFireDelay+(int)(flytime*1000.0);

				if (GrenadeFireOK && eth32.cg.snap->ps.grenadeTimeLeft && eth32.cg.snap->ps.grenadeTimeLeft < trigger){
					// fire!
					atkBtn->active = 0;

					// need to save fire time - grenades arent launched right when we release it
					// so need to lock sensitivity a bit longer
					grenadeFireTime = eth32.cg.time;
					doAim = true;
					GrenadeTicking = false;
					stopAutoTargets = false;
				} else {
					// keep tickin'
					atkBtn->active = 1;
				}
			}

			// user requested to fire the nade in autofire mode, so activate the logic
			if (!GrenadeTicking && this->attackPressed) {
				GrenadeTicking = true;
				atkBtn->active = 1;
			}

			// make sure we drop the potato if its getting too hot
			if (eth32.cg.snap->ps.grenadeTimeLeft && eth32.cg.snap->ps.grenadeTimeLeft < 500){
				GrenadeTicking = false;
				stopAutoTargets = false;
				atkBtn->active = 0;

				if (GrenadeFireOK) {
					grenadeFireTime = eth32.cg.time;
					doAim = true;
				}
			}

		} else {
			// non-autofire mode
			if (!this->attackPressed)
				grenadeFireTime = 0;

			if (this->attackPressed)
				GrenadeTicking = true;
			else if (GrenadeTicking){
				// released attack, which means fire!
				grenadeFireTime = 0;
				GrenadeTicking = false;
				if (GrenadeFireOK) {
					grenadeFireTime = eth32.cg.time;
					doAim = true;
				}
			}
		}

		if (GrenadeFireOK && eth32.settings.grenadeSenslock)
			doAim = true;

	} else if (eth32.cg.currentWeapon->attribs & WA_RIFLE_GRENADE) {
		point[2] += eth32.settings.riflenadeZ;
		GrenadeFireOK = ballisticPitchAngle(eth32.cg.muzzle, point, 2000.0f, 5.0f, &flytime, &pitch);
		if (GrenadeFireOK){
			*solution = true;
			Lastpitch = pitch;
			if (eth32.settings.valRifleTrajectory && !ballisticTrajectoryValid(eth32.cg.muzzle, point, pitch, flytime, 2000.0f))
				GrenadeFireOK = false;

			if ((this->attackPressed || eth32.settings.rifleAutoFire) && GrenadeFireOK){
				doAim = true;
				atkBtn->wasPressed = 1;
				stopAutoTargets = false;
			}
		}

		if (!GrenadeFireOK){		
			if (eth32.settings.allowMultiBounce && multibounceAimbot(point, &flytime, &pitch)) {
				*solution = GrenadeFireOK = RifleMultiBounce = true;
				Lastpitch = pitch;
				
				if ((this->attackPressed || eth32.settings.rifleAutoFire)){					
					atkBtn->wasPressed = 1;
					stopAutoTargets = false;
					doAim = pass = true;
				}
			} else {			
				GrenadeTicking = false;
				GrenadeFireOK = false;
			}
		}
	}

	if (doAim && pass) {
		LockSensitivity(true);

		if (!RifleMultiBounce)
			VectorCopy(point, grenadeTarget->aimPt);

		if (eth32.cg.currentWeapon->attribs & WA_RIFLE_GRENADE){
			Aimbot.PointGrenade(eth32.cg.muzzle, pitch);
		} else if ((eth32.cg.currentWeapon->attribs & WA_GRENADE)) {
			vec3_t newpt;
			vec3_t right;

			// correct for pitch angle ( forward != grenade launchdir )
			grenadePitchCorrection(pitch, &pitch);

			// set different muzzle ( eth32.cg.muzzle != grenade muzzle )
			AngleVectors(eth32.cg.refdefViewAngles, NULL, right, NULL);
			VectorMA(eth32.cg.muzzle, 20, right, newpt);
			newpt[2] -= 8;
			Aimbot.PointGrenade(newpt, pitch);
		}
	}

	// needed for prediction
	*fltime = flytime;
}

void CAimbot::PointGrenade(vec3_t vieworg, float pitch)
{
	vec3_t org, ang;

	// Get the vector difference and calc the angle form it
	VectorSubtract(grenadeTarget->aimPt, vieworg, org);
	Tools.VectorAngles(org, ang);

	ang[PITCH] = -pitch*180/M_PI;
	
	ANG_CLIP(ang[YAW]);
	ANG_CLIP(ang[PITCH]);
	
   	AnglesToAxis(ang, eth32.cg.refdef->viewaxis);
	
    ang[YAW] -= eth32.cg.refdefViewAngles[YAW];
	ANG_CLIP(ang[YAW]);
	
    ang[PITCH] -= eth32.cg.refdefViewAngles[PITCH];
	ANG_CLIP(ang[PITCH]);

	// Add the angle difference to the view (i.e aim at the target)
	*eth32.game.viewAngleX += ang[YAW];
	*eth32.game.viewAngleY += ang[PITCH];
}

//for now use the flytime as cutoff, approx 1 second
void CAimbot::PredictPlayer(player_t *player, float time, vec3_t pos, int type)
{
	if (!player)
		return;

	float tf = time;					// predict ahead time
	float tc = (float)eth32.cg.time;	// current time

	if (type < 0)
		type = eth32.settings.ballisticPredict;

	switch (type)
	{
		case RF_PREDICT_OFF:
			VectorCopy(player->lerpOrigin, pos);
			return;
		case RF_PREDICT_LINEAR: {
			VectorMA(player->lerpOrigin, tf, player->currentState->pos.trDelta, pos);
			return;
		}
		case RF_PREDICT_LINEAR2: {
			VectorMA(player->lerpOrigin, 0.5*tf, player->currentState->pos.trDelta, pos);
			return;
		}
		case RF_PREDICT_AVG: {
			int n = 0;
			double dx, dy, dz;
			dx = dy = dz = 0;

			for (int i=0; i<=MAX_HISTORY; i++) {
				if (tc - player->history[i].t > tf)
					continue;

				dx += (double)player->history[i].p[0];
				dy += (double)player->history[i].p[1];
				dz += (double)player->history[i].p[2];
				n++;
			}

			double q = 1.0/(double)n;
			dx = dx * q;
			dy = dy * q;
			dz = dz * q;

			pos[0] = (float)dx;
			pos[1] = (float)dy;
			pos[2] = (float)dz;

			return;
		}

		case RF_PREDICT_SMART: {
			int cidx = player->history_idx - 1;
			if (cidx < 0)
				cidx = MAX_HISTORY;

			vec3_t p, ds;
			bool pass = true;

			// check for constancy in position
			VectorCopy(player->history[cidx].p, p);

			for (int i=cidx-1; i>=0; i--) {
				if (tc - player->history[i].t > tf)
					continue;
				VectorSubtract(p, player->history[i].p, ds);
				if (DotProduct(ds, ds) > 22500.0) {
					pass = false;
					break;
				}
			}

			if (pass) {
				for (int i=MAX_HISTORY; i>cidx; i--) {
					if (tc - player->history[i].t > tf)
						continue;
					VectorSubtract(p, player->history[i].p, ds);
					if (DotProduct(ds, ds) > 22500.0) {
						pass = false;
						break;
					}
				}
			}

			if (pass) {
				PredictPlayer(player, time, pos, RF_PREDICT_AVG);
				return;
			}

			// moving, so check for velocity constancy
			pass = true;

			vec3_t dv, v0;
			float dt;
			float v2;
			int c = cidx -1;
			if (c < 0)
				c = MAX_HISTORY;

			dt = (player->history[cidx].t - player->history[c].t)*0.001;
			VectorSubtract(player->history[cidx].p, player->history[c].p, ds);
			VectorScale(ds, 1.0/dt, v0);
			v2 = 0.8*DotProduct(v0, v0);

			for (int i=cidx-1; i>0; i--) {
				if (tc - player->history[i].t > tf)
					continue;
				dt = (player->history[i].t - player->history[i-1].t)*0.001;
				VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
				VectorScale(ds, 1.0/dt, dv);

				if (DotProduct(dv, v0) < v2) {
					pass = false;
					break;
				}
			}

			if (pass) {
				for (int i=MAX_HISTORY; i>cidx+1; i--) {
					if (tc - player->history[i].t > tf)
						continue;
					dt = (player->history[i].t - player->history[i-1].t)*0.001;
					VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
					VectorScale(ds, 1.0/dt, dv);

					if (DotProduct(dv, v0) < v2) {
						pass = false;
						break;
					}
				}
			}

			if (pass) {
				PredictPlayer(player, time, pos, RF_PREDICT_LINEAR);
				return;
			}

			// player is apparantly moving erraticly, so give rough extrap.
			int count = 0;
			int n;

			vec3_t v1, v3;
			VectorCopy(vec3_origin, v1);
			VectorCopy(vec3_origin, v3);

			n = c = 0;

			for (int i=cidx; i>0; i--) {
				if (tc - player->history[i].t > tf)
					continue;
				dt = (player->history[i].t - player->history[i-1].t)*0.001;
				VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
				VectorScale(ds, 1.0/dt, dv);

				if (count < MAX_HISTORY/2) {
					VectorAdd(v1, dv, v1);
					n++;
				} else {
					VectorAdd(v3, dv, v3);
					c++;
				}
				count++;
			}

			for (int i=MAX_HISTORY; i>cidx+1; i--) {
				if (tc - player->history[i].t > tf)
					continue;
				dt = (player->history[i].t - player->history[i-1].t)*0.001;
				VectorSubtract(player->history[i].p, player->history[i-1].p, ds);
				VectorScale(ds, 1.0/dt, dv);

				if (count < MAX_HISTORY/2) {
					VectorAdd(v1, dv, v1);
					n++;
				} else {
					VectorAdd(v3, dv, v3);
					c++;
				}
				count++;
			}

			VectorScale(v1, 1.0/(float)n, v1);
			VectorScale(v3, 1.0/(float)c, v3);

			// make the latest velocities count more
			VectorMA(pos, 0.75*tf, v1, pos);
			VectorMA(pos, 0.25*tf, v3, pos);
		}
	}
}	
