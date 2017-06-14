root := ../../../..

include $(root)/build/cxx/rules.make
include $(root)/build/xsd/tree-rules.make


override XSDFLAGS += --generate-serialization --generate-wildcard --ordered-type batch


# Build.
#
driver: driver.o transactions.o

transactions.o: transactions.cxx transactions.hxx
driver.o: driver.cxx transactions.hxx

transactions.cxx transactions.hxx: transactions.xsd


# Test
#
.PHONY: test
test: driver transactions.xml
	./driver transactions.xml


# Clean.
#
.PHONY: clean
clean:
	rm -f transactions.o transactions.?xx driver.o driver
