/* Public Domain Curses */

#include "pdcsdl.h"

#include <stdlib.h>
#include <string.h>

#ifdef CHTYPE_LONG

# define A(x) ((chtype)x | A_ALTCHARSET)

chtype acs_map[128] =
{
    A(0), A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9),
    A(10), A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18),
    A(19), A(20), A(21), A(22), A(23), A(24), A(25), A(26), A(27),
    A(28), A(29), A(30), A(31), ' ', '!', '"', '#', '$', '%', '&',
    '\'', '(', ')', '*',

# ifdef PDC_WIDE
    0x2192, 0x2190, 0x2191, 0x2193,
# else
    A(0x1a), A(0x1b), A(0x18), A(0x19),
# endif

    '/',

# ifdef PDC_WIDE
    0x2588,
# else
    0xdb,
# endif

    '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
    '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

# ifdef PDC_WIDE
    0x2666, 0x2592,
# else
    A(0x04), 0xb1,
# endif

    'b', 'c', 'd', 'e',

# ifdef PDC_WIDE
    0x00b0, 0x00b1, 0x2591, 0x00a4, 0x2518, 0x2510, 0x250c, 0x2514,
    0x253c, 0x23ba, 0x23bb, 0x2500, 0x23bc, 0x23bd, 0x251c, 0x2524,
    0x2534, 0x252c, 0x2502, 0x2264, 0x2265, 0x03c0, 0x2260, 0x00a3,
    0x00b7,
# else
    0xf8, 0xf1, 0xb0, A(0x0f), 0xd9, 0xbf, 0xda, 0xc0, 0xc5, 0x2d,
    0x2d, 0xc4, 0x2d, 0x5f, 0xc3, 0xb4, 0xc1, 0xc2, 0xb3, 0xf3,
    0xf2, 0xe3, 0xd8, 0x9c, 0xf9,
# endif

    A(127)
};

# undef A

#endif

Uint32 pdc_lastupdate = 0;

#define MAXRECT 200     /* maximum number of rects to queue up before
                           an update is forced; the number was chosen
                           arbitrarily */

static SDL_Rect uprect[MAXRECT];       /* table of rects to update */
static chtype oldch = (chtype)(-1);    /* current attribute */
static int rectcount = 0;              /* index into uprect */
static short foregr = -2, backgr = -2; /* current foreground, background */

/* do the real updates on a delay */

void PDC_update_rects(void)
{
    if (rectcount)
    {
        /* if the maximum number of rects has been reached, we're
           probably better off doing a full screen update */

        if (rectcount == MAXRECT)
            SDL_Flip(pdc_screen);
        else
            SDL_UpdateRects(pdc_screen, rectcount, uprect);

        pdc_lastupdate = SDL_GetTicks();
        rectcount = 0;
    }
}

/* set the font colors to match the chtype's attribute */

static void _set_attr(chtype ch)
{
    ch &= (A_COLOR|A_BOLD|A_BLINK|A_REVERSE);

    if (oldch != ch)
    {
        short newfg, newbg;

        if (SP->mono)
            return;

        PDC_pair_content(PAIR_NUMBER(ch), &newfg, &newbg);

        newfg |= (ch & A_BOLD) ? 8 : 0;
        newbg |= (ch & A_BLINK) ? 8 : 0;

        if (ch & A_REVERSE)
        {
            short tmp = newfg;
            newfg = newbg;
            newbg = tmp;
        }

        if (newfg != foregr)
        {
#ifndef PDC_WIDE
            SDL_SetPalette(pdc_font, SDL_LOGPAL,
                           pdc_color + newfg, pdc_flastc, 1);
#endif
            foregr = newfg;
        }

        if (newbg != backgr)
        {
#ifndef PDC_WIDE
            if (newbg == -1)
                SDL_SetColorKey(pdc_font, SDL_SRCCOLORKEY, 0);
            else
            {
                if (backgr == -1)
                    SDL_SetColorKey(pdc_font, 0, 0);

                SDL_SetPalette(pdc_font, SDL_LOGPAL,
                               pdc_color + newbg, 0, 1);
            }
#endif
            backgr = newbg;
        }

        oldch = ch;
    }
}

/* draw a cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    SDL_Rect src, dest;
    chtype ch;
    int oldrow, oldcol;
#ifdef PDC_WIDE
    Uint16 chstr[2] = {0, 0};
#endif

    PDC_LOG(("PDC_gotoyx() - called: row %d col %d from row %d col %d\n",
             row, col, SP->cursrow, SP->curscol));

    if (SP->mono)
        return;

    oldrow = SP->cursrow;
    oldcol = SP->curscol;

    /* clear the old cursor */

    PDC_transform_line(oldrow, oldcol, 1, curscr->_y[oldrow] + oldcol);

    if (!SP->visibility)
        return;

    /* draw a new cursor by overprinting the existing character in
       reverse, either the full cell (when visibility == 2) or the
       lowest quarter of it (when visibility == 1) */

    ch = curscr->_y[row][col] ^ A_REVERSE;

    _set_attr(ch);

