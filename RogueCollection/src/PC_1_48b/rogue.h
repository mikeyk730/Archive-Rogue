//Rogue definitions and variable declarations
//rogue.h      1.4 (AI Design) 12/14/84

//Options set for PC rogue

//copy protection

#define PROTECTED
#define CSUM  -1632
#ifdef PROTECTED
#define P_DAMAGE  6
#else
#define P_DAMAGE  1
#endif PROTECTED

//if DEBUG or WIZARD is changed might as well recompile everything

#define HELP
#undef DEMO
#define DEMOTIME  10

//DEMO recompile: save.c rip.c io.c main.c

#define REV  1
#define VER  48

//If CODECSUM is changed recompile extern.c

#define SCOREFILE  "rogue.scr"
#define SAVEFILE   "rogue.sav"
#define ENVFILE    "rogue.opt"
#define IBM
#define MACROSZ  41

#define ifterse0  ifterse
#define ifterse1  ifterse
#define ifterse2  ifterse
#define ifterse3  ifterse
#define ifterse4  ifterse

//MANX C compiler funnies

#define bcopy(a,b)  movmem(&(b),&(a),sizeof(a))
#define stpchr  index
typedef unsigned char byte;
typedef unsigned char bool;

//Maximum number of different things

#define MAXROOMS     9
#define MAXTHINGS    9
#define MAXOBJ       9
#define MAXPACK      23
#define MAXTRAPS     10
#define AMULETLEVEL  26
#define NUMTHINGS    7  //number of types of things
#define MAXPASS      13 //upper limit on number of passages
#define MAXNAME      20 //Maximum Length of a scroll
#define MAXITEMS     83 //Maximum number of randomly generated things

//All the fun defines

#define shint           int
#define until(expr)     while(!(expr))
#define next(ptr)       (*ptr).l_next
#define prev(ptr)       (*ptr).l_prev
#ifdef UNIX
#define winat(y,x)             (moat(y,x)!=NULL?moat(y,x)->t_disguise:chat(y,x))
#define DISTANCE(y1,x1,y2,x2)  (((x2)-(x1))*((x2)-(x1))+((y2)-(y1))*((y2)-(y1)))
#endif
#ifdef UNIX
#define ce(a,b)         ((a).x==(b).x && (a).y==(b).y)
#else
#define ce(a,b)         _ce(&(a),&(b))
#endif
#define hero            player.t_pos
#define pstats          player.t_stats
#define pack            player.t_pack
#define proom           player.t_room
#define max_hp          player.t_stats.s_maxhp
#define attach(a,b)     _attach(&a,b)
#define detach(a,b)     _detach(&a,b)
#define free_list(a)    _free_list(&a)
#define max(a,b)        ((a)>(b)?(a):(b))
#define on(thing,flag)  (((thing).t_flags&(flag))!=0)
#define CTRL(ch)        (ch&037)
#define GOLDCALC        (rnd(50+10*level)+2)
#define ISRING(h,r)     (cur_ring[h]!=NULL && cur_ring[h]->o_which==r)
#define ISWEARING(r)    (ISRING(LEFT,r) || ISRING(RIGHT,r))
#define ISMULT(type)    (type==POTION || type==SCROLL || type==FOOD || type==GOLD)
#define chat(y,x)       (_level[INDEX(y,x)])
#define flat(y,x)       (_flags[INDEX(y,x)])
#define unc(cp)         (cp).y,(cp).x
#define isfloor(c)      ((c)==FLOOR || (c)==PASSAGE)
#define isgone(rp)      (((rp)->r_flags&ISGONE) && ((rp)->r_flags&ISMAZE)==0)
#ifdef WIZARD
#define debug           if (wizard) msg
#endif

//Things that appear on the screens

