#include "core.using"

namespace core {

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
#ifndef NDEBUG
namespace debug {

template<typename _i, iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value)> void Stream::writeElement (_i value) noexcept {
  if (!handle) {
    dieHard();
  }

  const char *f = "%llu";
  #ifdef OS_WIN32
  DSA(sizeof(unsigned long long) == 8, "value is not 64-bit");
  f = "%I64u";
  #endif
  int r = fprintf(handle, f, static_cast<unsigned long long>(value));
  if (r < 0) {
    dieHard("failed to write to stream\n");
  }
}

template<typename _i, iff(std::is_integral<_i>::value && std::is_signed<_i>::value)> void Stream::writeElement (_i value) noexcept {
  if (!handle) {
    dieHard();
  }

  const char *f = "%lld";
  #ifdef OS_WIN32
  DSA(sizeof(signed long long) == 8, "value is not 64-bit");
  f = "%I64d";
  #endif
  int r = fprintf(handle, f, static_cast<signed long long>(value));
  if (r < 0) {
    dieHard("failed to write to stream\n");
  }
}

template<typename ..._Ts> void Logger::write (_Ts ...ts) noexcept {
  if (!stream) {
    return;
  }

  stream->startLine();
  writeElements(ts...);
  stream->flush();
}

template<typename ..._Ts> void Logger::writeLine (_Ts ...ts) noexcept {
  if (!stream) {
    return;
  }

  stream->startLine();
  writeElements(ts...);
  stream->endLine();
  stream->flush();
}

template<typename _T0, typename ..._Ts> void Logger::writeElements (_T0 t0, _Ts ...ts) noexcept {
  stream->writeElement(t0);
  writeElements(ts...);
}

template<typename ..._Ts> void assertImpl (const char *file, int line, bool cond, _Ts ...ts) noexcept {
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
  if (!(expected == actual)) {
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

    if (!(*(expectedI++) == *(actualI++))) {
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
template<typename _i> _i createBitmask (iu index) noexcept {
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

template<typename _i> iu getLowestSetBit (_i value) noexcept {
  auto v = static_cast<typename std::make_unsigned<_i>::type>(value);

  int r;
  if (sizeof(v) > sizeof(long long)) {
    DPRE(false, "_i is too long");
  } else if (sizeof(v) > sizeof(long)) {
    r = __builtin_ffsll(static_cast<long long>(v));
  } else if (sizeof(v) > sizeof(int)) {
    r = __builtin_ffsl(static_cast<long>(v));
  } else {
    r = __builtin_ffs(static_cast<int>(v));
  }
  return static_cast<iu>(r) - 1;
}

template<typename _i> _i sl (_i value, iu sh) noexcept {
  #ifndef ARCH_OORLEFTSHIFT
  if (sh >= numeric_limits<_i>::bits) {
    return 0;
  }
  #endif

  return static_cast<_i>(value << sh);
}

#ifdef ARCH_TWOCINTS
template<typename _i, iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value)> _i sr (_i value, iu sh) noexcept {
  #ifndef ARCH_OORRIGHTSHIFT
  if (sh >= numeric_limits<_i>::bits) {
    return 0;
  }
  #endif

  return static_cast<_i>(value >> sh);
}

template<typename _i, iff(std::is_integral<_i>::value && std::is_signed<_i>::value)> _i sr (_i value, iu sh) noexcept {
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

template<typename _i, iff(std::is_integral<_i>::value)> void set (iu8f *ptr, _i value) noexcept {
  #ifdef ARCH_LOOSEALIGNMENT
  *reinterpret_cast<_i *>(ptr) = value;
  #else
  memcpy(reinterpret_cast<void *>(ptr), reinterpret_cast<void *>(&value), sizeof(_i));
  #endif
}

template<typename _i, iff(std::is_integral<_i>::value)> _i get (const iu8f *ptr) noexcept {
  #ifdef ARCH_LOOSEALIGNMENT
  return *reinterpret_cast<const _i *>(ptr);
  #else
  _i value;
  memcpy(reinterpret_cast<void *>(&value), reinterpret_cast<const void *>(ptr), sizeof(_i));
  return value;
  #endif
}

template<typename _i, typename _OutputIterator, bool _useSignedFormat> void writeIex (_OutputIterator &r_ptr, _i value, bool isNegative) {
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

    iu8f octet = static_cast<iu8f>((static_cast<iu8f>(value) & 0x7F) | 0x80);
    *(r_ptr++) = octet;
    value = static_cast<_i>(value >> 7);
    DW(, "next octet is ", octet, " - remainder of value is ", value);
  } while (true);
}

template<typename _i, typename _InputIterator, bool _validate, bool _useSignedFormat> std::tuple<_i, bool> readIex (_InputIterator &r_ptr, const _InputIterator &ptrEnd) {
  DS();
  DSPRE(std::is_integral<_i>::value && std::is_unsigned<_i>::value, "_i must be an unsigned type");
  DW(, "reading ", _useSignedFormat ? "signed" : "unsigned", " value");

  _i value = 0;
  iu valueIndex = 0;

  do {
    if (_validate && r_ptr == ptrEnd) {
      DW(, "ran out of buffer just before reading");
      throw PlainException(u8string(_useSignedFormat ? u8("signed") : u8("unsigned")) + u8(" external integer was truncated"));
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

      if (_validate) {
        is topBitIndex = numeric_limits<_i>::bits - 1 - static_cast<is>(valueIndex);
        if (topBitIndex < 0 || (topBitIndex < 7 && (octet >> (topBitIndex + 1)) != 0)) {
          DW(, "overflow has occurred");
          throw std::overflow_error(string<char>(_useSignedFormat ? "signed" : "unsigned") + " external integer was too big");
        }
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

template<typename _i, typename _OutputIterator, iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value)> void writeIeu (_OutputIterator &r_ptr, _i value) noexcept(noexcept(*(r_ptr++))) {
  writeIex<_i, _OutputIterator, false>(r_ptr, value, false);
}

template<typename _i, typename _OutputIterator, iff(std::is_integral<_i>::value && std::is_signed<_i>::value)> void writeIes (_OutputIterator &r_ptr, _i value) noexcept(noexcept(*(r_ptr++))) {
  typename std::make_unsigned<_i>::type mag;
  bool isNegative;
  if (value < 0) {
    mag = static_cast<decltype(mag)>(-value);
    isNegative = true;
  } else {
    mag = static_cast<decltype(mag)>(value);
    isNegative = false;
  }
  writeIex<decltype(mag), _OutputIterator, true>(r_ptr, mag, isNegative);
}

template<typename _i, typename _InputIterator, bool _validate> _i readIeuImpl (_InputIterator &r_ptr, const _InputIterator &ptrEnd) {
  return std::get<0>(readIex<_i, _InputIterator, _validate, false>(r_ptr, ptrEnd));
}

template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_unsigned<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value
)> _i readIeu (_InputIterator &r_ptr, const _InputIterator &ptrEnd) {
  return readIeuImpl<_i, _InputIterator, true>(r_ptr, ptrEnd);
}

template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_unsigned<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value &&
  noexcept(*((*static_cast<_InputIterator *>(nullptr))++))
)> _i readValidIeu (_InputIterator &r_ptr) noexcept {
  return readIeuImpl<_i, _InputIterator, false>(r_ptr, *static_cast<_InputIterator *>(nullptr));
}

template<typename _i, typename _InputIterator, bool _validate> _i readIesImpl (_InputIterator &r_ptr, const _InputIterator &ptrEnd) {
  typename std::make_unsigned<_i>::type mag;
  bool isNegative;
  std::tie(mag, isNegative) = readIex<decltype(mag), _InputIterator, _validate, true>(r_ptr, ptrEnd);
  if (isNegative) {
    if (_validate && mag > static_cast<decltype(mag)>(-numeric_limits<_i>::min())) {
      throw std::overflow_error("signed external integer had too big a negative value");
    }
    return static_cast<_i>(-static_cast<_i>(mag));
  } else {
    if (_validate && mag > static_cast<decltype(mag)>(numeric_limits<_i>::max())) {
      throw std::overflow_error("signed external integer had too big a positive value");
    }
    return static_cast<_i>(mag);
  }
}

template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_signed<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value
)> _i readIes (_InputIterator &r_ptr, const _InputIterator &ptrEnd) {
  return readIesImpl<_i, _InputIterator, true>(r_ptr, ptrEnd);
}

template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_signed<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value &&
  noexcept(*((*static_cast<_InputIterator *>(nullptr))++))
)> _i readValidIes (_InputIterator &r_ptr) noexcept {
  return readIesImpl<_i, _InputIterator, false>(r_ptr, *static_cast<_InputIterator *>(nullptr));
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
template<typename _T> template<typename ..._Ts> SlowHashWrapper<_T>::SlowHashWrapper (_Ts &&...ts) noexcept(noexcept(_T(std::forward<_Ts>(ts)...)) && noexcept(hashSlow(o))) :
  o(std::forward<_Ts>(ts)...), h(hashSlow(o))
{
}

template<typename _T> SlowHashWrapper<_T>::operator const _T & () const noexcept {
  return o;
}

template<typename _T> const _T &SlowHashWrapper<_T>::get () const noexcept {
  return o;
}

template<typename _T> _T SlowHashWrapper<_T>::release () && noexcept {
  return std::move(o);
}

template<typename _T> size_t SlowHashWrapper<_T>::hash () const noexcept {
  return h;
}

template<typename _T> bool operator== (const SlowHashWrapper<_T> &l, const SlowHashWrapper<_T> &r) noexcept(noexcept(l.get() == r.get())) {
  return l.hash() == r.hash() && l.get() == r.get();
}

template<typename _T> template<typename ..._Ts> FastHashWrapper<_T>::FastHashWrapper (_Ts &&...ts) noexcept(noexcept(_T(std::forward<_Ts>(ts)...))) :
  o(std::forward<_Ts>(ts)...)
{
}

template<typename _T> FastHashWrapper<_T>::operator const _T & () const noexcept {
  return o;
}

template<typename _T> const _T &FastHashWrapper<_T>::get () const noexcept {
  return o;
}

template<typename _T> _T FastHashWrapper<_T>::release () && noexcept {
  return std::move(o);
}

template<typename _T> size_t FastHashWrapper<_T>::hash () const noexcept {
  DSPRE(noexcept(hashFast(o)), "");
  return hashFast(o);
}

template<typename _T> bool operator== (const FastHashWrapper<_T> &l, const FastHashWrapper<_T> &r) noexcept(noexcept(l.get() == r.get())) {
  return l.get() == r.get();
}

template<typename _T, iff(
  std::is_same<size_t, decltype(hashSlow(std::declval<const _T>()))>::value
)> SlowHashWrapper<typename std::remove_reference<_T>::type> hashed (_T &&o) noexcept(
  noexcept(SlowHashWrapper<typename std::remove_reference<_T>::type>(std::forward<_T>(o)))
) {
  return SlowHashWrapper<typename std::remove_reference<_T>::type>(std::forward<_T>(o));
}

template<typename _T, iff(
  std::is_same<size_t, decltype(hashFast(std::declval<const _T>()))>::value && noexcept(hashFast(std::declval<const _T>()))
)> FastHashWrapper<typename std::remove_reference<_T>::type> hashed (_T &&o) noexcept(
  noexcept(FastHashWrapper<typename std::remove_reference<_T>::type>(std::forward<_T>(o)))
) {
  return FastHashWrapper<typename std::remove_reference<_T>::type>(std::forward<_T>(o));
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
template<typename _c> template<typename ..._Ts> string<_c>::string (_Ts &&...ts) :
  std::basic_string<_c>(std::forward<_Ts>(ts)...)
{
}

template<typename _c> string<_c>::string (typename string<_c>::size_type capacity) {
  this->reserve(capacity);
}

template<typename _c> string<_c>::string (const string<_c> &o) : string<_c>(o.size()) {
  this->append(o);
}

template<typename _c> string<_c>::string (string<_c> &o) : string<_c>(static_cast<const string<_c> &>(o)) {
}

template<typename _c> typename string<_c>::reference string<_c>::operator[] (typename string<_c>::size_type pos) {
  DPRE(pos < this->size());
  return std::basic_string<_c>::operator[](pos);
}

template<typename _c> typename string<_c>::const_reference string<_c>::operator[] (typename string<_c>::size_type pos) const {
  DPRE(pos < this->size());
  return std::basic_string<_c>::operator[](pos);
}

template<typename _c> _c *string<_c>::data () {
  return &std::basic_string<_c>::operator[](0);
}

template<typename _c> void string<_c>::append_any (typename string<_c>::size_type count) {
  this->append(count, 0x58);
}

template<typename _c> void string<_c>::resize_any (typename string<_c>::size_type count) {
  this->resize(count, 0x58);
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
}
