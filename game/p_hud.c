/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "g_local.h"



/*
======================================================================

INTERMISSION

======================================================================
*/

void MoveClientToIntermission (edict_t *ent)
{
	if (deathmatch->value || coop->value)
		ent->client->showscores = true;
	VectorCopy (level.intermission_origin, ent->s.origin);
	ent->client->ps.pmove.origin[0] = level.intermission_origin[0]*8;
	ent->client->ps.pmove.origin[1] = level.intermission_origin[1]*8;
	ent->client->ps.pmove.origin[2] = level.intermission_origin[2]*8;
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);
	ent->client->ps.pmove.pm_type = PM_FREEZE;
	ent->client->ps.gunindex = 0;
	ent->client->ps.blend[3] = 0;
	ent->client->ps.rdflags &= ~RDF_UNDERWATER;

	// clean up powerup info
	ent->client->quad_framenum = 0;
	ent->client->invincible_framenum = 0;
	ent->client->breather_framenum = 0;
	ent->client->enviro_framenum = 0;
	ent->client->grenade_blew_up = false;
	ent->client->grenade_time = 0;

	ent->viewheight = 0;
	ent->s.modelindex = 0;
	ent->s.modelindex2 = 0;
	ent->s.modelindex3 = 0;
	ent->s.modelindex = 0;
	ent->s.effects = 0;
	ent->s.sound = 0;
	ent->solid = SOLID_NOT;

	// add the layout

	if (deathmatch->value || coop->value)
	{
		DeathmatchScoreboardMessage (ent, NULL);
		gi.unicast (ent, true);
	}

}

void BeginIntermission (edict_t *targ)
{
	int		i, n;
	edict_t	*ent, *client;

	if (level.intermissiontime)
		return;		// already activated

	game.autosaved = false;

	// respawn any dead clients
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		if (client->health <= 0)
			respawn(client);
	}

	level.intermissiontime = level.time;
	level.changemap = targ->map;

	if (strstr(level.changemap, "*"))
	{
		if (coop->value)
		{
			for (i=0 ; i<maxclients->value ; i++)
			{
				client = g_edicts + 1 + i;
				if (!client->inuse)
					continue;
				// strip players of all keys between units
				for (n = 0; n < MAX_ITEMS; n++)
				{
					if (itemlist[n].flags & IT_KEY)
						client->client->pers.inventory[n] = 0;
				}
			}
		}
	}
	else
	{
		if (!deathmatch->value)
		{
			level.exitintermission = 1;		// go immediately to the next level
			return;
		}
	}

	level.exitintermission = 0;

	// find an intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if (!ent)
	{	// the map creator forgot to put in an intermission point...
		ent = G_Find (NULL, FOFS(classname), "info_player_start");
		if (!ent)
			ent = G_Find (NULL, FOFS(classname), "info_player_deathmatch");
	}
	else
	{	// chose one of four spots
		i = rand() & 3;
		while (i--)
		{
			ent = G_Find (ent, FOFS(classname), "info_player_intermission");
			if (!ent)	// wrap around the list
				ent = G_Find (ent, FOFS(classname), "info_player_intermission");
		}
	}

	VectorCopy (ent->s.origin, level.intermission_origin);
	VectorCopy (ent->s.angles, level.intermission_angle);

	// move all clients to the intermission point
	for (i=0 ; i<maxclients->value ; i++)
	{
		client = g_edicts + 1 + i;
		if (!client->inuse)
			continue;
		MoveClientToIntermission (client);
	}
}


