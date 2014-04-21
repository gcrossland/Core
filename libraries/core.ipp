#include "core.using"

namespace core {

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
#ifndef NDEBUG
namespace debug {

template<typename _i> iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value, void) Stream::writeElement (_i value) noexcept {
  if (!handle) dieHard();

  const char *f = "%llu";
  #ifdef OS_WIN32
  DSA(sizeof(unsigned long long) == 8, "value is not 64-bit");
  f = "%I64u";
  #endif
  int r = fprintf(handle, f, static_cast<unsigned long long>(value));
  if (r < 0) dieHard("failed to write to stream\n");
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_signed<_i>::value, void) Stream::writeElement (_i value) noexcept {
  if (!handle) dieHard();

  const char *f = "%lld";
  #ifdef OS_WIN32
  DSA(sizeof(signed long long) == 8, "value is not 64-bit");
  f = "%I64d";
  #endif
  int r = fprintf(handle, f, static_cast<signed long long>(value));
  if (r < 0) dieHard("failed to write to stream\n");
}

template<typename ..._Ts> void Logger::write (_Ts... ts) noexcept {
  if (!stream) {
    return;
  }

  stream->startLine();
  writeElements(ts...);
  stream->flush();
}

template<typename ..._Ts> void Logger::writeLine (_Ts... ts) noexcept {
  if (!stream) {
    return;
  }

  stream->startLine();
  writeElements(ts...);
  stream->endLine();
  stream->flush();
}

template<typename _T0, typename ..._Ts> void Logger::writeElements (_T0 t0, _Ts... ts) noexcept {
  stream->writeElement(t0);
  writeElements(ts...);
}

template<typename ..._Ts> void assertImpl (const char *file, int line, bool cond, _Ts... ts) noexcept {
  if (!cond) {
    Logger l;
    l.open(std::shared_ptr<Stream>(new Stream));
    l.write("assertion failed at ", file, ":", line);
    if (sizeof...(_Ts) != 0) {
      l.write(" - ", ts...);
    }
    l.writeLine();
    l.close();

    dieHard();
  }
}

}
#endif

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
template<typename _T0, typename _T1> void check (const _T0 &expected, const _T1 &actual) noexcept {
  if (expected != actual) {
    dieHard("test gave unexpected value\n");
  }
}

template<typename _InputIterator0, typename _InputIterator1> void check (_InputIterator0 expectedI, _InputIterator0 expectedEnd, _InputIterator1 actualI, _InputIterator1 actualEnd) {
  size_t count = 0;
  while (true) {
    bool expectedEnded = (expectedI == expectedEnd);
    bool actualEnded = (actualI == actualEnd);
    if (expectedEnded != actualEnded) {
      dieHard(expectedEnded ? "test expected value list shorter than actual\n" : "test actual value list shorter than expected\n");
    }
    if (expectedEnded) {
      break;
    }

    if (*(expectedI++) != *(actualI++)) {
      fprintf(stderr, "test gave unexpected value (at list index %d)\n", static_cast<int>(count));
      fflush(stderr);
      dieHard();
    }
    count++;
  }
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
#ifdef ARCH_TWOCINTS
template<typename _i> _i buildBitmask (iu index) noexcept {
  DPRE(index <= numeric_limits<_i>::bits, "index must be within range for the type _i");

  return static_cast<_i>(sl<_i>(1, index) - 1);
}
#endif

#ifdef ARCH_TWOCINTS
template<typename _i> _i extendSign (_i value, iu index) noexcept {
  DSPRE(numeric_limits<_i>::is_signed, "_i must be a signed type");
  DPRE(index < numeric_limits<_i>::bits, "index must be within range for the type _i");
  DPRE(index == numeric_limits<_i>::bits - 1 || static_cast<typename std::make_unsigned<_i>::type>(value) >> (index + 1) == 0, "value must be zero above the sign bit");

  return static_cast<_i>(value | ~((value & (static_cast<_i>(1) << index)) - 1));
}
#endif

template<typename _i> _i sl (_i value, iu sh) noexcept {
  #ifndef ARCH_OORLEFTSHIFT
  if (sh >= numeric_limits<_i>::bits) {
    return 0;
  }
  #endif

  return static_cast<_i>(value << sh);
}

#ifdef ARCH_TWOCINTS
template<typename _i> iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value, _i) sr (_i value, iu sh) noexcept {
  #ifndef ARCH_OORRIGHTSHIFT
  if (sh >= numeric_limits<_i>::bits) {
    return 0;
  }
  #endif

  return static_cast<_i>(value >> sh);
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_signed<_i>::value, _i) sr (_i value, iu sh) noexcept {
  #ifndef ARCH_OORRIGHTSHIFT
  if (sh >= numeric_limits<_i>::bits) {
    _i topBit = (value >> (numeric_limits<_i>::bits - 1)) & 0x1;
    return static_cast<_i>(~(topBit - 1));
  }
  #endif

  #ifdef ARCH_SIGNEDRIGHTSHIFT_ARITH
  return static_cast<_i>(value >> sh);
  #else
  return extendSign<_i>(static_cast<_i>(value >> sh), numeric_limits<_i>::bits - 1 - sh);
  #endif
}
#endif

