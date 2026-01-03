#line 72 "README.md"
#include <cassert>
#include <sstream>

#include "ostream.h"

static void test_ostream(const std::string& in, const std::string& expected) {
	std::ostringstream out;
	marked_files::ostream escaped { out };
	escaped << in;
	assert(out.str() == expected);
}

int main() {
	test_ostream("abc\ndef\n", "abc\ndef\n");
	test_ostream("abc\n%def\n", "abc\n%%def\n");
	test_ostream("abc\n%%def\n", "abc\n%%%def\n");
	test_ostream("a%c", "a%c");
}
