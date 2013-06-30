

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXX = clang++
endif

GH_PATH=/opt/oblong/greenhouse
PATH:=/opt/oblong/deps-64-8/bin:$(PATH)
GH_PKG_CONFIG=$(PKG_CONFIG_PATH):$(GH_PATH)/lib/pkgconfig pkg-config

CXXFLAGS = -Wempty-body \
  -fdiagnostics-show-option \
  -Wall \
  -Wpointer-arith \
  -Wno-shadow \
  -Wwrite-strings \
  -Wno-overloaded-virtual \
  -Wno-strict-overflow \
  -Wno-error=unused-variable \
  -Werror \
  `PKG_CONFIG_PATH=$(GH_PKG_CONFIG) --cflags --static libGreenhouse` \
  -g -O3 -std=gnu++0x

LDFLAGS=`PKG_CONFIG_PATH=$(GH_PKG_CONFIG) --libs --static libGreenhouse`

esc=`echo "\033"`
NO_COLOR=${esc}[0m
OK_COLOR=${esc}[36;21m

define EXP_STR

=======================================================================
TO HAVE YOUR APPLICATION FIND ASSETS IN A FOLDER OF YOUR CHOICE 
(SUCH AS YOUR CURRENT FOLDER), TRY ADDING ONTO THE OB_SHARE_PATH
ENVIRONMENT VARIABLE.  IT'S NOT TOO HARD.

MAYBE LIKE THIS (affects just one run of the program):

   OB_SHARE_PATH=/Users/foo/images:$$OB_SHARE_PATH ./my-greenhouse-program

OR EVEN LIKE THIS (affects current Terminal session):

   export OB_SHARE_PATH=`pwd`/share:$$OB_SHARE_PATH
                        ^^^^^
                  Minor trickiness here pastes in name of current dir
=======================================================================
endef
export EXP_STR

all: cities
clean:
	-rm cities

cities: cities.C
	${CXX} $^ -lGreenhouse ${LDFLAGS} ${CXXFLAGS} -o $@
	@echo	"$(OK_COLOR)$$EXP_STR$(NO_COLOR)"
