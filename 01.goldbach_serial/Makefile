# General Information.
APPNAME=main
DOXYGEN=doxygen # Doxyfile source name.
HEADERS=$(wildcard *.h) # Header files.
SOURCES=$(wildcard *.c) # Source files.
OBJECTS=$(SOURCES:%.c=build/%.o) # Object to later link in the executable.
EXEFILE=bin/$(APPNAME) # Final Executable.
EXECUTABLES=$(APPNAME)

# Compilers for Mac and Linux. TODO: include Windows compiler.
CC=cc # Alias of gcc compiler.
CC2=clang

# Compiling Flags:
# -g: Produce debugging information in the operating system's native format.
# -p: Generate extra code to write profile information suitable for the analysis program .
# -Wall -Wextra: Turns on most, but not all, compiler warnings.
# -lm: to use mathematical functions in C.
# -w: deactivate all flags.
# More info on: https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html 
CQUICKFLAGS=-w
CFLAGS=-g -Wall -Wextra

# Libraries to include:
# -pthread: works with pthread library. 
LIBS=

# Filters for Linter:
LINTFILTERS=$\
	-build/c++11,$\
	-build/header_guard,$\
	-build/include_subdir,$\
	-runtime/references

# Main compilation modes:
all: # Also know as the quick mode.
	$(CC) $(CQUICKFLAGS) $(SOURCES) -o $(APPNAME) $(LIBS)

.PHONY: report
report:
	$(CC) $(CFLAGS) $(SOURCES) -o $(APPNAME) $(LIBS)

.PHONY: release
release:
	mkdir bin/
	$(CC) $(CFLAGS) $(SOURCES) -o bin/$(APPNAME) $(LIBS)
	doxygen $(DOXYGEN)

# Special Make modes:
.PHONY: msan # Unitialized memory revision.
msan:
	$(CC2) $(CFLAGS) -fsanitize=memory $(SOURCES) -o $(APPNAME)_msan $(LIBS)

.PHONY: asan # Memory leaks revision.
asan:
	$(CC2) $(CFLAGS) -fsanitize=address $(SOURCES) -o $(APPNAME)_asan $(LIBS)

.PHONY: tsan
tsan:
	$(CC2) $(CFLAGS) -fsanitize=thread $(SOURCES) -o $(APPNAME)_tsan $(LIBS)

.PHONY: rebuild
rebuild:
	make clean
	make

.PHONY: lint
lint:
	cpplint --filter=$(LINTFILTERS) $(HEADERS) $(SOURCES)

.PHONY: gitignore
gitignore:
	echo "bin/ build/" | tr " " "\n" > .gitignore

.PHONY: clean
clean:
	rm -rf bin/ build/ $(APPNAME)_* $(APPNAME)*.dSYM