#define PASSAGE   (0xb1)
#define DOOR      (0xce)
#define FLOOR     (0xfa)
#define PLAYER    (0x01)
#define TRAP      (0x04)
#define STAIRS    (0xf0)
#define GOLD      (0x0f)
#define POTION    (0xad)
#define SCROLL    (0x0d)
#define MAGIC     '$'
#define BMAGIC    '+'
#define FOOD      (0x05)
#define STICK     (0xe7)
#define ARMOR     (0x08)
#define AMULET    (0x0c)
#define RING      (0x09)
#define WEAPON    (0x18)
#define CALLABLE  -1

#define VWALL   (0xba)
#define HWALL   (0xcd)
#define ULWALL  (0xc9)
#define URWALL  (0xbb)
#define LLWALL  (0xc8)
#define LRWALL  (0xbc)

//Various constants

#define BEARTIME     spread(3)
#define SLEEPTIME    spread(5)
#define HEALTIME     spread(30)
#define HOLDTIME     spread(2)
#define WANDERTIME   spread(70)
#define HUHDURATION  spread(20)
#define SEEDURATION  spread(300)
#define HUNGERTIME   spread(1300)
#define MORETIME     150
#define STOMACHSIZE  2000
#define STARVETIME   850
#define ESCAPE       27
#define LEFT         0
#define RIGHT        1
#define BOLT_LENGTH  6
#define LAMPDIST     3

//Save against things

#define VS_POISON        00
#define VS_PARALYZATION  00
#define VS_LUCK          01
#define VS_DEATH         00
#define VS_BREATH        02
#define VS_MAGIC         03

//Various flag bits

//flags for rooms

#define ISDARK  0x0001 //room is dark
#define ISGONE  0x0002 //room is gone (a corridor)
#define ISMAZE  0x0004 //room is a maze

//flags for objects

#define ISCURSED  0x0001 //object is cursed
#define ISKNOW    0x0002 //player knows details about the object
#define DIDFLASH  0x0004 //has the vorpal weapon flashed
#define ISEGO     0x0008 //weapon has control of player
#define ISMISL    0x0010 //object is a missile type
#define ISMANY    0x0020 //object comes in groups
#define ISREVEAL  0x0040 //Do you know who the enemy of the object is

//flags for creatures

#define ISBLIND   0x0001 //creature is blind
#define SEEMONST  0x0002 //hero can detect unseen monsters
#define ISRUN     0x0004 //creature is running at the player
#define ISFOUND   0x0008 //creature has been seen (used for objects)
#define ISINVIS   0x0010 //creature is invisible
#define ISMEAN    0x0020 //creature can wake when player enters room
#define ISGREED   0x0040 //creature runs to protect gold
#define ISHELD    0x0080 //creature has been held
#define ISHUH     0x0100 //creature is confused
#define ISREGEN   0x0200 //creature can regenerate
#define CANHUH    0x0400 //creature can confuse
#define CANSEE    0x0800 //creature can see invisible creatures
#define ISCANC    0x1000 //creature has special qualities cancelled
#define ISSLOW    0x2000 //creature has been slowed
#define ISHASTE   0x4000 //creature has been hastened
#define ISFLY     0x8000 //creature is of the flying type

//Flags for level map

#define F_PASS   0x040 //is a passageway
#define F_MAZE   0x020 //have seen this corridor before
#define F_REAL   0x010 //what you see is what you get
#define F_PNUM   0x00f //passage number mask
#define F_TMASK  0x007 //trap number mask

//Trap types

#define T_DOOR   00
#define T_ARROW  01
#define T_SLEEP  02
#define T_BEAR   03
#define T_TELEP  04
#define T_DART   05
#define NTRAPS   6

//Potion types

#define P_CONFUSE   0
#define P_PARALYZE  1
#define P_POISON    2
#define P_STRENGTH  3
#define P_SEEINVIS  4
#define P_HEALING   5
#define P_MFIND     6
#define P_TFIND     7
#define P_RAISE     8
#define P_XHEAL     9
#define P_HASTE     10
#define P_RESTORE   11
#define P_BLIND     12
#define P_NOP       13
#define MAXPOTIONS  14

//Scroll types

