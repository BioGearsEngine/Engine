root := ../../../..

include $(root)/build/cxx/rules.make
include $(root)/build/xsd/tree-rules.make


override XSDFLAGS += --generate-inline --generate-serialization \
--hxx-epilogue '\#include "people-custom.hxx"'

ifneq ($(shell uname -o),Msys)
override XSDFLAGS += --custom-type bio=/bio_base 
else
# Prevent MSYS from converting / to Windows path.
override XSDFLAGS += --custom-type bio=//bio_base
endif


# Build.
#
driver: driver.o people.o people-custom.o

people.o: people.cxx people.ixx people.hxx people-custom.hxx
driver.o: driver.cxx people.ixx people.hxx people-custom.hxx
people-custom.o: people-custom.cxx people.ixx people.hxx people-custom.hxx

people.cxx people.hxx people.ixx: people.xsd


# Test
#
.PHONY: test
test: driver people.xml
	./driver people.xml


# Clean.
#
.PHONY: clean
clean:
	rm -f people-custom.o people.o people.?xx driver.o driver
