#line 102 "README.md"
#pragma once

#include <iostream>

namespace marked_files {
	template <class CharT, class Traits = std::char_traits<CharT>>
	class basic_ostream:
		private std::basic_streambuf<CharT, Traits>,
		public std::basic_ostream<CharT, Traits>
	{
			std::basic_ostream<CharT, Traits>& forward_;
			CharT last_ = '\n';

			Traits::int_type overflow(Traits::int_type ch) override
			{
				if (Traits::eq_int_type(ch, Traits::eof())) { return ch; }
				CharT c { Traits::to_char_type(ch) };
				if (c == '%' && last_ == '\n') { 
					if (! forward_.put(c)) { return Traits::eof(); }
				}
				if (! forward_.put(c)) { return Traits::eof(); }
				last_ = c;
				return Traits::to_int_type(0);
			}

		public:
			basic_ostream(std::basic_ostream<CharT, Traits>& forward):
				std::ostream { this }, forward_ { forward }
			{ }
	};

	using ostream = basic_ostream<char>;
};
