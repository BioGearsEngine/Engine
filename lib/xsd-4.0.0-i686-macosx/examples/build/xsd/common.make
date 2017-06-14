# file      : examples/build/xsd/common.make
# copyright : Copyright (c) 2006-2014 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

XSD := $(shell if test -x $(root)/../bin/xsd; \
               then echo $(root)/../bin/xsd; \
               else echo xsd; fi)

override CPPFLAGS += $(shell if test -d $(root)/../libxsd; \
                             then echo -I$(root)/../libxsd; fi)