#define S_CONFUSE   0
#define S_MAP       1
#define S_HOLD      2
#define S_SLEEP     3
#define S_ARMOR     4
#define S_IDENT     5
#define S_SCARE     6
#define S_GFIND     7
#define S_TELEP     8
#define S_ENCH      9
#define S_CREATE    10
#define S_REMOVE    11
#define S_AGGR      12
#define S_NOP       13
#define S_VORPAL    14
#define MAXSCROLLS  15

//Weapon types

#define MACE        0
#define SWORD       1
#define BOW         2
#define ARROW       3
#define DAGGER      4
#define TWOSWORD    5
#define DART        6
#define CROSSBOW    7
#define BOLT        8
#define SPEAR       9
#define FLAME       10 //fake entry for dragon breath (ick)
#define MAXWEAPONS  10 //this should equal FLAME

//Armor types

#define LEATHER          0
#define RING_MAIL        1
#define STUDDED_LEATHER  2
#define SCALE_MAIL       3
#define CHAIN_MAIL       4
#define SPLINT_MAIL      5
#define BANDED_MAIL      6
#define PLATE_MAIL       7
#define MAXARMORS        8

//Ring types

#define R_PROTECT   0
#define R_ADDSTR    1
#define R_SUSTSTR   2
#define R_SEARCH    3
#define R_SEEINVIS  4
#define R_NOP       5
#define R_AGGR      6
#define R_ADDHIT    7
#define R_ADDDAM    8
#define R_REGEN     9
#define R_DIGEST    10
#define R_TELEPORT  11
#define R_STEALTH   12
#define R_SUSTARM   13
#define MAXRINGS    14

//Rod/Wand/Staff types

#define WS_LIGHT      0
#define WS_HIT        1
#define WS_ELECT      2
#define WS_FIRE       3
#define WS_COLD       4
#define WS_POLYMORPH  5
#define WS_MISSILE    6
#define WS_HASTE_M    7
#define WS_SLOW_M     8
#define WS_DRAIN      9
#define WS_NOP        10
#define WS_TELAWAY    11
#define WS_TELTO      12
#define WS_CANCEL     13
#define MAXSTICKS     14

//Now we define the structures and types

//Help list

struct h_list
{
  char h_ch;
  char *h_desc;
};

//Coordinate data type

typedef struct
{
  shint x;
  shint y;
} coord;

typedef unsigned int str_t;

//Stuff about magic items

struct magic_item
{
  char *mi_name;
  shint mi_prob;
  short mi_worth;
};

struct array
{
  char storage[MAXNAME+1];
};

//Room structure

struct room
{
  coord r_pos;      //Upper left corner
  coord r_max;      //Size of room
  coord r_gold;     //Where the gold is
  int r_goldval;    //How much the gold is worth
  short r_flags;    //Info about the room
  shint r_nexits;   //Number of exits
  coord r_exit[12]; //Where the exits are
};

//Structure describing a fighting being

struct stats
{
  str_t s_str;   //Strength
  long s_exp;    //Experience
  shint s_lvl;   //Level of mastery
  shint s_arm;   //Armor class
  shint s_hpt;   //Hit points
  char *s_dmg;   //String describing damage done
  shint s_maxhp; //Max hit points
};

//Structure for monsters and player

union thing
{
  struct
  {
    union thing *_l_next, *_l_prev; //Next pointer in link
    coord _t_pos;                   //Position
    char _t_turn;                   //If slowed, is it a turn to move
    char _t_type;                   //What it is
    byte _t_disguise;               //What mimic looks like
    byte _t_oldch;                  //Character that was where it was
    coord *_t_dest;                 //Where it is running to
    short _t_flags;                 //State word
    struct stats _t_stats;          //Physical description
    struct room *_t_room;           //Current room for thing
    union thing *_t_pack;           //What the thing is carrying
  } _t;
  struct
  {
    union thing *_l_next, *_l_prev; //Next pointer in link
    shint _o_type;                  //What kind of object it is
    coord _o_pos;                   //Where it lives on the screen
    char *_o_text;                  //What it says if you read it
    char _o_launch;                 //What you need to launch it
    char *_o_damage;                //Damage if used like sword
    char *_o_hurldmg;               //Damage if thrown
    shint _o_count;                 //Count for plural objects
    shint _o_which;                 //Which object of a type it is
    shint _o_hplus;                 //Plusses to hit
    shint _o_dplus;                 //Plusses to damage
    short _o_ac;                    //Armor class
    short _o_flags;                 //Information about objects
    char _o_enemy;                  //If it is enchanted, who it hates
    shint _o_group;                 //Group number for this object
  } _o;
};

