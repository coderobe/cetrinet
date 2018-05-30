CXX := clang++
BUILDTYPE ?= debug

TARGET := build/cetrinet

.PHONY: all build release release-windows clean

all: ${TARGET}

${TARGET}: build/build.ninja $(shell meson/wildcard 'src/**/*')
	ninja -C build 2>&1 | sed -u -E 's/\.\.\/(src\/)/\1/'

win${TARGET}.exe: winbuild/build.ninja $(shell meson/wildcard 'src/**/*')
	ninja -C winbuild

release: BUILDTYPE := release
release: ${TARGET}
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag ${TARGET}
	sstrip -z ${TARGET}
	upx --ultra-brute --overlay=strip ${TARGET}

release-windows: BUILDTYPE := release
release-windows: win${TARGET}.exe
	x86_64-w64-mingw32-strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag win${TARGET}.exe
	upx --ultra-brute --overlay=strip --strip-relocs=0 win${TARGET}.exe

clean:
	rm -rf build winbuild

build/build.ninja:
	CXX=${CXX} meson --buildtype ${BUILDTYPE} . build

winbuild/build.ninja:
	meson --buildtype ${BUILDTYPE} --cross-file windows.xcc . winbuild

run: ${TARGET}
	LSAN_OPTIONS=suppressions=asan/lsan.supp ASAN_OPTIONS=suppressions=asan/asan.supp ${TARGET}
