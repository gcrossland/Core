#include "header.hpp"
#include <cstring>

using core::UException;
using core::PlainException;
using std::exception;
using core::check;
using core::u8string;
using std::runtime_error;
using core::buildExceptionMessage;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void testExceptions () {
  try {
    throw PlainException(u8("cstring message"));
  } catch (UException &e) {
    check(u8string(u8("cstring message")), u8string(e.uWhat()));
    check(15, std::strlen(reinterpret_cast<const char *>(e.uWhat())));
  }

  try {
    throw PlainException(u8string(u8("string \u00B5essag\u1EB9")));
  } catch (UException &e) {
    check(u8string(u8("string \u00B5essag\u1EB9")), u8string(e.uWhat()));
    check(17, std::strlen(reinterpret_cast<const char *>(e.uWhat())));
  }

  const char8_t *m0 = u8("another cstring message");
  try {
    throw PlainException::create(m0);
  } catch (UException &e) {
    check(m0 == e.uWhat());
  }

  try {
    throw PlainException::create(u8("\u03B1nother %%\u03C3tring message"));
  } catch (UException &e) {
    check(u8string(u8("\u03B1nother %\u03C3tring message")), u8string(e.uWhat()));
    check(25, std::strlen(reinterpret_cast<const char *>(e.uWhat())));
  }
}

void testBuildExceptionMessage () {
  try {
    try {
      try {
        throw runtime_error("an op\xA3ration failed");
      } catch (...) {
        nthrow(PlainException(u8("a thing could not be comput\u00A3d")));
      }
    } catch (const exception &e) {
      nthrow(PlainException(u8("the user-requested action failed")));
    }
    check(false);
  } catch (const exception &e) {
    u8string wholeMsg = buildExceptionMessage(e);
    check(u8string(u8("The user-requested action failed: a thing could not be comput\u00A3d: an op?ration failed.")), wholeMsg);
  } catch (...) {
    check(false);
  }

  try {
    try {
      try {
        throw runtime_error("__ghi");
      } catch (...) {
        nthrow(PlainException(u8string(u8("_def"))));
      }
    } catch (const exception &e) {
      nthrow(PlainException(u8("_abc")));
    }
    check(false);
  } catch (const exception &e) {
    u8string wholeMsg = buildExceptionMessage(e);
    check(u8string(u8("abc: def: _ghi.")), wholeMsg);
  } catch (...) {
    check(false);
  }

  try {
    try {
      throw "lm";
    } catch (...) {
      nthrow(PlainException(u8("jk")));
    }
    check(false);
  } catch (const exception &e) {
    u8string wholeMsg = buildExceptionMessage(e);
    check(u8string(u8("Jk.")), wholeMsg);
  } catch (...) {
    check(false);
  }
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
