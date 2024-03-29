/* configuration file location, subdirectory of XDG_CONFIG_HOME */
static const char *dwm_cfg = "/dwm/dwm.toml";

static int borderpx, snap, showbar, topbar;
static const char *colors[SchemeLast][ColLast];
static float mfact;
static int nmaster, resizehints, lockfullscreen;
static const char **fonts, **tags;
static const Rule **rules;
static const Layout **layouts;
static const Key **keys;
static const Button **buttons;
static int n_fonts, n_tags, n_rules, n_layouts, n_keys, n_buttons;

static void
cfg_read_str(toml_table_t *conf, char *key, const char **dest)
{
	toml_datum_t d = toml_string_in(conf, key);
	if (!d.ok)
		return;
	*dest = strdup(d.u.s);
	free(d.u.s);
}

static void
cfg_read_float(toml_table_t *conf, char *key, float *dest)
{
	toml_datum_t d = toml_double_in(conf, key);
	if (!d.ok)
		return;
	*dest = d.u.d;
}

static void
cfg_read_int(toml_table_t *conf, char *key, int *dest)
{
	toml_datum_t d = toml_int_in(conf, key);
	if (!d.ok)
		return;
	*dest = d.u.i;
}

int
cfg_read_strarr(toml_table_t *conf, char *key, const char ***dest)
{
	toml_array_t *arr = toml_array_in(conf, key);
	int size = toml_array_nelem(arr);
	const char **dst = ecalloc(size+1, sizeof(char *));
	int i = 0;
	for (;; i++) {
		toml_datum_t d = toml_string_at(arr, i);
		if (!d.ok)
			break;
		dst[i] = strdup(d.u.s);
		free(d.u.s);
	}
	dst[i] = NULL;
	*dest = dst;
	return size;
}

