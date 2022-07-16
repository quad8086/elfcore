BUILDMODE=debug
SRCDIR=$(shell pwd)
INSTALL_ROOT=$(SRCDIR)
INSTALL_DIR=$(INSTALL_ROOT)/.install
BUILDFILES=thirdparty.mk sources.mk
VERSION=dev

LIBRARY=libelfcore.a
UNITTEST=unittest

ifeq ($(BUILDMODE),debug)
  CPPFLAGS=-g
  LDFLAGS=-g
endif

ifeq ($(BUILDMODE),opt)
  CPPFLAGS=-O2 -g
  LDFLAGS=-O2 -g
endif

CPPFLAGS+=-std=c++17 -m64 -Wall -Werror -I$(SRCDIR) -I$(SRCDIR)/test -DBUILDMODE=\"$(BUILDMODE)\" -DVERSION=\"$(VERSION)\"

ifeq ($(RELEASE),1)
  CPPFLAGS+=-DBOOST_DISABLE_ASSERTS
endif

LDFLAGS = -L$(SRCDIR)

include thirdparty.mk
include sources.mk

all: $(LIBRARY) $(UNITTEST)

libelfcore.a: $(LIBRARIES) $(OBJECTS)
	ar -crs $@ $(OBJECTS)

unittest: $(LIBRARIES) $(OBJECTS) $(UNITTEST_OBJECTS)
	$(CXX) $(OBJECTS) $(UNITTEST_OBJECTS) $(LDFLAGS) -o $@

install:
	mkdir -p $(INSTALL_DIR)
	for d in include bin lib test mk; do mkdir -p $(INSTALL_DIR)/$${d}; done
	install --mode 755 $(UNITTEST) $(INSTALL_DIR)/test/
	install --mode 755 $(LIBRARY) $(INSTALL_DIR)/lib/
	for f in $(INCLUDES); do install --mode 644 $$f $(INSTALL_DIR)/include/; done
	install --mode 644 thirdparty.mk $(INSTALL_DIR)/mk

dep: $(DEPENDS)

clean:
	$(RM) $(DEPENDS) $(OBJECTS) $(LIBRARY) $(UNITTEST) $(UNITTEST_OBJECTS) unittest.o unittest.d

%.d: %.cpp
	$(CXX) -M $(CPPFLAGS) -o $@ $<

print-%:
	@echo $*=$($*)

-include $(DEPENDS)
