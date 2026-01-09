# Modern UI Mod - Standalone Makefile
#
# This can be used to build the mod independently, or the rules can be
# integrated into the main Makefile.windows
#
# Usage:
#   make -f src/uimod/Makefile        # Build the mod
#   make -f src/uimod/Makefile clean  # Clean mod objects
#
# Or add to main Makefile.windows:
#   include src/uimod/Makefile

# Inherit from parent Makefile or set defaults
CC ?= clang
OPT ?= -O3
DEBUG ?= -gdwarf-4

# Compiler flags
UIMOD_CFLAGS = $(OPT) $(DEBUG) -Wall -Wextra -fPIC
UIMOD_CFLAGS += -I../../include -I..
UIMOD_CFLAGS += $(SDL_CFLAGS)

# Source files
UIMOD_SRCS = \
	uimod.c \
	ui_chat.c \
	ui_inventory.c \
	ui_equipment.c \
	ui_clock.c

# Object files
UIMOD_OBJS = $(UIMOD_SRCS:.c=.o)

# Output
UIMOD_DLL = ../../bin/uimod.dll

# Default target
all: $(UIMOD_DLL)

# Link the DLL
$(UIMOD_DLL): $(UIMOD_OBJS) ../../lib/moac.a
	$(CC) $(LDFLAGS) $(OPT) $(DEBUG) -shared -o $@ $(UIMOD_OBJS) ../../lib/moac.a

# Compile source files
%.o: %.c uimod.h
	$(CC) $(UIMOD_CFLAGS) -c -o $@ $<

# Dependencies
uimod.o: uimod.c uimod.h
ui_chat.o: ui_chat.c uimod.h
ui_inventory.o: ui_inventory.c uimod.h
ui_equipment.o: ui_equipment.c uimod.h
ui_clock.o: ui_clock.c uimod.h

# Clean
clean:
	-rm -f $(UIMOD_OBJS) $(UIMOD_DLL)

.PHONY: all clean