template<typename _i> iff(std::is_integral<_i>::value, void) set (iu8f *ptr, _i value) noexcept {
  #ifdef ARCH_LOOSEALIGNMENT
  *reinterpret_cast<_i *>(ptr) = value;
  #else
  memcpy(reinterpret_cast<void *>(ptr), reinterpret_cast<void *>(&value), sizeof(_i));
  #endif
}

template<typename _i> iff(std::is_integral<_i>::value, _i) get (const iu8f *ptr) noexcept {
  #ifdef ARCH_LOOSEALIGNMENT
  return *reinterpret_cast<const _i *>(ptr);
  #else
  _i value;
  memcpy(reinterpret_cast<void *>(&value), reinterpret_cast<const void *>(ptr), sizeof(_i));
  return value;
  #endif
}

template<typename _i, bool _useSignedFormat> void setIex (iu8f *&r_ptr, _i value, bool isNegative) noexcept {
  DS();
  DSPRE(std::is_integral<_i>::value && std::is_unsigned<_i>::value, "_i must be an unsigned type");
  DW(, "writing ", value, " - signed? ", _useSignedFormat, " - negative? ", isNegative);
  const _i lastOctetMask = _useSignedFormat ? 0x3F : 0x7F;
  do {
    if ((value & ~lastOctetMask) == 0) {
      iu8f octet = static_cast<iu8f>(value);
      if (_useSignedFormat) {
        octet |= static_cast<iu8f>(static_cast<iu8f>(isNegative) << 6);
      }
      DW(, "final octet is ", octet);
      *(r_ptr++) = octet;
      return;
    }

    *(r_ptr++) = (static_cast<iu8f>(value) & 0x7F) | 0x80;
    value = static_cast<_i>(value >> 7);
    DW(, "next octet is ", *(r_ptr - 1), " - remainder of value is ", value);
  } while (true);
}

