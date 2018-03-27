# ------------------------------------------------
# Makefile for C projects
# TODO: hacer una regla para crear las carpetas
# necesarias
# por ejemplo: make project
# ------------------------------------------------

# project name (generate executable with this name)
TARGET   = out

CC       = clang
# compiling flags here
# CFLAGS   = -std=c89 -Wall -I.
CFLAGS   = -std=c99 -Wall -I.

LINKER   = gcc -o
# linking flags here
LFLAGS   = -Wall -I. -lm

# change these to proper directories where each file should be
SRCDIR   = src
INCDIR   = inc
OBJDIR   = obj
BINDIR   = bin

SOURCES       := $(wildcard $(SRCDIR)/*.c)
SOURCES_TESTS := $(wildcard $(TESTDIR)/*.c)
INCLUDES_TEST := $(wildcard $(TESTDIR)/*.h)
INCLUDES      := $(wildcard $(INCDIR)/*.h)
OBJECTS       := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
