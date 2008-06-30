// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
/*
  testsoon.hpp: "Test soon" testing framework.

  Copyright (C) 2006 Aristid Breitkreuz, Ronny Pfannschmidt and 
                     Benjamin Bykowski

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Aristid Breitkreuz aribrei@arcor.de
  Ronny Pfannschmidt Ronny.Pfannschmidt@gmx.de
  Benjamin Bykowski bennybyko@gmx.de
*/

#ifndef TESTSOON_HPP
#define TESTSOON_HPP

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>
#include <boost/preprocessor/seq/replace.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/to_array.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/array/pop_front.hpp>
#include <boost/preprocessor/array/data.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/control/expr_iif.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/assign/list_of.hpp>

#ifndef NO_STDLIB
#include <string>
#include <sstream>
#include <vector>
#include <typeinfo>
#endif

namespace testsoon {

#ifndef NO_STDLIB

using std::string;

typedef std::vector<string> string_vector;
typedef std::ostream stream_class;

template<class T>
inline string object_to_string(T const &object) {
  std::ostringstream stream;
  stream << object;
  return stream.str();
}

inline string object_to_string(std::type_info const &object) {
  return string(object.name());
}

inline string object_to_string(char const * const object) {
  return string(object);
}

inline string object_to_string(string const &object) {
  return string(object);
}

#endif

#ifndef IN_DOXYGEN
class test_holder;
#endif

extern test_holder &tests();

#ifndef IN_DOXYGEN
#if defined(__EXCEPTIONS) || defined(_CPPUNWIND)
#define TESTSOON_EXCEPTIONS 1
#define TESTSOON_NO_EXCEPTIONS 0
#else
#define TESTSOON_EXCEPTIONS 0
#define TESTSOON_NO_EXCEPTIONS 1
#endif
#endif

class test_reporter;
class test_info;
class test_group;
class test_failure;

class statistics {
public:
  statistics() : good(0), bad(0) {}
  unsigned good;
  unsigned bad;
private:
  typedef bool (statistics::*bool_type)() const;
  bool do_not_compare_implicit_bool() const { return false; }
public:
  operator bool_type() const {
    return bad == 0 ? &statistics::do_not_compare_implicit_bool : 0;
  }
};

template<class T> bool operator==(statistics const &x, T const &) {
  return x.do_not_compare_implicit_bool();
}
template<class T> bool operator!=(statistics const &x, T const &) {
  return x.do_not_compare_implicit_bool();
}
template<class T> bool operator==(T const &, statistics const &x) {
  return x.do_not_compare_implicit_bool();
}
template<class T> bool operator!=(T const &, statistics const &x) {
  return x.do_not_compare_implicit_bool();
}

/**
 * Base class for reporters.
 */
class test_reporter {
public:
  /// Indicates the start of a whole test run.
  virtual void start() {}
  /// Indicates the end of a whole test run.
  virtual void stop() {}
  /// Indicates the start of a test group.
  virtual void begin_group(test_group const &group) { (void)group; }
  /// Indicates the end of a test group.
  virtual void end_group(test_group const &group) { (void)group; }
  /// Indicates that the next event will be success() or failure().
  virtual void before_tests(test_group const &group) { (void)group; }
  /// Indicates that the next event will not be success() or failure().
  virtual void after_tests(test_group const &group) { (void)group; }
  virtual void success(test_info const &info, string const &sequence_key) {
    (void)info;
    (void)sequence_key;
  }
  virtual void failure(test_info const &info, test_failure const &failure, 
                       string const &sequence_key) = 0;
  virtual void stats(statistics const &stat) { (void)stat; }
  virtual ~test_reporter() {}
};

#ifndef IN_DOXYGEN

class node {
public:
  node(test_group *, char const * const, bool = false);
  virtual void run(test_reporter &, statistics &) const = 0;
  virtual ~node() {}

  test_group const * const parent;
  char const * const name;

