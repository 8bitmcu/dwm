/* configuration file location, subdirectory of XDG_CONFIG_HOME */
static const char *dwm_cfg = "/dwm/dwm.toml";

static const char *colors[SchemeLast][ColLast] = {
	[SchemeNorm] = { "#bbbbbb", "#222222", "#444444" },
	[SchemeSel] = { "#eeeeee", "#005577", "#005577" },
	[SchemeStatus] = { "#bbbbbb", "#222222", "#000000" },
	[SchemeTagsSel] = { "#eeeeee", "#005577", "#000000" },
	[SchemeTagsNorm] = { "#bbbbbb", "#222222", "#000000" },
	[SchemeLayoutNorm] = { "#bbbbbb", "#222222", "#000000" },
	[SchemeInfoSel] = { "#eeeeee", "#005577", "#000000" },
	[SchemeInfoNorm] = { "#bbbbbb", "#222222", "#000000" },
	[SchemeSystrayNorm] = { "#000000", "#222222", "#000000" }
};
static int borderpx = 2, snap = 32, showbar = 1, topbar = 1;
static float mfact = 0.55;
static int nmaster = 1, resizehints = 1, lockfullscreen = 1;
static const char **tags;
static const Rule **rules;
static const Layout **layouts;
static const Key **keys;
static const Button **buttons;
static int n_tags = 0, n_rules = 0, n_layouts = 0, n_keys = 0, n_buttons = 0;

static unsigned int alphas[SchemeLast][3] = {
	[SchemeNorm] = { 255, 255, 255 },
	[SchemeSel] = { 255, 255, 255 },
	[SchemeStatus] = { 255, 255, 255 },
	[SchemeTagsSel] = { 255, 255, 255 },
	[SchemeTagsNorm] = { 255, 255, 255 },
	[SchemeLayoutNorm] = { 255, 255, 255 },
	[SchemeInfoSel] = { 255, 255, 255 },
	[SchemeInfoNorm] = { 255, 255, 255 },
	[SchemeSystrayNorm] = { 255, 255, 255 }
}; /* dwm-alpha */
static int systraypinning = 0, systrayonleft = 0, systrayspacing = 0, systrayiconpad = 0, 
           systraypinningfailfirst = 0, showsystray = 0, systrayiconsize = 0; /* dwm-systray */
static unsigned int gappih = 0, gappiv = 0, gappoh = 0, gappov = 0, smartgaps = 0, force_vsplit = 1; /* dwm-vanitygaps */
static int enableswallow = 0, swallowfloating = 0; /* dwm-swallow */
static int enablehidevacant = 0; /* dwm-hide_vacant_tags */
static Inset default_inset = { .x = 0, .y = 0, .w = 0, .h = 0 }; /* dwm-insets */
static const char *autostart = "", *autostart_blocking = ""; /* dwm-autostart */
static int barheight = 0; /* dwm-bar-height */
static int vertpad = 0, sidepad = 0; /* dwm-barpadding */
static int vertpadbar = 0, horizpadbar = 2; /* dwm-statuspadding */
static int showtitle = 1; /* dwm-notitle */
static int centretitle = 0; /* dwm-centretitle */
static int winicon = 0, iconsize = 0, iconspacing = 0, iconpad = 0; /* dwm-winicon */
static int ulinepad = 0, ulinestroke = 0, ulinevoffset = 0, ulineall = 0, showuline = 0; /* dwm-underlinetags */
static const char *statusbar; /* dwm-statuscmd */
static const char *font; /* dwm-pango */
static int pertag_showbars = 0, pertag_nmaster = 0, pertag_mfact = 0, pertag_layout = 0; /* dwm-pertag */
static int showlayout = 1;

#include "vanitygaps.c"

static int
cfg_read_str(toml_table_t *conf, char *key, const char **dest)
{
	toml_datum_t d = toml_string_in(conf, key);
	if (!d.ok)
		return 0;
	*dest = d.u.s;
	return 1;
}

static void
cfg_read_float(toml_table_t *conf, char *key, float *dest)
{
	toml_datum_t d = toml_double_in(conf, key);
	if (!d.ok)
		return;
	*dest = d.u.d;
}

static int
cfg_read_int(toml_table_t *conf, char *key, int *dest)
{
	toml_datum_t d = toml_int_in(conf, key);
	if (!d.ok)
		return 0;
  *dest = d.u.i;
	return 1;
}

