/** @file */
/* -----------------------------------------------------------------------------
   Core Library
   © Geoff Crossland 1998, 1999, 2003, 2004, 2005, 2007, 2008, 2013, 2014
----------------------------------------------------------------------------- */
#ifndef CORE_ALREADYINCLUDED
#define CORE_ALREADYINCLUDED

#include <type_traits>
#include <exception>
#include <limits>
#include <memory>
#include <uchar.h>
#include <string>

namespace core {

struct Version {
  unsigned int major;
  unsigned int minor;
};

void checkVersionImpl (const char *requirer, const char *requiree, Version required, Version available) noexcept;

#define _dependson_(LIB, MAJ, MIN) core::checkVersionImpl(__FILE__, #LIB, core::Version{MAJ, MIN}, LIB::VERSION)

extern const Version VERSION;

}

/* -----------------------------------------------------------------------------
   Abbreviations
----------------------------------------------------------------------------- */
/**
  Abbreviation for {@c private}.
*/
#define prv private:
/**
  Abbreviation for {@c protected}.
*/
#define prt protected:
/**
  Abbreviation for {@c public}.
*/
#define pub public:

namespace core { namespace iff_impl {
  enum class DummyTemplateParameter {};
}}

/**
  Abbreviation for using {@c std::enable_if<>::type} in place of a template
  parameter.
*/
#define iff(...) typename std::enable_if<__VA_ARGS__, core::iff_impl::DummyTemplateParameter>::type...

/**
  Abbreviation for {@c std::throw_with_nested()}.
*/
#define nthrow(E) std::throw_with_nested(E)

#define noreturn [[noreturn]]
#ifdef CDT
#define noreturn
#endif

/* -----------------------------------------------------------------------------
   Configurations for platforms
----------------------------------------------------------------------------- */
#if defined(OS_RISCOS) + defined(OS_DOS) + defined(OS_WIN32) != 1
#error Exactly one OS_* value must be set
#endif

#if defined(ARCH_ARM_32) + defined(ARCH_X86_32) != 1
#error Exactly one ARCH_* value must be set
#endif

#ifdef ARCH_ARM_32
#define ARCH_ARM
#endif

#ifdef ARCH_X86_32
#define ARCH_X86
#endif

/**
  @def ARCH_ENDIAN_BIG, ARCH_ENDIAN_LITTLE
  Specifies the endianness of the architecture.

  @def ARCH_TWOCINTS
  Specifies that signed integers are represented in two's complement.

  @def ARCH_OORLEFTSHIFT
  Specifies that left shifts of count greater than or equal to the number of
  bits in the source variable type are sane.

  @def ARCH_OORRIGHTSHIFT
  Specifies that right shifts of count greater than or equal to the number of
  bits in the source variable type are sane.

  @def ARCH_SIGNEDRIGHTSHIFT_LOG, ARCH_SIGNEDRIGHTSHIFT_ARITH
  Specifies that right shifts of signed integers are logical or arithmetic.

  @def ARCH_LOOSEALIGNMENT
  Specifies that pointers that are not correctly aligned for their type can
  still be dereferenced (possibly with a performance impact) and that values
  set (or re-got) will have the same sequence of bytes as when setting via an
  aligned pointer.
*/

// Given the processor type, declare what we know about it.
//
// Most of the processor property definitions are flags and can be set
// separately from any of the others. There is no requirement to set any of
// these; rather, if a processor property is set, then code may be able to
// perform optimisations over and above those allowed by the language rules
// (but, ideally, should also provide a fall-back implementation, for when the
// relevant processor property is not set).
//
// Some processor property definitions form sets of options of which exactly one
// must be set.

#ifdef ARCH_ARM
#ifndef ARCH_ENDIAN_BIG
#define ARCH_ENDIAN_LITTLE
#endif
#define ARCH_TWOCINTS
#define ARCH_OORLEFTSHIFT
#define ARCH_OORRIGHTSHIFT
#define ARCH_SIGNEDRIGHTSHIFT_ARITH
#endif

