#line 193 "README.md"
#pragma once

#include <iostream>

namespace marked_files {
	template <class CharT, class Traits = std::char_traits<CharT>>
	class basic_ostream:
		private std::basic_streambuf<CharT, Traits>,
		public std::basic_ostream<CharT, Traits>
	{
			std::basic_ostream<CharT, Traits>& forward_;
#line 222
			CharT last_ { '\n' };
#line 336
			bool in_command_ { false };
#line 236

			Traits::int_type forward_status() {
				return forward_ ? Traits::to_int_type(0) : Traits::eof();
			}

			Traits::int_type overflow(Traits::int_type ch) override
			{
				if (Traits::eq_int_type(ch, Traits::eof())) { return ch; }
				CharT c { Traits::to_char_type(ch) };
#line 343
				if (in_command_) {
					if (c == '\n') { return Traits::eof(); }
					forward_.put(c); last_ = c;
					return forward_status();
				}
#line 245
				if (c == '%' && last_ == '\n') { forward_.put(c); }
				forward_.put(c);
				last_ = c;
				return forward_status();
			}
#line 204

		public:
#line 352
			Traits::int_type open_command(const std::string& name) {
				if (last_ != '\n') { this->put('\n'); }
				in_command_ = true;
				*this << '%' << name << ' ';
				return forward_status();
			}

			Traits::int_type close_command() { 
				if (! in_command_) { return Traits::eof(); }
				in_command_ = false;
				this->put('\n');
				return forward_status();
			}
#line 206
			basic_ostream(std::basic_ostream<CharT, Traits>& forward):
				std::ostream { this }, forward_ { forward }
			{ }
	};

	using ostream = basic_ostream<char>;
};
