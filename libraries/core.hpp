/** @file */
/* -----------------------------------------------------------------------------
   Core Library
   © Geoff Crossland 1998-2022
----------------------------------------------------------------------------- */
#ifndef CORE_ALREADYINCLUDED
#define CORE_ALREADYINCLUDED

#include <type_traits>
#include <exception>
#include <limits>
#include <memory>
#include <cuchar>
#include <string>
#include <functional>
#include <cstddef>
#include <stdexcept>

#define _VERSION_EXPORT_NAME_(LIB, MAJ, MIN) _ ## LIB ## _ ## MAJ ## _ ## MIN ## _
#define _version_(LIB, MAJ, MIN) extern const bool _VERSION_EXPORT_NAME_(LIB, MAJ, MIN) = false;
#define _dependson_(LIB, MAJ, MIN) extern const bool _VERSION_EXPORT_NAME_(LIB, MAJ, MIN); static const bool _dep_ ## LIB ## _ = _VERSION_EXPORT_NAME_(LIB, MAJ, MIN);

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

/**
  Abbreviation for {@c std::throw_with_nested()}.
*/
#define nthrow(E) std::throw_with_nested(E)

/**
  Abbreviation for noexcept-specification/function-body pairs where the two
  contain identical expressions (the value of which may or may not be
  returned).
*/
#define noexcept_auto(...) noexcept(noexcept(__VA_ARGS__)) { __VA_ARGS__; }
#define noexcept_auto_return(...) noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

/**
  Abbreviation for creating an object wrapping the given no-arg functor and
  calling it on destruction.
*/
#define finally(...) auto _finally_ ## __LINE__ = core::finally::finallyImpl(__VA_ARGS__)

/* -----------------------------------------------------------------------------
   Configurations for platforms
----------------------------------------------------------------------------- */
#if defined(OS_RISCOS) + defined(OS_WIN32) + defined(OS_POSIX) != 1
#error Exactly one OS_* value must be set
#endif

#if defined(ARCH_ARM_32) + defined(ARCH_ARM_64) + defined(ARCH_X86_32) + defined(ARCH_X86_64) != 1
#error Exactly one ARCH_* value must be set
#endif

#ifdef ARCH_ARM_32
#define ARCH_ARM
#endif

#ifdef ARCH_ARM_64
#define ARCH_ARM
#endif

#ifdef ARCH_X86_32
#define ARCH_X86
#endif

#ifdef ARCH_X86_64
#define ARCH_X86
#endif

/**
  @def ARCH_ENDIAN_BIG, ARCH_ENDIAN_LITTLE
  Specifies the endianness of the architecture.

  @def ARCH_TWOCINTS
  Specifies that signed integers are represented in two's complement.

  @def ARCH_MAXLEFTSHIFT
  Specifies the maximum valid left shift count.

  @def ARCH_MAXRIGHTSHIFT
  Specifies the maximum valid right shift count.

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
#ifdef ARCH_ARM_32
#define ARCH_MAXLEFTSHIFT 255
#define ARCH_MAXRIGHTSHIFT 255
#endif
#define ARCH_SIGNEDRIGHTSHIFT_ARITH
#ifdef ARCH_ARM_64
#define ARCH_LOOSEALIGNMENT
#endif
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

#ifdef OS_POSIX
#if defined(ARCH_ARM_32) || defined(ARCH_X86_32)
typedef unsigned long long iu64f;
typedef signed long long is64f;
#else
typedef unsigned long iu64f;
typedef signed long is64f;
#endif
#else
typedef unsigned long long iu64f;
typedef signed long long is64f;
#endif
typedef unsigned int iu32f;
typedef signed int is32f;
typedef unsigned short iu16f;
typedef signed short is16f;
typedef unsigned char iu8f;
typedef signed char is8f;

typedef double f64f;
typedef float f32f;

#ifdef ARCH_ARM
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

typedef f64f f64;
typedef f32f f32;
#endif

#ifdef ARCH_X86
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

/**
  Returns the given value (which must be natural) in an unsigned type of the same
  size as the original.
*/
template<std::integral _i> typename std::make_unsigned<_i>::type unsign (_i v) noexcept;

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
namespace core::debug {

class Stream {
  prv static const char *const indent;