/*
==================
DeathmatchScoreboardMessage

==================
*/
void DeathmatchScoreboardMessage (edict_t *ent, edict_t *killer)
{
	char	entry[1024];
	char	string[1400];
	int		stringlength;
	int		i, j, k;
	int		sorted[MAX_CLIENTS];
	int		sortedscores[MAX_CLIENTS];
	int		score, total;
	int		picnum;
	int		x, y;
	gclient_t	*cl;
	edict_t		*cl_ent;
	char	*tag;

	// sort the clients by score
	total = 0;
	for (i=0 ; i<game.maxclients ; i++)
	{
		cl_ent = g_edicts + 1 + i;
		if (!cl_ent->inuse || game.clients[i].resp.spectator)
			continue;
		score = game.clients[i].resp.score;
		for (j=0 ; j<total ; j++)
		{
			if (score > sortedscores[j])
				break;
		}
		for (k=total ; k>j ; k--)
		{
			sorted[k] = sorted[k-1];
			sortedscores[k] = sortedscores[k-1];
		}
		sorted[j] = i;
		sortedscores[j] = score;
		total++;
	}

	// print level name and exit rules
	string[0] = 0;

	stringlength = strlen(string);

	// add the clients in sorted order
	if (total > 12)
		total = 12;

	for (i=0 ; i<total ; i++)
	{
		cl = &game.clients[sorted[i]];
		cl_ent = g_edicts + 1 + sorted[i];

		picnum = gi.imageindex ("i_fixme");
		x = (i>=6) ? 160 : 0;
		y = 32 + 32 * (i%6);

		// add a dogtag
		if (cl_ent == ent)
			tag = "tag1";
		else if (cl_ent == killer)
			tag = "tag2";
		else
			tag = NULL;
		if (tag)
		{
			Com_sprintf (entry, sizeof(entry),
				"xv %i yv %i picn %s ",x+32, y, tag);
			j = strlen(entry);
			if (stringlength + j > 1024)
				break;
			strcpy (string + stringlength, entry);
			stringlength += j;
		}

		// send the layout
		Com_sprintf (entry, sizeof(entry),
			"client %i %i %i %i %i %i ",
			x, y, sorted[i], cl->resp.score, cl->ping, (level.framenum - cl->resp.enterframe)/600);
		j = strlen(entry);
		if (stringlength + j > 1024)
			break;
		strcpy (string + stringlength, entry);
		stringlength += j;
	}

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
}


/*
==================
DeathmatchScoreboard

Draw instead of help message.
Note that it isn't that hard to overflow the 1400 byte message limit!
==================
*/
void DeathmatchScoreboard (edict_t *ent)
{
	DeathmatchScoreboardMessage (ent, ent->enemy);
	gi.unicast (ent, true);
}


/*
==================
Cmd_Score_f

Display the scoreboard
==================
*/
void Cmd_Score_f (edict_t *ent)
{
	ent->client->showinventory = false;
	ent->client->showhelp = false;

	if (!deathmatch->value && !coop->value)
		return;

	if (ent->client->showscores)
	{
		ent->client->showscores = false;
		return;
	}

	ent->client->showscores = true;
	DeathmatchScoreboard (ent);
}


/*
==================
HelpComputer

Draw help computer.
==================
*/
void HelpComputer (edict_t *ent)
{
	char	string[1024];
	char	*sk;

	if (skill->value == 0)
		sk = "easy";
	else if (skill->value == 1)
		sk = "medium";
	else if (skill->value == 2)
		sk = "hard";
	else
		sk = "hard+";

	// send the layout
	Com_sprintf (string, sizeof(string),
		"xv 32 yv 8 picn help "			// background
		"xv 202 yv 12 string2 \"%s\" "		// skill
		"xv 0 yv 24 cstring2 \"%s\" "		// level name
		"xv 0 yv 54 cstring2 \"%s\" "		// help 1
		"xv 0 yv 110 cstring2 \"%s\" "		// help 2
		"xv 50 yv 164 string2 \" kills     goals    secrets\" "
		"xv 50 yv 172 string2 \"%3i/%3i     %i/%i       %i/%i\" ", 
		sk,
		level.level_name,
		game.helpmessage1,
		game.helpmessage2,
		level.killed_monsters, level.total_monsters, 
		level.found_goals, level.total_goals,
		level.found_secrets, level.total_secrets);

	gi.WriteByte (svc_layout);
	gi.WriteString (string);
	gi.unicast (ent, true);
}


