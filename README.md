dwm - dynamic window manager (fork)
===================================
A bloated fork of suckless [dwm](https://dwm.suckless.org/) with lots of customizations.


This fork merges the following patches: 

- [dwm-alpha](https://dwm.suckless.org/patches/alpha): Allow dwm to have translucent bars, while keeping all the text on it opaque.
- [dwm-cfacts-vanitygaps](https://dwm.suckless.org/patches/vanitygaps): adds gaps between client windows and gaps between windows and the screen edge.
- [dwm-dwmc](https://dwm.suckless.org/patches/dwmc): Simple dwmc client using a fork of fsignal to communicate with dwm.
- [dwm-swallow](https://dwm.suckless.org/patches/swallow): adds "window swallowing" to dwm as known from Plan 9's windowing system.
- [dwm-systray](https://dwm.suckless.org/patches/systray): A simple system tray implementation.

And adds the following other changes:
- **configuration file**; loads configuration from `$XDG_CONFIG_HOME/dwm/dwm.toml`. This file must exists.

Have a look at [dwm.toml](dwm.toml)!



Building and installing
-----------------------

1. clone this repository locally on your machine
2. run `make clean && sudo make install` from within the repository folder
3. copy and edit the config file: `cp /etc/dwm/dwm.toml $XDG_CONFIG_HOME/dwm/dwm.toml`


Running dwm
-----------
Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)
