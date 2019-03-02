/* Copyright 2011 JetBrains s.r.o.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * $Revision: 88625 $
*/

#include <sstream>

#include <boost/test/unit_test.hpp>
#include <boost/test/results_collector.hpp>
#include <boost/test/utils/basic_cstring/io.hpp>
#include <boost/test/unit_test_log.hpp>
#include <boost/test/included/unit_test.hpp>

#include <teamcity_boost/teamcity_messages.h>

using namespace boost::unit_test;
using namespace std;
using boost::execution_exception;

namespace JetBrains {

// Custom formatter for TeamCity messages
class TeamcityBoostLogFormatter: public boost::unit_test::unit_test_log_formatter {
    TeamcityMessages messages;
    std::string currentDetails;
    std::string flowId;
    
public:
    explicit TeamcityBoostLogFormatter(const std::string &_flowId);
    TeamcityBoostLogFormatter();
    
    void log_start(std::ostream&, boost::unit_test::counter_t test_cases_amount);
    void log_finish(std::ostream&);

    virtual void log_exception_start(std::ostream& os, log_checkpoint_data const& lcd, boost::execution_exception const& ex);
    virtual void log_exception_finish(std::ostream& os);

    virtual void entry_context_start(std::ostream& os, log_level l);
    virtual void log_entry_context(std::ostream& os, const_string value);
    virtual void entry_context_finish(std::ostream& os);

    virtual void log_entry_context(std::ostream &os, log_level l, const_string value);
    virtual void entry_context_finish(std::ostream &os, log_level l);

    void log_build_info(std::ostream&);

    void test_unit_start(std::ostream&, boost::unit_test::test_unit const& tu);
    void test_unit_finish(std::ostream&,
        boost::unit_test::test_unit const& tu,
        unsigned long elapsed);
    void test_unit_skipped(std::ostream&, boost::unit_test::test_unit const& tu);

    void log_exception(std::ostream&,
        boost::unit_test::log_checkpoint_data const&,
        boost::unit_test::const_string explanation);

    void log_entry_start(std::ostream&,
        boost::unit_test::log_entry_data const&,
        log_entry_types let);
    void log_entry_value(std::ostream&, boost::unit_test::const_string value);
    void log_entry_finish(std::ostream&);
};

// Fake fixture to register formatter
struct TeamcityFormatterRegistrar {
    TeamcityFormatterRegistrar() 
    {
#if defined(_WIN32) || defined(_WIN64)
        if (JetBrains::underTeamcity()) {
            boost::unit_test::unit_test_log.set_formatter(new JetBrains::TeamcityBoostLogFormatter());
            boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_successful_tests);
        }
#endif
    }
};
BOOST_GLOBAL_FIXTURE(TeamcityFormatterRegistrar);

// Formatter implementation
string toString(const_string bstr) 
{
    stringstream ss;
    
    ss << bstr;
    
    return ss.str();
}

TeamcityBoostLogFormatter::TeamcityBoostLogFormatter(const std::string &_flowId)
: flowId(_flowId)
{}

TeamcityBoostLogFormatter::TeamcityBoostLogFormatter()
: flowId(getFlowIdFromEnvironment())
{}

//virtual 
void TeamcityBoostLogFormatter::log_start(ostream &out, counter_t test_cases_amount)
{}

//virtual 
void TeamcityBoostLogFormatter::log_finish(ostream &out)
{}

//virtual 
void TeamcityBoostLogFormatter::log_exception_start(ostream &os, log_checkpoint_data const &lcd, execution_exception const &ex)
{}

//virtual 
void TeamcityBoostLogFormatter::log_exception_finish(ostream &os)
{}

//virtual 
void TeamcityBoostLogFormatter::log_build_info(ostream &out)
{}

//virtual 
void TeamcityBoostLogFormatter::test_unit_start(ostream &out, test_unit const& tu) 
{
    messages.setOutput(out);

    if (tu.p_type == TUT_CASE) {
        messages.testStarted(tu.p_name, flowId);
    } else {
        messages.suiteStarted(tu.p_name, flowId);
    }
    
    currentDetails.clear();
}

//virtual 
void TeamcityBoostLogFormatter::test_unit_finish(ostream &out, test_unit const& tu, unsigned long elapsed) 
{
    messages.setOutput(out);

    test_results const& tr = results_collector.results(tu.p_id);
    if (tu.p_type == TUT_CASE) {
        if(!tr.passed()) {
            if(tr.p_skipped) {
                messages.testIgnored(tu.p_name, "ignored", flowId);
            } else if (tr.p_aborted) {
                messages.testFailed(tu.p_name, "aborted", currentDetails, flowId);
            } else {
                messages.testFailed(tu.p_name, "failed", currentDetails, flowId);
            }
        }
        
        messages.testFinished(tu.p_name, elapsed / 1000, flowId);
    } else {
        messages.suiteFinished(tu.p_name, flowId);
    }
}

//virtual 
void TeamcityBoostLogFormatter::test_unit_skipped(ostream &out, test_unit const& tu)
{}

//virtual 
void TeamcityBoostLogFormatter::log_exception(ostream &out, log_checkpoint_data const&, const_string explanation) 
{
    string what = toString(explanation);
    
    out << what << endl;
    currentDetails += what + "\n";
}

//virtual 
void TeamcityBoostLogFormatter::log_entry_start(ostream&, log_entry_data const&, log_entry_types let)
{}

//virtual 
void TeamcityBoostLogFormatter::log_entry_value(ostream &out, const_string value) 
{
    out << value;
    currentDetails += toString(value);
}

//virtual 
void TeamcityBoostLogFormatter::log_entry_finish(ostream &out) 
{
    out << endl;
    currentDetails += "\n";
}



//virtual
void TeamcityBoostLogFormatter::entry_context_start(ostream &os, log_level l)
{}

//virtual 
void TeamcityBoostLogFormatter::log_entry_context(ostream &os, const_string value)
{}

//virtual 
void TeamcityBoostLogFormatter::entry_context_finish(ostream &os)
{}

//virtual
void TeamcityBoostLogFormatter::log_entry_context(std::ostream &os, log_level l, const_string value)
{}

//virtual
void TeamcityBoostLogFormatter::entry_context_finish(std::ostream &os, log_level l)
{}

}
