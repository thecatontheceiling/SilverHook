CC=clang
AR=llvm-ar
LD=ld.lld
LIB=llvm-lib
CFLAGS=-Wall -Wextra -I./src -I. -O2

TARGET_DLL=dist/bin/silverhook.dll
TARGET_IMPLIB=dist/lib/silverhook.dll.a
TARGET_A=dist/lib/libsilverhook.a
TARGET_LIB=dist/lib/silverhook.lib

DLL_OBJDIR=obj/dll
STATIC_OBJDIR=obj/static

DLL_CORE_O=$(DLL_OBJDIR)/core.o
DLL_IAT_O=$(DLL_OBJDIR)/iat.o
DLL_ALL_OBJS=$(DLL_CORE_O) $(DLL_IAT_O)

STATIC_CORE_O=$(STATIC_OBJDIR)/core.o
STATIC_IAT_O=$(STATIC_OBJDIR)/iat.o
STATIC_ALL_OBJS=$(STATIC_CORE_O) $(STATIC_IAT_O)

.PHONY: all installheaders mkdirs example-static example-dll clean

all: mkdirs $(TARGET_DLL) $(TARGET_A) $(TARGET_LIB) installheaders

$(TARGET_DLL): $(DLL_ALL_OBJS)
	$(CC) -shared -o $(TARGET_DLL) $^ -Wl,--out-implib,$(TARGET_IMPLIB)

$(TARGET_A): $(STATIC_ALL_OBJS)
	$(AR) rcs $(TARGET_A) $^

$(TARGET_LIB): $(STATIC_ALL_OBJS)
	llvm-lib -out:$(TARGET_LIB) $^

installheaders:
	@cp src/silverhook.h dist/include/silverhook.h

mkdirs:
	@mkdir -p dist/lib
	@mkdir -p dist/bin
	@mkdir -p dist/include
	@mkdir -p $(DLL_OBJDIR)
	@mkdir -p $(STATIC_OBJDIR)

example-static: all
	$(CC) $(CFLAGS) -DSILVERHOOK_STATIC -I./dist/include example/example.c -o dist/bin/example-static.exe dist/lib/libsilverhook.a

example-dll: all
	$(CC) $(CFLAGS) -I./dist/include example/example.c -o dist/bin/example-dll.exe -L./dist/lib -lsilverhook.dll

clean:
	@rm -rf obj
	@rm -rf dist
	@echo Cleaned.

$(DLL_OBJDIR)/%.o: src/%.c | mkdirs
	$(CC) $(CFLAGS) -DSILVERHOOK_BUILD_DLL -c $< -o $@

$(STATIC_OBJDIR)/%.o: src/%.c | mkdirs
	$(CC) $(CFLAGS) -DSILVERHOOK_BUILD_STATIC -c $< -o $@