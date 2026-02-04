dwm - dynamic window manager (fork)
===================================
A bloated fork of suckless [dwm](https://dwm.suckless.org/) 6.8 with lots of customizations.


This fork merges the following patches: 

- [dwm-actualfullscreen](https://dwm.suckless.org/patches/actualfullscreen): Toggle fullscreen for a window.
- [dwm-alpha](https://dwm.suckless.org/patches/alpha): Allow dwm to have translucent bars, while keeping all the text on it opaque.
- [dwm-autostart](https://dwm.suckless.org/patches/autostart): Make dwm run `autostart_blocking` and `autostart &` before entering the handler loop.
- [dwm-barpadding](https://dwm.suckless.org/patches/barpadding): Adds verticle and horizontal space between the statusbar and the edge of the screen.
- [dwm-bar-height](https://dwm.suckless.org/patches/bar_height): Allows user to change dwm's default bar height.
- [dwm-center](https://dwm.suckless.org/patches/center): Add an iscentered rule to automatically center clients on the current monitor.
- [dwm-centretitle](https://dwm.suckless.org/patches/centretitle): Place the title of focused client in the middle of barwin.
- [dwm-cfacts-dragcfacts](https://github.com/bakkeby/patches/blob/master/dwm/dwm-cfacts-dragcfact-6.4.diff): Lets you resize vertical splits in layouts.
- [dwm-cfacts-vanitygaps](https://dwm.suckless.org/patches/vanitygaps): Adds gaps between client windows and gaps between windows and the screen edge.
- [dwm-colorbar](https://dwm.suckless.org/patches/colorbar): Lets you change the foreground and background color of every statusbar element.
- [dwm-dragfact](https://github.com/bakkeby/patches/blob/master/dwm/dwm-dragfact-6.3.diff): Lets you resize both horizontal/vertical splits in layouts.
- [dwm-dragmfact](https://github.com/bakkeby/patches/blob/master/dwm/dwm-dragmfact-6.3.diff): Lets you resize the horizontal splits in layouts.
- [dwm-dwmc](https://dwm.suckless.org/patches/dwmc): Simple dwmc client using a fork of fsignal to communicate with dwm.
- [dwm-hide_vacant_tags](https://dwm.suckless.org/patches/hide_vacant_tags): Prevents dwm from drawing tags with no clients on the bar.
- [dwm-insets](https://dwm.suckless.org/patches/insets): Defines custom insets from each edge of the screen.
- [dwm-notitle](https://dwm.suckless.org/patches/notitle): Hides the window title.
- [dwm-pango](https://dwm.suckless.org/patches/pango): Adds pango support for the status bar.
- [dwm-pertag](https://dwm.suckless.org/patches/pertag): Keeps layout, mwfact, barpos and nmaster per tag.
- [dwm-preserveonrestart](https://dwm.suckless.org/patches/preserveonrestart): Preserve client tags on restarting dwm.
- [dwm-shiftview](https://dwm.suckless.org/patches/nextprev): Increment or decrement the selected tag.
- [dwm-stacker](https://dwm.suckless.org/patches/stacker): Provides comprehensive utilities for managing the client stack.
- [dwm-statuscmd](https://dwm.suckless.org/patches/statuscmd): Adds the ability to signal a status monitor program when clicking on the status bar.
- [dwm-statuspadding](https://dwm.suckless.org/patches/statuspadding): Makes the amount of horizontal and vertical padding in the status bar configurable.
- [dwm-sticky](https://dwm.suckless.org/patches/sticky): A sticky client is visible on all tags.
- [dwm-swallow](https://dwm.suckless.org/patches/swallow): Adds "window swallowing" to dwm as known from Plan 9's windowing system.
- [dwm-systray](https://dwm.suckless.org/patches/systray): A simple system tray implementation.
- [dwm-underlinetags](https://dwm.suckless.org/patches/underlinetags): Underlines selected tags.
- [dwm-winicon](https://dwm.suckless.org/patches/winicon): Enables dwm to show window icons.

And adds the following other changes:
- **configuration file**; loads configuration from `$XDG_CONFIG_HOME/dwm/dwm.toml` as an option.
- **cascading styles**; default styles in config.h are overwriten by styles in dwm.toml; Scheme inherits other schemes.
- **sxhkd**; key bindings are defined in sxhkdrc using dwmc

Have a look at [dwm.toml](dwm.toml) and [sxhkdrc](sxhkdrc)!



Building and installing
-----------------------

1. clone this repository with submodules locally on your machine:
```Bash
git clone --recursive https://github.com/8bitmcu/dwm.git
```
2. run `make clean && sudo make install` from within the repository folder
3. copy and edit the config file: `cp /etc/dwm/dwm.toml $XDG_CONFIG_HOME/dwm/dwm.toml`
4. copy and edit the keybindings: `cp /etc/dwm/sxhkdrc $XDG_CONFIG_HOME/sxhkd/sxhkdrc`


Running dwm
-----------
Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)
