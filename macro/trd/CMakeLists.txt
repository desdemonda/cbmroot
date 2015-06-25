# digipar test

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/create_geometry_file_root_test.C)
add_test(trd_geo ${CBMROOT_BINARY_DIR}/macro/trd/create_geometry_file_root_test.sh)
SET_TESTS_PROPERTIES(trd_geo PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(trd_geo PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/create_digipar_root_test.C)
add_test(trd_digi_gen ${CBMROOT_BINARY_DIR}/macro/trd/create_digipar_root_test.sh)
SET_TESTS_PROPERTIES(trd_digi_gen PROPERTIES TIMEOUT "100")
SET_TESTS_PROPERTIES(trd_digi_gen PROPERTIES DEPENDS trd_geo)
SET_TESTS_PROPERTIES(trd_digi_gen PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

# 

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_sim.C)
add_test(trd_sim ${CBMROOT_BINARY_DIR}/macro/trd/trd_sim.sh)
SET_TESTS_PROPERTIES(trd_sim PROPERTIES TIMEOUT "600")
SET_TESTS_PROPERTIES(trd_sim PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_digi1.C)
add_test(trd_digi1 ${CBMROOT_BINARY_DIR}/macro/trd/trd_digi1.sh)
SET_TESTS_PROPERTIES(trd_digi1 PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(trd_digi1 PROPERTIES DEPENDS trd_sim)
SET_TESTS_PROPERTIES(trd_digi1 PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_digi2.C)
add_test(trd_digi2 ${CBMROOT_BINARY_DIR}/macro/trd/trd_digi2.sh)
SET_TESTS_PROPERTIES(trd_digi2 PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(trd_digi2 PROPERTIES DEPENDS trd_sim)
SET_TESTS_PROPERTIES(trd_digi2 PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

# 

# run benchmark only on selected nodes
# GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/trd/trd_sim_benchmark.C)
# add_test(trd_sim_benchmark ${CBMROOT_BINARY_DIR}/macro/trd/trd_sim_benchmark.sh)
# SET_TESTS_PROPERTIES(trd_sim_benchmark PROPERTIES TIMEOUT "3500")
# SET_TESTS_PROPERTIES(trd_sim_benchmark PROPERTIES DEPENDS trd_digi2)
# SET_TESTS_PROPERTIES(trd_sim_benchmark PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

