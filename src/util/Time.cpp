#include "util/Time.h"

#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iomanip>
#include <sstream>

namespace sniffles::util {

std::string NowIso8601() {
  using namespace boost::posix_time;

  ptime now = microsec_clock::local_time();

  std::ostringstream out;
  out << std::setfill('0') << std::setw(4) << now.date().year() << "-"
      << std::setw(2) << now.date().month().as_number() << "-"
      << std::setw(2) << now.date().day().as_number() << "T"
      << std::setw(2) << now.time_of_day().hours() << ":"
      << std::setw(2) << now.time_of_day().minutes() << ":"
      << std::setw(2) << now.time_of_day().seconds();

  return out.str();
}

std::string NowIso8601WithMicroseconds() {
  using namespace boost::posix_time;

  ptime now = microsec_clock::local_time();

  std::ostringstream out;
  out << std::setfill('0') << std::setw(4) << now.date().year() << "-"
      << std::setw(2) << now.date().month().as_number() << "-"
      << std::setw(2) << now.date().day().as_number() << "T"
      << std::setw(2) << now.time_of_day().hours() << ":"
      << std::setw(2) << now.time_of_day().minutes() << ":"
      << std::setw(2) << now.time_of_day().seconds() << "."
      << std::setw(6) << now.time_of_day().fractional_seconds();

  return out.str();
}

} // namespace sniffles::util