typedef union thing THING;

#define l_next      _t._l_next
#define l_prev      _t._l_prev
#define t_pos       _t._t_pos
#define t_turn      _t._t_turn
#define t_type      _t._t_type
#define t_disguise  _t._t_disguise
#define t_oldch     _t._t_oldch
#define t_dest      _t._t_dest
#define t_flags     _t._t_flags
#define t_stats     _t._t_stats
#define t_pack      _t._t_pack
#define t_room      _t._t_room
#define o_type      _o._o_type
#define o_pos       _o._o_pos
#define o_text      _o._o_text
#define o_launch    _o._o_launch
#define o_damage    _o._o_damage
#define o_hurldmg   _o._o_hurldmg
#define o_count     _o._o_count
#define o_which     _o._o_which
#define o_hplus     _o._o_hplus
#define o_dplus     _o._o_dplus
#define o_ac        _o._o_ac
#define o_charges   o_ac
#define o_goldval   o_ac
#define o_flags     _o._o_flags
#define o_group     _o._o_group
#define o_enemy     _o._o_enemy

//Array containing information on all the various types of monsters

struct monster
{
  char *m_name;         //What to call the monster
  shint m_carry;        //Probability of carrying something
  short m_flags;        //Things about the monster
  struct stats m_stats; //Initial stats
};

#define TOPSCORES  10

struct sc_ent
{
  char sc_name[38];
  int sc_rank;
  int sc_gold;
  int sc_fate;
  int sc_level;
};

//External variables

extern THING *cur_armor, *cur_ring[2], *cur_weapon, *lvl_obj, *mlist, player;
extern coord delta, oldpos;
extern struct room *oldrp, passages[], rooms[];
extern struct stats max_stats;
extern struct monster monsters[];
extern struct magic_item p_magic[], r_magic[], s_magic[], things[], ws_magic[];



//Structure and defines for the swint call

struct sw_regs {int ax; int bx; int cx; int dx; int si; int di; int ds; int es;};

#define SW_DOS  0x21
#define SW_KEY  0x16
#define SW_SCR  0x10
#define SW_DSK  0x13

#define LOW(x) ((x)&0xff)
#define HI(x)  ((x)>>8)

#define SETHILO(reg,hi,lo)  (reg)=((hi)<<8)|lo)

extern struct sw_regs *regs;

//processor flags

#define CF  0x001
#define PF  0x004
#define AF  0x010
#define ZF  0x040
#define SF  0x080
#define TF  0x100
#define IF  0x200
#define DF  0x400
#define OF  0x800



//Defines for things used in mach_dep.c
//@(#)extern.h5.1 (Berkeley) 5/11/82

//Don't change the constants, since they are used for sizes in many places in the program.

#define MAXSTR    80 //maximum length of strings
#define MAXLINES  25 //maximum number of screen lines used
#define MAXCOLS   80 //maximum number of screen columns used

//Now all the global variables

extern int maxitems;
extern int maxrow;
extern char *end_sb, *end_mem, *startmem;
extern char *_top, *_base;
extern int LINES, COLS;
extern int is_saved;
extern int scr_type;
extern int reinit;
extern int revno, verno;
extern int is_me;
extern int iguess;
extern int bailout;

extern char *l_menu, *l_name, *l_fruit, *l_score, *l_save, *l_macro, *l_drive;
extern char s_menu[], s_name[], s_fruit[], s_score[], s_save[], s_macro[];
extern char s_drive[], s_screen[];
extern char nullstr[], *it, *tbuf, *you, *no_mem;