int
cfg_read_strarr(toml_table_t *conf, char *key, const char ***dest, int ins_null)
{
	toml_array_t *arr = toml_array_in(conf, key);
	if (!arr)
		return 0;
	int size = toml_array_nelem(arr);
	const char **dst = ecalloc(size + (ins_null ? 1 : 0), sizeof(char *));
	int i = 0;
	for (;; i++) {
		toml_datum_t d = toml_string_at(arr, i);
		if (!d.ok)
			break;
		dst[i] = d.u.s;
	}
	if (ins_null)
		dst[i] = NULL;
	*dest = dst;
	return size;
}

void
parse_functionargs(toml_table_t *tbl, void (**func)(const Arg *arg), const Arg *arg)
{
	const char *field;

	cfg_read_str(tbl, "function", &field);
	if (!strcmp(field, "spawn")) {
		*func = spawn;
		const char **v;
		cfg_read_strarr(tbl, "argument", &v, 1);
		((Arg *)arg)->v = v;
	} else if (!strcmp(field, "togglebar"))
		*func = togglebar;
	else if (!strcmp(field, "focusstack")) {
		*func = focusstack;
		cfg_read_int(tbl, "argument", (int *)&arg->i);
	} else if (!strcmp(field, "incnmaster")) {
		*func = incnmaster;
		cfg_read_int(tbl, "argument", (int *)&arg->i);
	} else if (!strcmp(field, "setmfact")) {
		*func = setmfact;
		cfg_read_float(tbl, "argument", (float *)&arg->f);
	} else if (!strcmp(field, "zoom"))
		*func = zoom;
	else if (!strcmp(field, "view")) {
		*func = view;
		cfg_read_int(tbl, "argument", (int *)&arg->i);
	} else if (!strcmp(field, "killclient"))
		*func = killclient;
	else if (!strcmp(field, "setlayout")) {
		*func = setlayout;
		int i_field = -1;
		cfg_read_int(tbl, "argument", &i_field);
		if (i_field != -1) {
			const Layout ***v = (const Layout ***)&arg->v;
			*v = &layouts[i_field];
		} else
			arg = NULL;
	} else if (!strcmp(field, "togglefloating"))
		*func = togglefloating;
	else if (!strcmp(field, "tag")) {
		*func = tag;
		cfg_read_int(tbl, "argument", (int *)&arg->i);
	} else if (!strcmp(field, "focusmon")) {
		*func = focusmon;
		cfg_read_int(tbl, "argument", (int *)&arg->i);
	} else if (!strcmp(field, "tagmon")) {
		*func = tagmon;
		cfg_read_int(tbl, "argument", (int *)&arg->i);
	} else if (!strcmp(field, "sigstatusbar")) {
		*func = sigstatusbar;
		cfg_read_int(tbl, "argument", (int *)&arg->i);
	} else if (!strcmp(field, "quit"))
		*func = quit;
	else if (!strcmp(field, "movemouse"))
		*func = movemouse;
	else if (!strcmp(field, "resizemouse"))
		*func = resizemouse;
	else if (!strcmp(field, "toggleview"))
		*func = toggleview;
	else if (!strcmp(field, "toggletag"))
		*func = toggletag;
	else if (!strcmp(field, "dragmfact"))
		*func = dragmfact;
	else if (!strcmp(field, "dragcfact"))
		*func = dragcfact;
	else if (!strcmp(field, "dragfact"))
		*func = dragfact;
	else if (!strcmp(field, "resizeorfacts"))
		*func = resizeorfacts;
	free((char *)field);
}

int
parse_modifier(toml_table_t *tbl, char *field)
{
	const char **mods;
	int n_mods = cfg_read_strarr(tbl, field, &mods, 0);
	int mod = 0;
	for (int i = 0; i < n_mods; i++) {
		if (!strcmp("super", mods[i]))
			mod |= Mod4Mask;
		else if (!strcmp("shift", mods[i]))
			mod |= ShiftMask;
		else if (!strcmp("ctrl", mods[i]))
			mod |= ControlMask;
		else if (!strcmp("alt", mods[i]))
			mod |= Mod1Mask;
		free((void *)mods[i]);
	}
	free(mods);
	return mod;
}