/*
==================
Cmd_Help_f

Display the current help message
==================
*/
void Cmd_Help_f (edict_t *ent)
{
	// this is for backwards compatability
	if (deathmatch->value)
	{
		Cmd_Score_f (ent);
		return;
	}

	ent->client->showinventory = false;
	ent->client->showscores = false;

	if (ent->client->showhelp && (ent->client->pers.game_helpchanged == game.helpchanged))
	{
		ent->client->showhelp = false;
		return;
	}

	ent->client->showhelp = true;
	ent->client->pers.helpchanged = 0;
	HelpComputer (ent);
}


//=======================================================================

/*
===============
G_SetStats
===============
*/
void G_SetStats (edict_t *ent)
{
	gitem_t		*item;
	int			index, cells;
	int			power_armor_type;

	

	//
	// health
	//
	ent->client->ps.stats[STAT_HEALTH_ICON] = level.pic_health;
	ent->client->ps.stats[STAT_HEALTH] = ent->health;

	//
	// ammo
	//
	if (!ent->client->ammo_index /* || !ent->client->pers.inventory[ent->client->ammo_index] */)
	{
		ent->client->ps.stats[STAT_AMMO_ICON] = 0;
		ent->client->ps.stats[STAT_AMMO] = 0;
	}
	else
	{
		item = &itemlist[ent->client->ammo_index];
		ent->client->ps.stats[STAT_AMMO_ICON] = gi.imageindex (item->icon);
		ent->client->ps.stats[STAT_AMMO] = ent->client->pers.inventory[ent->client->ammo_index];
	}
	
	//
	// armor
	//
	power_armor_type = PowerArmorType (ent);
	if (power_armor_type)
	{
		cells = ent->client->pers.inventory[ITEM_INDEX(FindItem ("cells"))];
		if (cells == 0)
		{	// ran out of cells for power armor
			ent->flags &= ~FL_POWER_ARMOR;
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/power2.wav"), 1, ATTN_NORM, 0);
			power_armor_type = 0;;
		}
	}

	index = ArmorIndex (ent);
	if (power_armor_type && (!index || (level.framenum & 8) ) )
	{	// flash between power armor and other armor icon
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex ("i_powershield");
		ent->client->ps.stats[STAT_ARMOR] = cells;
	}
	else if (index)
	{
		item = GetItemByIndex (index);
		ent->client->ps.stats[STAT_ARMOR_ICON] = gi.imageindex (item->icon);
		ent->client->ps.stats[STAT_ARMOR] = ent->client->pers.inventory[index];
	}
	else
	{
		ent->client->ps.stats[STAT_ARMOR_ICON] = 0;
		ent->client->ps.stats[STAT_ARMOR] = 0;
	}

	//
	// pickup message
	//
	if (level.time > ent->client->pickup_msg_time)
	{
		ent->client->ps.stats[STAT_PICKUP_ICON] = 0;
		ent->client->ps.stats[STAT_PICKUP_STRING] = 0;
	}

	//
	// timers
	//
	if (ent->client->quad_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_quad");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->quad_framenum - level.framenum)/10;
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_invulnerability");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->invincible_framenum - level.framenum)/10;
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_envirosuit");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->enviro_framenum - level.framenum)/10;
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = gi.imageindex ("p_rebreather");
		ent->client->ps.stats[STAT_TIMER] = (ent->client->breather_framenum - level.framenum)/10;
	}
	else
	{
		ent->client->ps.stats[STAT_TIMER_ICON] = 0;
		ent->client->ps.stats[STAT_TIMER] = 0;
	}

	//
	// selected item
	//
	if (ent->client->pers.selected_item == -1)
		ent->client->ps.stats[STAT_SELECTED_ICON] = 0;
	else
		ent->client->ps.stats[STAT_SELECTED_ICON] = gi.imageindex (itemlist[ent->client->pers.selected_item].icon);

	ent->client->ps.stats[STAT_SELECTED_ITEM] = ent->client->pers.selected_item;

	//
	// layouts
	//
	ent->client->ps.stats[STAT_LAYOUTS] = 0;

	if (deathmatch->value)
	{
		if (ent->client->pers.health <= 0 || level.intermissiontime
			|| ent->client->showscores)
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}
	else
	{
		if (ent->client->showscores || ent->client->showhelp)
			ent->client->ps.stats[STAT_LAYOUTS] |= 1;
		if (ent->client->showinventory && ent->client->pers.health > 0)
			ent->client->ps.stats[STAT_LAYOUTS] |= 2;
	}

	//
	// frags
	//
	ent->client->ps.stats[STAT_FRAGS] = ent->client->resp.score;

	//
	// help icon / current weapon if not shown
	//
	if (ent->client->pers.helpchanged && (level.framenum&8) )
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex ("i_help");
	else if ( (ent->client->pers.hand == CENTER_HANDED || ent->client->ps.fov > 91)
		&& ent->client->pers.weapon)
		ent->client->ps.stats[STAT_HELPICON] = gi.imageindex (ent->client->pers.weapon->icon);
	else
		ent->client->ps.stats[STAT_HELPICON] = 0;

	ent->client->ps.stats[STAT_SPECTATOR] = 0;
}

