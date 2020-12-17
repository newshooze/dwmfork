# dwm version
VERSION = 6.2

# Customize below to fit your system

# paths
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# Xinerama, comment if you don't want it
#XINERAMALIBS  = -lXinerama
#XINERAMAFLAGS = -DXINERAMA

# freetype
FREETYPELIBS = -lfontconfig -lXft
FREETYPEINC = /usr/include/freetype2
# OpenBSD (uncomment)
#FREETYPEINC = ${X11INC}/freetype2

# pulseaudio
PULSEAUDIOLIBS = -lpulse
# defines for pavolume 
PAVOLUMEFLAGS = -DPAV_USE_AS_LIBRARY
# use pulseaudio
# comment out to remove the volume bar
# or if pulseaudio is not present.
DWMPULSEAUDIOFLAGS = -DDWM_PULSEAUDIO

PTHREADLIBS = -lpthread

XPMLIBS = -lXpm
# includes and libs

INCS = -I${X11INC} -I${FREETYPEINC}
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${FREETYPELIBS} ${PULSEAUDIOLIBS} ${XPMLIBS} ${PTHREADLIBS}

# flags
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
CPPFLAGS += ${PAVOLUMEFLAGS} ${DWMPULSEAUDIOFLAGS}
# debug
#CFLAGS   = -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
# no debug
CFLAGS   = -march=native -mtune=native
CFLAGS   += -std=c99 -pedantic -Wall -Wno-deprecated-declarations -Os ${INCS} ${CPPFLAGS}
LDFLAGS  = ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
CC = cc
