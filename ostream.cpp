#line 128 "README.md"
#include "ostream.h"

namespace marked_files {
	ostream::int_type ostream::overflow(int_type ch) {
		if (ch == '%' && last_ == '\n') { forward_.put(ch); }
		forward_.put(ch);
		last_ = ch;
		return 0;
	}
}
