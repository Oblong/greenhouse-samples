

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

radiology: radiology.C
	${CXX} `PKG_CONFIG_PATH=$(PKG_CONFIG_PATH):/opt/oblong/greenhouse/lib/pkgconfig /opt/oblong/deps-64-8/bin/pkg-config --libs --cflags --static libGreenhouse` -I.. -I../.. -lGreenhouse ${CXXFLAGS} $^ -o $@