#ifdef ARCH_X86
#define ARCH_ENDIAN_LITTLE
#define ARCH_TWOCINTS
#define ARCH_SIGNEDRIGHTSHIFT_ARITH
#define ARCH_LOOSEALIGNMENT
#endif

#if defined(ARCH_ENDIAN_LITTLE) + defined(ARCH_ENDIAN_BIG) != 1
#error Exactly one of ARCH_ENDIAN_LITTLE and ARCH_ENDIAN_BIG must be set
#endif

#if defined(ARCH_SIGNEDRIGHTSHIFT_LOG) + defined(ARCH_SIGNEDRIGHTSHIFT_ARITH) != 1
#error Exactly one of ARCH_SIGNEDRIGHTSHIFT_LOG and ARCH_SIGNEDRIGHTSHIFT_ARITH must be set
#endif

#ifdef ARCH_ARM_32
typedef unsigned long long iu64f;
typedef signed long long is64f;
typedef unsigned int iu32f;
typedef signed int is32f;
typedef unsigned short iu16f;
typedef signed short is16f;
typedef unsigned char iu8f;
typedef signed char is8f;

typedef iu64f iu64;
typedef is64f is64;
typedef iu32f iu32;
typedef is32f is32;
typedef iu32f iu16;
typedef is32f is16;
typedef iu32f iu8;
typedef is32f is8;
typedef iu32f iu;
typedef is32f is;

typedef double f64f;
typedef float f32f;

typedef f64f f64;
typedef f32f f32;
#endif

#ifdef ARCH_X86_32
typedef unsigned long long iu64f;
typedef signed long long is64f;
typedef unsigned int iu32f;
typedef signed int is32f;
typedef unsigned short iu16f;
typedef signed short is16f;
typedef unsigned char iu8f;
typedef signed char is8f;

typedef iu64f iu64;
typedef is64f is64;
typedef iu32f iu32;
typedef is32f is32;
typedef iu32f iu16;
typedef is32f is16;
typedef iu32f iu8;
typedef is32f is8;
typedef iu32f iu;
typedef is32f is;

typedef double f64f;
typedef float f32f;

typedef f64f f64;
typedef f32f f32;
#endif

namespace core {

/**
  This provides extra metadata for integer types.
*/
// TODO make__ or whatever it's called
template<typename _i> class numeric_limits : public std::numeric_limits<_i> {
  /**
    The number of bits in an instance of the type on the local platform.
  */
  pub static constexpr int bits = std::numeric_limits<_i>::digits + std::numeric_limits<_i>::is_signed;

  /**
    An upper bound on the number of octets needed to store an instance of the
    type on the local platform in an {@c ieu}/{@c ies}.
  */
  pub static constexpr int max_ie_octets = ((bits + std::numeric_limits<_i>::is_signed) + 6) / 7;
};

}

/* -----------------------------------------------------------------------------
   Debugging tools
----------------------------------------------------------------------------- */
// To aid debugging and any other diagnostic work necessary before proper
// string infrastructure is in place, we allow the concept of C++ native string
// literals as some platform-dependent developer-readable message format and
// include support for rendering these and some other primitives to debugging
// streams.

#ifndef NDEBUG
namespace core { namespace debug {

class Stream {
  prv static const char *const INDENT;

  prv FILE *handle;
  prv iu indentCount;
  prv bool atStartOfLine;

  pub explicit Stream () noexcept;
  pub explicit Stream (const char *filename) noexcept;
  pub ~Stream () noexcept;

  pub void enterScope () noexcept;
  pub void exitScope () noexcept;

  pub void startLine () noexcept;
  pub void endLine () noexcept;
  pub void flush () noexcept;
  pub void writeElement (const char *value) noexcept;
  pub void writeElement (const unsigned char *value) noexcept;
  pub template<typename _i, iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value)> void writeElement (_i value) noexcept;
  pub template<typename _i, iff(std::is_integral<_i>::value && std::is_signed<_i>::value)> void writeElement (_i value) noexcept;
};

class Logger {
  pub class ScopeProxy {
    prv std::shared_ptr<Stream> stream;
    prv const char *scopeName;

