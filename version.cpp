#include "header.hpp"

using std::get;
using core::check;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void testVersionSuccess () {
  _dependson_(core, 1, 0);
}

void testVersionFailure0 () {
  auto result = rerun(__FUNCTION__ + 4);
  check(get<0>(result) != 0);
  check(get<1>(result).size() != 0);
}

void testVersionFailure0Impl () {
  _dependson_(core, 1, 1);
}

void testVersionFailure1 () {
  auto result = rerun(__FUNCTION__ + 4);
  check(get<0>(result) != 0);
  check(get<1>(result).size() != 0);
}

void testVersionFailure1Impl () {
  _dependson_(core, 1, 0);
  _dependson_(core, 2, 0);
}

void testVersionFailure2 () {
  auto result = rerun(__FUNCTION__ + 4);
  check(get<0>(result) != 0);
  check(get<1>(result).size() != 0);
}

void testVersionFailure2Impl () {
  _dependson_(core, 0, 9999);
  _dependson_(core, 0, 9);
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