//MOD
char* single_Attributes_Weapon =

"if 20 "
"	xv -150 yv 50 pic 20"
//weapons
"	xv -130 yv 98 string  \"1 sword\"" //melee
"	xv -130 yv 108 string \"2 spear\""
"	xv -130 yv 118 string \"3 hammer\""
"	xv -130 yv 128 string \"4 sp-hm\""
"	xv -130 yv 138 string \"5 daggers\"" //range
"	xv -130 yv 148 string \"6 bst\""
"	xv -130 yv 158 string \"7 sp-bst\""
"	xv -130 yv 168 string \"8 canon\""
"	xv -130 yv 178 string \"9 fireball\""	//magic
"	xv -130 yv 188 string \"0 magehd\"" 
"	xv -130 yv 198 string \"g shield\""
"	xv -130 yv 208 string \"e reflect\"" //use environment
"	xv -130 yv 218 string \"(melee, range, magic)\""

"endif"
//weapons visual


"if 22"
//"	xv -340 yv -150 string  \"1\""
//"	xv -280 yv -160 num 2  22"
//
//"	xv -340 yv -120 string  \"2\""
//"	xv -280 yv -130 num 2  23"
//
//"	xv -340 yv -90  string  \"3\""
//"	xv -280 yv -100  num 2 24"
//
//"	xv -340 yv -60  string  \"4\""
//"	xv -280 yv -70  num 2  25"
//
//"	xv -340 yv -30  string  \"5\""
//"	xv -280 yv -40  num 2  26"
//
//"	xv -340 yv 0  string  \"6\""
//"	xv -280 yv -10  num 2  27"
//
//"	xv -340 yv 30  string  \"7\""
//"	xv -280 yv 20  num 2  28"
//
//"	xv -340 yv 60  string  \"8\""
//"	xv -280 yv 50  num 2  29"
//
//"	xv -340 yv 90  string  \"9\""
//"	xv -280 yv 80  num 2  30"
//
//"	xv -340 yv 120  string  \"0\""
//"	xv -280 yv 110  num 2  31"
//
//"	xv -340 yv 150  string  \"g\""
//"	xv -280 yv 140  num 2  17"
//
//"	xv -340 yv 180  string  \"e\""
//"	xv -280 yv 170  num 2  12"

"endif "

// XP			// New
"if 18 "			// New. If STAT_XP_ICON is not zero, then do
"	yv	160 "		// New  //controls y position of string
"	xv	-14 "		// New  //controls x position of string
"	num	2	19 "	// New. Display 2-digits with value from stat-array at index 19
"	xv	20"
"	pic	18 "		// New. Display icon
"endif "

// Status Screen
// Mod Help Menu
"if 21"
"	xv 156"
"	yv 50 "
"	pic 21"
"endif "
;

char* single_Attributes_Player =

"if 20 "
"	xv -150 yv 50 pic 20"
//weapons


"endif"

//Player Attributes visual
"if 22"
"	xv -340 yv -150 string  \"1\""
"	xv -280 yv -160 num 2  22"

"	xv -340 yv -120 string  \"2\""
"	xv -280 yv -130 num 2  23"

"	xv -340 yv -90  string  \"3\""
"	xv -280 yv -100  num 2  24"