extern struct array s_names[], _guesses[];
extern char *s_guess[], *p_guess[], *r_guess[], *ws_guess[];
extern char f_damage[];

extern bool amulet, after, again, askme, door_stop, expert, fastmode, faststate, fight_flush, firstmove, in_shell, jump, noscore, passgo, playing, running, save_msg, saw_amulet, slow_invent, terse, was_trapped, wizard;

extern bool p_know[], r_know[], s_know[], ws_know[];

extern char *a_names[], file_name[], fruit[], *flash, *he_man[], *helpcoms[], *helpobjs[], home[], huh[], macro[], *intense, outbuf[], *p_colors[], *prbuf, *r_stones[], *release, runch, *typeahead, take, *w_names[], whoami[], *ws_made[], *ws_type[];

extern byte *_level, *_flags;

extern int a_chances[], a_class[], count, dnum, food_left, fung_hit, fd, group, hungry_state, inpack, lastscore, level, max_level, mpos, no_command, no_food, no_move, ntraps, purse, quiet, total;

extern long seed, *e_levels;

extern int hit_mul;
extern char *your_na, *kild_by;
extern int goodchk;
extern char *_whoami;
extern int cksum;

#ifdef LOG

extern int captains_log;

#endif LOG



#define C_LEFT    0xcb
#define C_RIGHT   0xcd
#define C_UP      0xc8
#define C_DOWN    0xd0
#define C_HOME    0xc7
#define C_PGUP    0xc9
#define C_END     0xcf
#define C_PGDN    0xd1
#define C_ESCAPE  0x1b
#define C_INS     0xd2
#define C_DEL     0xd3
#define C_F1      0xbb
#define C_F2      0xbc
#define C_F3      0xbd
#define C_F4      0xbe
#define C_F5      0xbf
#define C_F6      0xc0
#define C_F7      0xc1
#define C_F8      0xc2
#define C_F9      0xc3
#define C_F10     0xc4
#define ALT_F9    0xf0



//Cursor motion header for Monochrome display

#define abs(x)              ((x)<0?-(x):(x))
#define NULL                0
#define TRUE                1
#define FALSE               0
#define refresh             stub
#define curscr              NULL
#define stdscr              NULL
#define hw                  NULL
#define BUFSIZ              128
#define wmove(a,b,c)        move(b,c)
#define wclear              clear
#define fputs(a,b)          addstr(a)
#define puts(s)             addstr(s)
#define wrefresh            stub
#define clearok             stub
#define leaveok             stub
#define endwin              wclose
#define touchwin            stub
#define gets                cgets
#define waddstr(w,s)        addstr(s)
#define mvwaddstr(w,a,b,c)  mvaddstr(a,b,c)
#define mvwaddch(w,a,b,c)   mvaddch(a,b,c)
#define getyx(a,b,c)        getxy(&b,&c)
#define getxy               getrc
#define inch()              (0xff&curch())
#define is_color            (scr_type!=7)
#define is_bw               (scr_type==0 || scr_type==2)
#define standend()          set_attr(0)
#define green()             set_attr(1)
#define cyan()              set_attr(2)
#define red()               set_attr(3)
#define magenta()           set_attr(4)
#define brown()             set_attr(5)
#define dgrey()             set_attr(6)
#define lblue()             set_attr(7)
#define lgrey()             set_attr(8)
#define lred()              set_attr(9)
#define lmagenta()          set_attr(10)
#define yellow()            set_attr(11)
#define uline()             set_attr(12)
#define blue()              set_attr(13)
#define standout()          set_attr(14)
#define high()              set_attr(15)
#define bold()              set_attr(16)
#define BX_UL               0
#define BX_UR               1
#define BX_LL               2
#define BX_LR               3
#define BX_VW               4
#define BX_HT               5
#define BX_HB               6
#define BX_SIZE             7

extern int scr_ds, old_page_no, no_check;
extern byte dbl_box[BX_SIZE], sng_box[BX_SIZE], fat_box[BX_SIZE];