  void print(stream_class &out) const;

private:
  friend class test_group;
  node *next;
};

inline stream_class &operator<<(stream_class &out, node const &n) {
  n.print(out);
  return out;
}

class test_group : public node {
public:
  test_group(test_group *parent, char const *name)
    : node(parent, name), child(0) {}
  
  void add(node *, bool);

  void run(test_reporter &, statistics &) const;

private:
  node *child;
  test_info *test;
};

inline node::node(test_group *parent, char const *name, bool is_test)
: parent(parent), name(name), next(0) {
  if (parent)
    parent->add(this, is_test);
}

inline void node::print(stream_class &out) const {
  if (parent) {
    parent->print(out);
    if (parent->parent)
      out << '/' << name;
    else
      out << '"' << name << '"';
  }
}

class test_holder : public test_group {
public:
  test_holder() : test_group(0, "") {}
  statistics run(test_reporter &rep) {
    statistics stats;
    run(rep, stats);
    return stats;
  }
  void run(test_reporter &rep, statistics &stats) {
    rep.start();
    test_group::run(rep, stats);
    rep.stop();
    rep.stats(stats);
  }
};

class test_file : public test_group {
public:
  test_file(test_group *parent, char const *file)
    : test_group(parent, file) {}
};

class test_info : public node {
public:
  test_info(test_group *group,
            char const *name, char const *file, unsigned line)
  : node(group, name, true), file(file), line(line) {}

  char const *file;
  unsigned const line;
};

#endif

class test_failure {
public:
  test_failure() : line(0) {}
  test_failure(char const *message,
               unsigned line,
               string_vector const &data = string_vector())
    : message(message), line(line), data(data) {}
  ~test_failure() {}
  char const *message;
  unsigned line;
  string_vector data;

  bool is_failure() { return line; }
};

struct failure_info {
  failure_info(test_info const &test, test_failure const &failure, 
               string const &value)
    : ptest(&test), failure(failure), value(value) {}

  test_info const *ptest;
  test_failure failure;
  string value;
};

class simple_reporter : public test_reporter {
public:
  typedef stream_class stream;
  simple_reporter(stream &out) : out(out) {}
  void before_tests(test_group const &group) {
    out << group << " : ";
    out.flush();
  }

  void after_tests(test_group const &) {
    out << '\n';
    out.flush();
  }

  void success(test_info const &, string const &){
    out << '.';
    out.flush();
  }

  void failure(test_info const &i, test_failure const &x, string const &k) {
    out << "[F=" << i.line << '.' << x.line;
    if (k != string())
      out << '<' << k << '>';
    out << ']';
    out.flush();
  }

protected:
  stream &out;
};

class concise_reporter : public simple_reporter {
public:  
  concise_reporter(stream &out) 
  : simple_reporter(out) {}

private:
  typedef std::vector<failure_info> failure_vector;
  failure_vector reports;
  
  void stats(statistics const &x) {
    out << (x.good + x.bad) << " tests, "
        << x.good << " succeeded, "
        << x.bad << " failed.\n";
    if (x.bad)
      out << "  .----------------.     \n"
          << " /                  \\   \n"
          << "(   FAILURES FOUND   )  \n"
          << " \\                  /  \n"
          << "  '----------------'   \n";
    out.flush();
  }
  
  void write_report() {
    for (failure_vector::const_iterator it = reports.begin();
        it != reports.end();
        ++it)
      write_report_entry(*it);
    out << '\n';
    out.flush();
  }

  void write_report_entry(failure_info const &info) {
    out << "\nError occured in test ";
    if (*info.ptest->name)
      out << '"' << info.ptest->name << "\" ";
    out << "in " << *info.ptest->parent
        << " on line " << info.ptest->line
        << " in check on line " << info.failure.line;
    if (info.value != string())
      out << " with value: \n\t" << info.value;
    else
      out << '.';
    out << "\nProblem: " << info.failure.message << '\n';

    if (!info.failure.data.empty()) {
      out << "Data:\n";
      for (string_vector::const_iterator it = info.failure.data.begin();
           it != info.failure.data.end();
           ++it) {
        out << "\t" << *it << '\n';
      }
    }
    out.flush();
  } 

