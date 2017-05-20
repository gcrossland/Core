#include "header.hpp"
#include <memory>

using std::vector;
using std::get;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void testDebugLoggingToStdout () {
  auto result = rerun(__FUNCTION__ + 4);
  core::check(0, get<0>(result));

  #ifndef NDEBUG
  const vector<std::string> &stderrLines = get<1>(result);
  const vector<std::string> expectedLines{
    "line 100",
    ">>testSimpleNesting",
    "  line 102",
    "  line 104",
    "  line 106",
    "<<testSimpleNesting",
    "line 108",
    ">>testSimpleNesting",
    "  line 110",
    "<<testSimpleNesting",
    "line 112",
    "line 114",
    "line 200",
    "line 201",
    ">>testSimpleNesting",
    "  line 202",
    "  line 203",
    "  >>testSimpleNesting",
    "    line 204",
    "    line 205",
    "  <<testSimpleNesting",
    "  line 206",
    "  line 207",
    "<<testSimpleNesting",
    ">>testSimpleNesting",
    "  line 208",
    "  line 209",
    "  >>testSimpleNesting",
    "    line 210",
    "    line 211",
    "  <<testSimpleNesting",
    "  line 212",
    "  line 213",
    "<<testSimpleNesting",
    "line 214",
    "line 215",
    "line 300",
    "line 301",
    ">>testSimpleNesting",
    "  line 302",
    "line 303",
    ">>testSimpleNesting",
    "  line 304",
    "  line 305",
    "<<testSimpleNesting",
    "  line 306",
    "line 307",
    "<<testSimpleNesting",
    ">>testSimpleNesting",
    "line 308",
    "  line 309",
    ">>testSimpleNesting",
    "  line 310",
    "  line 311",
    "<<testSimpleNesting",
    "line 312",
    "  line 313",
    "<<testSimpleNesting",
    "line 314",
    "line 315",
    "line 400",
    "line 401",
    ">>testDebugLoggingToStdoutImpl",
    "  line 402",
    "  line 403",
    "  line 404",
    "<<testDebugLoggingToStdoutImpl",
    "line 406",
    ">>testDebugLoggingToStdoutImpl",
    "  line 500",
    "  line 502",
    "  line 504",
    "  line 505",
    "  line 506",
    "  line 507",
    "<<testDebugLoggingToStdoutImpl"
  };
  core::check(expectedLines.begin(), expectedLines.end(), stderrLines.begin(), stderrLines.end());
  #endif
}

#ifndef NDEBUG
typedef core::debug::Logger Logger_;
#else
typedef bool Logger_;
#endif

void testSimpleNesting (int lnPrefix, Logger_ &__dl_A, Logger_ &__dl_B) {
  DW(A, "line ", lnPrefix, "00");
  DW(B, "line ", lnPrefix, "01");

  {
    DS(A);
    DW(A, "line ", lnPrefix, "02");
    DW(B, "line ", lnPrefix, "03");
    {
      DS(B);
      DW(A, "line ", lnPrefix, "04");
      DW(B, "line ", lnPrefix, "05");
    }
    DW(A, "line ", lnPrefix, "06");
    DW(B, "line ", lnPrefix, "07");
  }

  {
    DS(B);
    DW(A, "line ", lnPrefix, "08");
    DW(B, "line ", lnPrefix, "09");
    {
      DS(A);
      DWP(A, "line "); DW(A, lnPrefix, "10");
      DWP(B, "line "); DW(B, lnPrefix, "11");
    }
    DWP(A, "line ", lnPrefix); DW(A, "12");
    DWP(B, "line ", lnPrefix); DW(B, "13");
  }

  DWP(A, "line "); DWP(A, lnPrefix); DW(A, "14");
  DWP(B, "line "); DWP(B, lnPrefix); DW(B, "15");
}

