#include "header.hpp"

using core::PlainException;
using std::exception;
using core::check;
using std::string;
using std::runtime_error;
using core::buildExceptionMessage;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void testExceptions () {
  try {
    throw PlainException("cstring message");
  } catch (exception &e) {
    check(string("cstring message"), string(e.what()));
  }

  try {
    throw PlainException(string("string message"));
  } catch (exception &e) {
    check(string("string message"), string(e.what()));
  }

  const char *m0 = "another cstring message";
  try {
    throw PlainException::create(m0);
  } catch (exception &e) {
    check(m0 == e.what());
  }

  try {
    throw PlainException::create("another %%string message");
  } catch (exception &e) {
    check(string("another %string message"), string(e.what()));
  }
}

void testBuildExceptionMessage () {
  try {
    try {
      try {
        throw runtime_error("an operation failed");
      } catch (...) {
        nthrow(PlainException("a thing could not be computed"));
      }
    } catch (const exception &e) {
      nthrow(PlainException("the user-requested action failed"));
    }
    check(false);
  } catch (const exception &e) {
    string wholeMsg = buildExceptionMessage(e);
    check(string("The user-requested action failed: a thing could not be computed: an operation failed."), wholeMsg);
  } catch (...) {
    check(false);
  }

  try {
    try {
      try {
        throw runtime_error("__ghi");
      } catch (...) {
        nthrow(PlainException(string("_def")));
      }
    } catch (const exception &e) {
      nthrow(PlainException("_abc"));
    }
    check(false);
  } catch (const exception &e) {
    string wholeMsg = buildExceptionMessage(e);
    check(string("abc: def: _ghi."), wholeMsg);
  } catch (...) {
    check(false);
  }

  try {
    try {
      throw "lm";
    } catch (...) {
      nthrow(PlainException("jk"));
    }
    check(false);
  } catch (const exception &e) {
    string wholeMsg = buildExceptionMessage(e);
    check(string("Jk."), wholeMsg);
  } catch (...) {
    check(false);
  }
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
