# STerm - A simple and minimalistic terminal emulator based on VTE-2.90

## Requirements

* glib2
* GTK+-3.0
* VTE-2.90
* cmake >= 2.8.8

## Installation

```
mkdir build
cd build
cmake [OPTIONS] ..
make
su -c "make install"
```

### CMake options:

* **-DWITH_LIBRARY={ON,OFF}**
  This controls whether to build the shared library
  libsterm-core.so (to be used in further projects).
  Default: ON

* **-DWITH_URLPIPE={ON,OFF}**
  This controls whether to build the urlpipe tool.
  Default: ON

## Usage

See 'man sterm' for help on the main application.
The main configuration file, sterm.ini, should be placed in $HOME/.config/sterm/,
an example can be found in the doc/ subdirectory and will be installed into
/usr/share/doc/sterm/. See 'man sterm.ini' for a description of the options.


## Copyright

Copyright (c) 2014-2015 by Jakob Nixdorf <flocke@shadowice.org>