void testDebugLoggingToStdoutImpl () {
  // Test two loggers, neither open.
  DC(A);
  DC(B);
  testSimpleNesting(0, __dl_A, __dl_B);

  // Test two loggers, one open to an stderr Stream.
  DI(std::shared_ptr<core::debug::Stream> str0(new core::debug::Stream);)
  DOPEN(A, str0);
  testSimpleNesting(1, __dl_A, __dl_B);

  // Test two loggers, both open to an stderr Stream.
  DOPEN(B, str0);
  testSimpleNesting(2, __dl_A, __dl_B);

  // Test two loggers, both open to different stderr Streams.
  DI(std::shared_ptr<core::debug::Stream> str1(new core::debug::Stream);)
  DOPEN(B, str1);
  testSimpleNesting(3, __dl_A, __dl_B);

  // Test scopes vs. closing (with two loggers, both open to an stderr Stream).
  DOPEN(B, str0);
  DW(A, "line 400");
  DW(B, "line 401");
  {
    DS(B);
    DW(A, "line 402");
    DW(B, "line 403");
    DCLOSE(B);
    DW(A, "line 404");
    DW(B, "line 405");
  }
  DW(A, "line 406");
  DW(B, "line 407");

  // Test scopes vs. opening (with two loggers, both open to an stderr Stream).
  {
    DS(A);
    DW(A, "line 500");
    DW(B, "line 501");
    {
      DS(B);
      DW(A, "line 502");
      DW(B, "line 503");
      DOPEN(B, str0);
      DW(A, "line 504");
      DW(B, "line 505");
    }
    DW(A, "line 506");
    DW(B, "line 507");
  }
}

void testDebugLoggingToFile () {
  const char *logLeafName = ".testDebugLoggingToFile_log.txt";
  remove(logLeafName);

  {
    DC(A);
    DOPEN(A, std::shared_ptr<core::debug::Stream>(new core::debug::Stream(logLeafName)));

    DW(A, "line 00");
    {
      DS(A);
      DW(A, u8("line 01"));
      DCLOSE(A);
      DW(A, "line 02");
    }
    DW(A, "line 03");
  }

  FILE *f = fopen(logLeafName, "r");
  #ifndef NDEBUG
  core::check(f);
  char bfr[1024];
  size_t charCount = fread(bfr, sizeof(*bfr), sizeof(bfr) / sizeof(*bfr), f);
  core::check(ferror(f) == 0);
  fclose(f);
  remove(logLeafName);
  core::check(std::string("line 00\n>>testDebugLoggingToFile\n  \xEF\xBB\xBFline 01\xEF\xBB\xBF\n<<testDebugLoggingToFile\n"), std::string(bfr, charCount));
  #else
  core::check(!f);
  #endif
}

void testDebugAssertionSuccess () {
  DPRE(true);
  DA(true);
}

void testDebugAssertionFailure0 () {
  auto result = rerun(__FUNCTION__ + 4);
  #ifndef NDEBUG
  core::check(get<0>(result) != 0);

  bool foundMessage = false;
  for (const auto &line : get<1>(result)) {
    if (line.find("the failure message") != std::string::npos) {
      foundMessage = true;
      break;
    }
  }
  core::check(foundMessage);
  #else
  core::check(0, get<0>(result));
  #endif
}

void testDebugAssertionFailure0Impl () {
  DPRE(false, "the failure", " message");
}

void testDebugAssertionFailure1 () {
  auto result = rerun(__FUNCTION__ + 4);
  #ifndef NDEBUG
  core::check(get<0>(result) != 0);

  bool foundMessage = false;
  for (const auto &line : get<1>(result)) {
    if (line.find("message 0f failure") != std::string::npos) {
      foundMessage = true;
      break;
    }
  }
  core::check(foundMessage);
  #else
  core::check(0, get<0>(result));
  #endif
}

void testDebugAssertionFailure1Impl () {
  DA(1 != 1, "message ", 0, "f failure");
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
