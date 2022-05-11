#define SWORD_NORMAL_DAMAGE 50
#define SWORD_DEATHMATCH_DAMAGE 100
#define SWORD_KICK 500
#define SWORD_RANGE 35


/*
=============
fire_sword

attacks with the beloved sword of the highlander
 edict_t *self - entity producing it, yourself
 vec3_t start - The place you are
 vec3_t aimdir - Where you are looking at in this case
 int damage - the damage the sword inflicts
 int kick - how much you want that bitch to be thrown back
=============
*/

void fire_sword (edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int range){
	//You may recognize a lot of this from the fire lead command, which
	//is the one that I understood best what the hell was going on

	trace_t tr;             //Not entirely sure what this is, I know that it is used
					//to trace out the route of the weapon being used...gotta limit it

	vec3_t          dir;            //Another point I am unclear about
	vec3_t          forward;        //maybe someday I will know a little bit
	vec3_t          right;          //better about what these are
	vec3_t          up;
	vec3_t          end;

	tr = gi.trace(self->s.origin, NULL, NULL, start, self, MASK_SHOT);

	if (!(tr.fraction < 1.0))       //I can only assume this has something to do
								//with the progress of the trace
	{
		vectoangles(aimdir, dir);
		AngleVectors(dir, forward, right, up);             //possibly sets some of the angle vectors
												//as standards?

		VectorMA(start, range, aimdir, end);
		tr = gi.trace(self->s.origin, NULL, NULL, end, self, MASK_SHOT);
	}

	//The fire_lead had an awful lot of stuff in here dealing with the effect of the shot
	//upon water and whatnot, but a sword doesn't make you worry about that sort of stuff
	//thats why highlanders are so damn cool.

	if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
	{
		if (tr.fraction < 1.0)
		{
			if (tr.ent->takedamage)
			{
				//This tells us to damage the thing that in our path...hehe
				T_Damage(tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, kick, 0,0);
				gi.sound(self, CHAN_AUTO, gi.soundindex("misc/fhit3.wav"), 1, ATTN_NORM, 0);
			}
			else
			{
				if (strncmp(tr.surface->name, "sky", 3) != 0)
				{
					gi.WriteByte(svc_temp_entity);
					gi.WriteByte(TE_SPARKS);
					gi.WritePosition(tr.endpos);
					gi.WriteDir(tr.plane.normal);
					gi.multicast(tr.endpos, MULTICAST_PVS);

					/*if (self->client)
						PlayerNoise(self, tr.endpos, PNOISE_IMPACT);*/
					gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
				}
			}
		}
	}
	return;
}

void sword_attack(edict_t* ent, vec3_t g_offset, int damage)
{
	vec3_t  forward, right;
	vec3_t  start;
	vec3_t  offset;

	if (is_quad)
		damage *= 4;
	AngleVectors(ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 24, 8, ent->viewheight - 8);
	VectorAdd(offset, g_offset, offset);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale(forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;
	//sword, spear, hammer super hammer
	//ent->client->pers.ballista
	
	fire_sword(ent, start, forward, damage, SWORD_KICK,SWORD_RANGE);
}

void Weapon_Sword_Fire(edict_t* ent)
{
	int damage;
	if (deathmatch->value)
		damage = SWORD_DEATHMATCH_DAMAGE;
	else
		damage = SWORD_NORMAL_DAMAGE;
	sword_attack(ent, vec3_origin, damage);
	ent->client->ps.gunframe++;
}

void weapon_Sword(edict_t* ent)
{
	static int      pause_frames[] = { 19, 32, 0 };
	static int      fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_Sword_Fire);
}



#define SPEAR_NORMAL_DAMAGE 25
#define SPEAR_DEATHMATCH_DAMAGE 50
#define SPEAR_KICK 500
#define SPEAR_RANGE 40


/*
=============
fire_spear

attacks with the beloved sword of the highlander
 edict_t *self - entity producing it, yourself
 vec3_t start - The place you are
 vec3_t aimdir - Where you are looking at in this case
 int damage - the damage the sword inflicts
 int kick - how much you want that bitch to be thrown back
=============
*/

