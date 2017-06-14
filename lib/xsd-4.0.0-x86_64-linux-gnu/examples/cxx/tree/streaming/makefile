root := ../../..

include $(root)/build/cxx/rules.make
include $(root)/build/xsd/tree-rules.make


override XSDFLAGS += --generate-serialization


# Build.
#
driver: driver.o parser.o serializer.o grammar-input-stream.o position.o

position.o: position.cxx position.hxx 
driver.o: driver.cxx position.hxx parser.hxx serializer.hxx
parser.o: parser.cxx parser.hxx
serializer.o: serializer.cxx serializer.hxx
grammar-input-stream.o: grammar-input-stream.cxx grammar-input-stream.hxx

position.cxx position.hxx: position.xsd


# Test
#
.PHONY: test
test: driver position.xml
	./driver position.xml


# Clean.
#
.PHONY: clean
clean:
	rm -f position.o position.?xx parser.o serializer.o grammar-input-stream.o driver.o driver out.xml
