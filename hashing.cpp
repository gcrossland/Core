#include "header.hpp"
#include <algorithm>
#include <unordered_set>

using core::check;
using core::HashWrapper;
using core::hashSlow;
using core::hashFast;
using std::move;
using core::hashed;
using std::min;
using std::vector;
using std::unordered_set;
using std::reference_wrapper;
using std::ref;

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
size_t cCons0, cCons1, cCons2, cConsCopy, cConsMove, cHash, cEq;

void r () {
  cCons0 = cCons1 = cCons2 = cConsCopy = cConsMove = cHash = cEq = 0;
}

void checkC (size_t eCons0, size_t eCons1, size_t eCons2, size_t eConsCopy, size_t eConsMove, size_t eHash, size_t eEq) {
  check(eCons0, cCons0);
  check(eCons1, cCons1);
  check(eCons2, cCons2);
  check(eConsCopy, cConsCopy);
  check(eConsMove, cConsMove);
  check(eHash, cHash);
  check(eEq, cEq);
};

class SlowlyHashableThing {
  prv int a, b, c;

  pub SlowlyHashableThing () noexcept : a(1), b(2), c(3) {
    ++cCons0;
  }

  pub explicit SlowlyHashableThing (int a) noexcept : a(a), b(2), c(3) {
    ++cCons1;
  }

  pub SlowlyHashableThing (int a, int b) : a(a), b(b), c(3) {
    ++cCons2;
  }

  pub SlowlyHashableThing (const SlowlyHashableThing &o) : a(o.a), b(o.b), c(o.c) {
    ++cConsCopy;
  }

  pub SlowlyHashableThing (SlowlyHashableThing &&o) noexcept : a(o.a), b(o.b), c(o.c) {
    ++cConsMove;
    o.a = o.b = o.c = 0;
  }

  pub size_t hashSlow () const noexcept {
    ++cHash;
    return static_cast<size_t>(a * b * c);
  }

  friend bool operator== (const SlowlyHashableThing &l, const SlowlyHashableThing &r) noexcept {
    ++cEq;
    return l.a == r.a && l.b == r.b && l.c == r.c;
  }
};

class SlowlyHashableExceptingHashThing {
  prv int a, b, c;

  pub SlowlyHashableExceptingHashThing () noexcept : a(1), b(2), c(3) {
    ++cCons0;
  }

  pub explicit SlowlyHashableExceptingHashThing (int a) noexcept : a(a), b(2), c(3) {
    ++cCons1;
  }

  pub SlowlyHashableExceptingHashThing (int a, int b) : a(a), b(b), c(3) {
    ++cCons2;
  }

  pub SlowlyHashableExceptingHashThing (const SlowlyHashableExceptingHashThing &o) : a(o.a), b(o.b), c(o.c) {
    ++cConsCopy;
  }

  pub SlowlyHashableExceptingHashThing (SlowlyHashableExceptingHashThing &&o) noexcept : a(o.a), b(o.b), c(o.c) {
    ++cConsMove;
    o.a = o.b = o.c = 0;
  }

  pub size_t hashSlow () const {
    ++cHash;
    return static_cast<size_t>(a * b * c);
  }

  friend bool operator== (const SlowlyHashableExceptingHashThing &l, const SlowlyHashableExceptingHashThing &r) noexcept {
    ++cEq;
    return l.a == r.a && l.b == r.b && l.c == r.c;
  }
};

class SlowlyHashableExceptingEqThing {
  prv int a, b, c;

  pub SlowlyHashableExceptingEqThing () noexcept : a(1), b(2), c(3) {
    ++cCons0;
  }

  pub explicit SlowlyHashableExceptingEqThing (int a) noexcept : a(a), b(2), c(3) {
    ++cCons1;
  }

  pub SlowlyHashableExceptingEqThing (int a, int b) : a(a), b(b), c(3) {
    ++cCons2;
  }

  pub SlowlyHashableExceptingEqThing (const SlowlyHashableExceptingEqThing &o) : a(o.a), b(o.b), c(o.c) {
    ++cConsCopy;
  }

  pub SlowlyHashableExceptingEqThing (SlowlyHashableExceptingEqThing &&o) noexcept : a(o.a), b(o.b), c(o.c) {
    ++cConsMove;
    o.a = o.b = o.c = 0;
  }

  pub size_t hashSlow () const noexcept {
    ++cHash;
    return static_cast<size_t>(a * b * c);
  }

