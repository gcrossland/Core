#include "header.hpp"

using core::GeneralException;
using std::exception;
using core::check;
using std::string;
using std::runtime_error;
using core::buildExceptionMessage;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void testGeneralException () {
  try {
    throw GeneralException("cstring message");
  } catch (exception &e) {
    check(string("cstring message"), string(e.what()));
  }

  try {
    throw GeneralException(string("string message"));
  } catch (exception &e) {
    check(string("string message"), string(e.what()));
  }
}

void testBuildExceptionMessage () {
  try {
    try {
      try {
        throw runtime_error("an operation failed");
      } catch (...) {
        nthrow(GeneralException("a thing could not be computed"));
      }
    } catch (const exception &e) {
      nthrow(GeneralException("the user-requested action failed"));
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
        nthrow(GeneralException("_def"));
      }
    } catch (const exception &e) {
      nthrow(GeneralException("_abc"));
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
      nthrow(GeneralException("jk"));
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