    prv ScopeProxy (Logger &logger, const char *scopeName) noexcept;
    ScopeProxy (const ScopeProxy &) = delete;
    pub ScopeProxy (ScopeProxy &&) = default;
    pub ~ScopeProxy () noexcept;
    prv void writeMarker (bool entering) noexcept;

    friend class Logger;
  };

  prv std::shared_ptr<Stream> stream;

  pub Logger () noexcept;
  Logger (const Logger &) = delete;
  Logger &operator= (const Logger &) = delete;
  Logger (Logger &&) = delete;
  Logger &operator= (Logger &&) = delete;

  pub void open (const std::shared_ptr<Stream> &stream) noexcept;
  pub void close () noexcept;

  pub ScopeProxy defineScope (const char *scopeName) noexcept;
  pub template<typename ..._Ts> void write (_Ts ...ts) noexcept;
  pub template<typename ..._Ts> void writeLine (_Ts ...ts) noexcept;
  prv template<typename _T0, typename ..._Ts> void writeElements (_T0 t0, _Ts ...ts) noexcept;
  prv void writeElements () noexcept;
};

template<typename ..._Ts> void assertImpl (const char *file, int line, bool cond, _Ts ...ts) noexcept;

}}
#endif

#define DNAME(L) __dl_ ## L
#ifndef NDEBUG
/**
  Declares or defines a logger {@p L}.
*/
#define DC(L) core::debug::Logger DNAME(L)
/**
  Opens the logger {@p L}.
*/
#define DOPEN(L, S) DNAME(L).open(S)
/**
  Closes the logger {@p L}.
*/
#define DCLOSE(L) DNAME(L).close()
/**
  Makes the current scope be a scope for the logger {@p L} (if it's open).
*/
#define DS(L) core::debug::Logger::ScopeProxy __dlsp = DNAME(L).defineScope(__FUNCTION__)
/**
  Writes a line via the logger (if it's open).
*/
#define DW(L, ...) DNAME(L).writeLine(__VA_ARGS__)
/**
  Writes an incomplete line via the logger (if it's open).
*/
#define DWP(L, ...) DNAME(L).write(__VA_ARGS__)
/**
  Validates an assertion.
*/
#define DA(...) core::debug::assertImpl(__FILE__, __LINE__, __VA_ARGS__)
#else
#define DC(L) bool DNAME(L)
#define DOPEN(L, S)
#define DCLOSE(L)
#define DS(L)
#define DW(L, ...)
#define DWP(L, ...)
#define DA(...)
#endif
/**
  Validates an assertion at compile time.
*/
#define DSA static_assert
/**
  Validates a precondition.
*/
#define DPRE DA
/**
  Validates a precondition at compile time.
*/
#define DSPRE DSA

namespace core {

extern DC();

}

/* -----------------------------------------------------------------------------
   Testing utilities
----------------------------------------------------------------------------- */
namespace core {

noreturn void dieHard () noexcept;
noreturn void dieHard (const char *msg) noexcept;
/**
  Checks that {@p cond} is {@c true}.
*/
void check (const bool &cond) noexcept;
/**
  Checks that {@c actual} is equal to {@p expected}.
*/
template<typename _T0, typename _T1> void check (const _T0 &expected, const _T1 &actual) noexcept;
/**
  Checks that the list [{@c actualI}, {@c actualEnd}) is equal to
  [{@c expectedI}, {@c expectedEnd}).
*/
template<typename _InputIterator0, typename _InputIterator1> void check (_InputIterator0 expectedI, _InputIterator0 expectedEnd, _InputIterator1 actualI, _InputIterator1 actualEnd);
// TODO macro for test fn definition (it should have a specific signature) - can also add it to a vector of ptr to test fns
// TODO fn to run all tests + do reporting

}

