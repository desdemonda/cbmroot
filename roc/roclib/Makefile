include $(ROCSYS)/config/Makefile.config


ROCLIB_NAME = $(ROC_LIBDIR)/libRoc
ROCLIB = $(ROCLIB_NAME).so

ROCLIB_HEADERS = $(wildcard roc/*.h) $(wildcard nxyter/*.h)
ROCLIB_SRCS = $(wildcard src_roc/*.cxx) $(wildcard src_nxyter/*.cxx)
ROCLIB_OBJS = $(patsubst %.cxx, %.o, $(ROCLIB_SRCS))
ROCLIB_DEPS = $(patsubst %.cxx, %.d, $(ROCLIB_SRCS))


ROCDICT_LIBNAME = $(ROC_LIBDIR)/libRocDict
ROCDICT_LIB = $(ROCDICT_LIBNAME).so
ROCDICT_MAP = $(ROCDICT_LIBNAME).rootmap

ROCDICT_LINKDEF = LinkDef.h
ROCDICT_NAME = G__RocLib
ROCDICT_HDR = $(ROCDICT_NAME).h
ROCDICT_SRC = $(ROCDICT_NAME).cxx
ROCDICT_OBJ = $(ROCDICT_NAME).o

ROC_DEPENDENCIES += $(ROCLIB_DEPS)

all: $(ROCLIB)

clean:
	rm -f $(ROCLIB) $(ROCLIB_OBJS) $(ROCLIB_DEPS) $(ROCDICT_MAP) $(ROCDICT_NAME).*


ifdef ISROOT

$(ROCDICT_MAP): $(ROCDICT_LIB)
	@echo 'Building: $@'
	@$(ROOT_BIN)rlibmap -o $@ -l $(ROCDICT_LIB) -d $(ROCDICT_DEPLIBS) -c $(ROCDICT_LINKDEF)

$(ROCDICT_SRC): $(ROCDICT_LINKDEF) $(ROCLIB_HEADERS)
	$(ROOT_BIN)rootcint -f $@ -c -p $(ROCLIB_HEADERS) $(ROCDICT_LINKDEF)

$(ROCDICT_OBJ): CXXOPT += $(ROOT_CFLAGS)

$(ROCDICT_LIB): $(ROCDICT_OBJ)
	@echo 'Building: $@'
	@$(LD) -o $@ -shared -O $(ROCDICT_OBJ)

dict: $(ROCDICT_LIB) $(ROCDICT_MAP)

else

dict:
	echo "No root availabel, dictionary for libRoc cannot be generated"

endif



$(ROCLIB) : $(ROCLIB_OBJS)
	@echo 'Building: $@'
	@g++  -o $@ -shared -O $(ROCLIB_OBJS)


include $(ROCSYS)/config/Makefile.rules
