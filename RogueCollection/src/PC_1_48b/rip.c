//File for the fun ends. Death or a total win
//rip.c        1.4 (A.I. Design)       12/14/84

#include "rogue.h"

static int sc_fd;

//external curses IBM (jll)

extern int scr_type;

//score: Figure score and post it.

score(int amount, int flags, char monst)
{
#ifndef DEMO
#ifndef WIZARD

  struct sc_ent his_score, top_ten[TOPSCORES];
  int newfile = FALSE, rank = 0;
  char response = ' ';

  is_saved = TRUE;
  if (amount || flags || monst)
  {
    move(LINES-1, 0);
    cursor(TRUE);
    printw("[Press Enter to see rankings]");
    flush_type();
    wait_for('\r');
    move(LINES-1, 0);
  }
  while ((sc_fd = open(s_score, 0))<0)
  {
    printw("\n");
    if (noscore || (amount==0)) return;
    str_attr("No scorefile: %Create %Retry %Abort");
reread:
    switch (response = readchar())
    {
      case 'c': case 'C': close(creat(s_score, 0666));
      case 'r': case 'R': break;
      case 'a': case 'A': return;
      default: goto reread;
    }
  }
  printw("\n");
  get_scores(&top_ten);
  if (noscore!=TRUE)
  {
    strcpy(his_score.sc_name, whoami);
    his_score.sc_gold = amount;
    his_score.sc_fate = flags?flags:monst;
    his_score.sc_level = max_level;
    his_score.sc_rank = pstats.s_lvl;
    rank = add_scores(&his_score, &top_ten);
  }
  close(sc_fd);
  if (rank>0)
  {
    if ((sc_fd = creat(s_score, 0666))>=0) {put_scores(&top_ten); close(sc_fd);}
  }
  pr_scores(rank, &top_ten);

#endif WIZARD
#endif DEMO
}

#ifndef DEMO
#ifndef WIZARD

get_scores(struct sc_ent *top10)
{
  int i, retcode = 1;

  for (i = 0; i<TOPSCORES; i++, top10++)
  {
    if (retcode>0) retcode = read(sc_fd, top10, sizeof(struct sc_ent));
    if (retcode<=0) top10->sc_gold = 0;
  }
}

put_scores(struct sc_ent *top10)
{
  int i;

  for (i = 0; (i<TOPSCORES) && top10->sc_gold; i++, top10++)
  {
    if (write(sc_fd, top10, sizeof(struct sc_ent))<=0) return;
  }
}

pr_scores(int newrank, struct sc_ent *top10)
{
  int i;
  char *ki;
  int curl;
  char dthstr[30];
  char *altmsg;

  switch_page(old_page_no);
  clear();
  high();
  if (scr_type==7) standout();
  mvaddstr(0, 0, "Guildmaster's Hall Of Fame:");
  standend();
  yellow();
  mvaddstr(2, 0, "Gold");
  for (i = 0; i<TOPSCORES; i++, top10++)
  {
    altmsg = NULL;
    brown();
    if (newrank-1==i)
    {
      if (scr_type==7) standout(); else yellow();
    }
    if (top10->sc_gold<=0) break;
    curl = 4+((COLS==40)?(i*2):i);
    move(curl, 0);
    printw("%d ", top10->sc_gold);
    move(curl, 6);
    if (newrank-1!=i) red();
    printw("%s", top10->sc_name);
    if ((newrank)-1!=i) brown();
    if (top10->sc_level>=26) altmsg = " Honored by the Guild";
    if (isalpha(top10->sc_fate))
    {
      sprintf(dthstr, " killed by %s", killname((0xff&top10->sc_fate), TRUE));
      if (COLS==40 && strlen(dthstr)>23) strcpy(dthstr, " killed");
    }
    else switch(top10->sc_fate)
    {
      case 2: altmsg = " A total winner!"; break;
      case 1: strcpy(dthstr, " quit"); break;
      default: strcpy(dthstr, " weirded out");
    }
    if ((strlen(top10->sc_name)+10+strlen(he_man[top10->sc_rank-1]))<COLS)
    {
      if (top10->sc_rank>1 && (strlen(top10->sc_name))) printw(" \"%s\"", he_man[top10->sc_rank-1]);
    }
    if (COLS==40) move(curl+1, 6);
    if (altmsg==NULL) printw("%s on level %d", dthstr, top10->sc_level);
    else addstr(altmsg);
  }
  standend();
  if (COLS==80) addstr("\n\n\n\n");
}