  void success(test_info const &i, string const &k) {
    simple_reporter::success(i, k);
  }
    
  void failure(test_info const &i, test_failure const &x, string const &k) {
    simple_reporter::failure(i, x, k);
    reports.push_back(failure_info(i, x, k));
  }

  void stop() {
    write_report();
  }
};

typedef concise_reporter default_reporter;

#ifndef IN_DOXYGEN

inline void test_group::add(node *nchild, bool is_test) {
  node *tail = is_test ? test : child;
  if (!tail) {
    if (is_test) test = static_cast<test_info *>(nchild);
    else         child = nchild;
  } else {
    while (tail->next)
      tail = tail->next;    
    tail->next = nchild;
  }
}

inline void test_group::run(test_reporter &rep, statistics &stats) const {
  rep.begin_group(*this);
  if (test) {
    rep.before_tests(*this);
    for (node *it = test; it; it = it->next)
      it->run(rep, stats);
    rep.after_tests(*this);
  }
  for (node *it = child; it; it = it->next)
    it->run(rep, stats);
  rep.end_group(*this);
}

template<class T, class U>
inline void check_condition(T const &a, U const &b, bool cond,
                         char const *msg, unsigned line,
                         test_failure &fail) {
  if (!cond) {
    string_vector data;
    data.push_back(object_to_string(a));
    data.push_back(object_to_string(b));
    fail = test_failure(msg, line, data);
  }
}

template<class T, class U>
inline void check_equals(T const &a, U const &b,
                         char const *msg, unsigned line,
                         test_failure &fail) {
  if (!(a == b)) {
    string_vector data;
    data.push_back(object_to_string(a));
    data.push_back(object_to_string(b));
    fail = test_failure(msg, line, data);
  }
}

template<class F, class T>
inline void do_check1(F fun, T const &val,
                      char const *msg, unsigned line,
                      test_failure &fail) {
  if (!fun(val)) {
    string_vector data;
    data.push_back(object_to_string(val));
    fail = test_failure(msg, line, data);
  }
}

template<class F, class T, class U>
inline void do_check2(F fun, T const &a, U const &b,
                      char const *msg, unsigned line,
                      test_failure &fail) {
  if (!fun(a, b)) {
    string_vector data;
    data.push_back(object_to_string(a));
    data.push_back(object_to_string(b));
    fail = test_failure(msg, line, data);
  }
}

#endif

template<typename Type>
class range_generator {
public:
  typedef Type value_type;
  typedef Type const &const_reference;

  range_generator(const_reference a, const_reference b) : a(a), b(b) {}

  class iterator {
  public:
    iterator(const_reference v = value_type()) : v(v) {}
    iterator(iterator const &x) : v(x.v) {}
    
    bool operator!=(iterator const &rhs) {
      return v != rhs.v;
    }

    value_type &operator*() { return v;}

    iterator &operator++() { 
      ++v;
      return *this; 
    }

    iterator operator++(int) { 
      iterator tmp(*this); 
      ++*this;
      return tmp;
    }

  private:
    value_type v;
  };

  iterator begin() { return iterator(a); }
  iterator end() { return iterator(b); }

private:
  value_type a;
  value_type b;
};

template<typename Type>
class array_generator {
public:
  typedef Type value_type;
  typedef Type const &const_reference;
  typedef Type const *const_pointer;
  typedef const_pointer iterator;

  template<int N>
  array_generator(Type (&array)[N]) : a(array), b(array + N) {}

  template<int N>
  array_generator(Type const (&array)[N]) : a(array), b(array + N) {}

