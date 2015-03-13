# configuration path

ROCSYS := $(CURDIR)

CFGFILE = config/Makefile.arch

LOGINFILE = roclogin

DABC1TARBALL = $(wildcard dabc/dabcroc11.*)
DABC2TARBALL = $(wildcard dabc/dabcroc19.*)

ABBPACKTARBALL = $(wildcard abbpack/abbpack.*)

LOCALDABCSYS = $(CURDIR)/dabc

ifdef GO4SYS
IS_GO4=true
else
ifneq ($(shell go4-config --go4sys 2>/dev/null),)
IS_GO4=true
endif
endif


ifdef withspadic
ROC_WITHSPADIC=true
endif

ifdef withabb
ROC_WITHABB=true
endif

ifndef ROC_WITHDABC2
ifndef dabc1
ROC_WITHDABC2=true
DABCTARBALL=$(DABC2TARBALL)
else
ROC_WITHDABC2=false
DABCTARBALL=$(DABC1TARBALL)
endif
endif

ifndef DABCTARBALL
DABCTARBALL=$(DABC2TARBALL)
endif

DUMMYFILE = lib/.dummy

FASTRULES = clean clean-svn pack swpack

.PHONY:    all roclib dabccore dabcplugin rocdict rocgui simul \
           utils go4monitor onlinemonitor bnetplugin abbpack abbplugin \
           spadiclib spadicdict spadic-core spadic-dabc $(FASTRULES)


ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include $(CFGFILE)
endif

LOCALDABC=true
DABC_LIBDIR = $(ROCSYS)/lib

ifdef DABCSYS
ifneq ($(DABCSYS),$(LOCALDABCSYS))
LOCALDABC=false
DABC_LIBDIR = $(DABCSYS)/lib
endif
endif

ifndef DABCSYS
DABCSYS=$(LOCALDABCSYS)
endif

LIBTGTS = dabcplugin bnetplugin 

ABBPLUGINDEP = roclib dabccore dabcplugin

ifdef ROC_WITHABB
LIBTGTS += abbpack abbplugin
ABBPLUGINDEP += abbpack
endif

ifdef ROC_WITHSPADIC
LIBTGTS += spadic-core spadic-dabc
endif


all: roclib spadiclib rocdict spadicdict $(LIBTGTS) rocgui utils simul go4monitor onlinemonitor

roclib:
	+cd roclib; $(MAKE) all ROCSYS=$(ROCSYS)

spadiclib:
	+cd spadiclib; $(MAKE) lib ROCSYS=$(ROCSYS)

spadicdict: spadiclib
	+cd spadiclib; $(MAKE) dict ROCSYS=$(ROCSYS)


ifeq ($(LOCALDABC),true)