KeySym
parse_keysym(toml_table_t *tbl, char *f)
{
	const char *field;
	cfg_read_str(tbl, f, &field);
	KeySym val = XStringToKeysym(field);
	free((void *)field);
	return val;
}

void
parse_tag(toml_table_t *tbl, char *field, int *dest)
{
	int val;
	cfg_read_int(tbl, field, (int *)&val);
	val = 1 << val;
	dest = &val;
}

void
load_defaults(char *error)
{
	fprintf(stderr, error);
	font = strdup("monospace 10");
	statusbar = strdup("dwmblocks");
	autostart = strdup("");
	autostart_blocking = strdup("");

	n_layouts = 1;
	layouts = ecalloc(n_layouts, sizeof(Layout *));
	Layout *l = ecalloc(1, sizeof(Layout));
	l->symbol = "[]=";
	l->arrange = tile;
	layouts[0] = l;

	n_tags = 1;
	tags = ecalloc(1, sizeof(char *));
	tags[0] = strdup("1");
}

void
read_cfgfile()
{
	const char *config_file = strcat(getenv("XDG_CONFIG_HOME"), dwm_cfg);
	FILE *fp = fopen(config_file, "r");
	if (fp) {
		char errbuf[200];
		toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
		fclose(fp);
		if (conf) {
			cfg_read_int(conf, "borderpx", &borderpx);
			cfg_read_int(conf, "snap", &snap);
			cfg_read_int(conf, "showbar", &showbar);
			cfg_read_int(conf, "topbar", &topbar);
			cfg_read_int(conf, "gappih", (int *) &gappih);
			cfg_read_int(conf, "gappiv", (int *) &gappiv);
			cfg_read_int(conf, "gappoh", (int *) &gappoh);
			cfg_read_int(conf, "gappov", (int *) &gappov);
			cfg_read_int(conf, "smartgaps", (int *) &smartgaps);
			cfg_read_int(conf, "force_vsplit", (int *) &force_vsplit);
			cfg_read_int(conf, "nmaster", &nmaster);
			cfg_read_int(conf, "resizehints", &resizehints);
			cfg_read_int(conf, "lockfullscreen", &lockfullscreen);
			cfg_read_int(conf, "showlayout", &showlayout);
			cfg_read_int(conf, "showsystray", &showsystray);
			cfg_read_int(conf, "systraypinning", &systraypinning);
			cfg_read_int(conf, "systrayonleft", &systrayonleft);
			cfg_read_int(conf, "systrayspacing", &systrayspacing);
			cfg_read_int(conf, "systraypinningfailfirst", &systraypinningfailfirst);
			cfg_read_int(conf, "systrayiconsize", &systrayiconsize);
			cfg_read_int(conf, "systrayiconpad", &systrayiconpad);
			cfg_read_int(conf, "enableswallow", &enableswallow);
			cfg_read_int(conf, "swallowfloating", &swallowfloating);
			cfg_read_int(conf, "enablehidevacant", &enablehidevacant);
			cfg_read_int(conf, "barheight", &barheight);
			cfg_read_int(conf, "vertpad", &vertpad);
			cfg_read_int(conf, "sidepad", &sidepad);
			cfg_read_int(conf, "vertpadbar", &vertpadbar);
			cfg_read_int(conf, "horizpadbar", &horizpadbar);
			cfg_read_int(conf, "showtitle", &showtitle);
			cfg_read_int(conf, "centretitle", &centretitle);
			cfg_read_int(conf, "winicon", &winicon);
			cfg_read_int(conf, "iconsize", &iconsize);
			cfg_read_int(conf, "iconpad", &iconpad);
			cfg_read_int(conf, "iconspacing", &iconspacing);
			cfg_read_int(conf, "showuline", &showuline);
			cfg_read_int(conf, "ulinepad", &ulinepad);
			cfg_read_int(conf, "ulinestroke", &ulinestroke);
			cfg_read_int(conf, "ulinevoffset", &ulinevoffset);
			cfg_read_int(conf, "ulineall", &ulineall);
			cfg_read_int(conf, "pertag_showbars", &pertag_showbars);
			cfg_read_int(conf, "pertag_nmaster", &pertag_nmaster);
			cfg_read_int(conf, "pertag_mfact", &pertag_mfact);
			cfg_read_int(conf, "pertag_layout", &pertag_layout);
			if (!cfg_read_str(conf, "font", &font))
				font = strdup("monospace 10");
			if (!cfg_read_str(conf, "statusbar", &statusbar))
				statusbar = strdup("dwmblocks");
			if (!cfg_read_str(conf, "autostart", &autostart))
				autostart = strdup("");
			if (!cfg_read_str(conf, "autostart_blocking", &autostart_blocking))
				autostart_blocking = strdup("");
			cfg_read_float(conf, "mfact", &mfact);
			toml_table_t *tbl = toml_table_in(conf, "default_inset");
			if(tbl) {
				cfg_read_int(tbl, "x", (int *) &default_inset.x);
				cfg_read_int(tbl, "y", (int *) &default_inset.y);
				cfg_read_int(tbl, "w", (int *) &default_inset.w);
				cfg_read_int(tbl, "h", (int *) &default_inset.h);
			}
			n_tags = cfg_read_strarr(conf, "tags", &tags, 0);
			if (!n_tags) {
				n_tags = 1;
				tags = ecalloc(1, sizeof(char *));
				tags[0] = strdup("1");
			}

			const char *statusbar_colors[ColLast] = { "#bbbbbb", "#222222", "#444444" };
			unsigned int statusbar_alphas[3] = { 255, 255, 255 };
			toml_table_t *ctbl = toml_table_in(conf, "StatusBar");
			if(ctbl) {
				cfg_read_str(ctbl, "ColFg", &statusbar_colors[ColFg]);
				cfg_read_str(ctbl, "ColBg", &statusbar_colors[ColBg]);
				cfg_read_str(ctbl, "Border", &statusbar_colors[ColBorder]);
				cfg_read_int(ctbl, "FgAlpha", (int *) &statusbar_alphas[0]);
				cfg_read_int(ctbl, "BgAlpha", (int *) &statusbar_alphas[1]);
				cfg_read_int(ctbl, "BorderAlpha", (int *) &statusbar_alphas[2]);
			}

			/* color table */
			for (int j = 0; j < LENGTH(colorsn); j++) {
				toml_table_t *tbl = toml_table_in(conf, colorsn[j]);
				if(!tbl) {
					colors[j][ColFg] = statusbar_colors[ColFg];
					colors[j][ColBg] = statusbar_colors[ColBg];
					colors[j][ColBorder] = statusbar_colors[ColBorder];
					alphas[j][0] = statusbar_alphas[0];
					alphas[j][1] = statusbar_alphas[1];
					alphas[j][2] = statusbar_alphas[2];
					continue;
				}
				if (!cfg_read_str(tbl, "ColFg", &colors[j][ColFg]))
					colors[j][ColFg] = statusbar_colors[ColFg];
				if (!cfg_read_str(tbl, "ColBg", &colors[j][ColBg]))
					colors[j][ColBg] = statusbar_colors[ColBg];
				if (!cfg_read_str(tbl, "Border", &colors[j][ColBorder]))
					colors[j][ColBorder] = statusbar_colors[ColBorder];
				if (!cfg_read_int(tbl, "FgAlpha", (int *) &alphas[j][0]))
					alphas[j][0] = statusbar_alphas[0];
				if (!cfg_read_int(tbl, "BgAlpha", (int *) &alphas[j][1]))
					alphas[j][1] = statusbar_alphas[1];
				if (!cfg_read_int(tbl, "BorderAlpha", (int *) &alphas[j][2]))
					alphas[j][2] = statusbar_alphas[2];
			}
			/* rules */
			toml_array_t *d = toml_array_in(conf, "rules");
			if (d) {
				n_rules = toml_array_nelem(d);
				rules = ecalloc(n_rules, sizeof(Rule *));
				for (int i = 0; i < n_rules; i++) {
					toml_table_t *tbl = toml_table_at(d, i);
					if (!tbl)
						continue;
					Rule *r = ecalloc(1, sizeof(Rule));
					cfg_read_str(tbl, "class", &r->class);
					cfg_read_str(tbl, "instance", &r->instance);
					cfg_read_str(tbl, "title", &r->title);
					parse_tag(tbl, "tag", (int *) &r->tags);
					cfg_read_int(tbl, "isfloating", &r->isfloating);
					cfg_read_int(tbl, "iscentered", &r->iscentered);
					cfg_read_int(tbl, "isterminal", &r->isterminal);
					cfg_read_int(tbl, "issticky", &r->issticky);
					cfg_read_int(tbl, "noswallow", &r->noswallow);
					cfg_read_int(tbl, "monitor", &r->monitor);
					rules[i] = r;
				}
			}
			/* layouts */
			d = toml_array_in(conf, "layouts");
			if (d) {
				n_layouts = toml_array_nelem(d);
				layouts = ecalloc(n_layouts, sizeof(Layout *));
				for (int i = 0; i < n_layouts; i++) {
					const char *field;
					toml_table_t *tbl = toml_table_at(d, i);
					if (!tbl)
						continue;
					Layout *l = ecalloc(1, sizeof(Layout));
					cfg_read_str(tbl, "symbol", &l->symbol);
					cfg_read_str(tbl, "arrange", &field);
					if (!strcmp(field, "tile"))
						l->arrange = tile;
					else if (!strcmp(field, "monocle"))
						l->arrange = monocle;
					else if (!strcmp(field, "spiral"))
						l->arrange = spiral;
					else if (!strcmp(field, "dwindle"))
						l->arrange = dwindle;
					else if (!strcmp(field, "deck"))
						l->arrange = deck;
					else if (!strcmp(field, "bstack"))
						l->arrange = bstack;
					else if (!strcmp(field, "bstackhoriz"))
						l->arrange = bstackhoriz;
					else if (!strcmp(field, "grid"))
						l->arrange = grid;
					else if (!strcmp(field, "nrowgrid"))
						l->arrange = nrowgrid;
					else if (!strcmp(field, "horizgrid"))
						l->arrange = horizgrid;
					else if (!strcmp(field, "gaplessgrid"))
						l->arrange = gaplessgrid;
					else if (!strcmp(field, "centeredmaster"))
						l->arrange = centeredmaster;
					else if (!strcmp(field, "centeredfloatingmaster"))
						l->arrange = centeredfloatingmaster;
					else if (!strcmp(field, "nrowgrid"))
						l->arrange = nrowgrid;
					else
					l->arrange = NULL;
					layouts[i] = l;
					free((void *)field);
				}
			} else {
				n_layouts = 1;
				layouts = ecalloc(n_layouts, sizeof(Layout *));
				Layout *l = ecalloc(1, sizeof(Layout));
				l->symbol = "[]=";
				l->arrange = tile;
				layouts[0] = l;
			}
			/* buttons */
			d = toml_array_in(conf, "buttons");
			if (d) {
				n_buttons = toml_array_nelem(d);
				buttons = ecalloc(n_buttons, sizeof(Button *));
				for (int i = 0; i < n_buttons; i++) {
					toml_table_t *tbl = toml_table_at(d, i);
					if (!tbl)
						continue;
					// TODO: should be Button *
					Button *b = ecalloc(1, sizeof(Button));
					const char *field;
					cfg_read_str(tbl, "click", &field);
					if (!strcmp(field, "ClkLtSymbol"))
						b->click = ClkLtSymbol;
					else if (!strcmp(field, "ClkWinTitle"))
						b->click = ClkWinTitle;
					else if (!strcmp(field, "ClkStatusText"))
						b->click = ClkStatusText;
					else if (!strcmp(field, "ClkClientWin"))
						b->click = ClkClientWin;
					else if (!strcmp(field, "ClkTagBar"))
						b->click = ClkTagBar;
					else if (!strcmp(field, "ClkRootWin"))
						b->click = ClkRootWin;
					free((void *)field);
					if (cfg_read_str(tbl, "mask", &field)) {
						if (!strcmp(field, "super"))
							b->mask = Mod4Mask;
						else if (!strcmp(field, "shift"))
							b->mask = ShiftMask;
						else if (!strcmp(field, "ctrl"))
							b->mask = ControlMask;
						else if (!strcmp(field, "alt"))
							b->mask = Mod1Mask;
						free((void *)field);
					}
					cfg_read_str(tbl, "button", &field);
					if (!strcmp(field, "button1"))
						b->button = Button1;
					else if (!strcmp(field, "button2"))
						b->button = Button2;
					else if (!strcmp(field, "button3"))
						b->button = Button3;
					parse_functionargs(tbl, &b->func, &b->arg);
					free((void *)field);
					buttons[i] = b;
				}
			}
			toml_free(conf);
		} else {
			load_defaults(errbuf);
		}
	} else {
		load_defaults("could not open config");
	}
}


