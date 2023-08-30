/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int startwithgaps	     = 1;	 /* 1 means gaps are used by default */
static const unsigned int gappx     = 10;       /* default gap between windows in pixels */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Terminus:size=10", "fontawesome:size=12" };
static const char dmenufont[]       = "Terminus:size=10";
/*
 * Color references,
 *
 * Suckless Defaults:
 * #222222
 * #444444
 * #bbbbbb
 * #eeeeee
 * #005577
 *
 * Meine:
 * #3B77BC
 * #
 * #
 * #
 * #
 * #
 */
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#AB2B28";
static const unsigned int baralpha = OPAQUE; // 0xd0
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* Imports*/
#include <X11/XF86keysym.h>

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "Steam",    NULL,	  NULL,	      0,	    1,		 -1}
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
/* backlight */
static const char *brupcmd[] = { "xbacklight", "-inc", "10", NULL };
static const char *brdowncmd[] = { "xbacklight", "-dec", "10", NULL };
 /* voice control */
static const char *vdowncmd[] = { "pamixer", "--allow-boost", "-d", "3", NULL};
static const char *vupcmd[] = { "pamixer", "--allow-boost", "-i", "3", NULL};

/* Misc functions */
static const char *mutefn[] = {"pamixer", "-t",  NULL};

static const char *musicplay[] = {"", NULL};
static const char *musicprev[] = {"", NULL};
static const char *musicnext[] = {"", NULL};

static const char *redshifton[] = {"redshift", "-P", "-O", "4000", "&&", "notify-send", "test", NULL};
static const char *redshiftoff[] = {"redshift", "-P", "-O", "7000", "&&", "notify-send", "test", NULL};


static const char *scapture[] = {"myscrot", NULL};
static const char *musicplayer[] = {"alacritty", "-e", "ncmpcpp", NULL};
static const char *runfm[] = {"alacritty", "-e", "lf",  NULL};

static const char *rssreader[] = {"alacritty", "-e", "newsboat", NULL};
static const char *abook[] = {"alacritty", "-e", "abook", NULL};
static const char *browser[] = {"brave", NULL};
static const char *mailclient[] = {"alacritty", "-e", "neomutt", NULL};

static const char *suspend[] = {"systemctl", "suspend", NULL};
static const char *hibernate[] = {"systemctl", "hibernate", NULL};
static const char *redwm[] = {"kill" "$(pgrep", "dwm)", NULL};
static const char *slock[] = {"sudo", "slock", NULL};

static const char *procman[] = {"alacritty", "-e", "htop"};

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
        /* Bookmarking */
        { MODKEY,                       XK_z,      spawn,       SHCMD("xdotool type $(grep -v '^#' ~/.local/share/bookmarks | dmenu -i -l 50 | cut -d' ' -f1)") },
        { MODKEY|ShiftMask,             XK_z,      spawn,       SHCMD("alacritty -e $EDITOR ~/.local/share/bookmarks")}, 
	/* Useless for tile. Will be completely removed soon*/
     /* { MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} }, */
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	/* Killed in favor of redshift bindings*/
     /* { MODKEY,                       XK_r,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ShiftMask,             XK_r,      setlayout,      {.v = &layouts[4]} }, */
	{ MODKEY,			XK_r,	   spawn,	   {.v = redshiftoff } },
	{ MODKEY|ShiftMask,		XK_r,	   spawn,	   {.v = redshifton } },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
 	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_g,      setgaps,        {.i = -5 } },
	{ MODKEY,                       XK_g,      setgaps,        {.i = +5 } },
	{ MODKEY|ShiftMask,             XK_minus,  setgaps,        {.i = GAP_RESET } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = GAP_TOGGLE } },
        /* Misc controls  */
       	{ 0, XF86XK_AudioRaiseVolume,    spawn,		{ .v = vupcmd } },
	{ 0, XF86XK_AudioLowerVolume,    spawn,         { .v = vdowncmd } },
	{ 0, XF86XK_AudioMute,		 spawn,		{ .v = mutefn } },

	{ MODKEY,			XK_F5,	spawn,		{ .v = musicplayer } },

	{ 0, XF86XK_AudioPlay,		spawn,		{ .v = musicplay } },
       	{ 0, XF86XK_AudioPrev,		spawn,		{ .v = musicprev } },
	{ 0, XF86XK_AudioNext,		spawn,		{ .v = musicnext } },

	{ MODKEY,			XK_s, spawn,		SHCMD("maim -s | tee ~/Pictures/Screenshots/$(date +%s).png | xclip -selection clipboard -t image/png") },
        { MODKEY,                       XK_u, spawn,            SHCMD("galculator") },
	{ MODKEY,			XK_v, spawn,		{ .v = runfm } },
	{ MODKEY|ShiftMask,		XK_b, spawn,		{ .v = browser} },

	{ MODKEY|ShiftMask,		XK_x, spawn,		{ .v = suspend } },
	{ MODKEY|ShiftMask,		XK_F1, spawn,		{ .v = slock } },
	{ MODKEY|ShiftMask,		XK_z,  spawn,		{ .v = hibernate } },

	{ MODKEY|ShiftMask,		XK_h, spawn,		{ .v = procman } },
	{ MODKEY|ShiftMask,		XK_m, spawn,		{ .v = mailclient } },
	{ MODKEY|ShiftMask,		XK_o, spawn,		{ .v = abook } },
	{ MODKEY|ShiftMask,		XK_y, spawn,		{ .v = rssreader } },

	{ 0, XF86XK_MonBrightnessUp, spawn, {.v = brupcmd} },
	{ 0, XF86XK_MonBrightnessDown, spawn, {.v = brdowncmd} },

	/* Window controls */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

