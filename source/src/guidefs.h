// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CAimbot.h"

#define SETFLOAT(x)			((int)(x * 1000))		// only keeping up to 0.001
#define GETFLOAT(x)			(x / 1000.f)

// NOTE: cast arg0-arg4 to int if used, use SETFLOAT/GETFLOAT for floats

static const windef_t windows[] =
{
	{
		"Status",			// title
		WIN_STATUS,			// type
		GUI_MAINVIEW,			// view
		5, 422, 120, 60,			// x, y, w, h
		0,				// num controls
	},
	{
		"Weapon",			// title
		WIN_WEAPON,			// type
		GUI_MAINVIEW,			// view
		95, 422, 120, 60,		// x, y, w, h
		0,				// num controls
	},
	{
		"Respawn",		 	// title
		WIN_RESPAWN,			// type
		GUI_MAINVIEW,			// view
		280, 5, 38, 20,			// x, y, w, h
		0,				// num controls
	},
	{
		"Radar",			// title
		WIN_RADAR,			// type
		GUI_MAINVIEW,			// view
		395, 302, 120, 120,		// x, y, w, h
		0,						// num controls
	},
	{
		"Clients",			// title
		WIN_CLIENTS,			// type
		GUI_CLIENTVIEW,			// view
		5, 5, 630, 470,			// x, y, w, h
		0,				// num controls
	},
	{
		"Banner",
		WIN_BANNER,			// type
		GUI_MAINVIEW,			// view
		20, 20, 1, 1,			// x, y, w, h
		0,				// num controls
	},
	/** *******************************************************************
					AIMBOT
	******************************************************************* **/
	{
		"Aimbot",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 200, 325,		// x, y, w, h
		13,				// num controls
		{
			// Type				Label				X		Y		W			H		Arg0 ... Arg4
			{ CTRL_DROPBOX,		"Aimbot Mode", 		5, 		5,		190,		23,		0, AIMMODE_MAX-1, (uintptr_t)&eth32.settings.aimMode, (uintptr_t)aimModeText },
			{ CTRL_DROPBOX,		"Aim Type",			5,		34,		190,		23,		AIM_OFF, AIM_MAX-1, (uintptr_t)&eth32.settings.aimType, (uintptr_t)aimTypeText },
			{ CTRL_CHECKBOX,	"Autofire",			5,		63,		190,		8,		(uintptr_t)&eth32.settings.autofire },
			{ CTRL_CHECKBOX,	"Validate Attack",	5,		76,		190,		8,		(uintptr_t)&eth32.settings.atkValidate },
			{ CTRL_CHECKBOX,	"Target Lock",		5,		89,		190,		8,		(uintptr_t)&eth32.settings.lockTarget },
			{ CTRL_FLOATSLIDER, "FOV",				5,		102,	190,		20,		SETFLOAT(0), SETFLOAT(360), (uintptr_t)&eth32.settings.fov },
			{ CTRL_DROPBOX,		"Target Sort",		5,		127,	190,		23,		SORT_OFF, SORT_MAX-1, (uintptr_t)&eth32.settings.aimSort, (uintptr_t)sortTypeText },
			{ CTRL_DROPBOX,		"Aim Priority",		5,		159,	190,		23,		0, AP_MAX-1, (uintptr_t)&eth32.settings.headbody, (uintptr_t)priorityTypeText },
			{ CTRL_DROPBOX,		"Head Trace Style",	5,		185,	190,		23,		HEAD_CENTER, HEAD_MAX-1, (uintptr_t)&eth32.settings.headTraceType, (uintptr_t)headTraceTypeText },
			{ CTRL_DROPBOX,		"Body Trace Style",	5,		210,	190,		23,		BODY_CENTER, BODY_MAX-1, (uintptr_t)&eth32.settings.bodyTraceType, (uintptr_t)bodyTraceTypeText },
			{ CTRL_FLOATSLIDER,	"Dynamic Hitbox",   5,		235, 	190,		20,		SETFLOAT(0), SETFLOAT(3), (uintptr_t)&eth32.settings.dynamicHitboxScale },
			{ CTRL_FLOATSLIDER, "Anim. Correction", 5,		260,	190,		20,		SETFLOAT(-10), SETFLOAT(10), (uintptr_t)&eth32.settings.animCorrection },
			{ CTRL_CHECKBOX,	"Auto Crouch",		5,		285,	190,		8,		(uintptr_t)&eth32.settings.autoCrouch },
		}
	},
	{
		"Grenade Bot",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		320, 55, 150, 300,		// x, y, w, h
		16,						// num controls
		{
			// Type				Label					X		Y		W			H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Grenade Aimbot",		5,		5,		140,		8,		(uintptr_t)&eth32.settings.grenadeBot },
			{ CTRL_CHECKBOX,	"Riflenade Aimbot",		5,		20,		140,		8,		(uintptr_t)&eth32.settings.rifleBot },
			{ CTRL_CHECKBOX,	"Block fire",			5,		35,		140,		8,		(uintptr_t)&eth32.settings.grenadeBlockFire },
			{ CTRL_CHECKBOX,	"Grenade Trajectory",	5,		50,		140,		8,		(uintptr_t)&eth32.settings.valGrenTrajectory },
			{ CTRL_CHECKBOX,	"Rifle Trajectory",		5,		65,		140,		8,		(uintptr_t)&eth32.settings.valRifleTrajectory },
			{ CTRL_CHECKBOX,	"Grenade Senslock",		5,		78,		140,		8,		(uintptr_t)&eth32.settings.grenadeSenslock },
			{ CTRL_FLOATSLIDER,	"Rifle Z Corr.",		5,		93,		140,		20,		SETFLOAT(-50), SETFLOAT(50), (uintptr_t)&eth32.settings.riflenadeZ },
			{ CTRL_FLOATSLIDER,	"Grenade Z Corr.",		5,		118,	140,		20,		SETFLOAT(-50), SETFLOAT(50), (uintptr_t)&eth32.settings.grenadeZ },
			{ CTRL_INTSLIDER,	"Grenade Fire Delay",	5,		142,	140,		20,		0, 1000, (uintptr_t)&eth32.settings.grenadeFireDelay },
			{ CTRL_CHECKBOX,	"Grenade Autofire",		5,		165,	140,		8,		(uintptr_t)&eth32.settings.grenadeAutoFire },
			{ CTRL_CHECKBOX,	"Riflenade Autofire",	5,		178,	140,		8,		(uintptr_t)&eth32.settings.rifleAutoFire },
			{ CTRL_DROPBOX,		"Target Predict",		5,		191,	140,		23,		0, RF_PREDICT_MAX-1, (uintptr_t)&eth32.settings.ballisticPredict, (uintptr_t)riflePredictText},
			{ CTRL_CHECKBOX,	"Check Radius Damage",	5,		220,	140,		8,		(uintptr_t)&eth32.settings.ballisticRadiusDamage },
			{ CTRL_FLOATSLIDER,	"Blast radius",			5,		233,	140,		20,		SETFLOAT(30), SETFLOAT(500), (uintptr_t)&eth32.settings.radiusDamage },
			{ CTRL_CHECKBOX,	"Auto Targets",			5,		258,	140,		8,		(uintptr_t)&eth32.settings.autoGrenTargets },
			{ CTRL_CHECKBOX,	"Multi Bounce",			5,		271,	140,		8,		(uintptr_t)&eth32.settings.allowMultiBounce },
		}
	},
	{
		"Weapon Config",		// title
		WIN_WEAPCONFIG,			// type
		GUI_MENUVIEW,			// view
		157, 55, 150, 225,		// x, y, w, h
		0,				// num controls
	},
	/** *******************************************************************
					AIMBOT EXTRA
	******************************************************************* **/
	{
		"Corrections",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 250, 158,		// x, y, w, h
		5,				// num controls
		{
			{ CTRL_FLOATSLIDER,	"Head Hitbox Size",		5,	5, 		240,	20,		SETFLOAT(1), SETFLOAT(15), (uintptr_t)&eth32.settings.headBoxSize },
			{ CTRL_FLOATSLIDER,	"Body Hitbox Size",	    5,	30, 	240,	20,		SETFLOAT(1), SETFLOAT(40), (uintptr_t)&eth32.settings.bodybox },
			{ CTRL_CHECKBOX,	"Auto weapon Delay",   	5,	55,		240,	8,		(uintptr_t)&eth32.settings.autoDelay },
			{ CTRL_INTSLIDER,	"Weapon Delay Close",   5,	68, 	240,	20,		0,50, (uintptr_t)&eth32.settings.delayClose },
			{ CTRL_INTSLIDER,	"Weapon Delay Far",     5,	93, 	240,	20,		0,50, (uintptr_t)&eth32.settings.delayFar },
		}
	},
	{
		"Predictions",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 300, 250, 213,		// x, y, w, h
		9,				// num controls
		{
			// Type				Label					X	Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Preshoot",		    	5,	5,      240,	8,		(uintptr_t)&eth32.settings.preShoot },
			{ CTRL_CHECKBOX,	"Preaim",		    	5,	18,     240,	8,		(uintptr_t)&eth32.settings.preAim },
			{ CTRL_FLOATSLIDER,	"Preshoot Time",		5,	31,		240,	20,		SETFLOAT(0), SETFLOAT(300), (uintptr_t)&eth32.settings.preShootTime },
			{ CTRL_FLOATSLIDER,	"Preaim Time",			5,	56, 	240,	20,		SETFLOAT(0), SETFLOAT(300), (uintptr_t)&eth32.settings.preAimTime },
			{ CTRL_DROPBOX,		"Self Predict Type",	5,	81,		240,	23,		SPR_OFF, SPR_MAX-1, (uintptr_t)&eth32.settings.predSelfType, (uintptr_t)selfPredictText },
			{ CTRL_FLOATSLIDER,	"Self Predict",			5,	110,	240,	20,		SETFLOAT(-0.1), SETFLOAT(0.1), (uintptr_t)&eth32.settings.predSelf },
			{ CTRL_CHECKBOX,	"Auto predict BOTs",	5,	135,    240,	8,		(uintptr_t)&eth32.settings.autoPredictBots },
			{ CTRL_FLOATSLIDER,	"Target Predict",	    5,  148,	240,	20,		SETFLOAT(-0.1), SETFLOAT(0.1), (uintptr_t)&eth32.settings.pred },
			{ CTRL_FLOATSLIDER,	"BOT Predict",	        5,  173,	240,	20,		SETFLOAT(-0.1), SETFLOAT(0.1), (uintptr_t)&eth32.settings.predbot },
		}
	},
	{
		"Hitbox",		// title
		WIN_HITBOX,			// type
		GUI_MENUVIEW,			// view
		7, 55, 250, 140,		// x, y, w, h
		0,				// num controls
	},
	/** *******************************************************************
					VISUALS
	******************************************************************* **/
	{
		"Visuals",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 150, 299,		// x, y, w, h
		14,				// num controls
		{
			// Type				Label						X		Y		W			H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Original HUD",				5,		5,		140,		8,		(uintptr_t)&eth32.settings.guiOriginal },
			{ CTRL_CHECKBOX,	"Draw Hackvisuals",			5,		18,		140,		8,		(uintptr_t)&eth32.settings.drawHackVisuals },
			{ CTRL_CHECKBOX,	"Wallhack",					5,		31,		140,		8,		(uintptr_t)&eth32.settings.wallhack },
			{ CTRL_CHECKBOX,	"Draw Scope Blackout",		5,		44,		140,		8,		(uintptr_t)&eth32.settings.blackout },
			{ CTRL_CHECKBOX,	"Weapon Zoom",				5,		57,		140,		8,		(uintptr_t)&eth32.settings.weaponZoom },
			{ CTRL_FLOATSLIDER,	"Scoped Turning",			5,		70,		140,		20,		SETFLOAT(0.1), SETFLOAT(1.0), (uintptr_t)&eth32.settings.scopedTurnSpeed },
			{ CTRL_INTSLIDER,	"Smoke Visibility",			5,		95,		140,		20,		0, 100, (uintptr_t)&eth32.settings.smoketrnsp },
			{ CTRL_FLOATSLIDER,	"Radar Range",				5,		120,	140,		20,		SETFLOAT(100), SETFLOAT(10000), (uintptr_t)&eth32.settings.radarRange },
			{ CTRL_CHECKBOX,	"Banner",					5,		145,	140,		8,		(uintptr_t)&eth32.settings.guiBanner },
			{ CTRL_FLOATSLIDER,	"Banner size",				5,		158,	140,		20,		SETFLOAT(0), SETFLOAT(3), (uintptr_t)&eth32.settings.BannerScale },
			{ CTRL_CHECKBOX,	"Remove Foliage",			5,		183,	140,		8,		(uintptr_t)&eth32.settings.removeFoliage },
			{ CTRL_CHECKBOX,	"Draw Warning Messages",	5,		196,	140,		8,		(uintptr_t)&eth32.settings.warningMsg },
			{ CTRL_DROPBOX,		"Damage Indicator Type",	5,		209,	140,		23,		DMG_OFF, DMG_MAX-1, (uintptr_t)&eth32.settings.dmgIndicator, (uintptr_t)damageIndicatorText },
			{ CTRL_FLOATSLIDER,	"Dmg Arrow Size",			5,		234,	140,		20,		SETFLOAT(16), SETFLOAT(128), (uintptr_t)&eth32.settings.dmgArrSize },
		},
	},

	{
		"Hitbox Display",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 55, 150, 148,		// x, y, w, h
		8,				// num controls
		{
			// Type				Label				X		Y		W			H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Head",				5,		5,		140,		8,		(uintptr_t)&eth32.settings.drawHeadHitbox },
			{ CTRL_CHECKBOX,	"Head Axes",		5,		18,		140,		8,		(uintptr_t)&eth32.settings.drawHeadAxes },
			{ CTRL_CHECKBOX,	"Body",				5,		31,		140,		8,		(uintptr_t)&eth32.settings.drawBodyHitbox },
			{ CTRL_CHECKBOX,	"Show Aimpoints",	5,		44,		140,		8,		(uintptr_t)&eth32.settings.debugPoints},
			{ CTRL_CHECKBOX,	"Bullet Rail",		5,		57,		140,		8,		(uintptr_t)&eth32.settings.drawBulletRail },
			{ CTRL_CHECKBOX,	"Rail Wallhack",	5,		70,		140,		8,		(uintptr_t)&eth32.settings.railWallhack },
			{ CTRL_INTSLIDER,	"Head Trail Time",	5,		83,		140,		20,		0, 300, (uintptr_t)&eth32.settings.headRailTime },
			{ CTRL_INTSLIDER,	"Body Trail Time",	5,		108,	140,		20,		0, 300, (uintptr_t)&eth32.settings.bodyRailTime },
		},
	},

	{
		"ESP",				// title
		WIN_ESPCONFIG,			// type
		GUI_MENUVIEW,			// view
		310, 55, 150, 424,		// x, y, w, h
		14,				// num controls
		{
			{ CTRL_CHECKBOX,	"Missile Blast Light",	5,		130,		140,		8,		(uintptr_t)&eth32.settings.grenadeDlight },
			{ CTRL_CHECKBOX,	"Mortar Blast Light",	5,		143,		140,		8,		(uintptr_t)&eth32.settings.mortarDlight },
			{ CTRL_CHECKBOX,	"Disguised ESP",		5,		156,		140,		8,		(uintptr_t)&eth32.settings.drawDisguised },
			{ CTRL_CHECKBOX,	"Mortar Trace and Esp",	5,		169,		140,		8,		(uintptr_t)&eth32.settings.mortarTrace },
			{ CTRL_CHECKBOX,	"Arty Markers",			5,		182,		140,		8,		(uintptr_t)&eth32.settings.artyMarkers },
			{ CTRL_DROPBOX,  	"Class ESP",			5,      208,      	140,        23,     0, CLS_MAX-1, (uintptr_t)&eth32.settings.classEspType, (uintptr_t)classEspText },
			{ CTRL_FLOATSLIDER,	"Class ESP Size",		5,		233,		140,		20,		SETFLOAT(0), SETFLOAT(30), (uintptr_t)&eth32.settings.clsSize },
			{ CTRL_FLOATSLIDER,	"Class ESP Opacity",	5,		258,		140,		20,		SETFLOAT(0), SETFLOAT(1), (uintptr_t)&eth32.settings.clsOpacity },
			{ CTRL_CHECKBOX,	"Items ESP",			5,		283,		140,		8,		(uintptr_t)&eth32.settings.itemEsp },
			{ CTRL_FLOATSLIDER,	"Items ESP Size",		5,		296,		140,		20,		SETFLOAT(0), SETFLOAT(30), (uintptr_t)&eth32.settings.itemEspSize },
			{ CTRL_FLOATSLIDER,	"Items ESP Opacity",	5,		321,		140,		20,		SETFLOAT(0), SETFLOAT(1), (uintptr_t)&eth32.settings.itemEspOpacity },
			{ CTRL_CHECKBOX,	"Box ESP",				5,		346,		140,		8,		(uintptr_t)&eth32.settings.boxEsp },
			{ CTRL_INTSLIDER,	"Box ESP Border Size",	5,		359,		140,		20,		0, 5, (uintptr_t)&eth32.settings.boxEspBorder },
			{ CTRL_FLOATSLIDER,	"Box ESP Opacity",		5,		384,		140,		20,		SETFLOAT(0), SETFLOAT(1), (uintptr_t)&eth32.settings.boxEspOpacity },
		},
	},
	/** *******************************************************************
					VISUAL EXTRA
	******************************************************************* **/
	{
		"Chams",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 300, 200,		// x, y, w, h
		6,				// num controls
		{
			// Type				Label				X		Y		W			H		Arg0 ... Arg4
			{ CTRL_DROPBOX,		"Player Shader",	5,		5,		140,		23,		0, SHADER1_MAX-1, (uintptr_t)&eth32.settings.playerShader, (uintptr_t)Shader1Text },
			{ CTRL_CHECKBOX,	"Wallhack",			5,		30,		140,		8,		(uintptr_t)&eth32.settings.playerShaderWh },
			{ CTRL_DROPBOX,		"Weapon Shader",	5,		43,		140,		23,		0, SHADER1_MAX-1, (uintptr_t)&eth32.settings.weaponShader, (uintptr_t)Shader1Text },
			{ CTRL_CHECKBOX,	"Wallhack",			5,		68,		140,		8,		(uintptr_t)&eth32.settings.weaponShaderWh },
			{ CTRL_DROPBOX,		"Item Shader",		155,	5,		140,		23,		0, SHADER1_MAX-1, (uintptr_t)&eth32.settings.itemShader, (uintptr_t)Shader1Text },
			{ CTRL_CHECKBOX,	"Wallhack",			155,	30,		140,		8,		(uintptr_t)&eth32.settings.itemShaderWh },
		},
	},
	/** *******************************************************************
					COLOR PICKER
	******************************************************************* **/
	{
		"Colors",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		5, 20, 630, 176,		// x, y, w, h
		1,				// num controls
		{
			// Type		    Label			X		Y		W			H		Arg0 ... Arg4
			{ CTRL_COLORPICKER, "Picker",	5,		5,		620,		156 },
		},
	},
	
	
	/** *******************************************************************
					MISC
	******************************************************************* **/
	{
		"Misc",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 150, 217,		// x, y, w, h
		11,				// num controls
		{
			{ CTRL_CHECKBOX,	"Transparent Console",	5,		18,		140,	8,		(uintptr_t)&eth32.settings.transparantConsole },
			{ CTRL_CHECKBOX,	"Respawn Timers",		5,		31,		140,	8,		(uintptr_t)&eth32.settings.respawnTimers },
			{ CTRL_CHECKBOX,	"Auto Tapout",			5,		44,		140,	8,		(uintptr_t)&eth32.settings.autoTapout },
			{ CTRL_DROPBOX,		"PB Screenshot",		5,		57,		140,	23,		0, PB_SS_MAX-1, (uintptr_t)&eth32.settings.pbScreenShot, (uintptr_t)pbssText },
			{ CTRL_CHECKBOX,	"Original Viewvalues",	5,		86,		140,	8,		(uintptr_t)&eth32.settings.origViewValues },
			{ CTRL_CHECKBOX,	"Interpolated PS",		5,		99,		140,	8,		(uintptr_t)&eth32.settings.interpolatedPs },
			{ CTRL_CHECKBOX,	"Damage Feedback",		5,		112,	140,	8,		(uintptr_t)&eth32.settings.dmgFeedback },
			{ CTRL_CHECKBOX,	"Auto Vote",			5,		125,	140,	8,		(uintptr_t)&eth32.settings.autoVote },
			{ CTRL_CHECKBOX,	"Auto Complaint",		5,		138,	140,	8,		(uintptr_t)&eth32.settings.autoComplaint },
			{ CTRL_CHECKBOX,	"Anti Teamkill",		5,		151,	140,	8,		(uintptr_t)&eth32.settings.antiTk },
			{ CTRL_CHECKBOX,	"Spoof OS",				5,		164,	140,	8,		(uintptr_t)&eth32.settings.etproOs },
		},
	},
	{
		"Name Stealer",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		320, 370, 150, 133,		// x, y, w, h
		5,				// num controls
		{
			// Type			Label						X		Y		W			H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Name Steal",			5,		5,		140,		8,		(uintptr_t)&eth32.settings.doNamesteal },
			{ CTRL_INTSLIDER,	"Delay",				5,		18,		140,		20,		0, 20000, (uintptr_t)&eth32.settings.NamestealDelay },
			{ CTRL_INTSLIDER,	"Init Grace",			5,		41,		140,		20,		0, 20000, (uintptr_t)&eth32.settings.NamestealGrace },
			{ CTRL_DROPBOX,		"Steal type",			5,		64,		140,		23,		0, NAMESTEAL_MAX-1, (uintptr_t)&eth32.settings.NamestealMode, (uintptr_t)namestealText },
			{ CTRL_CHECKBOX,	"PB Exact Namesteal",	5,		93,		140,		8,		(uintptr_t)&eth32.settings.nsSmartMode },
		}
	},
};

