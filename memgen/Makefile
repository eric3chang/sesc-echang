MONODIR = /opt/local/mono
#MONODIR = /opt/local/mono-2.4.2.3/
GMCS = $(MONODIR)/bin/gmcs
#GMCSFLAGS = -warnaserror -debug
GMCSFLAGS = -debug
MONOLIB = Mono.Posix.dll
MKBUNDLE = $(MONODIR)/bin/mkbundle
MKBUNDLEFLAGS = -z --static

LOG4NETLIB = /t/cslt08/ajcg/log4net-1.2.10/bin/mono/2.0/release/log4net.dll

#LIBSRCFILES = $(shell find Source -name \*.cs)

CC = gcc
SONAME = libselect.so
REALNAME = libselect.so

#all: Output/Campaign.exe Output/SysManager.exe Output/libselect.so
all: memgen.exe

memgen.exe: memgen/MemGen.cs
	$(GMCS) $(GMCSFLAGS) \
		-out:memgen.exe memgen/MemGen.cs $(LIBSRCFILES) \
		-lib:$(MONODIR)/lib/mono/2.0 \
		-reference:$(MONOLIB)
	mono memgen.exe
	mv memoryConfigs/* ../WorkingFolder/memgen/memoryConfigs/

Output/Campaign.exe: Programs/Campaign.cs $(LIBSRCFILES)
	mkdir -p Output
	$(GMCS) $(GMCSFLAGS) \
		-out:Output/Campaign.exe Programs/Campaign.cs $(LIBSRCFILES) \
		-lib:$(MONODIR)/lib/mono/2.0 \
		-reference:$(MONOLIB),$(LOG4NETLIB)

Output/Campaign: Output/Campaign.exe
	env PKG_CONFIG_PATH=$(MONODIR)/lib/pkgconfig/ \
	    LD_LIBRARY_PATH=$(MONODIR)/lib/ \
	    $(MKBUNDLE) $(MKBUNDLEFLAGS) Output/Campaign.exe -o Output/Campaign --deps $(LOG4NETLIB)
	strip Output/Campaign

Output/SysManager.exe: Programs/SysManager.cs $(LIBSRCFILES)
	mkdir -p Output
	$(GMCS) $(GMCSFLAGS) \
		-out:Output/SysManager.exe Programs/SysManager.cs $(LIBSRCFILES) \
		-lib:$(MONODIR)/lib/mono/2.0 \
		-reference:$(MONOLIB),$(LOG4NETLIB)

Output/SysManager: Output/SysManager.exe
	env PKG_CONFIG_PATH=$(MONODIR)/lib/pkgconfig/ \
	    LD_LIBRARY_PATH=$(MONODIR)/lib/ \
	    $(MKBUNDLE) $(MKBUNDLEFLAGS) Output/SysManager.exe -o Output/SysManager --deps $(LOG4NETLIB)
	strip Output/SysManager

Output/libselect.so: Libraries/libselect.c
	${CC} -c Libraries/libselect.c -o Output/libselect.o
	${CC} -shared -Wl,-soname,${SONAME} -o Output/${REALNAME} Output/libselect.o

install:
	cp -u Output/Campaign.exe ../../bin
	cp -u Output/Campaign.exe.mdb ../../bin
	cp -u Output/SysManager.exe ../../bin
	cp -u Output/SysManager.exe.mdb ../../bin
	mkdir -p ../../lib/mono/2.0
	cp -u $(LOG4NETLIB) ../../lib/mono/2.0/
	cp -u Output/libselect.so ../../lib

clean:
	rm -rf memgen.exe
