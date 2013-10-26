#include "header.hpp"
#include <cstring>
#include <memory>

using std::tuple;
using std::vector;
using std::string;
using core::check;
using std::move;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int main (int argc, char *argv[]) {
  if (argc > 0) {
    processName = argv[0];
  }

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
  testGeneralException();

  return 0;
}

const char *processName = nullptr;

tuple<int, vector<string>> rerun (const char *arg) {
  if (!processName || processName[0] == 0) {
    check(false);
  }

  const char *stderrLeafName = ".err.txt";

  const string cmdLine = string(processName) + " " + arg + " 2>" + stderrLeafName;
  int rc = system(cmdLine.c_str());
  vector<string> stderrLines;

  FILE *f = fopen(stderrLeafName, "r");
  check(f);
  char bfr[1024]; // DODGY
  while (fgets(bfr, sizeof(bfr) / sizeof(*bfr), f) != nullptr) {
    size_t chrCount = strlen(bfr);
    if (chrCount > 0 && bfr[chrCount - 1] == '\n') {
      bfr[--chrCount] = 0;
    }
    stderrLines.emplace_back(bfr, chrCount);
  }
  fclose(f);
  remove(stderrLeafName);

  return tuple<int, vector<string>>(move(rc), move(stderrLines));
}