  iterator begin() { return a; }
  iterator end() { return b; }

private:
  const_pointer a, b;
};

/**
 * Add this macro to exactly one source file to ensure proper instantiation.
 */
#define TEST_REGISTRY \
  namespace testsoon { \
    test_holder &tests() { \
      static test_holder t; \
      return t; \
    } \
  }

/**
 * Declare a test group.
 * Test groups are nestable.
 */
#define TEST_GROUP(name) \
    namespace BOOST_PP_CAT(name, _helper) { \
      static ::testsoon::test_group *get_upper_test_group() { \
        return get_test_group(__FILE__); \
      } \
    } \
    namespace BOOST_PP_CAT(name, _testgroup) { \
      static ::testsoon::test_group * \
      get_test_group(char const *) { \
        static ::testsoon::test_group current( \
          BOOST_PP_CAT(name, _helper)::get_upper_test_group(), #name); \
        return &current; \
      } \
    } \
    namespace BOOST_PP_CAT(name, _testgroup)

#ifndef TESTSOON_DUMMY
/**
 * Declare a test (positional). You do not want to use this directly.
 * @param name The name of the test (quoted).
 * @param fixture A tuple consisting of whether to use a fixture and a fixture 
 *             class.
 */
#define PTEST(name, fixture, group_fixture, generator) \
  TESTSOON_PTEST1( \
    name, \
    BOOST_PP_CAT(test_, __LINE__), \
    BOOST_PP_TUPLE_ELEM(2, 0, fixture), \
    BOOST_PP_TUPLE_ELEM(2, 1, fixture), \
    group_fixture, \
    BOOST_PP_TUPLE_ELEM(2, 0, generator), \
    BOOST_PP_TUPLE_ELEM(2, 1, generator))
#else
#define PTEST(name, fixture, group_fixture, generator) \
  static void BOOST_PP_CAT(dummy_, __LINE__) ()
#endif

/**
 * Declare a test (optional name only).
 * @param name The name of the test (not quoted).
 */
#define TEST(name) \
  PTEST(#name, (0, ~), 0, (0, ()))

/**
 * Declare a test with fixture.
 * @param name The name of the test (not quoted).
 * @param fixture_class The fixture class to use.
 */
#define FTEST(name, fixture_class) \
  PTEST(#name, (1, fixture_class), 0, (0, ()))

/**
 * Declare a test with default group fixture, named group_fixture.
 * @param name The name of the test (not quoted).
 */
#define GFTEST(name) \
  PTEST(#name, (0, ~), 1, (0, ()))

#ifndef IN_DOXYGEN

#if TESTSOON_NO_EXCEPTIONS
#define TESTSOON_FAILURE_P ::testsoon::test_failure &testsoon_failure
#else
#define TESTSOON_FAILURE_P int
#endif

#define TESTSOON_TEST_PARAM( \
    has_fixture, \
    fixture_class, \
    has_group_fixture, \
    has_generator, \
    generator_class) \
  BOOST_PP_IIF(has_fixture, fixture_class &fixture, int), \
  BOOST_PP_IIF(has_group_fixture, group_fixture_t &group_fixture, int), \
  BOOST_PP_IIF(has_generator, generator_class::const_reference value, int), \
  TESTSOON_FAILURE_P

#define TESTSOON_PTEST1( \
    name, \
    test_class, \
    has_fixture, \
    fixture_class, \
    group_fixture, \
    has_generator, \
    generator_seq) \
  TESTSOON_PTEST2( \
    name, test_class, has_fixture, fixture_class, \
    TESTSOON_TEST_PARAM( \
      has_fixture, fixture_class, group_fixture, \
      has_generator, BOOST_PP_SEQ_HEAD(generator_seq)), \
    group_fixture, \
    has_generator, \
    BOOST_PP_SEQ_HEAD(generator_seq), \
    BOOST_PP_ARRAY_POP_FRONT(BOOST_PP_SEQ_TO_ARRAY(generator_seq)) \
  )

#if TESTSOON_EXCEPTIONS

#define TESTSOON_TESTRUN(x) \
  try { \
    x; \
    reporter.success(*this, key); \
    ++stats.good; \
  } catch (::testsoon::test_failure const &state) { \
    reporter.failure(*this, state, key); \
    ++stats.bad; \
  }

#else

#define TESTSOON_TESTRUN(x) \
  ::testsoon::test_failure state; \
  x; \
  if (!state.is_failure()) { \
    reporter.success(*this, key); \
    ++stats.good; \
  } else { \
    reporter.failure(*this, state, key); \
    ++stats.bad; \
  }

#endif

#define TESTSOON_PTEST2(\
    name, \
    test_class, \
    has_fixture, \
    fixture_class, \
    test_param, \
    has_group_fixture, \
    has_generator, \
    generator_class, \
    generator_param) \
  namespace { \
    struct test_class \
    : public ::testsoon::test_info { \
      test_class () : ::testsoon::test_info( \
            get_test_group(__FILE__), name, __FILE__, __LINE__) {} \
      void run( \
          ::testsoon::test_reporter &reporter, \
          ::testsoon::statistics &stats \
      ) const { \
        BOOST_PP_EXPR_IIF(has_fixture, fixture_class fixture;) \
        BOOST_PP_EXPR_IIF(has_group_fixture, group_fixture_t group_fixture;) \
        BOOST_PP_EXPR_IIF(has_generator, \
        generator_class gen \
          BOOST_PP_IIF( \
            BOOST_PP_EQUAL(BOOST_PP_ARRAY_SIZE(generator_param), 0), \
            BOOST_PP_EMPTY(), \
            BOOST_PP_ARRAY_DATA(generator_param)); \
        for (generator_class::iterator i = gen.begin(); i != gen.end(); ++i)) {\
          ::testsoon::string key \
            BOOST_PP_EXPR_IIF( \
              has_generator, \
              (::testsoon::object_to_string(*i))); \
          TESTSOON_TESTRUN( \
            do_test( \
                BOOST_PP_IIF(has_fixture, fixture, 0), \
                BOOST_PP_IIF(has_group_fixture, group_fixture, 0), \
                BOOST_PP_IIF(has_generator, *i, 0), \
                BOOST_PP_IIF(TESTSOON_NO_EXCEPTIONS, state, 0) \
            ) \
          ) \
        } \
      } \
      void do_test(test_param) const; \
    } BOOST_PP_CAT(test_obj_, __LINE__); \
  } \
  void test_class::do_test(test_param) const


#define TESTSOON_GEN_TUPLE2SEQ_PROCESS2(x, y) \
  ((x, y)) \
  TESTSOON_GEN_TUPLE2SEQ_PROCESS

#define TESTSOON_GEN_TUPLE2SEQ_PROCESS(x, y) \
  ((x, y)) \
  TESTSOON_GEN_TUPLE2SEQ_PROCESS2

#define TESTSOON_GEN_TUPLE2SEQ_PROCESS_ELIM
#define TESTSOON_GEN_TUPLE2SEQ_PROCESS2_ELIM

#define TESTSOON_GEN_TUPLE2SEQ(x) \
  BOOST_PP_CAT(TESTSOON_GEN_TUPLE2SEQ_PROCESS x, _ELIM)

#define TESTSOON_PARAM_CHANGES(x) \
  ((0, BOOST_PP_SEQ_ELEM(0, TESTSOON_PARAM_INITIAL))) \
  BOOST_PP_SEQ_FOR_EACH( \
    TESTSOON_PARAM_EXPAND, \
    ~, \
    TESTSOON_GEN_TUPLE2SEQ(x))

#define TESTSOON_PARAM_EXPAND(r, d, e) \
  TESTSOON_PARAM_EXPAND2 e

#define TESTSOON_PARAM_EXPAND2(x, y) \
  ((BOOST_PP_CAT(TESTSOON_PARAM__, x)(y)))

#define TESTSOON_PARAM_COMBINE(s, state, x) \
  BOOST_PP_SEQ_REPLACE( \
    state, \
    BOOST_PP_TUPLE_ELEM(2, 0, x), \
    BOOST_PP_TUPLE_ELEM(2, 1, x))

#define TESTSOON_PARAM_INVOKE2(x) \
  BOOST_PP_SEQ_TO_TUPLE( \
    BOOST_PP_SEQ_FOLD_LEFT( \
      TESTSOON_PARAM_COMBINE, \
      TESTSOON_PARAM_INITIAL, \
      TESTSOON_PARAM_CHANGES(x)))

#define TESTSOON_PARAM_INVOKE(x) \
  TESTSOON_PARAM_INVOKEx(TESTSOON_PARAM_INVOKE2(x))


#define TESTSOON_PARAM__name(x)           0, x
#define TESTSOON_PARAM__n(x)              TESTSOON_PARAM__name(x)
#define TESTSOON_PARAM__fixture(x)        1, (1, x)
#define TESTSOON_PARAM__f(x)              TESTSOON_PARAM__fixture(x)
#define TESTSOON_PARAM__group_fixture(x)  2, x
#define TESTSOON_PARAM__gf(x)             TESTSOON_PARAM__group_fixture(x)
#define TESTSOON_PARAM__generator(x)      3, (1, x)
#define TESTSOON_PARAM__gen(x)            TESTSOON_PARAM__generator(x)

#define TESTSOON_PARAM_SHORT_GENERATOR(n, x) \
  TESTSOON_PARAM__generator( \
    (n < \
      BOOST_PP_TUPLE_ELEM(3, 0, x) \
    >) \
    (BOOST_PP_TUPLE_ELEM(3, 1, x)) \
    (BOOST_PP_TUPLE_ELEM(3, 2, x)) \
  )

#define TESTSOON_PARAM__range(x) \
  TESTSOON_PARAM_SHORT_GENERATOR(::testsoon::range_generator, x)
#define TESTSOON_PARAM__array(x) \
  TESTSOON_PARAM_SHORT_GENERATOR(::testsoon::array_generator, x)
#define TESTSOON_PARAM__values(x) \
  TESTSOON_PARAM__generator( \
    (std::vector<BOOST_PP_SEQ_HEAD(x)>) \
    ( \
      (boost::assign::list_of BOOST_PP_SEQ_TAIL(x)). \
      operator std::vector<BOOST_PP_SEQ_HEAD(x)>() \
    ) \
  )

#define TESTSOON_PARAM_INITIAL \
  ("") ((0, ~)) (0) ((0, ()))

#define TESTSOON_PARAM_INVOKEx(x) \
  PTEST x

#if TESTSOON_EXCEPTIONS

#define TESTSOON_FAIL(msg, data) \
    throw ::testsoon::test_failure(msg, __LINE__, data);

#define TESTSOON_ENCLOSURE(x) \
  do { \
    ::testsoon::test_failure testsoon_failure; \
    x; \
    if (testsoon_failure.is_failure()) \
      throw testsoon_failure; \
  } while (false)

#else //TESTSOON_NO_EXCEPTIONS

#define TESTSOON_FAIL(msg, data) \
  testsoon_failure = ::testsoon::test_failure(msg, __LINE__, data); \
  return;

#define TESTSOON_ENCLOSURE(x) \
  do { \
    x; \
    if (testsoon_failure.is_failure()) \
      return; \
  } while (false)

#endif

#endif //IN_DOXY

/**
 * Declare a test (named parameters).
 * @param name The name of the test (quoted).
 */
#define XTEST(named_parameter_sequence) \
  TESTSOON_PARAM_INVOKE(named_parameter_sequence)

/**
 * Check whether two values are equal.
 * If both values are not equal, the test will fail.
 * @param a Some value.
 * @param b Another value.
 */
#define TESTSOON_Equals(a, b) \
  TESTSOON_ENCLOSURE( \
    ::testsoon::check_equals(a, b, \
    "not equal: " #a " and " #b, __LINE__, testsoon_failure); \
  )

#define Equals(a, b) TESTSOON_Equals(a, b)

/**
 * Check a binary condition.
 * If does not evaluate to true, the test will fail.
 * @param condition A condition in the variables a and b
 * @param a Some value.
 * @param b Another value.
 */
#define TESTSOON_True(a, b, condition) \
  TESTSOON_ENCLOSURE( \
    ::testsoon::check_condition(a, b, condition, \
    "false: " #condition, __LINE__, testsoon_failure); \
  )
#define True(a, b, condition) TESTSOON_True(a,b,condition)

#define VERY_CLOSE_EPS (1e-10)
#define VeryClose(a,b) True(a,b,fabs((a)-(b)) < VERY_CLOSE_EPS)

#define CLOSE_EPS (1e-7)
#define Close(a,b) True(a,b,fabs((a)-(b)) < CLOSE_EPS)
#define CloseEps(a,b,eps) True(a,b,fabs((a)-(b)) < (eps))
#define LessThan(a,b) True(a,b,a < b)
#define GreaterThan(a,b) True(a,b,a > b)
#define GreaterThanEq(a,b) True(a,b,a >= b)

#define NotClose(a,b) True(a,b,fabs((a)-(b)) > CLOSE_EPS)

/**
 * Check that an expression throws.
 * If no exception is thrown, the test will fail.
 * @param x The expression to test.
 * @param t The exception type to check for.
 * @param w The expected value of caught_exception.what().
 */
#if TESTSOON_EXCEPTIONS
#define TESTSOON_Throws(x, t, w) \
  do { \
    try { \
      (x); \
      TESTSOON_FAIL("not throwed " #t, ::testsoon::string_vector()); \
    } catch (t &e) { \
      if ( \
        ::testsoon::string(w) != ::testsoon::string() && \
        ::testsoon::string(e.what()) != ::testsoon::string((w))) \
        TESTSOON_FAIL("throwed " #t " with wrong message", \
          ::testsoon::string_vector(1, e.what())); \
    } \
  } while (0)
#else
#define TESTSOON_Throws(x, t, w) \
  TESTSOON_Check(!"Throws check without exception support")
#endif

#define Throws(x, t, w) TESTSOON_Throws(x, t, w)

/**
 * Check that an expression does not throw.
 * If a specified exception is thrown, the test will fail.
 * @param x The expression to test.
 * @param t The exception type to check for or "..." (without quotes).
 */
#if TESTSOON_EXCEPTIONS
#define TESTSOON_Nothrows(x, t) \
	do { \
		try { \
			(x); \
		} catch (t) { \
			TESTSOON_FAIL("throwed " #t, ::testsoon::string_vector()); \
		} \
	} while (0)
#else
#define TESTSOON_Nothrows(x, t) \
  TESTSOON_Check(!"Nothrows check without exception support")
#endif
#define Nothrows(x, t) TESTSOON_Nothrows(x, t)

#define TESTSOON_Check(x) \
  do { \
    if (!(x)) \
      TESTSOON_FAIL("check " #x, ::testsoon::string_vector()); \
  } while (0)

#define Check(x) TESTSOON_Check(x)

#define TESTSOON_Check1(x, a) \
  TESTSOON_ENCLOSURE( \
    ::testsoon::do_check1(x, a, "check1 " #x, __LINE__, testsoon_failure); \
  )

#define Check1(x, a) TESTSOON_Check1(x, a)

#define TESTSOON_Check2(x, a, b) \
  TESTSOON_ENCLOSURE( \
    ::testsoon::do_check2(x, a, b, "check2 " #x, __LINE__, testsoon_failure); \
  )

#define Check2(x, a, b) TESTSOON_Check2(x, a, b)

} //namespace testsoon

#ifndef IN_DOXYGEN

inline static ::testsoon::test_group *
get_test_group(char const *filename) {
  static ::testsoon::test_file file(&::testsoon::tests(), filename);
  return &file;
}

#endif

#endif