void fire_spear(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int range) {
	//You may recognize a lot of this from the fire lead command, which
	//is the one that I understood best what the hell was going on

	trace_t tr;             //Not entirely sure what this is, I know that it is used
					//to trace out the route of the weapon being used...gotta limit it

	vec3_t          dir;            //Another point I am unclear about
	vec3_t          forward;        //maybe someday I will know a little bit
	vec3_t          right;          //better about what these are
	vec3_t          up;
	vec3_t          end;

	tr = gi.trace(self->s.origin, NULL, NULL, start, self, MASK_SHOT);

	if (!(tr.fraction < 1.0))       //I can only assume this has something to do
								//with the progress of the trace
	{
		vectoangles(aimdir, dir);
		AngleVectors(dir, forward, right, up);             //possibly sets some of the angle vectors
												//as standards?

		VectorMA(start, range, aimdir, end);
		tr = gi.trace(self->s.origin, NULL, NULL, end, self, MASK_SHOT);
	}

	//The fire_lead had an awful lot of stuff in here dealing with the effect of the shot
	//upon water and whatnot, but a sword doesn't make you worry about that sort of stuff
	//thats why highlanders are so damn cool.

	if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
	{
		if (tr.fraction < 1.0)
		{
			if (tr.ent->takedamage)
			{
				//This tells us to damage the thing that in our path...hehe
				T_Damage(tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, kick, 0, 0);
				gi.sound(self, CHAN_AUTO, gi.soundindex("misc/fhit3.wav"), 1, ATTN_NORM, 0);
			}
			else
			{
				if (strncmp(tr.surface->name, "sky", 3) != 0)
				{
					gi.WriteByte(svc_temp_entity);
					gi.WriteByte(TE_SPARKS);
					gi.WritePosition(tr.endpos);
					gi.WriteDir(tr.plane.normal);
					gi.multicast(tr.endpos, MULTICAST_PVS);

					/*if (self->client)
						PlayerNoise(self, tr.endpos, PNOISE_IMPACT);*/
					gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
				}
			}
		}
	}
	return;
}

void spear_attack(edict_t* ent, vec3_t g_offset, int damage)
{
	vec3_t  forward, right;
	vec3_t  start;
	vec3_t  offset;

	if (is_quad)
		damage *= 4;
	AngleVectors(ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 24, 8, ent->viewheight - 8);
	VectorAdd(offset, g_offset, offset);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale(forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;
	//sword, spear, hammer super hammer
	//ent->client->pers.ballista

	fire_spear(ent, start, forward, damage, SPEAR_KICK, SPEAR_RANGE);
}

void Weapon_Spear_Fire(edict_t* ent)
{
	int damage;
	if (deathmatch->value)
		damage = SPEAR_DEATHMATCH_DAMAGE;
	else
		damage = SPEAR_NORMAL_DAMAGE;
	spear_attack(ent, vec3_origin, damage);
	ent->client->ps.gunframe++;
}

void weapon_Spear(edict_t* ent)
{
	static int      pause_frames[] = { 19, 32, 0 };
	static int      fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_Spear_Fire);
}

#define HAMMER_NORMAL_DAMAGE 200
#define HAMMER_DEATHMATCH_DAMAGE 400
#define HAMMER_KICK 1000
#define HAMMER_RANGE 35


/*
=============
fire_hammer

attacks with the beloved sword of the highlander
 edict_t *self - entity producing it, yourself
 vec3_t start - The place you are
 vec3_t aimdir - Where you are looking at in this case
 int damage - the damage the sword inflicts
 int kick - how much you want that bitch to be thrown back
=============
*/

void fire_Hammer(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int range) {
	//You may recognize a lot of this from the fire lead command, which
	//is the one that I understood best what the hell was going on

	trace_t tr;             //Not entirely sure what this is, I know that it is used
					//to trace out the route of the weapon being used...gotta limit it

	vec3_t          dir;            //Another point I am unclear about
	vec3_t          forward;        //maybe someday I will know a little bit
	vec3_t          right;          //better about what these are
	vec3_t          up;
	vec3_t          end;

	tr = gi.trace(self->s.origin, NULL, NULL, start, self, MASK_SHOT);

	if (!(tr.fraction < 1.0))       //I can only assume this has something to do
								//with the progress of the trace
	{
		vectoangles(aimdir, dir);
		AngleVectors(dir, forward, right, up);             //possibly sets some of the angle vectors
												//as standards?

		VectorMA(start, range, aimdir, end);
		tr = gi.trace(self->s.origin, NULL, NULL, end, self, MASK_SHOT);
	}

	//The fire_lead had an awful lot of stuff in here dealing with the effect of the shot
	//upon water and whatnot, but a sword doesn't make you worry about that sort of stuff
	//thats why highlanders are so damn cool.

	if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
	{
		if (tr.fraction < 1.0)
		{
			if (tr.ent->takedamage)
			{
				//This tells us to damage the thing that in our path...hehe
				T_Damage(tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, kick, 0, 0);
				gi.sound(self, CHAN_AUTO, gi.soundindex("misc/fhit3.wav"), 1, ATTN_NORM, 0);
			}
			else
			{
				if (strncmp(tr.surface->name, "sky", 3) != 0)
				{
					gi.WriteByte(svc_temp_entity);
					gi.WriteByte(TE_SPARKS);
					gi.WritePosition(tr.endpos);
					gi.WriteDir(tr.plane.normal);
					gi.multicast(tr.endpos, MULTICAST_PVS);

					/*if (self->client)
						PlayerNoise(self, tr.endpos, PNOISE_IMPACT);*/
					gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
				}
			}
		}
	}
	return;
}

