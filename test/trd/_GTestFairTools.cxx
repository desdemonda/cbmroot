#include "CbmTrdGeoHandler.h"

#include "TFile.h"
#include "TGeoManager.h"
#include "TString.h"
#include "TSystem.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <boost/regex.hpp>



// Base class to use the same basic setup for parameterized and
// non-parameterized tests
// Here one defines everything which is common for all the different
// test cases
template <class T> class _TestTrdGeoHandlerBase : public T
{
  protected:

  TFile* fInputFile;
  TString fFileName;

  virtual void SetUp() {

    TString fFileName = gSystem->Getenv("VMCWORKDIR");
    fFileName += "/input/qa/trd_squared_segmented_geometry.root";
    fInputFile = new TFile(fFileName,"READ");
  }

};

// This is the derived class for the non-parameterized test cases.
class  TrdGeoHandlerTest : public _TestTrdGeoHandlerBase<testing::Test> {};

// This is the derived class for the parameterized test cases.
//class TrdGeoHandlerParamTest : public _TestTrdGeoHandlerBase<
//  testing::TestWithParam<const char*> >
//{
//  protected:
//    virtual void SetUp() {
//      logLevelSettingToTest=GetParam();
//      OutputString = "I am here.";
//      fLogger = FairLogger::GetLogger();
//      SetFairLoggerDefaultValues();
//    }
//};

//class VerbosityLevelTest : public _TestFairLoggerBase<
//  testing::TestWithParam<const char*> >
//{
//  protected:
//    virtual void SetUp() {
//      logLevelSettingToTest="INFO";
//      verbosityLevel=GetParam();
//      OutputString = "I am here.";
//      fLogger = FairLogger::GetLogger();
//      SetFairLoggerDefaultValues();
//    }
//    std::string verbosityLevel;
//};

TEST_F(TrdGeoHandlerTest, CheckDefaultSettings)
{
  cout << "Filename: " << fFileName << endl;
  fInputFile->Print();  
}


//TEST_F(FairToolsTest, CheckOutputOnlyToFile)
//{
//  handler.BeginCapture();
//
//  char fileName[25];
//  tmpnam(fileName);
//  fLogger->SetLogFileName(fileName);
//  fLogger->SetLogToFile(true);
//  fLogger->SetLogToScreen(false);
//  LogNoArguments(fLogger, OutputString);
//
//  handler.EndCapture();
//
//  std::vector<std::string> v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString);
//  FairTestOutputHandler outputhandler(fileName);
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckFileOutput(outputhandler, v);
//  }
//}
//
//
//TEST_F(FairToolsTest, CheckWrongLogLevelSettings)
//{
//
//  fLogger->SetLogToFile(false);
//  fLogger->SetLogToScreen(true);
//
//  handler.BeginCapture();
//  fLogger->SetLogScreenLevel("BLA");
//  LogNoArguments(fLogger, OutputString);
//  handler.EndCapture();
//
//  std::vector<std::string> v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString);
//  std::string outString="Log level \"BLA\" not supported. Use default level \"INFO\".";
//  std::vector<std::string>::iterator it;
//  it = v.begin();
//  it = v.insert ( it , outString );
//
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckScreenOutput(handler, v);
//  }
//}
//
//TEST_F(FairToolsTest, CheckVerbosityLevelSettings)
//{
//
//  fLogger->SetLogToFile(false);
//  fLogger->SetLogToScreen(true);
//
//  handler.BeginCapture();
//  fLogger->SetLogVerbosityLevel("BLA");
//  LogNoArguments(fLogger, OutputString);
//  handler.EndCapture();
//
//  std::vector<std::string> v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString);
//  std::string outString="Verbosity level \"BLA\" not supported. Use default level \"LOW\".";
//  std::vector<std::string>::iterator it;
//  it = v.begin();
//  it = v.insert ( it , outString );
//
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckScreenOutput(handler, v);
//  }
//
//}
//
//
//TEST_F(FairToolsTest, testScreenAndFileOutputWithoutArgument)
//{
//  handler.BeginCapture();
//
//  char fileName[25];
//  tmpnam(fileName);
//  fLogger->SetLogFileName(fileName);
//  fLogger->SetLogToScreen(true);
//  fLogger->SetLogToFile(true);
//  LogNoArguments(fLogger, OutputString);
//
//  handler.EndCapture();
//
//  std::vector<std::string> v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString, fileName);
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckScreenOutput(handler, v);
//  }
//
//  v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString);
//  FairTestOutputHandler outputhandler(fileName);
//
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckFileOutput(outputhandler, v);
//  }
//}
//
//
//
//TEST_P(LogLevelTest, testAllLogLevelsToScreenAndFile)
//{
//  fLogger->SetLogFileLevel(logLevelSettingToTest.c_str());
//  fLogger->SetLogScreenLevel(logLevelSettingToTest.c_str());
//
//  FairCaptureOutput handler(true, false);
//  handler.BeginCapture();
//  char fileName[25];
//  tmpnam(fileName);
//  fLogger->SetLogFileName(fileName);
//  fLogger->SetLogToScreen(true);
//  fLogger->SetLogToFile(true);
//  LogNoArguments(fLogger, OutputString);
//  handler.EndCapture();
//
//  std::vector<std::string> v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString, fileName);
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckScreenOutput(handler, v);
//  }
//
//  v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString);
//  FairTestOutputHandler outputhandler(fileName);
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckFileOutput(outputhandler, v);
//  }
//}
//
//INSTANTIATE_TEST_CASE_P(TestAllLogLevels,
//                        LogLevelTest,
//                        ::testing::ValuesIn(TestLogs));
//
//TEST_P(VerbosityLevelTest, testAllVerbosityLevelsToScreenAndFile)
//{
//  fLogger->SetLogVerbosityLevel(verbosityLevel.c_str());
//
//  handler.BeginCapture();
//
//  //  char fileName[25];
//  //  tmpnam(fileName);
//  //  fLogger->SetLogFileName(fileName);
//  fLogger->SetLogToScreen(true);
//  fLogger->SetLogToFile(false);
//  LogNoArguments(fLogger, OutputString);
//
//  handler.EndCapture();
//
//  //  std::vector<std::string> v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString, fileName);
//  //  std::vector<std::string> v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString);
//  std::vector<std::string> v = CreateExpectedLogLevels(logLevelSettingToTest);
//
//
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    int Lines = v.size();
//
//    for (int i = 0; i < Lines; ++i) {
//      EXPECT_TRUE( CheckVerboseOutput( v[i], OutputString, verbosityLevel, handler.GetCaptureLine(i) ) );
//    }
//  }
//
//  /*
//  v = CreateExpectedOutputNoArguments(logLevelSettingToTest, OutputString);
//  FairTestOutputHandler outputhandler(fileName);
//  {
//    SCOPED_TRACE(logLevelSettingToTest);
//    CheckFileOutput(outputhandler, v);
//  }
//  */
//}
//
//INSTANTIATE_TEST_CASE_P(TestAllVerbosityLevels,
//                        VerbosityLevelTest,
//                        ::testing::ValuesIn(VerbosityLevelArray));