"	xv -340 yv -60  string  \"4\""
"	xv -280 yv -70  num 2  25"

"	xv -340 yv -30  string  \"5\""
"	xv -280 yv -40  num 2  26"

"	xv -340 yv 0  string  \"6\""
"	xv -280 yv -10  num 2  27"

"	xv -340 yv 30  string  \"7\""
"	xv -280 yv 20  num 2  28"

"	xv -340 yv 60  string  \"8\""
"	xv -280 yv 50  num 2  29"


//player attributes
"	xv -28 yv 98 string  \"1 bs attack\""
"	xv -28 yv 108 string \"2 max hp\""
"	xv -28 yv 118 string \"3 crit %\""
"	xv -28 yv 128 string \"4 crit dmg\""
"	xv -28 yv 138 string \"5 soul gain\""
"	xv -130 yv 98 string \"6 melee-ev\""
"	xv -130 yv 108 string \"7 ranged-ev\""
"	xv -130 yv 118 string \"8 magic-ev\""

"endif "

// XP			// New
"if 18 "			// New. If STAT_XP_ICON is not zero, then do
"	yv	160 "		// New  //controls y position of string
"	xv	-14 "		// New  //controls x position of string
"	num	2	19 "	// New. Display 2-digits with value from stat-array at index 19
"	xv	20"
"	pic	18 "		// New. Display icon
"endif "

// Status Screen
// Mod Help Menu
"if 21"
"	xv 156"
"	yv 50 "
"	pic 21"
"endif "
;

char* single_statusbar2 =
"yb	-24 "

// health
"xv	0 "
"hnum "
"xv	50 "
"pic 0 "

// ammo
"if 2 "
"	xv	100 "
"	anum "
"	xv	150 "
"	pic 2 "
"endif "

// armor
"if 4 "
"	xv	200 "
"	rnum "
"	xv	250 "
"	pic 4 "
"endif "

// selected item
"if 6 "
"	xv	296 "
"	pic 6 "
"endif "

"yb	-50 "

// picked up item
"if 7 "
"	xv	0 "
"	pic 7 "
"	xv	26 "
"	yb	-42 "
"	stat_string 8 "
"	yb	-50 "
"endif "

// timer
"if 9 "
"	yb	-24 "		// New. Set Y-cursor -24 pixels from physical screen bottom
"	xr	-58 "		// New. Set X-cursor -58 pixels from physical screen right
"	num	2	10 "
"	xr	-24 "		// New
"	pic	9 "
"endif "
// New

//  help / weapon icon 
"if 11 "
"	xv	148 "
"	pic	11 "
"endif "
;

