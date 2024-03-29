/*
 * Functions to implement the various sticks one might find
 * while wandering around the dungeon.
 *
 * @(#)sticks.c	3.14 (Berkeley) 6/15/81
 *
 * Rogue: Exploring the Dungeons of Doom
 * Copyright (C) 1980, 1981 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include "curses.h"
#include <ctype.h>
#include <string.h>
#include "rogue.h"

void
fix_stick(struct object *cur)
{
    if (strcmp(ws_type[cur->o_which], "staff") == 0)
	strcpy(cur->o_damage,"2d3");
    else
	strcpy(cur->o_damage,"1d1");
    strcpy(cur->o_hurldmg,"1d1");

    cur->o_charges = 3 + rnd(5);
    switch (cur->o_which)
    {
	case WS_HIT:
	    cur->o_hplus = 3;
	    cur->o_dplus = 3;
	    strcpy(cur->o_damage,"1d8");
	when WS_LIGHT:
	    cur->o_charges = 10 + rnd(10);
    }
}

void
do_zap(int gotdir)
{
    struct linked_list *item;
    struct object *obj;
    struct room *rp;
    struct thing *tp;
    int y, x;

    if ((item = get_item("zap with", STICK)) == NULL)
	return;
    obj = (struct object *) ldata(item);
    if (obj->o_type != STICK)
    {
	msg("You can't zap with that!");
	after = FALSE;
	return;
    }
    if (obj->o_charges == 0)
    {
	msg("Nothing happens.");
	return;
    }
    if (!gotdir)
	do {
	    delta.y = rnd(3) - 1;
	    delta.x = rnd(3) - 1;
	} while (delta.y == 0 && delta.x == 0);
    switch (obj->o_which)
    {
	case WS_LIGHT:
	    /*
	     * Reddy Kilowat wand.  Light up the room
	     */
	    ws_know[WS_LIGHT] = TRUE;
	    if ((rp = roomin(&hero)) == NULL)
		msg("The corridor glows and then fades");
	    else
	    {
		addmsg("The room is lit");
		if (!terse)
		    addmsg(" by a shimmering blue light.");
		endmsg();
		rp->r_flags &= ~ISDARK;
		/*
		 * Light the room and put the player back up
		 */
		light(&hero);
		mvwaddrawch(cw, hero.y, hero.x, PLAYER);
	    }
	when WS_DRAIN:
	    /*
	     * Take away 1/2 of hero's hit points, then take it away
	     * evenly from the monsters in the room (or next to hero
	     * if he is in a passage)
	     */
	    if (pstats.s_hpt < 2)
	    {
		msg("You are too weak to use it.");
		return;
	    }
	    else if ((rp = roomin(&hero)) == NULL)
		drain(hero.y-1, hero.y+1, hero.x-1, hero.x+1);
	    else
		drain(rp->r_pos.y, rp->r_pos.y+rp->r_max.y,
		    rp->r_pos.x, rp->r_pos.x+rp->r_max.x);
	when WS_POLYMORPH:
	case WS_TELAWAY:
	case WS_TELTO:
	case WS_CANCEL:
	{
	    int monster;
	    int oldch;
	    int rm;

	    y = hero.y;
	    x = hero.x;
	    while (step_ok(winat(y, x)))
	    {
		y += delta.y;
		x += delta.x;
	    }
	    if (ismons(monster = CMVWINCH(mw, y, x)))
	    {
		int omonst = monster;

		if (monster == 'F')
		    player.t_flags &= ~ISHELD;
		item = find_mons(y, x);
		tp = (struct thing *) ldata(item);
		if (obj->o_which == WS_POLYMORPH)
		{
		    detach(mlist, item);
		    oldch = tp->t_oldch;
		    delta.y = y;
		    delta.x = x;
		    new_monster(item, monster = rnd(26) + 'A', &delta);
		    if (!(tp->t_flags & ISRUN))
			runto(&delta, &hero);
		    if (ismons(CMVWINCH(cw, y, x)))
			mvwaddrawch(cw, y, x, monster);
		    tp->t_oldch = oldch;
		    ws_know[WS_POLYMORPH] |= (monster != omonst);
		}
		else if (obj->o_which == WS_CANCEL)
		{
		    tp->t_flags |= ISCANC;
		    tp->t_flags &= ~ISINVIS;
		}
		else
		{
		    if (obj->o_which == WS_TELAWAY)
		    {
			do
			{
			    rm = rnd_room();
			    rnd_pos(&rooms[rm], &tp->t_pos);
			} until(winat(tp->t_pos.y, tp->t_pos.x) == FLOOR);
		    }
		    else
		    {
			tp->t_pos.y = hero.y + delta.y;
			tp->t_pos.x = hero.x + delta.x;
		    }
		    if (ismons(CMVWINCH(cw, y, x)))
			mvwaddrawch(cw, y, x, tp->t_oldch);
		    tp->t_dest = &hero;
		    tp->t_flags |= ISRUN;
		    mvwaddrawch(mw, y, x, ' ');
		    mvwaddrawch(mw, tp->t_pos.y, tp->t_pos.x, monster);
		    if (tp->t_pos.y != y || tp->t_pos.x != x)
			tp->t_oldch = CMVWINCH(cw, tp->t_pos.y, tp->t_pos.x);
		}
	    }
	}
	when WS_MISSILE:
	{
	    static struct object bolt =
	    {
		'*' , {0, 0}, 0, "", "1d4" , 0, 0, 100, 1, 0, 0, 0
	    };

	    do_motion(&bolt, delta.y, delta.x);
	    if (ismons(CMVWINCH(mw, bolt.o_pos.y, bolt.o_pos.x))
		&& !save_throw(VS_MAGIC, THINGPTR(find_mons(unc(bolt.o_pos)))))
		    hit_monster(unc(bolt.o_pos), &bolt);
	    else if (terse)
		msg("Missile vanishes");
	    else
		msg("The missile vanishes with a puff of smoke");
	    ws_know[WS_MISSILE] = TRUE;
	}
	when WS_HIT:
	{
	    int ch;

	    delta.y += hero.y;
	    delta.x += hero.x;
	    ch = winat(delta.y, delta.x);
	    if (ismons(ch))
	    {
		if (rnd(20) == 0)
		{
		    strcpy(obj->o_damage,"3d8");
		    obj->o_dplus = 9;
		}
		else
		{
		    strcpy(obj->o_damage,"1d8");
		    obj->o_dplus = 3;
		}
		fight(&delta, ch, obj, FALSE);
	    }
	}
	when WS_HASTE_M:
	case WS_SLOW_M:
	    y = hero.y;
	    x = hero.x;
	    while (step_ok(winat(y, x)))
	    {
		y += delta.y;
		x += delta.x;
	    }
	    if (ismons(CMVWINCH(mw, y, x)))
	    {
		item = find_mons(y, x);
		tp = (struct thing *) ldata(item);
		if (obj->o_which == WS_HASTE_M)
		{
		    if (on(*tp, ISSLOW))
			tp->t_flags &= ~ISSLOW;
		    else
			tp->t_flags |= ISHASTE;
		}
		else
		{
		    if (on(*tp, ISHASTE))
			tp->t_flags &= ~ISHASTE;
		    else
			tp->t_flags |= ISSLOW;
		    tp->t_turn = TRUE;
		}
		delta.y = y;
		delta.x = x;
		runto(&delta, &hero);
	    }
	when WS_ELECT:
	case WS_FIRE:
	case WS_COLD:
	{
	    int dirch;
	    char *name;
	    int ch;
	    int bounced, used;
	    coord pos;
	    coord spotpos[BOLT_LENGTH];
	    static struct object bolt =
	    {
		'*' , {0, 0}, 0, "", "6d6" , 0, 0, 100, 0, 0, 0 ,0
	    };


	    switch (delta.y + delta.x)
	    {
		case 0: dirch = '/';
		when 1: case -1: dirch = (delta.y == 0 ? '-' : '|');
		when 2: case -2: dirch = '\\';
	    }
	    pos = hero;
	    bounced = FALSE;
	    used = FALSE;
	    if (obj->o_which == WS_ELECT)
		name = "bolt";
	    else if (obj->o_which == WS_FIRE)
		name = "flame";
	    else
		name = "ice";
	    for (y = 0; y < BOLT_LENGTH && !used; y++)
	    {
		ch = winat(pos.y, pos.x);
		spotpos[y] = pos;
		switch (ch)
		{
		    case DOOR:
		    case SECRETDOOR:
		    case VWALL:
		    case HWALL:
			PC_GFX_WALL_CASES
		    case ' ':
			bounced = TRUE;
			delta.y = -delta.y;
			delta.x = -delta.x;
			y--;
			msg("The bolt bounces");
			break;
		    default:
			if (!bounced && ismons(ch))
			{
			    if (!save_throw(VS_MAGIC, THINGPTR(find_mons(unc(pos)))))
			    {
				bolt.o_pos = pos;
				hit_monster(unc(pos), &bolt);
				used = TRUE;
			    }
			    else if (ch != 'M' || show(pos.y, pos.x) == 'M')
			    {
				if (terse)
				    msg("%s misses", name);
				else
				    msg("The %s whizzes past the %s", name, monsters[ch-'A'].m_name);
				runto(&pos, &hero);
			    }
			}
			else if (bounced && pos.y == hero.y && pos.x == hero.x)
			{
			    bounced = FALSE;
			    if (!save(VS_MAGIC))
			    {
				if (terse)
				    msg("The %s hits", name);
				else
				    msg("You are hit by the %s", name);
				if ((pstats.s_hpt -= roll(6, 6)) <= 0)
				    death('b');
				used = TRUE;
			    }
			    else
				msg("The %s whizzes by you", name);
			}
			mvwaddrawch(cw, pos.y, pos.x, dirch);
			draw(cw);
		}
		pos.y += delta.y;
		pos.x += delta.x;
	    }
	    for (x = 0; x < y; x++)
		mvwaddrawch(cw, spotpos[x].y, spotpos[x].x, show(spotpos[x].y, spotpos[x].x));
	    ws_know[obj->o_which] = TRUE;
	}
	when WS_NOP:
	otherwise:
	    msg("What a bizarre schtick!");
    }
    obj->o_charges--;
}