add_scores(struct sc_ent *newscore, struct sc_ent *oldlist)
{
  struct sc_ent *sentry, *insert;
  int retcode = TOPSCORES+1;

  for (sentry = &oldlist[TOPSCORES-1]; sentry>=oldlist; sentry--)
  {
    if ((unsigned)newscore->sc_gold>(unsigned)sentry->sc_gold)
    {
      insert = sentry;
      retcode--;
      if ((insert<&oldlist[TOPSCORES-1]) && sentry->sc_gold) sentry[1] = *sentry;
    }
    else break;
  }
  if (retcode==11) return 0;
  *insert = *newscore;
  return retcode;
}

#endif WIZARD
#endif DEMO

//death: Do something really fun when he dies
death(char monst)
{
  char *killer;
  char buf[MAXSTR];
  int year;

#ifndef DEMO

  purse -= purse/10;
  switch_page(old_page_no);
  clear();
  drop_curtain();
  if (is_color) brown();
  box((COLS==40)?1:7, (COLS-28)/2, 22, (COLS+28)/2);
  standend();
  center(10, "REST");
  center(11, "IN");
  center(12, "PEACE");
  red();
  center(21, "  *    *      * ");
  green();
  center(22, "___\\/(\\/)/(\\/ \\\\(//)\\)\\/(//)\\\\)//(\\__");
  standend();
  if (scr_type==7) uline();
  center(14, your_na);
  standend();
  killer = killname(monst, TRUE);
  strcpy(buf, "killed by");
  center(15, buf);
  center(16, kild_by);
  sprintf(buf, "%u Au", purse);
  center(18, buf);
  regs->ax = 0x2a<<8;
  swint(SW_DOS, regs);
  year = regs->cx;
  sprintf(buf, "%u", year);
  center(19, buf);
  raise_curtain();
  move(LINES-1, 0);
  score(purse, 0, monst);

#else DEMO

  demo(0);
  killer = killname(monst, TRUE);
  strcpy(buf, "This time you were killed by");
  strcat(buf, " ");
  strcat(buf, killer);
  if (strlen(buf)>(COLS-2)) center(6, "This time you were killed");
  else center(6, buf);
  move(LINES-2, 0);

#endif DEMO

  wclose(0);
  exit();
}

//total_winner: Code for a winner
total_winner()
{
#ifndef DEMO

  THING *obj;
  int worth;
  byte c;
  int oldpurse;

  switch_page(old_page_no);
  clear();

#ifdef MINROG

  if (!terse)
  {
    standout();
    printw("                                                               \n");
    printw("  @   @               @   @           @          @@@  @     @  \n");
    printw("  @   @               @@ @@           @           @   @     @  \n");
    printw("  @   @  @@@  @   @   @ @ @  @@@   @@@@  @@@      @  @@@    @  \n");
    printw("   @@@@ @   @ @   @   @   @     @ @   @ @   @     @   @     @  \n");
    printw("      @ @   @ @   @   @   @  @@@@ @   @ @@@@@     @   @     @  \n");
    printw("  @   @ @   @ @  @@   @   @ @   @ @   @ @         @   @  @     \n");
    printw("   @@@   @@@   @@ @   @   @  @@@@  @@@@  @@@     @@@   @@   @  \n");
  }
  printw("                                                               \n");
  printw("     Congratulations, you have made it to the light of day!    \n");
  standend();
  printw("\nYou have joined the elite ranks of those who have escaped the\n");
  printw("Dungeons of Doom alive.  You journey home and sell all your loot at\n");
  printw("a great profit and are admitted to the fighters guild.\n");

#else

  printw("Congratulations!\n\nYou have made it to the light of day!\n\n\n\n");
  printw("You journey home and sell all your\n");
  printw("loot at a great profit and are\n");
  printw("admitted to the fighters guild.\n\n\n");

#endif MINROG

  mvaddstr(LINES-1, 0, "--Press space to continue--");
  wait_for(' ');
  clear();
  mvaddstr(0, 0, "   Worth  Item");
  oldpurse = purse;
  for (c = 'a', obj = pack; obj!=NULL; c++, obj = next(obj))
  {
    switch (obj->o_type)
    {
      case FOOD:
        worth = 2*obj->o_count;
      break;

      case WEAPON:
        switch (obj->o_which)
        {
          case MACE: worth = 8; break;
          case SWORD: worth = 15; break;
          case CROSSBOW: worth = 30; break;
          case ARROW: worth = 1; break;
          case DAGGER: worth = 2; break;
          case TWOSWORD: worth = 75; break;
          case DART: worth = 1; break;
          case BOW: worth = 15; break;
          case BOLT: worth = 1; break;
          case SPEAR: worth = 5; break;
        }
        worth *= 3*(obj->o_hplus+obj->o_dplus)+obj->o_count;
        obj->o_flags |= ISKNOW;
      break;

      case ARMOR:
        switch (obj->o_which)
        {
          case LEATHER: worth = 20; break;
          case RING_MAIL: worth = 25; break;
          case STUDDED_LEATHER: worth = 20; break;
          case SCALE_MAIL: worth = 30; break;
          case CHAIN_MAIL: worth = 75; break;
          case SPLINT_MAIL: worth = 80; break;
          case BANDED_MAIL: worth = 90; break;
          case PLATE_MAIL: worth = 150; break;
        }
        worth += (9-obj->o_ac)*100;
        worth += (10*(a_class[obj->o_which]-obj->o_ac));
        obj->o_flags |= ISKNOW;
      break;

      case SCROLL:
        worth = s_magic[obj->o_which].mi_worth;
        worth *= obj->o_count;
        if (!s_know[obj->o_which]) worth /= 2;
        s_know[obj->o_which] = TRUE;
      break;

      case POTION:
        worth = p_magic[obj->o_which].mi_worth;
        worth *= obj->o_count;
        if (!p_know[obj->o_which]) worth /= 2;
        p_know[obj->o_which] = TRUE;
      break;

      case RING:
        worth = r_magic[obj->o_which].mi_worth;
        if (obj->o_which==R_ADDSTR || obj->o_which==R_ADDDAM || obj->o_which==R_PROTECT || obj->o_which==R_ADDHIT)
        if (obj->o_ac>0) worth += obj->o_ac*100;
        else worth = 10;
        if (!(obj->o_flags&ISKNOW)) worth /= 2;
        obj->o_flags |= ISKNOW;
        r_know[obj->o_which] = TRUE;
      break;

      case STICK:
        worth = ws_magic[obj->o_which].mi_worth;
        worth += 20*obj->o_charges;
        if (!(obj->o_flags&ISKNOW)) worth /= 2;
        obj->o_flags |= ISKNOW;
        ws_know[obj->o_which] = TRUE;
      break;

      case AMULET:
        worth = 1000;
      break;
    }
    if (worth<0) worth = 0;
    move(c-'a'+1, 0);
    printw("%c) %5d  %s", c, worth, inv_name(obj, FALSE));
    purse += worth;
  }
  move(c-'a'+1, 0);
  printw("   %5u  Gold Pieces          ", oldpurse);
  score(purse, 2, 0);

#endif DEMO

  exit(0);
}