/* -----------------------------------------------------------------------------
   Platform-tailored utilities
----------------------------------------------------------------------------- */
namespace core {

/**
  Builds an integer with the bottom {@p index} bits set.
*/
template<typename _i> _i createBitmask (iu index) noexcept;
/**
  Treats bit {@p index} in {@p value} (which must be zero
  above {@p index}) as a two's-complement sign bit and sign extends
  {@p value}.
*/
template<typename _i> _i extendSign (_i value, iu index) noexcept;
/**
  Gets the index of the least significant set bit in {@p value} or, if
  {@p value} is zero, a value greater than or equal to
  ::numeric_limits<_i>::bits.
*/
template<typename _i> iu getLowestSetBit (_i value) noexcept;

/**
  Performs a left shift on a value, where the shift can be greater than the
  number of bits that the value's type can hold.
*/
template<typename _i> _i sl (_i value, iu sh) noexcept;
/**
  Performs an arithmetic right shift on a signed value or a logical right shift
  on an unsigned value, where the shift can be greater than the number of bits
  that the value's type can hold.

  @param value the value to shift.
  @param sh the number of places to shift right by.
  @return value asr sh
*/
template<typename _i, iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value)> _i sr (_i value, iu sh) noexcept;
template<typename _i, iff(std::is_integral<_i>::value && std::is_signed<_i>::value)> _i sr (_i value, iu sh) noexcept;

// These allow writing of integer types to octet arrays of arbitrary alignment,
// using the local platform's integer representation format (i.e. giving the
// same result as a memcpy of the integer value to an iu8f[])...

/**
  Writes a value of type {@p _i} to the given octet array.
*/
template<typename _i, iff(std::is_integral<_i>::value)> void set (iu8f *ptr, _i value) noexcept;
/**
  Reads a value of type {@p _i} from the given octet array.
*/
template<typename _i, iff(std::is_integral<_i>::value)> _i get (const iu8f *ptr) noexcept;

// ... and a platform-independent variable-length format.

// TODO validate _OutputIterator
template<typename _i, typename _OutputIterator, iff(std::is_integral<_i>::value && std::is_unsigned<_i>::value)> void writeIeu (_OutputIterator &r_ptr, _i value) noexcept(noexcept(*(r_ptr++)));
template<typename _i, typename _OutputIterator, iff(std::is_integral<_i>::value && std::is_signed<_i>::value)> void writeIes (_OutputIterator &r_ptr, _i value) noexcept(noexcept(*(r_ptr++)));
template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_unsigned<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value
)> _i readIeu (_InputIterator &r_ptr, const _InputIterator &ptrEnd);
template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_unsigned<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value &&
  noexcept(*((*static_cast<_InputIterator *>(nullptr))++))
)> _i readValidIeu (_InputIterator &r_ptr) noexcept;
template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_signed<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value
)> _i readIes (_InputIterator &r_ptr, const _InputIterator &ptrEnd);
template<typename _i, typename _InputIterator, iff(
  std::is_integral<_i>::value && std::is_signed<_i>::value &&
  std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<_InputIterator>::iterator_category>::value &&
  std::is_same<typename std::iterator_traits<_InputIterator>::value_type, iu8f>::value &&
  noexcept(*((*static_cast<_InputIterator *>(nullptr))++))
)> _i readValidIes (_InputIterator &r_ptr) noexcept;

}

/* -----------------------------------------------------------------------------
   Characters
----------------------------------------------------------------------------- */

#ifndef __STDC_UTF_32__
#error __STDC_UTF_32__ is required
#endif

#ifndef __STDC_UTF_16__
#error __STDC_UTF_16__ is required
#endif

/**
  A type for storing UTF-8 code units.
*/
typedef unsigned char char8_t;
/**
  The standard non-fixed type for holding Unicode code points.
*/
typedef iu32 uchar;

/**
  Creates a UTF-8 string literal of char8_ts.
*/
#define u8(S) reinterpret_cast<const char8_t *>(u8"" S)
/**
  Creates a UTF-32 string literal of char32_ts.
*/
#define u32(S) (U"" S)