void G_SetSoulAllocationScreen(edict_t* ent) {
	gclient_t* cl;
	cl = ent->client;
	if (cl->pers.showSoulAllocation) {
		char* currentStatusBar = "";
		switch (cl->pers.currentAttributeScreen)	
		{
		case PLAYER:
			cl->ps.stats[STAT_SWORD_LEVEL] = cl->pers.baseDamageMultiplier;
			cl->ps.stats[STAT_SPEAR_LEVEL] = cl->pers.maxHPMultiplier;
			cl->ps.stats[STAT_HAMMER_LEVEL] = cl->pers.critChance;
			cl->ps.stats[STAT_SUPERHAMMER_LEVEL] = cl->pers.critDamage;
			cl->ps.stats[STAT_DAGGERS_LEVEL] = cl->pers.soulgainMultiplier;
			cl->ps.stats[STAT_BALLISTA_LEVEL] = cl->pers.weapon_melee;
			cl->ps.stats[STAT_SPBALLISTA_LEVEL] = cl->pers.weapon_ranged;
			cl->ps.stats[STAT_CANON_LEVEL] = cl->pers.weapon_magic;
			currentStatusBar = single_Attributes_Player;
			break;
		case WEAPON:
			
			switch (cl->pers.weapon_melee) // Melee Weapons
			{
			case STAGE1:
				cl->ps.stats[STAT_SWORD_LEVEL] = 1;
				cl->ps.stats[STAT_SPEAR_LEVEL] = 0;
				cl->ps.stats[STAT_HAMMER_LEVEL] = 0;
				cl->ps.stats[STAT_SUPERHAMMER_LEVEL] = 0;
			case STAGE2:
				cl->ps.stats[STAT_SWORD_LEVEL] = 1;
				cl->ps.stats[STAT_SPEAR_LEVEL] = 1;
				cl->ps.stats[STAT_HAMMER_LEVEL] = 0;
				cl->ps.stats[STAT_SUPERHAMMER_LEVEL] = 0;
			case STAGE3:
				cl->ps.stats[STAT_SWORD_LEVEL] = 1;
				cl->ps.stats[STAT_SPEAR_LEVEL] = 1;
				cl->ps.stats[STAT_HAMMER_LEVEL] = 1;
				cl->ps.stats[STAT_SUPERHAMMER_LEVEL] = 0;
			case STAGE4:
				cl->ps.stats[STAT_SWORD_LEVEL] = 1;
				cl->ps.stats[STAT_SPEAR_LEVEL] = 1;
				cl->ps.stats[STAT_HAMMER_LEVEL] = 1;
				cl->ps.stats[STAT_SUPERHAMMER_LEVEL] = 1;
			default:
				break;
			}

			switch (cl->pers.weapon_ranged)
			{
			case STAGE1:
				cl->ps.stats[STAT_DAGGERS_LEVEL] = 1;
				cl->ps.stats[STAT_BALLISTA_LEVEL] = 0;
				cl->ps.stats[STAT_SPBALLISTA_LEVEL] = 0;
				cl->ps.stats[STAT_CANON_LEVEL] = 0;
			case STAGE2:
				cl->ps.stats[STAT_DAGGERS_LEVEL] = 1;
				cl->ps.stats[STAT_BALLISTA_LEVEL] = 1;
				cl->ps.stats[STAT_SPBALLISTA_LEVEL] = 0;
				cl->ps.stats[STAT_CANON_LEVEL] = 0;
			case STAGE3:
				cl->ps.stats[STAT_DAGGERS_LEVEL] = 1;
				cl->ps.stats[STAT_BALLISTA_LEVEL] = 1;
				cl->ps.stats[STAT_SPBALLISTA_LEVEL] = 1;
				cl->ps.stats[STAT_CANON_LEVEL] = 0;
			case STAGE4:
				cl->ps.stats[STAT_DAGGERS_LEVEL] = 1;
				cl->ps.stats[STAT_BALLISTA_LEVEL] = 1;
				cl->ps.stats[STAT_SPBALLISTA_LEVEL] = 1;
				cl->ps.stats[STAT_CANON_LEVEL] = 1;
			default:
				break;
			}

			switch (cl->pers.weapon_magic)
			{
			case STAGE1:
				cl->ps.stats[STAT_FIREBALL_LEVEL] = 1;
				cl->ps.stats[STAT_MAGEHAND_LEVEL] = 0;
				cl->ps.stats[STAT_SHIELD_LEVEL] = 0;
				cl->ps.stats[STAT_REFLECT_LEVEL] = 0;
			case STAGE2:
				cl->ps.stats[STAT_FIREBALL_LEVEL] = 1;
				cl->ps.stats[STAT_MAGEHAND_LEVEL] = 1;
				cl->ps.stats[STAT_SHIELD_LEVEL] = 0;
				cl->ps.stats[STAT_REFLECT_LEVEL] = 0;
			case STAGE3:
				cl->ps.stats[STAT_FIREBALL_LEVEL] = 1;
				cl->ps.stats[STAT_MAGEHAND_LEVEL] = 1;
				cl->ps.stats[STAT_SHIELD_LEVEL] = 1;
				cl->ps.stats[STAT_REFLECT_LEVEL] = 0;
			case STAGE4:
				cl->ps.stats[STAT_FIREBALL_LEVEL] = 1;
				cl->ps.stats[STAT_MAGEHAND_LEVEL] = 1;
				cl->ps.stats[STAT_SHIELD_LEVEL] = 1;
				cl->ps.stats[STAT_REFLECT_LEVEL] = 1;
			default:
				break;
			}
			currentStatusBar = single_Attributes_Weapon;
			// Weapons
			break;
		default:
			currentStatusBar = single_statusbar2;
			break;
		}
		ent->client->ps.stats[STAT_XP_ICON] = level.pic_XP;
		ent->client->ps.stats[STAT_XP] = ent->client->pers.playersouls;
		ent->client->ps.stats[STAT_SCREEN] = level.pic_SoulAllocationMenu;

		gi.configstring(CS_STATUSBAR, currentStatusBar);
		

		

		//Attributes

	}
	else {
		ent->client->ps.stats[STAT_HEALTH] = ent->health;
		ent->client->ps.stats[STAT_XP_ICON] = 0;
		ent->client->ps.stats[STAT_XP] = 0;
		ent->client->ps.stats[STAT_SCREEN] = 0;

		gi.configstring(CS_STATUSBAR, single_statusbar2);
		
		// Weapons
		//cl->ps.stats[STAT_SWORD_LEVEL] = 0;
		//cl->ps.stats[STAT_SPEAR_LEVEL] = 0;
		//cl->ps.stats[STAT_HAMMER_LEVEL] = 0;
		//cl->ps.stats[STAT_SUPERHAMMER_LEVEL] = 0;
		//cl->ps.stats[STAT_DAGGERS_LEVEL] = 0;
		//cl->ps.stats[STAT_BALLISTA_LEVEL] = 0;
		//cl->ps.stats[STAT_SPBALLISTA_LEVEL] = 0;
		//cl->ps.stats[STAT_CANON_LEVEL] = 0;
		//cl->ps.stats[STAT_FIREBALL_LEVEL] = 0;
		//cl->ps.stats[STAT_MAGEHAND_LEVEL] = 0;
		//cl->ps.stats[STAT_SHIELD_LEVEL] = 0;


		//Attributes
		//cl->ps.stats[STAT_PLAYER_BASEDAMAGE] = 0;
		//cl->ps.stats[STAT_PLAYER_MAXHP] = 0;
		//cl->ps.stats[STAT_PLAYER_CRITCHANCE] = 0;
		//cl->ps.stats[STAT_PLAYER_CRITDAMAGE] = 0;
		//cl->ps.stats[STAT_PLAYER_SOULGAIN] = 0;
	}

	if (ent->client->pers.showHelpModMenu) {
		ent->client->ps.stats[STAT_HELP_MOD_MENU] = level.pic_HelpMenuMod;
	}
	else {
		ent->client->ps.stats[STAT_HELP_MOD_MENU] = 0;
	}
}