  friend bool operator== (const SlowlyHashableExceptingEqThing &l, const SlowlyHashableExceptingEqThing &r) {
    ++cEq;
    return l.a == r.a && l.b == r.b && l.c == r.c;
  }
};

class QuicklyHashableThing {
  prv int a, b, c;

  pub QuicklyHashableThing () noexcept : a(1), b(2), c(3) {
    ++cCons0;
  }

  pub explicit QuicklyHashableThing (int a) noexcept : a(a), b(2), c(3) {
    ++cCons1;
  }

  pub QuicklyHashableThing (int a, int b) : a(a), b(b), c(3) {
    ++cCons2;
  }

  pub QuicklyHashableThing (const QuicklyHashableThing &o) : a(o.a), b(o.b), c(o.c) {
    ++cConsCopy;
  }

  pub QuicklyHashableThing (QuicklyHashableThing &&o) noexcept : a(o.a), b(o.b), c(o.c) {
    ++cConsMove;
    o.a = o.b = o.c = 0;
  }

  pub size_t hashFast () const noexcept {
    ++cHash;
    return static_cast<size_t>(a * b * c);
  }

  friend bool operator== (const QuicklyHashableThing &l, const QuicklyHashableThing &r) noexcept {
    ++cEq;
    return l.a == r.a && l.b == r.b && l.c == r.c;
  }
};

class QuicklyHashableExceptingEqThing {
  prv int a, b, c;

  pub QuicklyHashableExceptingEqThing () noexcept : a(1), b(2), c(3) {
    ++cCons0;
  }

  pub explicit QuicklyHashableExceptingEqThing (int a) noexcept : a(a), b(2), c(3) {
    ++cCons1;
  }

  pub QuicklyHashableExceptingEqThing (int a, int b) : a(a), b(b), c(3) {
    ++cCons2;
  }

  pub QuicklyHashableExceptingEqThing (const QuicklyHashableExceptingEqThing &o) : a(o.a), b(o.b), c(o.c) {
    ++cConsCopy;
  }

  pub QuicklyHashableExceptingEqThing (QuicklyHashableExceptingEqThing &&o) noexcept : a(o.a), b(o.b), c(o.c) {
    ++cConsMove;
    o.a = o.b = o.c = 0;
  }

  pub size_t hashFast () const noexcept {
    ++cHash;
    return static_cast<size_t>(a * b * c);
  }

  friend bool operator== (const QuicklyHashableExceptingEqThing &l, const QuicklyHashableExceptingEqThing &r) {
    ++cEq;
    return l.a == r.a && l.b == r.b && l.c == r.c;
  }
};