void
setlayoutex(const Arg *arg)
{
	setlayout(&((Arg) { .v = &layouts[arg->i] }));
}

void
viewex(const Arg *arg)
{
	view(&((Arg) { .ui = 1 << arg->ui }));
}

void
viewall(const Arg *arg)
{
	view(&((Arg){.ui = ~0}));
}

void
toggleviewex(const Arg *arg)
{
	toggleview(&((Arg) { .ui = 1 << arg->ui }));
}

void
tagex(const Arg *arg)
{
	tag(&((Arg) { .ui = 1 << arg->ui }));
}

void
toggletagex(const Arg *arg)
{
	toggletag(&((Arg) { .ui = 1 << arg->ui }));
}

void
tagall(const Arg *arg)
{
	tag(&((Arg){.ui = ~0}));
}

void
focusstack_dec(const Arg *arg)
{
	focusstack(&((Arg) { .i = INC(-1) }));
}

void
focusstack_inc(const Arg *arg)
{
	focusstack(&((Arg) { .i = INC(+1) }));
}

void
focusstack_prev(const Arg *arg)
{
	focusstack(&((Arg) { .i = PREVSEL }));
}

void
pushstack_dec(const Arg *arg)
{
	pushstack(&((Arg) { .i = INC(-1) }));
}