void Hammer_attack(edict_t* ent, vec3_t g_offset, int damage)
{
	vec3_t  forward, right;
	vec3_t  start;
	vec3_t  offset;

	if (is_quad)
		damage *= 4;
	AngleVectors(ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 24, 8, ent->viewheight - 8);
	VectorAdd(offset, g_offset, offset);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale(forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;
	//sword, spear, hammer super hammer
	//ent->client->pers.ballista

	fire_Hammer(ent, start, forward, damage, HAMMER_KICK, HAMMER_RANGE);
}

void Weapon_Hammer_Fire(edict_t* ent)
{
	int damage;
	if (deathmatch->value)
		damage = HAMMER_DEATHMATCH_DAMAGE;
	else
		damage = HAMMER_NORMAL_DAMAGE;
	Hammer_attack(ent, vec3_origin, damage);
	ent->client->ps.gunframe++;
}

void weapon_Hammer(edict_t* ent)
{
	static int      pause_frames[] = { 19, 32, 0 };
	static int      fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_Hammer_Fire);
}

#define SPHAMMER_NORMAL_DAMAGE 400
#define SPHAMMER_DEATHMATCH_DAMAGE 800
#define SPHAMMER_KICK 500
#define SPHAMMER_RANGE 100


/*
=============
fire_SPhammer

attacks with the beloved sword of the highlander
 edict_t *self - entity producing it, yourself
 vec3_t start - The place you are
 vec3_t aimdir - Where you are looking at in this case
 int damage - the damage the sword inflicts
 int kick - how much you want that bitch to be thrown back
=============
*/

void fire_SPHammer(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int kick, int range) {
	//You may recognize a lot of this from the fire lead command, which
	//is the one that I understood best what the hell was going on

	trace_t tr;             //Not entirely sure what this is, I know that it is used
					//to trace out the route of the weapon being used...gotta limit it

	vec3_t          dir;            //Another point I am unclear about
	vec3_t          forward;        //maybe someday I will know a little bit
	vec3_t          right;          //better about what these are
	vec3_t          up;
	vec3_t          end;

	tr = gi.trace(self->s.origin, NULL, NULL, start, self, MASK_SHOT);

	if (!(tr.fraction < 1.0))       //I can only assume this has something to do
								//with the progress of the trace
	{
		vectoangles(aimdir, dir);
		AngleVectors(dir, forward, right, up);             //possibly sets some of the angle vectors
												//as standards?

		VectorMA(start, range, aimdir, end);
		tr = gi.trace(self->s.origin, NULL, NULL, end, self, MASK_SHOT);
	}

	//The fire_lead had an awful lot of stuff in here dealing with the effect of the shot
	//upon water and whatnot, but a sword doesn't make you worry about that sort of stuff
	//thats why highlanders are so damn cool.

	if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
	{
		if (tr.fraction < 1.0)
		{
			if (tr.ent->takedamage)
			{
				//This tells us to damage the thing that in our path...hehe
				T_Damage(tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, kick, 0, 0);
				gi.sound(self, CHAN_AUTO, gi.soundindex("misc/fhit3.wav"), 1, ATTN_NORM, 0);
			}
			else
			{
				if (strncmp(tr.surface->name, "sky", 3) != 0)
				{
					gi.WriteByte(svc_temp_entity);
					gi.WriteByte(TE_SPARKS);
					gi.WritePosition(tr.endpos);
					gi.WriteDir(tr.plane.normal);
					gi.multicast(tr.endpos, MULTICAST_PVS);

					/*if (self->client)
						PlayerNoise(self, tr.endpos, PNOISE_IMPACT);*/
					gi.sound(self, CHAN_AUTO, gi.soundindex("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
				}
			}
		}
	}
	return;
}

void SPHammer_attack(edict_t* ent, vec3_t g_offset, int damage)
{
	vec3_t  forward, right;
	vec3_t  start;
	vec3_t  offset;

	if (is_quad)
		damage *= 4;
	AngleVectors(ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 24, 8, ent->viewheight - 8);
	VectorAdd(offset, g_offset, offset);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale(forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;
	//sword, spear, hammer super hammer
	//ent->client->pers.ballista

	fire_SPHammer(ent, start, forward, damage, SPHAMMER_KICK, SPHAMMER_RANGE);
}

void Weapon_SPHammer_Fire(edict_t* ent)
{
	int damage;
	if (deathmatch->value)
		damage = SPHAMMER_DEATHMATCH_DAMAGE;
	else
		damage = SPHAMMER_NORMAL_DAMAGE;
	SPHammer_attack(ent, vec3_origin, damage);
	ent->client->ps.gunframe++;
}

void weapon_SPHammer(edict_t* ent)
{
	static int      pause_frames[] = { 19, 32, 0 };
	static int      fire_frames[] = { 5, 0 };

	Weapon_Generic(ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_SPHammer_Fire);
}