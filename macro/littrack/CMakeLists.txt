GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test.sh)

# Define the different setups to be tested
List(APPEND  setup  electron  muon)

ForEach(__setup IN  LISTS  setup)
  File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/commit_tests/events_${__setup})
  File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/commit_tests/results_${__setup})

  Set(testname global_sim_${__setup})

  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test.sh ${__setup} sim)
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "420")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Set(_dep ${testname})
  Set(testname global_reco_${__setup})
  
  Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test.sh ${__setup} reco)
  Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "420")
  Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  Set_Tests_Properties(${testname} PROPERTIES DEPENDS ${_dep})
 
EndForEach(__setup IN LISTS setup)

GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/commit_tests/run_test_mvd.sh)

List(APPEND  setup_mvd  delta  no_delta)

ForEach(__setup_mvd IN  LISTS  setup_mvd)
  File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/commit_tests/events_mvd_${__setup_mvd})
  File(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/commit_tests/results_mvd_${__setup_mvd})

  Set(testname_mvd mvd_sim_${__setup_mvd})

  Add_Test(${testname_mvd} ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_mvd.sh ${__setup_mvd} sim)
  Set_Tests_Properties(${testname_mvd} PROPERTIES TIMEOUT "420")
  Set_Tests_Properties(${testname_mvd} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
  Set(_dep ${testname_mvd})
  Set(testname_mvd mvd_reco_${__setup_mvd})
  
  Add_Test(${testname_mvd} ${CBMROOT_BINARY_DIR}/macro/littrack/commit_tests/run_test_mvd.sh ${__setup_mvd} reco)
  Set_Tests_Properties(${testname_mvd} PROPERTIES TIMEOUT "420")
  Set_Tests_Properties(${testname_mvd} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  Set_Tests_Properties(${testname_mvd} PROPERTIES DEPENDS ${_dep})
 
EndForEach(__setup_mvd IN LISTS setup_mvd)


# Additional nightly tests
If(BUILD_LITTRACK_EXPERIMENTAL)
  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_reco.sh)
  
  List(APPEND electron_geo v13p_3e)
  List(APPEND electron_collision auau_25gev_centr auau_25gev_mbias) 
  ForEach(geo IN  LISTS  electron_geo)
    ForEach(collision IN  LISTS  electron_collision)
      Set(testname littrack_global_reco_electron_${geo}_${collision})
      Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh ${testname} electron ${geo} ${collision})
      Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "54000")
      Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
    EndForEach(collision IN  LISTS  electron_collision)
  EndForEach(geo IN  LISTS  electron_geo)
  
  
  List(APPEND muon_geo  v13f)
  List(APPEND muon_collision auau_25gev_centr auau_25gev_mbias) 
  ForEach(geo IN  LISTS  muon_geo)
    ForEach(collision IN  LISTS  muon_collision)
      Set(testname littrack_global_reco_muon_${geo}_${collision})
      Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_reco.sh ${testname} muon ${geo} ${collision})
      Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "54000")
      Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
    EndForEach(collision IN  LISTS  muon_collision)
  EndForEach(geo IN  LISTS  muon_geo)
  
  
  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh)
  
  List(APPEND mvd_delta delta no_delta) 
  List(APPEND mvd_collision auau_25gev_centr auau_25gev_mbias) 
  ForEach(delta IN  LISTS  mvd_delta)
    ForEach(collision IN  LISTS  mvd_collision)
      Set(testname littrack_mvd_reco_${delta}_${collision})
      Add_Test(${testname} ${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_mvd.sh ${testname} ${collision} ${delta})
      Set_Tests_Properties(${testname} PROPERTIES TIMEOUT "54000")
      Set_Tests_Properties(${testname} PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
    EndForEach(collision IN  LISTS  mvd_collision)
  EndForEach(delta IN  LISTS  mvd_delta)
  
  
#  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_field.sh)
#  Add_Test(littrack_field "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_field.sh")
#  Set_Tests_Properties(littrack_field PROPERTIES TIMEOUT "3600")
#  Set_Tests_Properties(littrack_field PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")
  
#  GENERATE_TEST_SCRIPT(${CBMROOT_SOURCE_DIR}/macro/littrack/nightly_tests/run_test_fieldapr.sh)
#  Add_Test(littrack_fieldapr "${CBMROOT_BINARY_DIR}/macro/littrack/nightly_tests/run_test_fieldapr.sh")
#  Set_Tests_Properties(littrack_fieldapr PROPERTIES TIMEOUT "3600")
#  Set_Tests_Properties(littrack_fieldapr PROPERTIES PASS_REGULAR_EXPRESSION "Test Passed;All ok")

EndIf(BUILD_LITTRACK_EXPERIMENTAL)
