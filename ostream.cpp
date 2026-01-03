#line 128 "README.md"
#include "ostream.h"

namespace marked_files {
	ostream::int_type ostream::overflow(int_type ch) {
		if (ch == '%' && last_ == '\n') { 
			if (! forward_.put(ch)) { return traits_type::eof(); }
		}
		if (! forward_.put(ch)) { return traits_type::eof(); }
		last_ = ch;
		return 0;
	}
}