namespace core {

/**
  Instances hold a sequence of 0 or more values of a POD type.
*/
template<typename _c> class string :
  public std::basic_string<_c> // DODGY not really
{
  pub template<typename ..._Ts> string (_Ts &&...ts);
  /**
    Constructs an empty string with capacity at least the specified value.
   */
  pub explicit string (typename string<_c>::size_type capacity);
  /**
    Copy constructs a string (leaving it without excess capacity).
   */
  pub string (const string<_c> &o);
  pub string (string<_c> &o);
  pub string<_c> &operator= (const string<_c> &) = default;
  pub string (string<_c> &&) = default;
  pub string<_c> &operator= (string<_c> &&) = default;

  /**
    @param pos (in [0, ::size()))
   */
  pub typename string<_c>::reference operator[] (typename string<_c>::size_type pos);
  /**
    @param pos (in [0, ::size()))
   */
  pub typename string<_c>::const_reference operator[] (typename string<_c>::size_type pos) const;
  using std::basic_string<_c>::data;
  /**
    Returns a pointer to the underlying array (so that
    {@c this->data() + a == &this[a]}). The values in
    [{@c this->data()}, {@c this->data() + this->size()}) are pointers valid for
    dereferencing; {@c this->data() + this->size()} is a pointer not necessarily
    valid for dereferencing (and, if it happens to be so valid, the value of such is
    undefined). Otherwise as std::basic_string::data().
   */
  pub _c *data ();
  /**
    Appends the specified number of characters of unspecified value to the
    string.
   */
  pub void append_any (typename string<_c>::size_type count);
  /**
    Resizes the string to contain the specified number of characters. If the size
    increases, additional characters of unspecified value are appended.
  */
  pub void resize_any (typename string<_c>::size_type count);
};

/**
  Instances hold a sequence of 0 or more valid characters encoded in UTF-8.
*/
typedef string<char8_t> u8string;
/**
  Instances hold a sequence of 0 or more valid characters encoded in UTF-32.
*/
typedef string<char32_t> u32string;

}

/* -----------------------------------------------------------------------------
   Exception utilities
----------------------------------------------------------------------------- */
namespace core {

/**
  A base exception type for those with Unicode messages.
*/
class UException : public virtual std::exception {
  /**
    Returns a pointer to the start of a \0-terminated UTF-8 string (valid until
    destruction) describing the cause of the exception.
  */
  pub virtual const char8_t *uWhat () const noexcept = 0;
  pub const char *what () const noexcept override;
};

/**
  Builds a message for the given exception and its chain of causes, based on
  std::exception::what(). Each sub-message is assumed to be a sentence, but
  without leading capital or trailing full stop (which are added as needed); if
  a sub-message starts with '_', it is elided and the following character is
  never capitalised.
*/
u8string createExceptionMessage (const std::exception &rootException, bool capitaliseHead);

/**
  An exception type that uses only a user-readable string to express its cause.
  It is expected that the message should be of format compatible with
  ::createExceptionMessage().
*/
class PlainException : public virtual UException {
  prv const char8_t *const literalMsg;
  prv const std::shared_ptr<const u8string> composedMsg;

  pub explicit PlainException (const u8string &msg);
  pub explicit PlainException (u8string &&msg);
  /**
    @param msg the message (valid forever).
  */
  pub explicit PlainException (const char8_t *msg) noexcept;
  pub PlainException (const PlainException &) = default;
  pub PlainException &operator= (const PlainException &) = default;
  pub PlainException (PlainException &&) = default;
  pub PlainException &operator= (PlainException &&) = default;

  /**
    Returns a PlainException with message created by interpolating the given
    message template (which must be a string literal). Substitutions all start
    with the character '%'; '%%' represents a literal percent character.

    @param msgTemplate the message template (valid forever).
  */
  // TODO do gettext-style translation on the template
  // TODO actually do interpolation; have the values as extra args (so position only, rather than by name, unfortunately)
  pub static PlainException create (const char8_t *msgTemplate);
  prv static void interpolate (const char8_t *msgTemplate, u8string &r_out);

  pub virtual const char8_t *uWhat () const noexcept override;
};

}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
#include "core.ipp"
#endif
