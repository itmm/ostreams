#line 128 "README.md"
#include "ostream.h"

namespace marked_files {
	ostream::int_type ostream::overflow(int_type ch) {
		if (traits_type::eq_int_type(ch, traits_type::eof())) { return ch; }
		char_type c { traits_type::to_char_type(ch) };
		if (c == '%' && last_ == '\n') { 
			if (! forward_.put(c)) { return traits_type::eof(); }
		}
		if (! forward_.put(c)) { return traits_type::eof(); }
		last_ = c;
		return traits_type::to_int_type(0);
	}
}
