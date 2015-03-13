# -*- mode: cmake -*-


#message(" -- Read CTestCustom.cmake --")

# -----------------------------------------------------------
# -- Number of warnings to display
# -----------------------------------------------------------

set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "500" )

# -----------------------------------------------------------
# -- Number of errors to display
# -----------------------------------------------------------

set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS   "50" )

# -----------------------------------------------------------
# -- Warning execptions
# -----------------------------------------------------------

Set(CTEST_CUSTOM_ERROR_EXCEPTION
    ${CTEST_CUSTOM_ERROR_EXCEPTION}
    "/include/boost"
    "/boost/include"
    "/include/root"
    "/root/include"
    "boost::"
)

set(CTEST_CUSTOM_WARNING_EXCEPTION
	${CTEST_CUSTOM_WARNING_EXCEPTION}

	# -- doxygen warnings
# 	"of command \@param is not found in the argument list of" 
#	"for \\link command"
#	"for \\ref command"
#	"\\class statement"
#	"\\file statement"
#	"are not documented:"
#	"Skipping documentation"
#	"has a brief description"

 	# -- CLHEP and Pluto warnings
#        "/include/CLHEP/"
        "CLHEP"
        "PParticle.h"
        "PDataBase.h"
        "PMesh.h"
        "PStaticData.h"
        "PUtils.h"
        "PDistribution.h"
        "/pluto/"

 	# -- htrack warnings
        # should be switched on, but there are so many
#        "htrack"
        "bitfield::operator"
        "bytefield::operator"
        "bitfield operator"
        "bytefield operator"
        "DataRootObjectLIB/src/histogramData.cxx"
 

        # -- warnings from ubuntu systems which are a little to much
        # -- probably defined warn-unused-result. ignoring the result
        # -- of fgets is common practice. A work around would be to
        # -- store the return value in a dummy variable
        "ignoring return value of 'char* fgets(char*, int, FILE*)'"
        "ignoring return value of 'char* fscanf(char*, int, FILE*)'"

        # -- boost warnings
        "/include/boost/exception/exception.hpp:"
        "/include/boost/smart_ptr/detail/sp_convertible.hpp:"
        "/include/boost/smart_ptr/shared_ptr.hpp:"  
        "/include/boost"
        "/boost/include"

        # -- Root warnings when installed in installation dir
        "/include/root/"
        # -- Root warnings which should not show up in the test setup
        "/include/G__ci.h:"
        "/include/TAttImage.h:"
        "/include/TBuffer.h"
        "/include/TChainElement.h"
        "/include/TCollectionProxyInfo.h"
        "/include/TCut.h:"
        "/include/TEveBoxSet.h:"
        "/include/TEveTrackPropagator.h:"
        "/include/TEveTrackPropagator.h:"
        "/include/TEveVector.h:"
        "/include/TFcnAdapter.h:"
        "/include/TFitterMinuit.h:"
        "/include/TGeoMatrix.h:"
        "/include/TGeoPainter.h:"
        "/include/TGDMLParse.h"
        "/include/TList.h:"
        "/include/TMap.h:"
        "/include/TMatrixT.h:"
        "/include/TMatrixTSym.h:"
        "/include/TMemberInspector.h:"
        "/include/TObjArray.h:"
        "/include/TRefArray.h:"
        "/include/TString.h"
        "/include/Minuit2/BasicFunctionGradient.h:"
        "/include/Minuit2/MnUserParameterState.h:"
        "/include/Minuit2/StackAllocator.h:"
        "/include/TMVA/ClassInfo.h:"
        "/include/TMVA/Config.h:"
        "/include/TMVA/Configurable.h:"
        "/include/TMVA/DataInputHandler.h:"
        "/include/TMVA/DataSet.h:"
        "/include/TMVA/DataSetInfo.h:"
        "/include/TMVA/DataSetManager.h:"
        "/include/TMVA/Event.h:"
        "/include/TMVA/Factory.h:"
        "/include/TMVA/KDEKernel.h:"
        "/include/TMVA/Option.h:"
        "/include/TMVA/PDF.h:"
        "/include/TMVA/Reader.h:"
        "/include/TMVA/Types.h:"

        # -- Geant3 warnings
        "TGeant3.h"
        "TGeant3TGeo.h"

        # -- Errors which are filtered for the time being
        # -- MbsAPI is only a copy from elsewhere so don't know what to do
        # -- FairTSBufferFunctional.h has to be fixed by Tobias
	"MbsAPI"
	"FairTSBufferFunctional.h"
        "FairRegistry.h"
        "FairDbTableRow.h"
        "FairDbResult.h"
        "FairDbValidityRec.h"
        "Fair"
        "base/source"
        "std::binary_function"

        # -- VC warnings
        "common/undomacros.h"
        "common/macros.h"
        "common/types.h"
        "common/storage.h"
        "common/aliasingentryhelper.h"
        "common/logarithm.h"
        "common/vectortuple.h"
        "common/interleavedmemory.h"
        "avx/vectorhelper.h"
        "avx/mask.h"
        "avx/limits.h"
        "avx/vector.h"
        "avx/writemaskedvector.h"
        "avx/writemaskedvector.tcc"
        "avx/writemaskedvector.tcc"
        "sse/mask.h"
        "sse/vector.h"
        "sse/limits.h"
        "sse/types.h"
        "sse/helperimpl.h"
        "sse/vector.tcc"
        "sse/vectorhelper.h"
        "sse/vectorhelper.tcc"
        "sse/casts.h"
        "sse/shuffle.h"
        "sse/intrinsics.h"
        "Vc/Allocator"
        "Vc/global.h"

        # -- warnings from fles subdirector
        "fles/ipc"
        "fles/ctrl"
        "fles/flib_dpb"
        "fles/spadic"
        "fles/tsclient"
        "fles/softdpb"
        "L1/vectors/P4_F32vec4.h"
        "Dict.cxx"
  )

# -----------------------------------------------------------
# -- Warning addon's
# -----------------------------------------------------------
set(CTEST_CUSTOM_WARNING_MATCH	${CTEST_CUSTOM_WARNING_MATCH}
	)


Set (CTEST_CUSTOM_COVERAGE_EXCLUDE 
     ".*Dict.h"
     ".*Dict.cxx"
     ".*Fair.*"
     ".*GTest.*"
    )