dabc/build.dummy: $(DABCTARBALL)
        ifeq ($(wildcard dabc/base),)
	@rm -rf $(filter-out $(wildcard dabc/*.tar.gz), $(wildcard dabc/*))
	cd dabc; tar xzf $(notdir $(DABCTARBALL))
        endif
	@(if [ ! -f $@ ] ; then touch $@; fi)

dabccore: dabc/build.dummy
	+cd dabc; $(MAKE) all
	cp -f dabc/bin/* bin
	cp -f dabc/lib/* lib
	@echo "Build DABC done"
	
else

dabccore: 
	@echo Use external dabc at $(DABCSYS)

endif

abbpack:
ifeq ($(wildcard abbpack/Makefile),)
	cd abbpack; tar xzf $(notdir $(ABBPACKTARBALL)) 
endif	
	cd abbpack; $(MAKE) TGTBIN=$(ROCSYS)/bin TGTLIB=$(ROCSYS)/lib 

dabcplugin: roclib dabccore
ifneq ($(ROC_WITHDABC2),false)
	+cd plugin2; $(MAKE) libs DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC2-ROC plugin done"
else
	+cd plugin; $(MAKE) libs DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC-ROC plugin done"
endif

bnetplugin: dabcplugin 
ifneq ($(ROC_WITHDABC2),false)
#	+cd bnet; $(MAKE) libs DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC2-ROC-BNET plugin is not yet ready"
else
	+cd bnet; $(MAKE) libs DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC-ROC-BNET plugin done"
endif


abbplugin: $(ABBPLUGINDEP)
ifneq ($(ROC_WITHDABC2),false)
	+cd abbplugin2; $(MAKE) libs exes DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC2-ABB plugin done"
else
	+cd abbplugin; $(MAKE) libs exes DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "DABC-ABB plugin done"
endif	

spadic-core: spadiclib 
	+cd spadiclib; $(MAKE) TGTLIB=$(ROCSYS)/lib TGTBIN=$(ROCSYS)/bin ROCSYS=$(ROCSYS) 
	@echo "Spadic library done"

spadic-dabc: spadic-core dabccore
ifeq ($(ROC_WITHDABC2),true)
	+cd spadicplugin2; $(MAKE) DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "Spadic plugin for DABC2 done"
else
	+cd spadicplugin; $(MAKE) DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS) ROCTGTDIR=$(ROCSYS)
	@echo "Spadic plugin done"
endif

rocdict: $(LIBTGTS)
	+cd roclib; $(MAKE) dict ROCSYS=$(ROCSYS)

rocgui: rocdict
	+cd gui; $(MAKE) all ROCSYS=$(ROCSYS)

simul:
	+cd simulator; $(MAKE) all ROCSYS=$(ROCSYS)

utils: roclib $(LIBTGTS)
	+cd utilities; $(MAKE) all ROCSYS=$(ROCSYS)

go4monitor: roclib $(LIBTGTS) rocdict
ifdef IS_GO4
	+cd go4monitor; $(MAKE) all ROCSYS=$(ROCSYS)
else
	@echo "Cannot build go4monitor - GO4SYS not defined"
endif

onlinemonitor: roclib spadiclib spadicdict $(LIBTGTS) rocdict
ifdef IS_GO4
	+cd onlinemonitor; $(MAKE) all ROCSYS=$(ROCSYS)
else
	@echo "Cannot build onlinemonitor - GO4SYS not defined"
endif


clean:
ifdef IS_GO4
	cd go4monitor; $(MAKE) clean ROCSYS=$(ROCSYS)
	cd onlinemonitor; $(MAKE) clean ROCSYS=$(ROCSYS)
endif
ifneq ($(wildcard $(CFGFILE)),)
	cd utilities; $(MAKE) clean ROCSYS=$(ROCSYS)
	cd simulator; $(MAKE) clean ROCSYS=$(ROCSYS)
	cd gui; $(MAKE) clean ROCSYS=$(ROCSYS)
	cd roclib; $(MAKE) clean ROCSYS=$(ROCSYS)
endif
ifneq ($(wildcard $(DABCSYS)/config/*),)
ifeq ($(ROC_WITHDABC2),true)
	cd plugin2; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
	cd spadicplugin2; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
	cd abbplugin2; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
else
	cd bnet; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
	cd plugin; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
	cd spadicplugin; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
	cd abbplugin; $(MAKE) clean DABCSYS=$(DABCSYS) ROCSYS=$(ROCSYS)
endif
endif
	cd spadiclib; $(MAKE) ROCSYS=$(ROCSYS) clean
	@rm -rf $(filter-out $(ABBPACKTARBALL), $(wildcard abbpack/*))
	@rm -rf $(filter-out $(wildcard dabc/*.tar.gz), $(wildcard dabc/*))
	@rm -rf lib bin include doxydoc $(CFGFILE) $(LOGINFILE)
	@echo "ROClib clean done"

clean-svn:
	find . -name ".svn" -type d -exec rm -rf {} \;
	@echo "Clean svn-specific files done"

clean-bin:
	@rm -rf */*.o */*.d */*/*.o */*/*.d */G__*.* */$(shell uname -m)
	@echo "Clean binary files"
	

pack: clean
	@echo "Producing roclib.tar.gz ..."
	tar chf roclib.tar . --exclude=.svn --exclude=roclib.tar
	gzip roclib.tar
	@echo "Tar ball roclib.tar.gz produced"

swpack: clean
	@echo "Producing roclib_sw.tar.gz ..."
	tar chf roclib_sw.tar abbpack abbplugin analysis bnet config dabc go4monitor onlinemonitor gui plugin roclib simulator test_scripts utilities Makefile *.txt --exclude=.svn
	gzip roclib_sw.tar
	@echo "Tar ball roclib_sw.tar.gz produced"


## from here is configuration part, not required in other tools

ifdef debug
CXXOPT = -g -O2 -Wall -fPIC
else
CXXOPT = -O2 -fPIC
endif

ifeq ($(shell uname -m),x86_64)
CXXOPT    += -m64
endif


ifdef ROOTSYS
ISROOT = true
ROOT_BIN = $(ROOTSYS)/bin/
else
ifneq ($(shell which root-config 2>/dev/null),)
ISROOT = true
ROOT_BIN = $(shell root-config --exec-prefix)/bin/
endif
endif


$(CFGFILE):
	@echo "Generating $@"
	@rm -f $@
	@echo "#### Generated file, not change ###" >> $@
	@echo "CXXOPT = $(CXXOPT)" >> $@
	@echo "" >> $@
ifdef ISROOT
	@echo "ISROOT = true" >> $@
	@echo "ROOT_BIN = $(ROOT_BIN)" >> $@
	@echo "ROOT_LIBS = $(shell $(ROOT_BIN)root-config --libs)" >> $@
	@echo "ROOT_GLIBS = $(shell $(ROOT_BIN)root-config --glibs)" >> $@
	@echo "ROOT_INCLUDE = $(shell $(ROOT_BIN)root-config --incdir)" >> $@
	@echo "ROOT_CFLAGS = $(shell $(ROOT_BIN)root-config --cflags)" >> $@
	@echo "" >> $@
endif
	@echo 'ROC_LIBDIR = $$(ROCSYS)/lib' >> $@
	@echo 'ROC_BINDIR = $$(ROCSYS)/bin' >> $@
	@echo "" >> $@
	@echo "ROC_WITHDABC = true" >> $@
	@echo "DABC_LIBDIR = $(DABC_LIBDIR)" >> $@
	@echo "ROC_WITHDABC2 = $(ROC_WITHDABC2)" >> $@
	@echo "ROC_INCLUDES += -I$(ROCSYS)/include" >> $@
	@echo "ROC_INCLUDES += -I$(DABCSYS)/include" >> $@
ifdef ROC_WITHSPADIC
	@echo "ROC_WITHSPADIC = true" >> $@
endif
ifdef ROC_WITHABB
	@echo "ROC_WITHABB = true" >> $@
	@echo "ROCDICT_DEPLIBS += $(ROCSYS)/lib/libDabcAbb.so" >> $@
endif
	@echo "ROCDICT_DEPLIBS += $(ROCSYS)/lib/libDabcRoc.so $(DABC_LIBDIR)/libDabcMbs.so $(DABC_LIBDIR)/libDabcBase.so $(ROCSYS)/lib/libRoc.so" >> $@
	@echo "ROC_LIBSSET = -L$(DABC_LIBDIR) -lDabcBase -lDabcMbs -L$(ROCSYS)/lib -lRoc -lDabcRoc" >> $@
	@echo 'ROC_ROOTLIBSSET = $$(ROC_LIBSSET) -lRocDict' >> $@
ifdef ROC_WITHABB
ifndef stddaemon
	@echo "ROC_LIBSSET += -lpcidriver -lmprace -lDabcAbb" >> $@
else
	@echo "ROC_LIBSSET += -lpcidriver -lmprace -labbdaemon -lDabcAbb" >> $@
endif
endif
	@echo "SPADIC_LIBSSET = -L$(ROCSYS)/lib -lSpadic" >> $@
ifdef ROC_WITHSPADIC
	@echo "SPADIC_LIBSSET += -L$(DABC_LIBDIR) -lDabcBase -lDabcMbs -L$(ROCSYS)/lib -lDabcSpadic" >> $@
ifdef QTDIR
	@echo "SPADIC_INCLUDES += -I$(ROCSYS)/spadiclib/include -I$(ROCSYS)/spadicplugin -I$(QTDIR)/include -I$(QTDIR)/include/QtCore" >> $@
else
	@echo "SPADIC_INCLUDES += -I$(ROCSYS)/spadiclib/include -I$(ROCSYS)/spadicplugin $(shell pkg-config QtCore --cflags-only-I)" >> $@
endif
endif
	@echo 'SPADIC_ROOTLIBSSET = $$(SPADIC_LIBSSET) -lSpadicDict' >> $@
ifdef IS_GO4
	@echo "ROC_GO4INCLUDES = -I$(ROCSYS)/onlinemonitor/framework -I$(ROCSYS)/onlinemonitor/rocmonitor -I$(ROCSYS)/onlinemonitor/spadicmonitor -I$(ROCSYS)/onlinemonitor/epicsmonitor" >> $@
	@echo "ROC_GO4SOFLAGS0 = -Wl,-rpath,$(ROCSYS)/lib -L$(ROCSYS)/lib -lGo4CBMonline" >> $@
	@echo "ROC_GO4SOFLAGS = -Wl,-rpath,$(ROCSYS)/lib -L$(ROCSYS)/lib -lGo4CBMonline -lGo4FastRoc -lGo4FastSpadic -lGo4FastEpics" >> $@
	@echo "ROC_GO4LIBDEP = $(ROCSYS)/lib/libGo4FastEpics.so $(ROCSYS)/lib/libGo4FastRoc.so $(ROCSYS)/lib/libGo4FastSpadic.so $(ROCSYS)/lib/libGo4CBMonline.so" >> $@
ifdef ROC_WITHSPADIC
	@echo "ROC_GO4LIBDEP += $(ROCSYS)/lib/libDabcSpadic.so" >> $@
endif
	@echo "ROC_GO4LIBDEP += $(ROCSYS)/lib/libSpadic.so $(ROCSYS)/lib/libSpadicDict.so" >> $@
	@echo 'ROC_GO4LIBDEP += $$(ROCDICT_DEPLIBS)' >> $@
endif



$(LOGINFILE):
	@rm -f $@
	@echo "# this is generated file, use it to configure enviroment for ROClib" >> $@
	@echo "# just type '. roclogin' in shell, not forget space in between" >> $@
	@echo "" >> $@
ifdef ROOTSYS
	@echo "# uncomment following lines to configure your ROOT installation" >> $@
	@echo '# export ROOTSYS=$(ROOTSYS)' >> $@
	@echo '# export LD_LIBRARY_PATH=.:$$ROOTSYS/lib:$$LD_LIBRARY_PATH' >> $@
	@echo '# export PATH=$$ROOTSYS/bin:$$PATH' >> $@
	@echo "" >> $@
endif
	@echo "# in GSI do . rootlogin to use ROOT" >> $@
	@echo "" >> $@
	@echo 'if [ -z `which root-config 2>/dev/null` ]; then echo "ROOT is not configured!"; fi' >> $@
	@echo "" >> $@
	@echo 'export ROCSYS=$(ROCSYS)' >> $@
	@echo 'export PATH=$$ROCSYS/bin:$$PATH' >> $@
	@echo 'export LD_LIBRARY_PATH=.:$$ROCSYS/lib:$$LD_LIBRARY_PATH' >> $@
ifeq ($(LOCALDABC),true)
	@echo "" >> $@
	@echo 'export DABCSYS=$(LOCALDABCSYS)' >> $@
endif
ifdef IS_GO4
	@echo "" >> $@
	@echo "# this is required to run go4monitor analysis from any directory" >> $@
	@echo '# export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$$ROCSYS/go4monitor' >> $@
endif
	@echo "" >> $@
	@echo 'echo "ROClib (DABC-based) is configured"' >> $@
	@echo 'echo "To run gui type:  rocGui roc_address"' >> $@
	@echo 'echo "To run rocutil:   rocutil b=roc_addess -"' >> $@
	@chmod +x $@
	@echo " $@ script created"

$(DUMMYFILE): Makefile $(CFGFILE) $(LOGINFILE)
	@(if [ ! -d lib ] ; then mkdir -p lib; fi)
	@(if [ ! -d bin ] ; then mkdir -p bin; fi)
	@(if [ ! -d include ] ; then mkdir -p include; ln -s ../roclib/nxyter include/nxyter; ln -s ../roclib/roc include/roc; ln -s ../roclib/feet include/feet; ln -s ../spadiclib/spadic include/spadic; fi)
	@(if [ ! -f $@ ] ; then touch $@; fi)

ifeq ($(findstring $(MAKECMDGOALS), $(FASTRULES)),)
-include $(DUMMYFILE)
endif