void
parse_functionargs(toml_table_t *tbl, void (**func)(const Arg *arg), const Arg *arg)
{
	const char *field = ecalloc(128, sizeof(char));
	cfg_read_str(tbl, "function", &field);
	if (!strcmp(field, "spawn")) {
		*func = spawn;
		cfg_read_strarr(tbl, "argument", (const char ***) &arg->v);
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
    if(i_field != -1) {
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
}

int
parse_modifier(toml_table_t *tbl, char *field)
{
	const char **mods;
	int n_mods = cfg_read_strarr(tbl, field, &mods);
	int mod = 0;
	for (int i = 0; i < n_mods; i++)
		if (!strcmp("super", mods[i]))
			mod |= Mod4Mask;
		else if (!strcmp("shift", mods[i]))
			mod |= ShiftMask;
		else if (!strcmp("ctrl", mods[i]))
			mod |= ControlMask;
		else if (!strcmp("alt", mods[i]))
			mod |= Mod1Mask;
	return mod;
}

KeySym
parse_keysym(toml_table_t *tbl, char *f)
{
	const char *field = ecalloc(128, sizeof(char));
	cfg_read_str(tbl, f, &field);
	return XStringToKeysym(field);
}

void
parse_tag(toml_table_t *tbl, char *field, int *dest)
{
	int val;
	cfg_read_int(tbl, field, (int *)&val);
	*dest = 1 << val;
}

void
read_cfgfile()
{
	const char *config_file = strcat(getenv("XDG_CONFIG_HOME"), dwm_cfg);
	FILE *fp = fopen(config_file, "r");
	const char *field = ecalloc(128, sizeof(char));
	if (fp) {
		char errbuf[200];
		toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
		fclose(fp);
		if (conf) {
			cfg_read_int(conf, "borderpx", &borderpx);
			cfg_read_int(conf, "snap", &snap);
			cfg_read_int(conf, "showbar", &showbar);
			cfg_read_int(conf, "topbar", &topbar);
			cfg_read_int(conf, "nmaster", &nmaster);
			cfg_read_int(conf, "resizehints", &resizehints);
			cfg_read_int(conf, "lockfullscreen", &lockfullscreen);
			cfg_read_float(conf, "mfact", &mfact);
			n_fonts = cfg_read_strarr(conf, "fonts", &fonts);
			n_tags = cfg_read_strarr(conf, "tags", &tags);
			/* color table */
			for (int j = 0; j < LENGTH(colorsn); j++) {
				toml_table_t *tbl = toml_table_in(conf, colorsn[j]);
				cfg_read_str(tbl, "ColFg", &colors[j][ColFg]);
				cfg_read_str(tbl, "ColBg", &colors[j][ColBg]);
				cfg_read_str(tbl, "Border", &colors[j][ColBorder]);
			}
			/* rules */
			toml_array_t *d = toml_array_in(conf, "rules");
			n_rules = toml_array_nelem(d);
			rules = ecalloc(n_rules, sizeof(Rule));
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
				cfg_read_int(tbl, "monitor", &r->monitor);
				rules[i] = r;
			}
			/* layouts */
			d = toml_array_in(conf, "layouts");
			n_layouts = toml_array_nelem(d);
			layouts = ecalloc(n_layouts, sizeof(Layout));
			for (int i = 0; i < n_layouts; i++) {
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
				else
					l->arrange = NULL;
				layouts[i] = l;
			}
			/* keys */
			d = toml_array_in(conf, "keys");
			n_keys = toml_array_nelem(d);
			toml_array_t *tagkeys = toml_array_in(conf, "tagkeys");
			toml_array_t *tagkey = toml_array_in(conf, "tagkey");
			int n_tagkeys = toml_array_nelem(tagkeys);
			int n_tagkey = toml_array_nelem(tagkey);
			keys = ecalloc(((n_tagkey * n_tagkeys) + n_keys), sizeof(Key));
			for (int i = 0; i < n_keys; i++) {
				toml_table_t *tbl = toml_table_at(d, i);
				Key *k = ecalloc(1, sizeof(Key));
				k->mod = parse_modifier(tbl, "modifier");
				k->keysym = parse_keysym(tbl, "key");
				parse_functionargs(tbl, &k->func, &k->arg);
				keys[i] = k;
			}
			/* tag keys */
			for (int i = 0; i < n_tagkeys; i++) {
				toml_table_t *tks = toml_table_at(tagkeys, i);
				for (int j = 0; j < n_tagkey; j++) {
					toml_table_t *tk = toml_table_at(tagkey, j);
					Key *k = ecalloc(1, sizeof(Key));
					k->mod = parse_modifier(tk, "modifier");
					k->keysym = parse_keysym(tks, "key");
					parse_functionargs(tk, &k->func, &k->arg);
					parse_tag(tks, "tag", (int *)&k->arg.ui);
					keys[n_keys++] = k;
				}
			}
			/* buttons */
			d = toml_array_in(conf, "buttons");
			n_buttons = toml_array_nelem(d);
			buttons = ecalloc(n_buttons, sizeof(Button));
			for (int i = 0; i < n_buttons; i++) {
				toml_table_t *tbl = toml_table_at(d, i);
				if (!tbl)
					continue;
				Button *b = ecalloc(1, sizeof(Button));
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
				cfg_read_str(tbl, "mask", &field);
				if (!strcmp(field, "super"))
					b->mask = Mod4Mask;
				else if (!strcmp(field, "shift"))
					b->mask = ShiftMask;
				else if (!strcmp(field, "ctrl"))
					b->mask = ControlMask;
				else if (!strcmp(field, "alt"))
					b->mask = Mod1Mask;
				cfg_read_str(tbl, "button", &field);
				if (!strcmp(field, "button1"))
					b->button = Button1;
				else if (!strcmp(field, "button2"))
					b->button = Button2;
				else if (!strcmp(field, "button3"))
					b->button = Button3;
				parse_functionargs(tbl, &b->func, &b->arg);
				buttons[i] = b;
			}
			toml_free(conf);
		}
		fprintf(stderr, errbuf);
	}
}
