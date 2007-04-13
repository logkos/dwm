/* © 2006-2007 Anselm R. Garbe <garbeam at gmail dot com>
 * © 2006-2007 Sander van Dijk <a dot h dot vandijk at gmail dot com>
 * See LICENSE file for license details.
 *
 * dynamic window manager is designed like any other X client as well. It is
 * driven through handling X events. In contrast to other X clients, a window
 * manager selects for SubstructureRedirectMask on the root window, to receive
 * events about window (dis-)appearance.  Only one X connection at a time is
 * allowed to select for this event mask.
 *
 * Calls to fetch an X event from the event queue are blocking.  Due reading
 * status text from standard input, a select()-driven main loop has been
 * implemented which selects for reads on the X connection and STDIN_FILENO to
 * handle all data smoothly. The event handlers of dwm are organized in an
 * array which is accessed whenever a new event has been fetched. This allows
 * event dispatching in O(1) time.
 *
 * Each child of the root window is called a client, except windows which have
 * set the override_redirect flag.  Clients are organized in a global
 * doubly-linked client list, the focus history is remembered through a global
 * stack list. Each client contains an array of Bools of the same size as the
 * global tags array to indicate the tags of a client.  For each client dwm
 * creates a small title window, which is resized whenever the (_NET_)WM_NAME
 * properties are updated or the client is moved/resized.
 *
 * Keys and tagging rules are organized as arrays and defined in the config.h
 * file. These arrays are kept static in event.o and tag.o respectively,
 * because no other part of dwm needs access to them.  The current layout is
 * represented by the lt pointer.
 *
 * To understand everything else, start reading main.c:main().
 */

#include "config.h"
#include <X11/Xlib.h>

/* mask shorthands, used in event.c and client.c */
#define BUTTONMASK		(ButtonPressMask | ButtonReleaseMask)

enum { NetSupported, NetWMName, NetLast };		/* EWMH atoms */
enum { WMProtocols, WMDelete, WMState, WMLast };	/* default atoms */
enum { CurNormal, CurResize, CurMove, CurLast };	/* cursor */
enum { ColBorder, ColFG, ColBG, ColLast };		/* color */

typedef struct Client Client;
struct Client {
	char name[256];
	int x, y, w, h;
	int rx, ry, rw, rh; /* revert geometry */
	int basew, baseh, incw, inch, maxw, maxh, minw, minh;
	int minax, maxax, minay, maxay;
	long flags; 
	unsigned int border;
	Bool isbanned, isfixed, ismax, isfloating;
	Bool *tags;
	Client *next;
	Client *prev;
	Client *snext;
	Window win;
};

typedef struct {
	int x, y, w, h;
	unsigned long norm[ColLast];
	unsigned long sel[ColLast];
	Drawable drawable;
	GC gc;
	struct {
		int ascent;
		int descent;
		int height;
		XFontSet set;
		XFontStruct *xfont;
	} font;
} DC; /* draw context */

typedef struct {
	const char *symbol;
	void (*arrange)(void);
} Layout;

extern const char *tags[];		/* all tags */
char stext[256];			/* status text */
int screen, sx, sy, sw, sh;		/* screen geometry */
int wax, way, wah, waw;			/* windowarea geometry */
unsigned int bh, blw;			/* bar height, bar layout label width */
unsigned int ntags, numlockmask;	/* number of tags, dynamic lock mask */
void (*handler[LASTEvent])(XEvent *);	/* event handler */
Atom wmatom[WMLast], netatom[NetLast];
Bool selscreen, *seltag;		/* seltag is array of Bool */
Client *clients, *sel, *stack;		/* global client list and stack */
Cursor cursor[CurLast];
DC dc;					/* global draw context */
Display *dpy;
Layout *lt;
Window root, barwin;

/* client.c */
void attach(Client *c);			/* attaches c to global client list */
void configure(Client *c);		/* send synthetic configure event */
void detach(Client *c);			/* detaches c from global client list */
void focus(Client *c);			/* focus c, c may be NULL */
void killclient(const char *arg);		/* kill c nicely */
void manage(Window w, XWindowAttributes *wa);	/* manage new client */
void resize(Client *c, int x, int y,
		int w, int h, Bool sizehints);	/* resize with given coordinates c*/
void togglefloating(const char *arg);	/* toggles focused client between floating/tiled state */
void updatesizehints(Client *c);		/* update the size hint variables of c */
void updatetitle(Client *c);		/* update the name of c */
void unmanage(Client *c);		/* destroy c */

/* draw.c */
void drawstatus(void);			/* draw the bar */
void drawtext(const char *text, unsigned long col[ColLast]);	/* draw text */
unsigned int textw(const char *text);	/* return the width of text in px*/

/* event.c */
void grabkeys(void);			/* grab all keys defined in config.h */

/* layout.c */
void floating(void);			/* arranges all windows floating */
void focusclient(const char *arg);	/* focuses next(1)/previous(-1) visible client */
void incmasterw(const char *arg);	/* increments the master width with arg's index value */
void incnmaster(const char *arg);	/* increments nmaster with arg's index value */
void initlayouts(void);			/* initialize layout array */
Client *nexttiled(Client *c);		/* returns tiled successor of c */
void restack(void);			/* restores z layers of all clients */
void setlayout(const char *arg);		/* sets layout, -1 toggles */
void togglemax(const char *arg);		/* toggles maximization of floating client */
void zoom(const char *arg);		/* zooms the focused client to master area, arg is ignored */

/* main.c */
void quit(const char *arg);		/* quit dwm nicely */
int xerror(Display *dsply, XErrorEvent *ee);	/* dwm's X error handler */

/* tag.c */
void compileregs(void);			/* initialize regexps of rules defined in config.h */
Bool isvisible(Client *c);		/* returns True if client is visible */
void settags(Client *c, Client *trans);	/* sets tags of c */
void tag(const char *arg);		/* tags c with arg's index */
void toggletag(const char *arg);		/* toggles c tags with arg's index */
void toggleview(const char *arg);	/* toggles the tag with arg's index (in)visible */
void view(const char *arg);		/* views the tag with arg's index */

/* util.c */
void *emallocz(unsigned int size);	/* allocates zero-initialized memory, exits on error */
void eprint(const char *errstr, ...);	/* prints errstr and exits with 1 */
void spawn(const char *arg);		/* forks a new subprocess with arg's cmd */
