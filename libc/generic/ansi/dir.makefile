
$c_SRCDIR = $(TREE_PATH)/$c/src
$c_HEADERDIR := $(TREE_PATH)/$c/include
$c_OBJDIR := $(BUILD_PATH)/$c/obj

$c_HEADERS := alloca.h assert.h ctype.h errno.h inttypes.h limits.h locale.h math.h setjmp.h signal.h \
	stdio.h stdlib.h string.h time.h wchar.h \
	mlibc/clockid_t.h mlibc/seek.h mlibc/time_t.h mlibc/timespec.h \
	mlibc/ensure.h

$c_OBJECTS := assert-stubs.o ctype-stubs.o errno-stubs.o inttypes-stubs.o \
	locale-stubs.o math-stubs.o signal-stubs.o \
	stdio-stubs.o stdlib-stubs.o string-stubs.o time-stubs.o
$c_OBJECT_PATHS := $(addprefix $($c_OBJDIR)/,$($c_OBJECTS))

$c_CXX := x86_64-managarm-g++
$c_CPPFLAGS := -std=c++1z -Wall
$c_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/private
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/ansi/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/generic/posix/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/compilers/gcc/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/machine/x86_64/include
$c_CPPFLAGS += -I$(TREE_PATH)/libc/platform/x86_64-managarm/include
$c_CPPFLAGS += -I$(FRIGG_PATH)/include
$c_CPPFLAGS += -DFRIGG_HAVE_LIBC -DFRIGG_HIDDEN
$c_CXXFLAGS := $($c_CPPFLAGS) -fPIC -O2
$c_CXXFLAGS += -fno-builtin -fno-rtti -fno-exceptions

$c_TARGETS := clean-$c install-headers-$c $($c_OBJECT_PATHS)

.PHONY: clean-$c install-headers-$c

clean-$c:
	rm -f $($c_OBJECT_PATHS)
	rm -f $($c_OBJECT_PATHS:%.o=%.d)

install-headers-$c:
	mkdir -p  $(SYSROOT_PATH)/usr/include
	mkdir -p  $(SYSROOT_PATH)/usr/include/mlibc
	for f in $($c_HEADERS); do \
		install -p $($c_HEADERDIR)/$$f $(SYSROOT_PATH)/usr/include/$$f; done

$($c_OBJDIR):
	mkdir -p $@

$($c_OBJDIR)/%.o: $($c_SRCDIR)/%.cpp | $($c_OBJDIR)
	$($c_CXX) -c -o $@ $($c_CXXFLAGS) $<
	$($c_CXX) $($c_CPPFLAGS) -MM -MP -MF $(@:%.o=%.d) -MT "$@" -MT "$(@:%.o=%.d)" $<

-include $($c_OBJECT_PATHS:%.o=%.d)

