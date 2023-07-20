# Wallpaper
Simple wallpaper script for Linux running X11 display server, and using the SDL2
library. Wallpaper transitions between random files in a directory every few
seconds, and can be configured.

## Installation
```
git clone https://github.com/AnsonTran/wallpaper
cd wallpaper
make
make install
```

# Usage
```
Usage: wallpaper [OPTION...] DIRECTORY
wallpaper - Simple X11 wallpaper script using SDL2

  -d, --duration=SECS        Wallpaper duration
  -f, --fade=SECS            Fade duration between wallpapers
  -i, --initial=FILE         Choose a starting wallpaper
  -r, --randomize            Cycle through wallpapers at random
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <trananson@protonmail.com>.
```
