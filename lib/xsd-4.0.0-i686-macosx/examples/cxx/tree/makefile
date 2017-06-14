dirs := binary caching embedded custom hello library messaging mixed \
multiroot order performance polymorphism streaming wildcard

ifeq ($(WITH_ZLIB),1)
dirs += compression
endif

ifeq ($(WITH_XQILLA),1)
dirs += xpath
endif

.PHONY: all $(dirs)

all: $(dirs)

$(dirs):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

makefile: ;
% :: $(dirs) ;
