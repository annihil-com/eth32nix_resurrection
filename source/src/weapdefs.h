// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

// Weapon definitions to control aimbot/autofire/etc.
//
//	Going to try to share settings for mods that can, TCE is most definitely going to be a seperate list
//  NoQuarter adds a bunch of weapons, but may be able to add on to original list.. will try to capture weapon names/nums
//  as I can... right now, only etpub/etmain/shrub/etpro get assigned a list until I or someone gets the others defined
//  Also, eth32.cg.currentWeapon is assigned upon respawn and during weapon change.

// etmain, etpub, shrub, etpro, jaymod
static weapdef_t normalWeapons[] =
{
	// Name					Type			Attributes
	{ "None",				WT_NONE,		WA_NONE											},	// 0
	{ "Knife",				WT_KNIFE,		WA_USER_DEFINED	| WA_NO_AMMO					},	// 1
	{ "Luger",				WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 2
	{ "MP-40",				WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 3
	{ "Grenade",			WT_EXPLOSIVE,	WA_ONLY_CLIP | WA_BALLISTIC	| WA_GRENADE		},	// 4 axis
	{ "Panzerfaust",		WT_HEAVY,		WA_PANZER | WA_ONLY_CLIP						},	// 5
	{ "Flamethrower",		WT_HEAVY,		WA_ONLY_CLIP									},	// 6
	{ "Colt",				WT_PISTOL,		WA_USER_DEFINED									},	// 7
	{ "Thompson",			WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 8
	{ "Grenade",			WT_EXPLOSIVE,	WA_ONLY_CLIP | WA_BALLISTIC	| WA_GRENADE		},	// 9 allies
	{ "Sten",				WT_STEN,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 10
	{ "Syringe",			WT_NONE,		WA_ONLY_CLIP									},	// 11
	{ "Ammo",				WT_NONE,		WA_NO_AMMO										},	// 12
	{ "Arty",				WT_NONE,		WA_NONE											},	// 13
	{ "Silenced Luger",		WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 14
	{ "Dynamite",			WT_EXPLOSIVE,	WA_NO_AMMO										},	// 15
	{ "Smoketrail",			WT_HEAVY,		WA_NONE											},	// 16
	{ "MapMortar",			WT_HEAVY,		WA_NONE											},	// 17
	{ "BigExplosion",		WT_HEAVY,		WA_NONE											},	// 18
	{ "Medkit",				WT_NONE,		WA_NO_AMMO										},	// 19
	{ "Binoculars",			WT_NONE,		WA_NO_AMMO										},	// 20
	{ "Pliers",				WT_NONE,		WA_NO_AMMO										},	// 21
	{ "Air Strike",			WT_HEAVY,		WA_NO_AMMO										},	// 22
	{ "KAR-98",				WT_RIFLE,		WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 23
	{ "Carbine",			WT_RIFLE,		WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 24
	{ "Garand",				WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_UNSCOPED	},	// 25
	{ "Landmine",			WT_EXPLOSIVE,	WA_ONLY_CLIP									},	// 26
	{ "Satchel",			WT_EXPLOSIVE,	WA_NO_AMMO										},	// 27
	{ "Detonator",			WT_NONE,		WA_NO_AMMO | WA_SATCHEL							},	// 28
	{ "Tripmine",			WT_EXPLOSIVE,	WA_NONE											},	// 29
	{ "Smoke Grenade",		WT_EXPLOSIVE,	WA_NO_AMMO										},	// 30
	{ "MG-42",				WT_MG,			WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 31
	{ "K-43",				WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_UNSCOPED	},	// 32
	{ "FG-42",				WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_UNSCOPED	},	// 33
	{ "DummyMG42",			WT_NONE,		WA_NONE											},	// 34
	{ "Mortar",				WT_HEAVY,		WA_NONE											},	// 35 not deployed
	{ "Lock Pick",			WT_NONE,		WA_NONE											},	// 36
	{ "Akimbo Colt",		WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 7	},	// 37
	{ "Akimbo Luger",		WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 2	},	// 38
	{ "Rifle Grenade",		WT_EXPLOSIVE,	WA_RIFLE_GRENADE | WA_BALLISTIC					},	// 39 axis
	{ "Rifle Grenade",		WT_EXPLOSIVE,	WA_RIFLE_GRENADE | WA_BALLISTIC					},	// 40 allies
	{ "Silenced Colt",		WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 41
	{ "Garand (Scoped)",	WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_SCOPED, 25	},	// 42 scoped
	{ "K-43 (Scoped)",		WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_SCOPED, 32	},	// 43 scoped
	{ "FG-42 (Scoped)",		WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_SCOPED, 33	},	// 44 scoped
	{ "Mortar",				WT_HEAVY,		WA_MORTAR										},	// 45 deployed set
	{ "Adrenaline",			WT_NONE,		WA_ONLY_CLIP									},	// 46
	{ "Silenced Colts",		WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 7	},	// 47
	{ "Silenced Lugers",	WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 2	},	// 48
	{ "Mobile MG-42",		WT_MG,			WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 49
	// JAYMOD Additions
	{ "Poison Needle",		WT_NONE,		WA_NONE											},	// 50
	{ "Unknown 51",			WT_NONE,		WA_NONE											},	// 51
	{ "M97",				WT_SHOTGUN,		WA_USER_DEFINED | WA_BLOCK_FIRE					},	// 52
};

static weapdef_t noquarterWeapons[] =
{
	// Name			Type			Attributes
	{ "None",		WT_NONE,		WA_NONE						},	// 0
	{ "Knife",		WT_KNIFE,		WA_USER_DEFINED	| WA_NO_AMMO			},	// 1
	{ "Luger",		WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 2
	{ "MP-40",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 3
	{ "Grenade",		WT_EXPLOSIVE,		WA_ONLY_CLIP					},	// 4 axis
	{ "Panzerfaust",	WT_HEAVY,		WA_PANZER					},	// 5
	{ "Flamethrower",	WT_HEAVY,		WA_USER_DEFINED					},	// 6
	{ "Colt",		WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 7
	{ "Thompson",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 8
	{ "Grenade",		WT_EXPLOSIVE,		WA_ONLY_CLIP					},	// 9 allies
	{ "Sten",		WT_STEN,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 10
	{ "Syringe",		WT_NONE,		WA_ONLY_CLIP					},	// 11
	{ "Ammo",		WT_NONE,		WA_NO_AMMO					},	// 12
	{ "Arty",		WT_HEAVY,		WA_NONE						},	// 13
	{ "Silenced Luger",	WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 14
	{ "Dynamite",		WT_EXPLOSIVE,		WA_NONE						},	// 15
	{ "Smoketrail",		WT_HEAVY,		WA_NONE						},	// 16
	{ "Unknown 17",		WT_NONE,		WA_NONE						},	// 17
	{ "Medkit",		WT_NONE,		WA_NO_AMMO					},	// 18
	{ "Binoculars",		WT_NONE,		WA_NO_AMMO					},	// 19
	{ "Pliers",		WT_NONE,		WA_NO_AMMO					},	// 20
	{ "Air Strike",		WT_HEAVY,		WA_NO_AMMO					},	// 21
	{ "KAR98",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 22
	{ "Carbine",		WT_RIFLE,		WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 23
	{ "Garand",		WT_RIFLE,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_UNSCOPED	},	// 24
	{ "Landmine",		WT_EXPLOSIVE,		WA_ONLY_CLIP					},	// 25
	{ "Satchel",		WT_EXPLOSIVE,		WA_NO_AMMO					},	// 26
	{ "Detonator",		WT_NONE,		WA_NO_AMMO | WA_SATCHEL				},	// 27
	{ "Tripmine",		WT_EXPLOSIVE,		WA_NONE						},	// 28
	{ "Smoke Grenade",	WT_EXPLOSIVE,		WA_NO_AMMO					},	// 29
	{ "MG-42",		WT_MG,			WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 30
	{ "K-43",		WT_RIFLE,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_UNSCOPED	},	// 31
	{ "FG-42",		WT_RIFLE,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_UNSCOPED	},	// 32
	{ "DummyMG42",		WT_NONE,		WA_NONE						},	// 33
	{ "Mortar",		WT_HEAVY,		WA_NONE						},	// 34 not deployed
	{ "Akimbo Colt",	WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 7	},	// 35
	{ "Akimbo Luger",	WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 2	},	// 36
	{ "Rifle Grenade",	WT_EXPLOSIVE,		WA_RIFLE_GRENADE				},	// 37 axis
	{ "Rifle Grenade",	WT_EXPLOSIVE,		WA_RIFLE_GRENADE				},	// 38 allies
	{ "Silenced Colt",	WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 39
	{ "Garand (Scoped)",	WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_SCOPED	},	// 40 scoped
	{ "K-43 (Scoped)",	WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_SCOPED	},	// 41 scoped
	{ "FG-42",		WT_SNIPER,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_SCOPED	},	// 42 scoped
	{ "Mortar",		WT_HEAVY,		WA_MORTAR					},	// 43 deployed set
	{ "Adrenaline",		WT_NONE,		WA_NONE						},	// 44
	{ "Silenced Colts",	WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 7	},	// 45
	{ "Silenced Lugers",	WT_PISTOL,		WA_USER_DEFINED | WA_BLOCK_FIRE | WA_AKIMBO, 2	},	// 46
	{ "Mobile MG-42",	WT_MG,			WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 47
	{ "Shotgun",		WT_SHOTGUN,		WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 48
	{ "Knife",		WT_KNIFE,		WA_USER_DEFINED	| WA_NO_AMMO			},	// 49
	{ "Browning",		WT_MG,			WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 50
	{ "Browning Set",	WT_MG,			WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 51
	{ "BAR",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 52
	{ "BAR Set",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 53
	{ "STG 44",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 54
	{ "Sten MKII",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE			},	// 55
	{ "Bazooka",		WT_HEAVY,		WA_ONLY_CLIP					},	// 56
	{ "MP-34",		WT_SMG,			WA_USER_DEFINED | WA_BLOCK_FIRE	| WA_OVERHEAT	},	// 57
	{ "Firebolt",		WT_NONE,		WA_NONE						},	// 58
	{ "Mortar 2",		WT_HEAVY,		WA_NONE						},	// 59
	{ "Mortar 2 Set",	WT_HEAVY,		WA_NONE						},	// 60
	{ "Venom",		WT_MG,			WA_USER_DEFINED | WA_BLOCK_FIRE | WA_OVERHEAT	},	// 61
	{ "Poison Syringe",	WT_NONE,		WA_NONE						},	// 62
};

// kobject: added just in case so currentWeapon is always valid
extern weapdef_t nullWeapon;
