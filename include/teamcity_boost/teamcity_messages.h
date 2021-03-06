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

#ifndef H_TEAMCITY_MESSAGES
#define H_TEAMCITY_MESSAGES

#include <string>
#include <iostream>

namespace JetBrains {

std::string getFlowIdFromEnvironment();
bool underTeamcity();

class TeamcityMessages {
    std::ostream *m_out;
    
protected:
    std::string escape(const std::string& s);

    void openMsg(const std::string &name);
    void writeProperty(const std::string& name, const std::string& value);
    void closeMsg();

public:
    TeamcityMessages();
    
    void setOutput(std::ostream &);
    
    void suiteStarted(const std::string& name, const std::string& flowid = "");
    void suiteFinished(const std::string& name, const std::string& flowid = "");
    
    void testStarted(const std::string& name, const std::string& flowid = "");
    void testFailed(const std::string& name, const std::string& message, const std::string& details, const std::string& flowid = "");
    void testIgnored(const std::string& name, const std::string& message, const std::string& flowid = "");
    void testFinished(const std::string& name, int durationMs = -1, const std::string& flowid = "");
};

}

#endif /* H_TEAMCITY_MESSAGES */
