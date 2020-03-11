DEFTARGS = PhatBak

## get the latest git tag and create C++ macros for source
#VERSION := $(perl {@tags = grep /^Release-/, `git tag --merged`; $_ = pop @tags; chomp; s/^Release-//;$_})
#VERSION_MAJOR := $(perl {my @parts = split /\./, $VERSION; $parts[0];})
#VERSION_MINOR := $(perl {my @parts = split /\./, $VERSION; $parts[1];})
#VERSION_ARG = -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR)

DBG = 1
ifdef DBG
    # DBG=1 creates debuggable code else fastest
    MYCFLAGS  += -g -O0 -fweb -DDBGMSG -DLOCKCHECK
else
    MYCFLAGS  += -O3 -funroll-loops -finline-functions -fno-diagnostics-color
endif

CXXFLAGS =
CPPFLAGS += -std=c++17 $(MYCFLAGS)
LDFLAGS  += -lpthread -lstdc++fs -lzstd -lmhash

.PHONY: default
default:  $(DEFTARGS)

Opts.o:  CXXFLAGS=$(VERSION_ARG)

.PHONY: clean
clean:
	rm -f *.o
	rm -f tartar ttdump
        rm -rf .makepp