template<typename _Wrappee, bool _noexceptHash, bool _noexceptEq, bool _cachedHash, bool _hashingEq> void testValueHashing () {
  check((!_cachedHash || _noexceptHash) == noexcept(HashWrapper<_Wrappee>()));
  r();
  HashWrapper<_Wrappee> o0;
  checkC(1, 0, 0, 0, 0, _cachedHash, 0);

  check((!_cachedHash || _noexceptHash) == noexcept(HashWrapper<_Wrappee>(4)));
  r();
  HashWrapper<_Wrappee> o1(4); // different value, different hash
  checkC(0, 1, 0, 0, 0, _cachedHash, 0);

  check(!noexcept(HashWrapper<_Wrappee>(2, 1)));
  r();
  HashWrapper<_Wrappee> o2(2, 1); // different value, same hash
  checkC(0, 0, 1, 0, 0, _cachedHash, 0);

  _Wrappee x;
  check(!noexcept(HashWrapper<_Wrappee>(x)));
  r();
  HashWrapper<_Wrappee> o3(x);
  checkC(0, 0, 0, 1, 0, _cachedHash, 0);

  check((!_cachedHash || _noexceptHash) == noexcept(HashWrapper<_Wrappee>(move(x))));
  r();
  HashWrapper<_Wrappee> o4(move(x));
  checkC(0, 0, 0, 0, 1, _cachedHash, 0);

  _Wrappee y;
  check(!noexcept(hashed(y)));
  r();
  auto o5 = hashed(y);
  cConsMove = min(cConsMove, static_cast<size_t>(0)); // exactly 0 moves iff copy elision is complete
  checkC(0, 0, 0, 1, 0, _cachedHash, 0);

  check((!_cachedHash || _noexceptHash) == noexcept(hashed(move(y))));
  r();
  auto o6 = hashed(move(y));
  cConsMove = min(cConsMove, static_cast<size_t>(1)); // exactly 1 move iff copy elision is complete
  checkC(0, 0, 0, 0, 1, _cachedHash, 0);

  check(!noexcept(HashWrapper<_Wrappee>(o4)));
  r();
  HashWrapper<_Wrappee> o7(o4);
  checkC(0, 0, 0, 1, 0, 0, 0);

  check(noexcept(HashWrapper<_Wrappee>(move(o7))));
  r();
  HashWrapper<_Wrappee> o8(move(o7));
  checkC(0, 0, 0, 0, 1, 0, 0);

  r();
  check(&o0.get() >= static_cast<void *>(&o0));
  check(&o0.get() + 1 <= static_cast<void *>(&o0 + 1));
  checkC(0, 0, 0, 0, 0, 0, 0);

  r();
  check(o0.hash() == o0.hash());
  check(o0.hash() != o1.hash());
  check(o0.hash() == o2.hash());
  check(o0.hash() == o3.hash());
  check(o0.hash() == o4.hash());
  checkC(0, 0, 0, 0, 0, _cachedHash ? 0 : 10, 0);

  check((_noexceptEq && (!(_hashingEq && !_cachedHash) || _noexceptHash)) == noexcept(o0 == o0));
  r();
  check(o0 == o0);
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, 1);
  r();
  check(!(o0 == o1));
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, !_hashingEq);
  r();
  check(!(o0 == o2));
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, 1);
  r();
  check(o0 == o3);
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, 1);
  r();
  check(o0 == o4);
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, 1);

  r();
  _Wrappee z(move(o2).release());
  cConsMove = min(cConsMove, static_cast<size_t>(1)); // exactly 1 move iff copy elision is complete
  checkC(0, 0, 0, 0, 1, 0, 0);
  check(_Wrappee(2, 1), z);

  vector<HashWrapper<_Wrappee>> v;
  r();
  v.emplace_back(4);
  checkC(0, 1, 0, 0, 0, _cachedHash, 0);

  unordered_set<HashWrapper<_Wrappee>> s;
  r();
  s.emplace(4);
  checkC(0, 1, 0, 0, 0, 1, 0); // DODGY depends on ins and outs of unordered_set impl
  r();
  check(s.find(o0) == s.end());
  check(s.find(o1) != s.end());
  if (_cachedHash) { // DODGY depends on ins and outs of unordered_set impl
    check(cHash == 0);
  } else {
    check(cHash != 0);
  }
  check(cEq != 0);
}

