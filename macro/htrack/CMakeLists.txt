GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/htrack/runProduceFile.C)
add_test(runProducer ${CBMROOT_BINARY_DIR}/macro/htrack/runProduceFile.sh)
SET_TESTS_PROPERTIES(runProducer PROPERTIES TIMEOUT "225")
SET_TESTS_PROPERTIES(runProducer PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

# 2014-07-03 disable test of runHoughStsTrackFinder.C, it fails since 2014-02-18
#GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/htrack/runHoughStsTrackFinder.C)
#add_test(runHough ${CBMROOT_BINARY_DIR}/macro/htrack/runHoughStsTrackFinder.sh)
#SET_TESTS_PROPERTIES(runHough PROPERTIES TIMEOUT "600")
#SET_TESTS_PROPERTIES(runHough PROPERTIES DEPENDS runProducer)
#SET_TESTS_PROPERTIES(runHough PROPERTIES PASS_REGULAR_EXPRESSION "TestPassed;All ok")

