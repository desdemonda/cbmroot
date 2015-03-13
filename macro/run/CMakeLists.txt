# start of test CBM setups from geometry/setup
GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/run_sim.C)
GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/run_digi.C)
GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/run_reco.C)
GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/run_qa.C)
GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/check_overlaps.C)


# Define the number of events to run
Set(NumEvents 2)

# Define the different setups to be tested
List(APPEND cbm_setup sis100_hadron sis100_electron sis300_electron)

ForEach(setup IN LISTS cbm_setup)

  Set(testname ${setup}_run_sim)
  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/run/run_sim.sh ${NumEvents} \"${setup}\")
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "600")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Set(dep_sim ${testname})

  Set(testname ${setup}_run_reco)
  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/run/run_reco.sh ${NumEvents} \"${setup}\")
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "600")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  Set_Tests_Properties(${testname} PROPERTIES DEPENDS ${dep_sim})

  Set(dep_reco ${testname})

  Set(testname ${setup}_run_digi)
  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/run/run_digi.sh ${NumEvents} \"${setup}\")
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "300")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  Set_Tests_Properties(${testname} PROPERTIES DEPENDS ${dep_sim})

  Set(testname ${setup}_run_qa)
  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/run/run_qa.sh ${NumEvents} \"${setup}\")
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "300")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  Set_Tests_Properties(${testname} PROPERTIES DEPENDS ${dep_reco})

  Set(testname ${setup}_overlap)
  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/run/check_overlaps.sh \"${setup}\")
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "600")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  Set_Tests_Properties(${testname} PROPERTIES DEPENDS ${dep_sim})

EndForEach(setup IN LISTS cbm_setup)
# end of test CBM setups from geometry/setup

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/run.C)
add_test(run ${CBMROOT_BINARY_DIR}/macro/run/run.sh)
SET_TESTS_PROPERTIES(run PROPERTIES TIMEOUT "300")
SET_TESTS_PROPERTIES(run PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/checkFields.C)
add_test(checkFields ${CBMROOT_BINARY_DIR}/macro/run/checkFields.sh)
SET_TESTS_PROPERTIES(checkFields PROPERTIES TIMEOUT "60")
#SET_TESTS_PROPERTIES(run_sim PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/checkFieldMap.C)
add_test(checkFieldMap ${CBMROOT_BINARY_DIR}/macro/run/checkFieldMap.sh)
SET_TESTS_PROPERTIES(checkFieldMap PROPERTIES TIMEOUT "60")
#SET_TESTS_PROPERTIES(run_sim PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

GENERATE_ROOT_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/run/checkFieldSym.C)
add_test(checkFieldSym ${CBMROOT_BINARY_DIR}/macro/run/checkFieldSym.sh)
SET_TESTS_PROPERTIES(checkFieldSym PROPERTIES TIMEOUT "60")
#SET_TESTS_PROPERTIES(run_sim PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

Install(FILES run_sim.C run_digi.C run_reco.C run_qa.C check_overlaps.C run.C checkFields.C checkFieldMap.C checkFieldSym.C
        DESTINATION share/cbmroot/macro/run
       )
INSTALL(CODE "FILE(MAKE_DIRECTORY \${CMAKE_INSTALL_PREFIX}/share/cbmroot/macro/run/data)")