template<typename _Referent, bool _noexceptHash, bool _noexceptEq, bool _cachedHash, bool _hashingEq> void testReferenceHashing () {
  typedef reference_wrapper<_Referent> _Wrappee;

  _Referent v0;
  check((!_cachedHash || _noexceptHash) == noexcept(HashWrapper<_Wrappee>(v0)));
  r();
  HashWrapper<_Wrappee> o0(v0);
  checkC(0, 0, 0, 0, 0, _cachedHash, 0);

  _Referent v1(4); // different value, different hash
  check((!_cachedHash || _noexceptHash) == noexcept(HashWrapper<_Wrappee>(v1)));
  r();
  HashWrapper<_Wrappee> o1(v1);
  checkC(0, 0, 0, 0, 0, _cachedHash, 0);

  _Referent v2(2, 1); // different value, same hash
  check((!_cachedHash || _noexceptHash) == noexcept(HashWrapper<_Wrappee>(v2)));
  r();
  HashWrapper<_Wrappee> o2(v2);
  checkC(0, 0, 0, 0, 0, _cachedHash, 0);

  _Referent v3;
  _Wrappee x(v3);
  check((!_cachedHash || _noexceptHash) == noexcept(HashWrapper<_Wrappee>(x)));
  r();
  HashWrapper<_Wrappee> o3(x);
  checkC(0, 0, 0, 0, 0, _cachedHash, 0);

  _Referent v5;
  _Wrappee y(v5);
  check((!_cachedHash || _noexceptHash) == noexcept(hashed(y)));
  r();
  auto o5 = hashed(y);
  checkC(0, 0, 0, 0, 0, _cachedHash, 0);

  check(noexcept(HashWrapper<_Wrappee>(o3)));
  r();
  HashWrapper<_Wrappee> o7(o3);
  checkC(0, 0, 0, 0, 0, 0, 0);

  r();
  check(&o0.get() >= static_cast<void *>(&o0));
  check(&o0.get() + 1 <= static_cast<void *>(&o0 + 1));
  check(&o3.get() != &o7.get());
  check(&o3.get().get() == &o7.get().get());
  checkC(0, 0, 0, 0, 0, 0, 0);

  r();
  check(o0.hash() == o0.hash());
  check(o0.hash() != o1.hash());
  check(o0.hash() == o2.hash());
  check(o0.hash() == o3.hash());
  checkC(0, 0, 0, 0, 0, _cachedHash ? 0 : 8, 0);

  check((_noexceptEq && (!(_hashingEq && !_cachedHash) || _noexceptHash)) == noexcept(o0 == o0));
  r();
  check(o0 == o0);
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, 1);
  r();
  check(!(o0 == o1));
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, !_hashingEq);
  r();
  check(!(o0 == o2));
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, 1);
  r();
  check(o0 == o3);
  checkC(0, 0, 0, 0, 0, _hashingEq && !_cachedHash ? 2 : 0, 1);

  r();
  _Wrappee z(move(o2).release());
  checkC(0, 0, 0, 0, 0, 0, 0);
  check(_Wrappee(v2), z);
  check(_Wrappee(v2).get(), z.get());

  vector<HashWrapper<_Wrappee>> v;
  r();
  v.emplace_back(v1);
  checkC(0, 0, 0, 0, 0, _cachedHash, 0);

  unordered_set<HashWrapper<_Wrappee>> s;
  r();
  s.emplace(v1);
  checkC(0, 0, 0, 0, 0, 1, 0); // DODGY depends on ins and outs of unordered_set impl
  r();
  check(s.find(o0) == s.end());
  check(s.find(o1) != s.end());
  if (_cachedHash) { // DODGY depends on ins and outs of unordered_set impl
    check(cHash == 0);
  } else {
    check(cHash != 0);
  }
  check(cEq != 0);
  check(s.find(HashWrapper<_Wrappee>(v0)) == s.end());
  check(s.find(HashWrapper<_Wrappee>(v1)) != s.end());
  check(s.find(hashed(ref(v0))) == s.end());
  check(s.find(hashed(ref(v1))) != s.end());
}

void testHashing () {
  testValueHashing<SlowlyHashableThing, true, true, true, true>();
  testValueHashing<SlowlyHashableExceptingHashThing, false, true, true, true>();
  check(sizeof(HashWrapper<SlowlyHashableThing>) == sizeof(HashWrapper<SlowlyHashableExceptingHashThing>));
  testValueHashing<SlowlyHashableExceptingEqThing, true, false, true, true>();
  check(sizeof(HashWrapper<SlowlyHashableThing>) == sizeof(HashWrapper<SlowlyHashableExceptingEqThing>));

  testValueHashing<QuicklyHashableThing, true, true, false, false>();
  check(sizeof(HashWrapper<SlowlyHashableThing>) >= sizeof(HashWrapper<QuicklyHashableThing>));
  testValueHashing<QuicklyHashableExceptingEqThing, true, false, false, false>();
  check(sizeof(HashWrapper<QuicklyHashableThing>) == sizeof(HashWrapper<QuicklyHashableExceptingEqThing>));

  testReferenceHashing<SlowlyHashableThing, true, true, true, true>();
  testReferenceHashing<SlowlyHashableExceptingHashThing, false, true, true, true>();
  check(sizeof(HashWrapper<reference_wrapper<SlowlyHashableThing>>) == sizeof(HashWrapper<reference_wrapper<SlowlyHashableExceptingHashThing>>));
  testReferenceHashing<SlowlyHashableExceptingEqThing, true, false, true, true>();
  check(sizeof(HashWrapper<reference_wrapper<SlowlyHashableThing>>) == sizeof(HashWrapper<reference_wrapper<SlowlyHashableExceptingEqThing>>));

  testReferenceHashing<QuicklyHashableThing, true, true, false, false>();
  check(sizeof(HashWrapper<reference_wrapper<SlowlyHashableThing>>) >= sizeof(HashWrapper<reference_wrapper<QuicklyHashableThing>>));
  testReferenceHashing<QuicklyHashableExceptingEqThing, true, false, false, false>();
  check(sizeof(HashWrapper<reference_wrapper<QuicklyHashableThing>>) == sizeof(HashWrapper<reference_wrapper<QuicklyHashableExceptingEqThing>>));
}

/* -----------------------------------------------------------------------------
----------------------------------------------------------------------------- */
