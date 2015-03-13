GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/mvd/qa/mvd_qa1_transUrqmd.C)
add_test(mvd_qa1_transUrqmd ${CBMROOT_BINARY_DIR}/macro/mvd/qa/mvd_qa1_transUrqmd.sh)
SET_TESTS_PROPERTIES(mvd_qa1_transUrqmd PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(mvd_qa1_transUrqmd PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/mvd/qa/mvd_qa2_transDelta.C)
add_test(mvd_qa2_transDelta ${CBMROOT_BINARY_DIR}/macro/mvd/qa/mvd_qa2_transDelta.sh)
SET_TESTS_PROPERTIES(mvd_qa2_transDelta PROPERTIES TIMEOUT "500")
SET_TESTS_PROPERTIES(mvd_qa2_transDelta PROPERTIES DEPENDS mvd_qa1_transUrqmd)
SET_TESTS_PROPERTIES(mvd_qa2_transDelta PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/mvd/qa/mvd_qa3_digitize.C)
add_test(mvd_qa3_digitize ${CBMROOT_BINARY_DIR}/macro/mvd/qa/mvd_qa3_digitize.sh)
SET_TESTS_PROPERTIES(mvd_qa3_digitize PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(mvd_qa3_digitize PROPERTIES DEPENDS mvd_qa2_transDelta)
SET_TESTS_PROPERTIES(mvd_qa3_digitize PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