void
pushstack_inc(const Arg *arg)
{
	pushstack(&((Arg) { .i = INC(+1) }));
}

void
pushstack_prev(const Arg *arg)
{
	pushstack(&((Arg) { .i = PREVSEL }));
}

void
updateinset_x(const Arg *arg)
{
  default_inset.x = arg->i;
	updateinset(&((Arg) { .v = (void *) &default_inset }));
}

void
updateinset_y(const Arg *arg)
{
  default_inset.y = arg->i;
	updateinset(&((Arg) { .v = (void *) &default_inset }));
}

void
updateinset_w(const Arg *arg)
{
  default_inset.w = arg->i;
	updateinset(&((Arg) { .v = (void *) &default_inset }));
}

void
updateinset_h(const Arg *arg)
{
  default_inset.h = arg->i;
	updateinset(&((Arg) { .v = (void *) &default_inset }));
}


/* signal definitions */
/* signum must be greater than 0 */
/* trigger signals using `xsetroot -name "fsignal:<signame> [<type> <value>]"` */
static Signal signals[] = {
	/* signum             function */
	{ "defaultgaps",      defaultgaps },
	{ "focusstack",       focusstack },
	{ "focusstack_dec",   focusstack_dec },
	{ "focusstack_inc",   focusstack_inc },
	{ "focusstack_prev",  focusstack_prev },
	{ "pushstack",        pushstack },
	{ "pushstack_dec",    pushstack_dec },
	{ "pushstack_inc",    pushstack_inc },
	{ "pushstack_prev",   pushstack_prev },
	{ "shiftview",        shiftview },
	{ "setmfact",         setmfact },
	{ "setcfact",         setcfact },
	{ "togglebar",        togglebar },
	{ "incnmaster",       incnmaster },
	{ "incrgaps",         incrgaps },
	{ "incrigaps",        incrigaps },
	{ "incrogaps",        incrogaps },
	{ "incrivgaps",       incrivgaps },
	{ "incrihgaps",       incrihgaps },
	{ "incrovgaps",       incrovgaps },
	{ "incrohgaps",       incrohgaps },
	{ "togglefloating",   togglefloating },
	{ "focusmon",         focusmon },
	{ "tagmon",           tagmon },
	{ "zoom",             zoom },
	{ "view",             view },
	{ "viewall",          viewall },
	{ "viewex",           viewex },
	{ "toggleview",       view },
	{ "toggleviewex",     toggleviewex },
	{ "tag",              tag },
	{ "tagall",           tagall },
	{ "tagex",            tagex },
	{ "togglegaps",       togglegaps },
	{ "toggletag",        tag },
	{ "togglesticky",     togglesticky },
	{ "togglefullscreen", togglefullscreen },
	{ "toggletagex",      toggletagex },
	{ "killclient",       killclient },
	{ "quit",             quit },
	{ "setlayout",        setlayout },
	{ "setlayoutex",      setlayoutex },
	{ "updateinset_x",    updateinset_x },
	{ "updateinset_y",    updateinset_y },
	{ "updateinset_w",    updateinset_w },
	{ "updateinset_h",    updateinset_h }
};

