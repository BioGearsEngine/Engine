/**************************************************************************************
Copyright 2015 Applied Research Associates, Inc.
Licensed under the Apache License, Version 2.0 (the "License"); you may not use
this file except in compliance with the License. You may obtain a copy of the License
at:
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
**************************************************************************************/

#include "CommonDataModelTest.h"
#include "patient/SEPatient.h"
#include "Serializer.h"
#include "utils/TimingProfile.h"
#include "utils/FileUtils.h"
#include "utils/testing/SETestReport.h"
#include "utils/testing/SETestCase.h"

void CommonDataModelTest::ReadPatientDirectory(const std::string& rptDirectory)
{
  TimingProfile pTimer;
  std::string testName = "ReadPatientDirectory";
	Logger logger(rptDirectory + "\\" + testName+".log");
	SEPatient obj (&logger);

	std::string dir = GetCurrentWorkingDirectory();	
	dir.append("\\patients");

	SETestReport testReport(&logger);
  SETestSuite&  testSuite = testReport.CreateTestSuite();
	testSuite.SetName(testName);

	std::vector<std::string> files;
	ListFiles(dir, files, ".xml");
	for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) 
	{
		if(it->find("xml")!=std::string::npos)
		{
      pTimer.Start("Case");
      SETestCase& testCase = testSuite.CreateTestCase();
			logger.Info(it->c_str());				
			if(!obj.LoadFile(*it))				
				testCase.AddFailure("Unable to load patient "+*it);
      testCase.GetDuration().SetValue(pTimer.GetElapsedTime_s("Case"), TimeUnit::s);
      testCase.SetName(obj.GetName());
		}
	}
	testReport.WriteFile(rptDirectory +"\\"+testName+"Report.xml");
}