/*
===============
G_CheckChaseStats
===============
*/
void G_CheckChaseStats (edict_t *ent)
{
	int i;
	gclient_t *cl;

	for (i = 1; i <= maxclients->value; i++) {
		cl = g_edicts[i].client;
		if (!g_edicts[i].inuse || cl->chase_target != ent)
			continue;
		memcpy(cl->ps.stats, ent->client->ps.stats, sizeof(cl->ps.stats));
		G_SetSpectatorStats(g_edicts + i);
	}
}

/*
===============
G_SetSpectatorStats
===============
*/
void G_SetSpectatorStats (edict_t *ent)
{
	gclient_t *cl = ent->client;

	if (!cl->chase_target)
		G_SetStats (ent);

	cl->ps.stats[STAT_SPECTATOR] = 1;

	// layouts are independant in spectator
	cl->ps.stats[STAT_LAYOUTS] = 0;
	if (cl->pers.health <= 0 || level.intermissiontime || cl->showscores)
		cl->ps.stats[STAT_LAYOUTS] |= 1;
	if (cl->showinventory && cl->pers.health > 0)
		cl->ps.stats[STAT_LAYOUTS] |= 2;

	if (cl->chase_target && cl->chase_target->inuse)
		cl->ps.stats[STAT_CHASE] = CS_PLAYERSKINS + 
			(cl->chase_target - g_edicts) - 1;
	else
		cl->ps.stats[STAT_CHASE] = 0;
}

