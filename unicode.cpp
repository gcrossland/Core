#include "header.hpp"

using core::check;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
void testUnicodeCodeUnits () {
  check(std::is_pod<char8_t>::value);
  check(sizeof(iu8f), sizeof(char8_t));
  check(alignof(iu8f), alignof(char8_t));
  check(sizeof(iu16f), sizeof(char16_t));
  check(alignof(iu16f), alignof(char16_t));
  check(sizeof(iu32f), sizeof(char32_t));
  check(alignof(iu32f), alignof(char32_t));

  auto s0 = u8("c"); // U+00063
  check(std::char_traits<char8_t>::length(s0) == 1);

  auto s1 = u8("µ"); // U+000B5
  check(std::char_traits<char8_t>::length(s1) == 2);

  auto s2 = u8("α"); // U+003B1
  check(std::char_traits<char8_t>::length(s2) == 2);

  auto s3 = u8("ℤ"); // U+02124
  check(std::char_traits<char8_t>::length(s3) == 3);

  auto s4 = u8("𝄞"); // U+1D11E
  check(std::char_traits<char8_t>::length(s4) == 4);
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
