#include "parsers/ValidatedReportParser.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "constants/Constants.hpp"
#include "parsers/JsonReportParser.hpp"
#include "parsers/XmlReportParser.hpp"

namespace {
bool validateTaxYear(const types::Report& report)
{
    using Clock = std::chrono::system_clock;

    const auto now = Clock::now();
    const std::time_t now_c = Clock::to_time_t(now);
    const struct tm* parts = std::localtime(&now_c);
    const auto currentYear = 1900 + parts->tm_year;

    return report.year <= currentYear;
}
} // namespace

namespace parsers {
template <typename BaseReportParser>
std::optional<types::Report> ValidatedReportParser<BaseReportParser>::parse(const std::string_view rawReport) const
    try {
    if (const auto report = BaseReportParser::parse(rawReport)) {
        if (report->amount > 0.0 and validateTaxYear(*report)) {
            std::string tax = report->tax;
            boost::algorithm::to_lower(tax);

            if (constants::validTaxNames.count(tax))
                return report;
        }
    }

    return std::nullopt;
} catch (const std::exception& e) {
    std::cerr << __FILE__ << ' ' << e.what() << '\n';
    return std::nullopt;
}

template class ValidatedReportParser<JsonReportParser>;
template class ValidatedReportParser<XmlReportParser>;
} // namespace parsers
