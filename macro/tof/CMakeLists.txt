GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/tof/digitizer/run_tof_disim.C)
add_test(tof_disim ${CBMROOT_BINARY_DIR}/macro/tof/digitizer/run_tof_disim.sh)
SET_TESTS_PROPERTIES(tof_disim PROPERTIES TIMEOUT "600")
SET_TESTS_PROPERTIES(tof_disim PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/tof/digitizer/run_digi_test.C)
add_test(tof_digi1 ${CBMROOT_BINARY_DIR}/macro/tof/digitizer/run_digi_test.sh)
SET_TESTS_PROPERTIES(tof_digi1 PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(tof_digi1 PROPERTIES DEPENDS tof_disim)
SET_TESTS_PROPERTIES(tof_digi1 PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

### To do later: run with different digitizer/clusterizer options
#GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/tof/digitizer/tof_digi2.C)
#add_test(tof_digi2 ${CBMROOT_BINARY_DIR}/macro/tof/digitizer/tof_digi2.sh)
#SET_TESTS_PROPERTIES(tof_digi2 PROPERTIES TIMEOUT "300")
#SET_TESTS_PROPERTIES(tof_digi2 PROPERTIES DEPENDS tof_disim)
#SET_TESTS_PROPERTIES(tof_digi2 PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")


