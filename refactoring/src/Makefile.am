## The Cartesian SpreadSheet

ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = spreadsheet

EXTRA_DIST = ack fact fib header 


spreadsheet_SOURCES = spreadsheet.h spreadsheet.cc \
                      TLobjects.h TLobjects.cc \
                      Pivot.h Pivot.cc \
                      List_uuid_n_content.h List_uuid_n_content.cc \
                      HeaderDims_In_PivotOrds.cc HeaderDims_In_PivotOrds.h \
                      main.cc 
spreadsheet_LDFLAGS = -ltlsystem
AM_CXXFLAGS = -std=gnu++0x -DBOOST_SPIRIT_UNICODE
