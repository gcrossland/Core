#include "header.hpp"

using std::get;
using core::check;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void testVersionSuccess () {
  dependson(core, 1, 0);
}

void testVersionFailure0 () {
  auto result = rerun(__FUNCTION__ + 4);
  check(get<0>(result) != 0);
  check(get<1>(result).size() != 0);
}

void testVersionFailure0Impl () {
  dependson(core, 1, 1);
}

void testVersionFailure1 () {
  auto result = rerun(__FUNCTION__ + 4);
  check(get<0>(result) != 0);
  check(get<1>(result).size() != 0);
}

void testVersionFailure1Impl () {
  dependson(core, 1, 0);
  dependson(core, 2, 0);
}

void testVersionFailure2 () {
  auto result = rerun(__FUNCTION__ + 4);
  check(get<0>(result) != 0);
  check(get<1>(result).size() != 0);
}

void testVersionFailure2Impl () {
  dependson(core, 0, 99999);
  dependson(core, 0, 9);
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