/*
 * drain:
 *	Do drain hit points from player shtick
 */

void
drain(int ymin, int ymax, int xmin, int xmax)
{
    int i, j, cnt;
    struct thing *ick;
    struct linked_list *item;

    /*
     * First count how many things we need to spread the hit points among
     */
    cnt = 0;
    for (i = ymin; i <= ymax; i++)
	for (j = xmin; j <= xmax; j++)
	    if (ismons(CMVWINCH(mw, i, j)))
		cnt++;
    if (cnt == 0)
    {
	msg("You have a tingling feeling");
	return;
    }
    cnt = pstats.s_hpt / cnt;
    pstats.s_hpt /= 2;
    /*
     * Now zot all of the monsters
     */
    for (i = ymin; i <= ymax; i++)
	for (j = xmin; j <= xmax; j++)
	    if (ismons(CMVWINCH(mw, i, j)) &&
	        ((item = find_mons(i, j)) != NULL))
	    {
		ick = (struct thing *) ldata(item);
		if ((ick->t_stats.s_hpt -= cnt) < 1)
		    killed(item, cansee(i, j) && !on(*ick, ISINVIS));
	    }
}

/*
 * charge a wand for wizards.
 */
char *
charge_str(struct object *obj)
{
    static char buf[20];

    if (!(obj->o_flags & ISKNOW))
	buf[0] = '\0';
    else if (terse)
	sprintf(buf, " [%d]", obj->o_charges);
    else
	sprintf(buf, " [%d charges]", obj->o_charges);
    return buf;
}