//killname: Convert a code to a monster name
char *killname(char monst, bool doart)
{
  char *sp;
  bool article;

  sp = prbuf;
  article = TRUE;
  switch (monst)
  {
    case 'a': sp = "arrow"; break;
    case 'b': sp = "bolt"; break;
    case 'd': sp = "dart"; break;
    case 's': sp = "starvation"; article = FALSE; break;
    case 'f': sp = "fall"; break;
    default:
      if (monst>='A' && monst<='Z') sp = monsters[monst-'A'].m_name;
      else {sp = "God"; article = FALSE;}
    break;
  }
  if (doart && article) sprintf(prbuf, "a%s ", vowelstr(sp));
  else prbuf[0] = '\0';
  strcat(prbuf, sp);
  return prbuf;
}

#ifdef DEMO

//For the demonstration version of rogue we really want to Print out a message when the game ends telling them how to order the game.
demo(int endtype)
{
  int i;
  char demobuf[81];

  switch_page(old_page_no);
  clear();
  if (is_color) brown();
  box(0, 0, LINES-2, COLS-1);
  bold();
  center(2, "ROGUE:  The Adventure Game");
  standend();
  if (is_color) lmagenta();
  sprintf(demobuf, "Sorry, %s but this is just a demonstration", whoami);
  if (terse) sprintf(demobuf, "Sorry, this is just a demonstration");
  center(4, demobuf);
  //quitter
  if (endtype==1)
  {
    sprintf(demobuf, "You quit with %u pieces of Gold", purse);
    center(6, demobuf);
  }
  else if (endtype==DEMOTIME)
  {
    sprintf(demobuf, "You ended with %u gold pieces", purse);
    center(6, demobuf);
  }
  if (terse) center(8, "If you're interested in doing some");
  else center(8, "But, if you're interested in doing some");
  center(9, "more exploring in the Dungeons of Doom");
  if (is_color) red();
  center(11, "Please Contact:                      ");
  if (!is_color) uline(); else standend();
  center(13, "A. I. Design");
  center(14, "P.O. Box  3685");
  center(15, "Santa Clara, California 95055");
  if (is_color) red();
  center(17, "(408) 296-1634");
  if (is_color) yellow(); else standend();
  center(19, "(C) Copyright 1983");
  high();
  center(20, "Artificial Intelligence Design");
  if (is_color) yellow(); else standend();
  center(21, "All Rights Reserved");
  if (endtype==0) return;
  move(LINES-2, 0);
  wclose(0);
  exit(0);
}

#endif DEMO