template<typename _i, bool _useSignedFormat> std::tuple<_i, bool> getIex (const iu8f *&r_ptr, const iu8f *ptrEnd)
{
  DS();
  DSPRE(std::is_integral<_i>::value && std::is_unsigned<_i>::value, "_i must be an unsigned type");
  DPRE(r_ptr <= ptrEnd);
  DW(, "reading ", _useSignedFormat ? "signed" : "unsigned", " value");

  _i value = 0;
  iu valueIndex = 0;

  do {
    if (r_ptr == ptrEnd) {
      DW(, "ran out of buffer just before reading");
      throw PlainException(std::string(_useSignedFormat ? "signed" : "unsigned") + " external integer was truncated");
    }

    iu8f octet = *(r_ptr++);
    DW(, "read octet ", octet);

    if ((octet & 0x80) == 0) {
      bool isNegative;
      if (_useSignedFormat) {
        isNegative = ((octet & 0x40) != 0);
        octet = octet & 0x3F;
      } else {
        isNegative = false;
      }

      is topBitIndex = numeric_limits<_i>::bits - 1 - static_cast<is>(valueIndex);
      if (topBitIndex < 0 || (topBitIndex < 7 && (octet >> (topBitIndex + 1)) != 0)) {
        DW(, "overflow has occurred");
        throw std::overflow_error(std::string(_useSignedFormat ? "signed" : "unsigned") + " external integer was too big");
      }

      value |= static_cast<_i>(static_cast<_i>(octet) << valueIndex);
      DW(, "final value is ", value, " - negative? ", isNegative);
      return std::tuple<_i, bool>(value, isNegative);
    }

    value = value | sl(static_cast<_i>(octet & 0x7F), valueIndex);
    valueIndex += 7;
    DW(, "new state of value is ", value, " (with valueIndex ", valueIndex, ")");
  } while (true);
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value, void) setIeu (iu8f *&r_ptr, _i value) noexcept {
  setIex<_i, false>(r_ptr, value, false);
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_signed<_i>::value, void) setIes (iu8f *&r_ptr, _i value) noexcept {
  typename std::make_unsigned<_i>::type mag;
  bool isNegative;
  if (value < 0) {
    mag = static_cast<decltype(mag)>(-value);
    isNegative = true;
  } else {
    mag = static_cast<decltype(mag)>(value);
    isNegative = false;
  }
  setIex<decltype(mag), true>(r_ptr, mag, isNegative);
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value, _i) getIeu (const iu8f *&r_ptr, const iu8f *ptrEnd) {
  return std::get<0>(getIex<_i, false>(r_ptr, ptrEnd));
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value, _i) getIeu (iu8f *&r_ptr, const iu8f *ptrEnd) {
  return getIeu<_i>(const_cast<const iu8f *&>(r_ptr), ptrEnd);
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value, _i) getValidIeu (const iu8f *&r_ptr) noexcept {
  try {
    return getIeu<_i>(r_ptr, r_ptr + numeric_limits<_i>::max_ie_octets);
  } catch (...) {
    DA(false, "value out of range");
    return 0;
  }
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value, _i) getValidIeu (iu8f *&r_ptr) noexcept {
  return getValidIeu<_i>(const_cast<const iu8f *&>(r_ptr));
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_signed<_i>::value, _i) getIes (const iu8f *&r_ptr, const iu8f *ptrEnd) {
  typename std::make_unsigned<_i>::type mag;
  bool isNegative;
  std::tie(mag, isNegative) = getIex<decltype(mag), true>(r_ptr, ptrEnd);
  if (isNegative) {
    if (mag > static_cast<decltype(mag)>(-numeric_limits<_i>::min())) {
      throw std::overflow_error("signed external integer had too big a negative value");
    }
    return static_cast<_i>(-static_cast<_i>(mag));
  } else {
    if (mag > static_cast<decltype(mag)>(numeric_limits<_i>::max())) {
      throw std::overflow_error("signed external integer had too big a positive value");
    }
    return static_cast<_i>(mag);
  }
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_signed<_i>::value, _i) getIes (iu8f *&r_ptr, const iu8f *ptrEnd) {
  return getIes<_i>(const_cast<const iu8f *&>(r_ptr), ptrEnd);
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_signed<_i>::value, _i) getValidIes (const iu8f *&r_ptr) noexcept {
  try {
    return getIes<_i>(r_ptr, r_ptr + numeric_limits<_i>::max_ie_octets);
  } catch (...) {
    DA(false, "value out of range");
    return 0;
  }
}

template<typename _i> iff(std::is_integral<_i>::value && std::is_signed<_i>::value, _i) getValidIes (iu8f *&r_ptr) noexcept {
  return getValidIes<_i>(const_cast<const iu8f *&>(r_ptr));
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}