static const assetdef_t assetDefs[] =
{
//	Key					Type					Target
	{ "titlecolor",		ASSET_VEC4,				(void*)eth32.guiAssets.titleColor },
	{ "textcolor1",		ASSET_VEC4,				(void*)eth32.guiAssets.textColor1 },
	{ "textcolor2",		ASSET_VEC4,				(void*)eth32.guiAssets.textColor2 },
	{ "titleleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleLeft },
	{ "titlecenter",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleCenter },
	{ "titleright",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleRight },
	{ "winleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winLeft },
	{ "wintop",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTop },
	{ "wintopl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTopLeft },
	{ "wincenter",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winCenter },
	{ "txtinputl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputLeft },
	{ "txtinputc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputCenter },
	{ "txtinputr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputRight },
	{ "btnl",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnLeft },
	{ "btnc",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnCenter },
	{ "btnr",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnRight },
	{ "btnsell",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselLeft },
	{ "btnselc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselCenter },
	{ "btnselr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselRight },
	{ "check",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.check },
	{ "checkbox",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.checkBox },
	{ "mouse",			ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.mousePtr },
	{ "dropboxarrow",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.dropboxArrow },
	{ "sliderbtn",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderBtn },
	{ "slidertrack",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderTrack },
	{ "camcorner",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.camCorner },
};
