# file      : examples/build/cxx/rules.make
# copyright : Copyright (c) 2006-2014 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(root)/build/cxx/compilers.make


# GNU g++
#
ifeq ($(cxx_id),gnu)
  CXXFLAGS := -W -Wall -O3
endif

# Clang
#
ifeq ($(cxx_id),clang)
  CXXFLAGS := -W -O3
endif

# Intel C++
#
ifeq ($(cxx_id),intel)
  CXXFLAGS := -w1 -O2
endif


# Sun C++
#
ifeq ($(cxx_id),sun)
  CXXFLAGS := -O
endif


# HP aCC
#
ifeq ($(cxx_id),hp)

  # By default Xerces-C++ is built with -mt for aCC.
  # 2334 no suitable copy constructor (e.g., for std::auto_ptr)
  #
  CXXFLAGS := -Aa -O -mt +W2334
endif


# IBM XL C++
#
ifeq ($(cxx_id),ibm)
  CXXFLAGS := -qrtti -O
endif


# Rules.
#
%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%: %.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
