## The Cartesian SpreadSheet

SUBDIRS = examples

## Place generated object files (.o) into the same directory as their source
## files, in order to avoid collisions when non-recursive make is used.
AUTOMAKE_OPTIONS = subdir-objects

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

## Defining an executable target to be installed into the directory
## named by the predefined variable $(bindir).
bin_PROGRAMS = spreadsheet

## Define the list of source files for the "spreadsheet" target.
## The file extension .cc is recognized by Automake, and causes it to
## produce rules which invoke ## the C++ compiler to produce an object
## file from each source file. Header files (.h) will also be 
## included in distribution archives of the project.
spreadsheet_SOURCES = spreadsheet.h spreadsheet.cc main.cc 
spreadsheet_LDFLAGS = -ltlsystem
AM_CXXFLAGS = -std=gnu++0x
