#line 102 "README.md"
#pragma once

#include <iostream>

namespace marked_files {
	class ostream: private std::streambuf, public std::ostream {
			std::ostream& forward_;
			int_type last_ = '\n';

			int_type overflow(int_type ch) override;

		public:
			ostream(std::ostream& forward):
				std::ostream { this }, forward_ { forward }
			{ }
	};
};
