## The Cartesian SpreadSheet

## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

## Defining an executable target to be installed into the directory
## named by the predefined variable $(bindir).
bin_PROGRAMS = window


## Define the list of source files for the "spreadsheet" target.
## The file extension .cc is recognized by Automake, and causes it to
## produce rules which invoke ## the C++ compiler to produce an object
## file from each source file. Header files (.h) will also be 
## included in distribution archives of the project.
window_SOURCES = examplewindow.cc examplewindow.h labeledoptionmenu.cc labeledoptionmenu.h main.cc
