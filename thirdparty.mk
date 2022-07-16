FSPREFIX=/elf/apps
CXX=$(FSPREFIX)/gcc/10.3.0/bin/g++

BOOST_VERSION=1.78.0-gcc1030
FMT_VERSION=8.1.1

# boost
BOOST=$(FSPREFIX)/boost/$(BOOST_VERSION)
BOOST_INCLUDE=$(BOOST)/include
BOOST_LIB=$(BOOST)/lib
CPPFLAGS += -isystem$(BOOST_INCLUDE)
LDFLAGS += -L$(BOOST_LIB) -Wl,-rpath=$(BOOST_LIB) -lboost_filesystem -lboost_date_time -lboost_system -lboost_program_options -lboost_iostreams -lboost_regex -lboost_atomic -lboost_unit_test_framework

# fmt
FMT=$(FSPREFIX)/fmt/$(FMT_VERSION)
FMT_INCLUDE=$(FMT)/include
FMT_LIB=$(FMT)/lib
CPPFLAGS+=-isystem$(FMT_INCLUDE)
LDFLAGS+=-L$(FMT_LIB) -Wl,-rpath=$(FMT_LIB) -lfmt