  prv FILE *handle;
  prv iu indentCount;
  prv bool atStartOfLine;

  pub explicit Stream () noexcept;
  pub explicit Stream (const char *pathName) noexcept;
  pub ~Stream () noexcept;

  pub void enterScope () noexcept;
  pub void exitScope () noexcept;

  pub void startLine () noexcept;
  pub void endLine () noexcept;
  pub void flush () noexcept;
  pub void writeElement (const char *value) noexcept;
  pub void writeElement (const char8_t *value) noexcept;
  pub template<std::unsigned_integral _i> void writeElement (_i value) noexcept;
  pub template<std::signed_integral _i> void writeElement (_i value) noexcept;
};

class Logger {
  pub class ScopeProxy {
    prv std::shared_ptr<Stream> stream;
    prv const char *scopeName;

    prv ScopeProxy (Logger &logger, const char *scopeName) noexcept;
    ScopeProxy (const ScopeProxy &) = delete;
    ScopeProxy &operator= (const ScopeProxy &) = delete;
    pub ScopeProxy (ScopeProxy &&) = default;
    ScopeProxy &operator= (ScopeProxy &&) = delete;
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

}
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
  Wraps tokens that will be included only in debug mode.
*/
#define DI(...) __VA_ARGS__
/**
  Validates an assertion.
*/
#define DA(...) core::debug::assertImpl(__FILE__, __LINE__, __VA_ARGS__)
#else
#define DC(L) bool DNAME(L)
#define DOPEN(L, S) DNAME(L)
#define DCLOSE(L) DNAME(L)
#define DS(L)
#define DW(L, ...)
#define DWP(L, ...)
#define DI(...)
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

[[noreturn]] void dieHard () noexcept;
[[noreturn]] void dieHard (const char *msg) noexcept;
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
template<
  typename _InputIterator0, typename _InputEndIterator0, typename _InputIterator1, typename _InputEndIterator1
> void check (_InputIterator0 expectedI, _InputEndIterator0 expectedEnd, _InputIterator1 actualI, _InputEndIterator1 actualEnd);
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
template<std::unsigned_integral _i> _i sr (_i value, iu sh) noexcept;
template<std::signed_integral _i> _i sr (_i value, iu sh) noexcept;

// These allow writing of integer types to octet arrays of arbitrary alignment,
// using the local platform's integer representation format (i.e. giving the
// same result as a memcpy of the integer value to an iu8f[])...

/**
  Writes a value of type {@p _i} to the given octet array.
*/
template<std::integral _i> void set (iu8f *ptr, _i value) noexcept;
/**
  Reads a value of type {@p _i} from the given octet array.
*/
template<std::integral _i> _i get (const iu8f *ptr) noexcept;

// ... and a platform-independent variable-length format.

/**
  An input iterator of value type {@p _T}.
*/
template<typename _Iterator, typename _T> concept InputIterator =
  std::input_or_output_iterator<_Iterator> &&
  std::derived_from<typename std::iterator_traits<_Iterator>::iterator_category, std::input_iterator_tag> &&
  std::same_as<std::iter_value_t<_Iterator>, _T>
;
/**
  An output iterator of value type {@p _T}.
*/
template<typename _Iterator, typename _T> concept OutputIterator = std::output_iterator<_Iterator, _T>;

/**
  Writes a value of type {@p _i} to the given octet stream in an unsigned
  variable-length format.
*/
template<std::unsigned_integral _i, core::OutputIterator<iu8f> _OutputIterator> void writeIeu (_OutputIterator &r_ptr, _i value) noexcept(noexcept(*(r_ptr++)));
/**
  Writes a value of type {@p _i} to the given octet stream in a signed
  variable-length format.
*/
template<std::signed_integral _i, core::OutputIterator<iu8f> _OutputIterator> void writeIes (_OutputIterator &r_ptr, _i value) noexcept(noexcept(*(r_ptr++)));

/**
  Reads a value of type {@p _i} from the given octet stream in an unsigned
  variable-length format, if possible.
*/
template<
  std::unsigned_integral _i, core::InputIterator<iu8f> _InputIterator, typename _InputEndIterator
> _i readIeu (_InputIterator &r_ptr, const _InputEndIterator &ptrEnd);
/**
  Reads a value of type {@p _i} from the given octet stream in an unsigned
  variable-length format.
*/
template<
  std::unsigned_integral _i, core::InputIterator<iu8f> _InputIterator
> requires requires (_InputIterator i) {
  {*(i++)} noexcept;
} _i readValidIeu (_InputIterator &r_ptr) noexcept;
/**
  Reads a value of type {@p _i} from the given octet stream in a signed
  variable-length format, if possible.
*/
template<
  std::signed_integral _i, core::InputIterator<iu8f> _InputIterator, typename _InputEndIterator
> _i readIes (_InputIterator &r_ptr, const _InputEndIterator &ptrEnd);
/**
  Reads a value of type {@p _i} from the given octet stream in a signed
  variable-length format.
*/
template<
  std::signed_integral _i, core::InputIterator<iu8f> _InputIterator
> requires requires (_InputIterator i) {
  {*(i++)} noexcept;
} _i readValidIes (_InputIterator &r_ptr) noexcept;

}

/* -----------------------------------------------------------------------------
   Equality and order
----------------------------------------------------------------------------- */
/**
  eq operator implementation for objects based on their eq member function.
*/
template<typename _T> requires requires (const _T &l, const _T &r) {
  l.operator==(r);
} bool operator== (const _T &l, const _T &r) noexcept_auto_return(
  l.operator==(r)
);
/**
  ne operator implementation for objects based on their eq member function.
*/
template<typename _T> requires requires (const _T &l, const _T &r) {
  l.operator==(r);
} bool operator!= (const _T &l, const _T &r) noexcept_auto_return(
  !l.operator==(r)
)

/**
  lt operator implementation for objects based on their lt member function.
*/
template<typename _T> requires requires (const _T &l, const _T &r) {
  l.operator<(r);
} bool operator< (const _T &l, const _T &r) noexcept_auto_return(
  l.operator<(r)
)
/**
  gt operator implementation for objects based on their lt member function.
*/
template<typename _T> requires requires (const _T &l, const _T &r) {
  l.operator<(r);
} bool operator> (const _T &l, const _T &r) noexcept_auto_return(
  r.operator<(l)
)
/**
  le operator implementation for objects based on their lt member function.
*/
template<typename _T> requires requires (const _T &l, const _T &r) {
  l.operator<(r);
} bool operator<= (const _T &l, const _T &r) noexcept_auto_return(
  !r.operator<(l)
)
/**
  ge operator implementation for objects based on their lt member function.
*/
template<typename _T> requires requires (const _T &l, const _T &r) {
  l.operator<(r);
} bool operator>= (const _T &l, const _T &r) noexcept_auto_return(
  !l.operator<(r)
)

// We'll allow reference_wrapper<>ed things to be compared without needing
// implicit conversion.

template<typename _T> bool operator== (const std::reference_wrapper<_T> &l, const std::reference_wrapper<_T> &r) noexcept_auto_return(
  l.get() == r.get()
)
template<typename _T> bool operator!= (const std::reference_wrapper<_T> &l, const std::reference_wrapper<_T> &r) noexcept_auto_return(
  l.get() != r.get()
)
template<typename _T> bool operator< (const std::reference_wrapper<_T> &l, const std::reference_wrapper<_T> &r) noexcept_auto_return(
  l.get() < r.get()
)
template<typename _T> bool operator> (const std::reference_wrapper<_T> &l, const std::reference_wrapper<_T> &r) noexcept_auto_return(
  l.get() > r.get()
)
template<typename _T> bool operator<= (const std::reference_wrapper<_T> &l, const std::reference_wrapper<_T> &r) noexcept_auto_return(
  l.get() <= r.get()
)
template<typename _T> bool operator>= (const std::reference_wrapper<_T> &l, const std::reference_wrapper<_T> &r) noexcept_auto_return(
  l.get() >= r.get()
)

/* -----------------------------------------------------------------------------
   Lifetime management utilities
----------------------------------------------------------------------------- */
namespace core::finally {

template<typename _F> class Finally {
  prv _F functor;
  prv bool live;

  pub explicit Finally (_F &&functor);
  Finally (const Finally &) = delete;
  Finally &operator= (const Finally &) = delete;
  pub Finally (Finally &&o);
  Finally &operator= (Finally &&) = delete;
  pub ~Finally () noexcept;
};

template<typename _F> finally::Finally<_F> finallyImpl (_F &&functor);

}

/* -----------------------------------------------------------------------------
   Array utilities
----------------------------------------------------------------------------- */
namespace core {

/**
  Returns the {@c ptrdiff_t} difference between {@p first} and {@p last},
  where {@c first <= last}, as a {@c size_t}.
*/
template<typename _I> requires requires (const _I &first, const _I &last) {
  {last - first} -> std::same_as<ptrdiff_t>;
} size_t offset (const _I &first, const _I &last) noexcept(noexcept(last - first));
template<typename _T> size_t offset (_T *first, _T *last) noexcept;

}

/**
  Implementation of iterator advancement that admits {@c size_t} for {@c ptrdiff_t}.
*/
template<typename _I> requires requires (const _I &i) {
  {i - i} -> std::same_as<ptrdiff_t>;
} _I operator+ (const _I &i, size_t o) noexcept(noexcept(i + std::declval<ptrdiff_t>())) {
  DPRE(o <= static_cast<typename std::make_unsigned<ptrdiff_t>::type>(core::numeric_limits<ptrdiff_t>::max())  , "o must be a size");

  return i + static_cast<ptrdiff_t>(o);
}

/* -----------------------------------------------------------------------------
   Hashing
----------------------------------------------------------------------------- */
namespace core {

/**
  Hashes a sequence of octets.
*/
size_t hash (const iu8f *i, const iu8f *end) noexcept;

/**
  Implementation of {@c size_t hashSlow (const _T &)}) that leans on a
  corresponding member function.
*/
template<typename _T> requires requires (const _T &o) {
  {o.hashSlow()} -> std::same_as<size_t>;
} size_t hashSlow (const _T &o) noexcept_auto_return(
  o.hashSlow()
)
/**
  Implementation of {@c size_t hashFast (const _T &) noexcept}) that leans on a
  corresponding member function.
*/
template<typename _T> requires requires (const _T &o) {
  {o.hashFast()} noexcept -> std::same_as<size_t>;
} size_t hashFast (const _T &o) noexcept {
  return o.hashFast();
}

/**
  Exposes a hash value (from {@c size_t hashSlow (const _T &)}) that may be
  non-trivial to calculate.
*/
template<typename _T> concept SlowHashable = requires (const _T &o) {
  {hashSlow(o)} -> std::same_as<size_t>;
};
/**
  Exposes a hash value (from {@c size_t hashFast (const _T &) noexcept}) that is
  so trivial to calculate that it does not need to be persistently cached.
*/
template<typename _T> concept FastHashable = requires (const _T &o) {
  {hashFast(o)} noexcept -> std::same_as<size_t>;
};

/**
  Implementation of {@c size_t hashSlow (const std::reference_wrapper<_T> &)})
  that leans on the referenced object.
*/
template<SlowHashable _T> size_t hashSlow (const std::reference_wrapper<_T> &o) noexcept_auto_return(
  hashSlow(o.get())
)
/**
  Implementation of {@c size_t hashFast (const std::reference_wrapper<_T> &) noexcept})
  that leans on the referenced object.
*/
template<FastHashable _T> size_t hashFast (const std::reference_wrapper<_T> &o) noexcept {
  return hashFast(o.get());
}

template<typename _T> class SlowHashWrapper {
  prv _T o;
  prv size_t h;

  /**
    Constructs the wrapped object in-place (by calling the constructor for
    {@c _T} with the given arguments forwarded) and stores its hash.
  */
  pub template<typename ..._Ts> requires requires (_Ts &&...ts) {
    {_T(std::forward<_Ts>(ts)...)} -> std::same_as<_T>;
  } explicit SlowHashWrapper (_Ts &&...ts) noexcept(noexcept(_T(std::forward<_Ts>(ts)...)) && noexcept(hashSlow(o)));

  /**
    Returns a reference to the wrapped object.
  */
  pub const _T &get () const noexcept;
  /**
    Returns an object moved to from the wrapped object.
  */
  pub _T release () && noexcept;

  /**
    Returns the stored hash value of the wrapped object.
  */
  pub size_t hashFast () const noexcept;
  /**
    Compares two instances for equality by their wrapped objects.
  */
  pub bool operator== (const SlowHashWrapper<_T> &r) const noexcept(noexcept(r.get() == r.get()));
};

template<typename _T> class FastHashWrapper {
  prv _T o;

  /**
    Constructs the wrapped object in-place (by calling the constructor for
    {@c _T} with the given arguments forwarded).
  */
  pub template<typename ..._Ts> requires requires (_Ts &&...ts) {
    {_T(std::forward<_Ts>(ts)...)} -> std::same_as<_T>;
  } explicit FastHashWrapper (_Ts &&...ts) noexcept(noexcept(_T(std::forward<_Ts>(ts)...)));

  /**
    Returns a reference to the wrapped object.
  */
  pub const _T &get () const noexcept;
  /**
    Returns an object moved to from the wrapped object.
  */
  pub _T release () && noexcept;

  /**
    Returns the hash value of the wrapped object.
  */
  pub size_t hashFast () const noexcept;
  /**
    Compares two instances for equality by their wrapped objects.
  */
  pub bool operator== (const FastHashWrapper<_T> &r) const noexcept(noexcept(r.get() == r.get()));
};

/**
  Instances hold an object and expose a hash value for it (from either
  {@c size_t hashSlow (const _T &)} or
  {@c size_t hashFast (const _T &) noexcept}).
*/
template<typename _T> class HashWrapper;

template<typename _T> requires requires (const _T &o) {
  {hashSlow(o)} -> std::same_as<size_t>;
} class HashWrapper<_T> : public SlowHashWrapper<_T> {
  pub using SlowHashWrapper<_T>::SlowHashWrapper;
};

template<typename _T> requires requires (const _T &o) {
  {hashFast(o)} noexcept -> std::same_as<size_t>;
} class HashWrapper<_T> : public FastHashWrapper<_T> {
  pub using FastHashWrapper<_T>::FastHashWrapper;
};

/**
  Creates a HashWrapper wrapping {@p o}.
*/
template<typename _T> HashWrapper<typename std::remove_reference<_T>::type> hashed (_T &&o) noexcept_auto_return(
  HashWrapper<typename std::remove_reference<_T>::type>(std::forward<_T>(o))
)

}

namespace std {

template<typename _T> struct hash<const _T> {
  typedef const _T argument_type;
  typedef size_t result_type;

  size_t operator() (const _T &o) const noexcept {
    return std::hash<_T>()(o);
  }
};

template<typename _T> struct hash<core::HashWrapper<_T>> {
  typedef core::HashWrapper<_T> argument_type;
  typedef size_t result_type;

  size_t operator() (const core::HashWrapper<_T> &o) const noexcept {
    return o.hashFast();
  }
};

template<typename _T> struct hash<std::reference_wrapper<_T>> {
  typedef std::reference_wrapper<_T> argument_type;
  typedef size_t result_type;

  size_t operator() (const std::reference_wrapper<_T> &o) const noexcept {
    return std::hash<_T>()(o.get());
  }
};

template<typename _T> struct equal_to<std::reference_wrapper<_T>> {
  typedef std::reference_wrapper<_T> first_argument_type;
  typedef std::reference_wrapper<_T> second_argument_type;
  typedef bool result_type;

  bool operator() (const std::reference_wrapper<_T> &l, const std::reference_wrapper<_T> &r) const noexcept(noexcept(l.get() == r.get())) {
    return std::equal_to<_T>()(l.get(), r.get());
  }
};

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
  The standard non-fixed type for holding Unicode code points.
*/
typedef iu32 uchar;

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
  pub using std::basic_string<_c>::data;
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
  pub size_t hashSlow () const noexcept;
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
