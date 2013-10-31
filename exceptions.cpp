#include "header.hpp"

using namespace core;
using std::exception;
using std::string;

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
