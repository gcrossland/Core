#include "header.hpp"
#include <cstring>
#include <memory>

using std::tuple;
using std::vector;
using core::check;
using std::move;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int main (int argc, char *argv[]) {
  processName = argc > 0 ? argv[0] : nullptr;

  if (argc == 2) {
    char *arg = argv[1];
    if (strcmp(arg, "VersionFailure0") == 0) {
      testVersionFailure0Impl();
    } else if (strcmp(arg, "VersionFailure1") == 0) {
      testVersionFailure1Impl();
    } else if (strcmp(arg, "VersionFailure2") == 0) {
      testVersionFailure2Impl();
    } else if (strcmp(arg, "DebugLoggingToStdout") == 0) {
      testDebugLoggingToStdoutImpl();
    } else if (strcmp(arg, "DebugAssertionFailure0") == 0) {
      testDebugAssertionFailure0Impl();
    } else if (strcmp(arg, "DebugAssertionFailure1") == 0) {
      testDebugAssertionFailure1Impl();
    }
    return 0;
  }

  testVersionSuccess();
  testVersionFailure0();
  testVersionFailure1();
  testVersionFailure2();
  testIntegers();
  testDebugLoggingToStdout();
  testDebugLoggingToFile();
  testDebugAssertionSuccess();
  testDebugAssertionFailure0();
  testDebugAssertionFailure1();
  testExceptions();
  testCreateExceptionMessage();
  testCreateBitmask();
  testExtendSign();
  testShifting();
  testSetAndGet();
  testIex();
  testHashing();

  return 0;
}

const char *processName;

tuple<int, vector<std::string>> rerun (const char *arg) {
  if (!processName || processName[0] == 0) {
    check(false);
  }

  const char *stderrLeafName = ".err.txt";

  const std::string cmdLine = std::string(processName) + " " + arg + " 2>" + stderrLeafName;
  int rc = system(cmdLine.c_str());
  vector<std::string> stderrLines;

  FILE *f = fopen(stderrLeafName, "r");
  check(f);
  char bfr[1024]; // DODGY
  while (fgets(bfr, static_cast<int>(sizeof(bfr) / sizeof(*bfr)), f) != nullptr) {
    size_t chrCount = strlen(bfr);
    if (chrCount > 0 && bfr[chrCount - 1] == '\n') {
      --chrCount;
    }
    stderrLines.emplace_back(bfr, chrCount);
  }
  fclose(f);
  remove(stderrLeafName);

  return tuple<int, vector<std::string>>(move(rc), move(stderrLines));
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
