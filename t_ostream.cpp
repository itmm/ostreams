#line 82 "README.md"
#include <cassert>
#include <sstream>

#include "ostream.h"

static void test_ostream(const std::string& in, const std::string& expected) {
	std::ostringstream out;
	marked_files::ostream escaped { out };
	escaped << in;
	assert(out.str() == expected);
}

// test functions
#line 287

static inline void add_simple_command() {
	std::ostringstream out;
	marked_files::ostream escaped { out };
	escaped << "abc";
	escaped.open_command("test");
	escaped << "xyz";
	escaped.close_command();
	assert(out.str() == "abc\n%test xyz\n");
}

#line 167

static inline void escape_first_percent() {
	test_ostream("%abc", "%%abc");
}

#line 150

static inline void dont_escape_inner_percent() {
	test_ostream("a%c", "a%c");
}

#line 134

static inline void escape_double_percent() {
	test_ostream("abc\n%%def\n", "abc\n%%%def\n");
}
#line 118

static inline void escape_single_percent() {
	test_ostream("abc\n%def\n", "abc\n%%def\n");
}
#line 103

static inline void simple_test() {
	test_ostream("abc\ndef\n", "abc\ndef\n");
}
#line 74
int main() {
	// run tests
#line 300
	add_simple_command();
#line 174
	escape_first_percent();
#line 157
	dont_escape_inner_percent();
#line 140
	escape_double_percent();
#line 124
	escape_single_percent();
#line 109
	simple_test();
#line 76
}
