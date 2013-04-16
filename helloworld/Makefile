

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXX = clang++
endif
CXXFLAGS = -Wempty-body \
  -fdiagnostics-show-option \
  -Wall \
  -Wpointer-arith \
  -Wshadow \
  -Wwrite-strings \
  -Wno-overloaded-virtual \
  -Wno-strict-overflow \
  -Wno-error=unused-variable \
  -Werror \
  -g -O3 -std=gnu++0x

NO_COLOR=\x1b[0m
OK_COLOR=\x1b[36;21m

define EXP_STR

======================================================================
TO HAVE YOUR APPLICATION FIND THE RESOURCES IN THE LOCAL SHARE FOLDER,
PLEASE RUN THE FOLLOWING:

export OB_SHARE_PATH=`pwd`/share:$$OB_SHARE_PATH

*Note: This will only effect your current terminal window.
======================================================================
endef
export EXP_STR

helloworld: helloworld.C
	${CXX} `PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):/opt/oblong/greenhouse/lib/pkgconfig /opt/oblong/deps-64-8/bin/pkg-config --libs --cflags --static libGreenhouse` -I.. -lGreenhouse ${CXXFLAGS} $^ -o $@
	@echo	"$(OK_COLOR)$$EXP_STR$(NO_COLOR)"
