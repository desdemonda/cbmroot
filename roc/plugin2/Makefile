include $(DABCSYS)/config/Makefile.config

## normally should be like this for every module, but can be specific

ifndef ROCSYS
ROCSYS=..
endif

ROC_EXTRALIBS = 

ROCDIRI         = roc
ROCDIRS         = src

ifdef ROCTGTDIR
ROCPLUGIN_TGTDIR     = $(ROCTGTDIR)/lib
else
ROCPLUGIN_TGTDIR     = $(TGTDLLPATH)
endif

ROC_LIBNAME     = $(LIB_PREFIX)DabcRoc
ROC_LIB         = $(ROCPLUGIN_TGTDIR)/$(ROC_LIBNAME).$(DllSuf)

## must be similar for every module

ROC_H           = $(wildcard $(ROCDIRI)/*.$(HedSuf))
ROC_S           = $(wildcard $(ROCDIRS)/*.$(SrcSuf))
ROC_O           = $(patsubst %.$(SrcSuf), $(BLD_DIR)/%.$(ObjSuf), $(ROC_S))
ROC_D           = $(patsubst %.$(SrcSuf), $(BLD_DIR)/%.$(DepSuf), $(ROC_S))

# used in the main Makefile

ALLDEPENDENC    += $(ROC_D)

libs:: $(ROC_LIB) 

##### local rules #####

$(ROC_O) $(ROC_D) : INCLUDES += $(ROCSYS)/roclib . 

$(ROC_LIB):  $(ROC_O)
	@$(MakeLib) $(ROC_LIBNAME) "$(ROC_O)" $(ROCPLUGIN_TGTDIR) "-L$(ROCSYS)/lib -lRoc"

########### extra rules #############

include $(DABCSYS)/config/Makefile.rules
