INCLUDES=elf_exception.h elf_util.h elf_time.h boost_enum.hpp

SOURCES=elf_exception.cpp elf_util.cpp elf_time.cpp

UNITTEST_SOURCES=test/unittest_driver.cpp test/test_elf_time.cpp

OBJECTS=$(SOURCES:.cpp=.o)
UNITTEST_OBJECTS:=$(UNITTEST_SOURCES:.cpp=.o)

DEPENDS:=$(SOURCES:.cpp=.d)
DEPENDS+=$(UNITTEST_SOURCES:.cpp=.d)
