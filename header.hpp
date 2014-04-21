#ifndef HEADER_ALREADYINCLUDED
#define HEADER_ALREADYINCLUDED

#include <vector>
#include <string>
#include <tuple>
#include "libraries/core.hpp"

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
int main (int argc, char *argv[]);
extern const char *processName;
std::tuple<int, std::vector<std::string>> rerun (const char *arg);

void testVersionSuccess ();
void testVersionFailure0 ();
void testVersionFailure0Impl ();
void testVersionFailure1 ();
void testVersionFailure1Impl ();
void testVersionFailure2 ();
void testVersionFailure2Impl ();
void testIntegers ();
void testDebugLoggingToStdout ();
void testDebugLoggingToStdoutImpl ();
void testDebugLoggingToFile ();
void testDebugAssertionSuccess ();
void testDebugAssertionFailure0 ();
void testDebugAssertionFailure0Impl ();
void testDebugAssertionFailure1 ();
void testDebugAssertionFailure1Impl ();
void testGeneralException ();
void testBuildExceptionMessage ();
void testBuildBitmask ();
void testExtendSign ();
void testShifting ();
void testSetAndGet ();
void testIex ();

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
#endif