#ifdef CHTYPE_LONG
    if (ch & A_ALTCHARSET && !(ch & 0xff80))
        ch = acs_map[ch & 0x7f];
#endif
    src.h = (SP->visibility == 1) ? pdc_fheight >> 2 : pdc_fheight;
    src.w = pdc_fwidth;

    dest.y = (row + 1) * pdc_fheight - src.h + pdc_yoffset;
    dest.x = col * pdc_fwidth + pdc_xoffset;

#ifdef PDC_WIDE
    chstr[0] = ch & A_CHARTEXT;

    pdc_font = TTF_RenderUNICODE_Solid(pdc_ttffont, chstr, pdc_color[foregr]);
    if (pdc_font)
    {
        dest.h = src.h;
        dest.w = src.w;
        src.x = 0;
        src.y = 0;
        SDL_SetColorKey(pdc_font, 0, 0);
        SDL_SetPalette(pdc_font, SDL_LOGPAL, pdc_color + backgr, 0, 1);
        SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
        SDL_FreeSurface(pdc_font);
        pdc_font = NULL;
    }
#else

    src.x = (ch & 0xff) % 32 * pdc_fwidth;
    src.y = (ch & 0xff) / 32 * pdc_fheight + (pdc_fheight - src.h);

    SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
#endif

    if (oldrow != row || oldcol != col)
    {
        if (rectcount == MAXRECT)
            PDC_update_rects();

        uprect[rectcount++] = dest;
    }
}

/* handle the A_*LINE attributes */

static void _highlight(SDL_Rect *src, SDL_Rect *dest, chtype ch)
{
    short col = SP->line_color;
#ifdef PDC_WIDE
    Uint16 chstr[2] = {'_', 0};
#endif

    if (SP->mono)
        return;

    if (ch & (A_UNDERLINE | A_OVERLINE | A_STRIKEOUT))
    {
#ifdef PDC_WIDE
        if (col == -1)
            col = foregr;

        pdc_font = TTF_RenderUNICODE_Solid(pdc_ttffont, chstr, pdc_color[col]);
        if (pdc_font)
        {
           src->x = 0;
           src->y = 0;

           if (backgr != -1)
               SDL_SetColorKey(pdc_font, SDL_SRCCOLORKEY, 0);

           if( ch & A_UNDERLINE)
                SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
           if( ch & A_OVERLINE)
           {
                dest->y -= pdc_fheight - 1;
                SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
                dest->y += pdc_fheight - 1;
           }
           if( ch & A_STRIKEOUT)
           {
                dest->y -= pdc_fheight / 2;
                SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
                dest->y += pdc_fheight / 2;
           }
           SDL_FreeSurface(pdc_font);
           pdc_font = NULL;
        }
#else
        if (col != -1)
            SDL_SetPalette(pdc_font, SDL_LOGPAL,
                           pdc_color + col, pdc_flastc, 1);

        src->x = '_' % 32 * pdc_fwidth;
        src->y = '_' / 32 * pdc_fheight;

        if (backgr != -1)
            SDL_SetColorKey(pdc_font, SDL_SRCCOLORKEY, 0);

        if( ch & A_UNDERLINE)
            SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
        if( ch & A_OVERLINE)
        {
           dest->y -= pdc_fheight - 1;
           SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
           dest->y += pdc_fheight - 1;
        }
        if( ch & A_STRIKEOUT)
        {
           dest->y -= pdc_fheight / 2;
           SDL_BlitSurface(pdc_font, src, pdc_screen, dest);
           dest->y += pdc_fheight / 2;
        }

        if (backgr != -1)
            SDL_SetColorKey(pdc_font, 0, 0);

        if (col != -1)
            SDL_SetPalette(pdc_font, SDL_LOGPAL,
                           pdc_color + foregr, pdc_flastc, 1);
#endif
    }

    if (ch & (A_LEFTLINE|A_RIGHTLINE))
    {
        if (col == -1)
            col = foregr;

        dest->w = 1;

        if (ch & A_LEFTLINE)
            SDL_FillRect(pdc_screen, dest, pdc_mapped[col]);

        if (ch & A_RIGHTLINE)
        {
            dest->x += pdc_fwidth - 1;
            SDL_FillRect(pdc_screen, dest, pdc_mapped[col]);
            dest->x -= pdc_fwidth - 1;
        }

        dest->w = pdc_fwidth;
    }
}

#ifdef USE_PC_GFX
#define PASSAGE   (0xb1)
#define DOOR      (0xce)
#define FLOOR     (0xfa)
#define PLAYER    (0x01)
#define TRAP      (0x04)
#define STAIRS    (0xf0)
#define GOLD      (0x0f)
#define POTION    (0xad)
#define SCROLL    (0x0d)
#define FOOD      (0x05)
#define STICK     (0xe7)
#define ARMOR     (0x08)
#define AMULET    (0x0c)
#define RING      (0x09)
#define WEAPON    (0x18)
#define VWALL     (0xba)
#define HWALL     (0xcd)
#define ULWALL    (0xc9)
#define URWALL    (0xbb)
#define LLWALL    (0xc8)
#define LRWALL    (0xbc)

unsigned int GetColor(int chr, int attr)
{
    //if it is inside a room
    if (attr == 0x07|| attr == 0) switch (chr)
    {
    case DOOR:
    case VWALL: case HWALL:
    case ULWALL: case URWALL: case LLWALL: case LRWALL:
        return 0x06; //brown
    case FLOOR:
        return 0x0a; //light green
    case STAIRS:
        return 0xa0; //black on light green
    case TRAP:
        return 0x05; //magenta
    case GOLD:
    case PLAYER:
        return 0x0e; //yellow
    case POTION:
    case SCROLL:
    case STICK:
    case ARMOR:
    case AMULET:
    case RING:
    case WEAPON:
        return 0x09; //light blue
    case FOOD:
        return 0x04; //red
    }
    //if inside a passage or a maze
    else if (attr == 0x70) switch (chr)
    {
    case FOOD:
        return 0x74; //red on grey
    case GOLD: case PLAYER:
        return 0x7e; //yellow on grey
    case POTION: case SCROLL: case STICK: case ARMOR: case AMULET: case RING: case WEAPON:
        return 0x71; //blue on grey
    }

    return attr;
}
#endif

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    SDL_Rect src, dest, lastrect;
    int j;
#ifdef PDC_WIDE
    Uint16 chstr[2] = {0, 0};
#endif

    PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

    if (rectcount == MAXRECT)
        PDC_update_rects();

    src.h = pdc_fheight;
    src.w = pdc_fwidth;

    dest.y = pdc_fheight * lineno + pdc_yoffset;
    dest.x = pdc_fwidth * x + pdc_xoffset;
    dest.h = pdc_fheight;
    dest.w = pdc_fwidth * len;

    /* if the previous rect was just above this one, with the same width
       and horizontal position, then merge the new one with it instead
       of adding a new entry */

    if (rectcount)
        lastrect = uprect[rectcount - 1];

    if (rectcount && lastrect.x == dest.x && lastrect.w == dest.w)
    {
        if (lastrect.y + lastrect.h == dest.y)
            uprect[rectcount - 1].h = lastrect.h + pdc_fheight;
        else
            if (lastrect.y != dest.y)
                uprect[rectcount++] = dest;
    }
    else
        uprect[rectcount++] = dest;

    dest.w = pdc_fwidth;

#ifdef PDC_WIDE
    src.x = 0;
    src.y = 0;
#endif

    for (j = 0; j < len; j++)
    {
        chtype ch = srcp[j];

#ifdef USE_PC_GFX
        //mdk:hack
        if (lineno >= 23) {
            ch += ((chtype)0x0e << PDC_COLOR_SHIFT);
        }
        if (lineno > 0 && lineno < 23) {
            int color = (ch >> PDC_COLOR_SHIFT) & 0xff;
            int new_color = GetColor(ch&0xff, color);
            ch = (ch & ~A_COLOR) | ((chtype)new_color << PDC_COLOR_SHIFT);
        }
#endif

        _set_attr(ch);
#ifdef CHTYPE_LONG
        if (ch & A_ALTCHARSET && !(ch & 0xff80))
            ch = (ch & (A_ATTRIBUTES ^ A_ALTCHARSET)) | acs_map[ch & 0x7f];
#endif
        if (backgr == -1)
            SDL_LowerBlit(pdc_tileback, &dest, pdc_screen, &dest);

#ifdef PDC_WIDE
        chstr[0] = ch & A_CHARTEXT;
        pdc_font = TTF_RenderUNICODE_Solid(pdc_ttffont, chstr,
                                           pdc_color[foregr]);

        if (pdc_font)
        {
            if (backgr != -1)
            {
                SDL_SetColorKey(pdc_font, 0, 0);
                SDL_SetPalette(pdc_font, SDL_LOGPAL,
                               pdc_color + backgr, 0, 1);
            }
            SDL_BlitSurface(pdc_font, &src, pdc_screen, &dest);
            SDL_FreeSurface(pdc_font);
            pdc_font = NULL;
        }
#else
        src.x = (ch & 0xff) % 32 * pdc_fwidth;
        src.y = (ch & 0xff) / 32 * pdc_fheight;

        SDL_LowerBlit(pdc_font, &src, pdc_screen, &dest);
#endif

        if (ch & (A_UNDERLINE|A_LEFTLINE|A_RIGHTLINE|A_OVERLINE|A_STRIKEOUT))
            _highlight(&src, &dest, ch);

        dest.x += pdc_fwidth;
    }
}
