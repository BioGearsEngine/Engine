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
package mil.tatrc.physiology.utilities.testing;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.InetAddress;
import java.util.*;

import mil.tatrc.physiology.datamodel.CDMSerializer;
import mil.tatrc.physiology.datamodel.bind.EnumOnOff;
import mil.tatrc.physiology.datamodel.bind.TestReportData;
import mil.tatrc.physiology.datamodel.properties.CommonUnits.TimeUnit;
import mil.tatrc.physiology.datamodel.scenario.SEScenarioAutoSerialization;
import mil.tatrc.physiology.utilities.EmailUtil;
import mil.tatrc.physiology.utilities.FileUtils;
import mil.tatrc.physiology.utilities.Log;
import mil.tatrc.physiology.utilities.LogListener;
import mil.tatrc.physiology.utilities.UnitConverter;
import mil.tatrc.physiology.utilities.csv.CSVComparison;
import mil.tatrc.physiology.utilities.csv.plots.CSVComparePlotter;
import mil.tatrc.physiology.utilities.csv.plots.CSVComparePlotter.PlotType;
import mil.tatrc.physiology.utilities.csv.plots.PlotDriver;

public class TestDriver
{
	static
	{  	  	
		UnitConverter.initialize(System.getProperty("user.dir"));
	}
	
	public interface Executor
	{
		boolean ExecuteTest(TestJob job);
	}

	public static void main(String[] args)
	{
		TestDriver me = new TestDriver();
		if(args.length == 0)
		{
			Log.error("No configFile specified");
			return;
		}
		if(!(new File(args[0])).exists())
		{
			Log.error("ConfigFile "+args[0]+" not found");
			return;
		}
		if(args.length == 4)
		{
			me.commitHash = args[1];
			me.environment = args[2];
			me.architecture = args[3];
		}
		me.processConfigFile(args[0]);

		int     availableThreads = Runtime.getRuntime().availableProcessors();
		boolean isPlotting;
		int     activeThreads=0;
		int     engineThreads=1;
		if(me.numThreads <= 0)
			engineThreads = availableThreads + me.numThreads;
		else
			engineThreads = me.numThreads;
		if(engineThreads<=0)
			engineThreads = 1;
		if(engineThreads > availableThreads)
		{
			engineThreads = availableThreads;
			Log.info("You dont have " + me.numThreads + " available on your computer.");
		}

		Log.info("I can run " + availableThreads + " threads, I am going to use " + engineThreads);

		do
		{
			
				activeThreads = 0;
				isPlotting = false;
				for(TestJob job : me.jobs)
				{
					if(job.state == JobState.Executing)
						activeThreads++;
					if(job.state == JobState.Comparing)
					{
						activeThreads++;
						isPlotting = true;
					}
				}
				if(!isPlotting && activeThreads != engineThreads)
				{// Look for a job to compare first		
					for(TestJob job : me.jobs)
					{  					  				
						if(job.state == JobState.Executed)
						{
							if(job.PlottableResults && !job.skipPlot)
							{
								CompareThread cThread = me.new CompareThread();
								job.state = JobState.Comparing;
								cThread.job = job;
								activeThreads++;
								cThread.start();  						
								break;
							}
							else
							{
								job.state = JobState.Complete;
							}						
						}
					}
				}
				if(activeThreads != engineThreads)
				{
					for(TestJob job : me.jobs)
					{
						if(job.state==JobState.Provisioned)
						{
							ExecuteThread eThread = me.new ExecuteThread();
							eThread.job = job;
							job.state = JobState.Executing;
							eThread.start();
							activeThreads++;
						}
						if(activeThreads == engineThreads)
							break;						
					}
				}			
				if(activeThreads==0)
					break;

			try
			{Thread.sleep(100);}
			catch (InterruptedException ex)
			{ Log.error("I have insomnia...",ex); }
		}while(true);


		me.createReport();
	}

	public String environment = "";
	public String architecture = "";
	public String commitHash = "";

	protected String name;

	protected int    numThreads=0;
	protected double percentDifference=2.0;

	boolean sendEmail = true;
	protected  String emailRecipients;  	
	protected String emailSender;
	protected String emailSubject;
	protected String emailSMTP;
	
	protected boolean useStates=false;
	protected SEScenarioAutoSerialization autoSerialization=null;
	protected String patientFiles;

	protected Map<String,String> macros = new HashMap<String,String>();
	protected Map<String,Class<TestDriver.Executor>> executors = new HashMap<String,Class<TestDriver.Executor>>();
	protected List<TestJob> jobs = new ArrayList<TestJob>();	

	protected Map<TestJob,String> job2groups = new HashMap<TestJob,String>();
	protected Map<String,List<String>> groups = new LinkedHashMap<String,List<String>>();

	protected enum JobState {Provisioned, Executing, Executed, Comparing, Complete}
	
	public class TestJob extends LogListener
	{
		public TestJob(){ listen(false);	}
		public boolean      useState=false;
		public SEScenarioAutoSerialization autoSerialization=null;
		public String       patientFile = null;
		public JobState     state=JobState.Provisioned;
		public String       name;
		public boolean      skipExecution = false;
		public boolean      skipPlot = false;
		public boolean      knownFailure = false;
		public TestDriver.Executor executor = null;
		public boolean      PlottableResults = false;
		public PlotType     plotType = PlotType.FastPlot;//Only plot every nth data point
		public double       percentDifference;
		public List<String> baselineFiles = new ArrayList<String>();
		public String       baselineDirectory = null;
		public List<String> computedFiles = new ArrayList<String>();
		public String       computedDirectory = null;
		public List<String> reportFiles = new ArrayList<String>();
		public String       resultsFiles = null;
		
		public TestJob clone()
		{
			TestJob copy = new TestJob();
			copy.useState = this.useState;
			copy.patientFile = this.patientFile;
			copy.state = this.state;
			copy.name = this.name;
			copy.skipExecution = this.skipExecution;
			copy.skipPlot = this.skipPlot;
			copy.knownFailure = this.knownFailure;
			copy.executor = this.executor;
			copy.PlottableResults = this.PlottableResults;
			copy.plotType = this.plotType;
			copy.percentDifference = this.percentDifference;
			copy.baselineFiles.addAll(this.baselineFiles);
			copy.baselineDirectory = this.baselineDirectory;
			copy.computedFiles.addAll(this.computedFiles);
			copy.computedDirectory = this.computedDirectory;
			copy.reportFiles.addAll(this.reportFiles);
			copy.resultsFiles = this.resultsFiles;
			return copy;
		}

		public void handleDebug(String msg) { Log.debug("["+name+"] "+msg,""); }
		public void handleInfo(String msg)  { Log.info("["+name+"] "+msg,""); }
		public void handleWarn(String msg)  { Log.warn("["+name+"] "+msg,""); }
		public void handleError(String msg) { Log.error("["+name+"] "+msg,""); }
		public void handleFatal(String msg) { Log.fatal("["+name+"] "+msg,""); }
	}

	public void processConfigFile(String configFile)
	{  	  	
		this.name = configFile.substring(0, configFile.lastIndexOf('.'));
		Log.setFileName(this.name+".log");

		String key;
		String value;

		this.percentDifference = 2.0;

		this.emailRecipients = null;
		this.emailSender = null;
		this.emailSubject = null;
		this.emailSMTP = null;
		this.sendEmail = true;

		this.executors.clear();
		this.jobs.clear();

		boolean ExecuteJobs=true;
		boolean PlotResults=true;
		String currentGroup = this.name;

		// Parse the config file
		// TODO, make sure all \ are converted to /
		try
		{
			BufferedReader br = new BufferedReader(new FileReader(configFile));
			String line;
			while ((line = br.readLine()) != null)
			{
				if (line.length() == 0 || line.startsWith("#"))
					continue;
				if(line.startsWith("@group"))
				{
					currentGroup = line.substring(6).trim();
					if(currentGroup.isEmpty())
						currentGroup = this.name;
					continue;
				}
				if(line.indexOf('=') == -1)
					continue;

				line = line.trim();
				key = line.substring(0, line.indexOf('='));
				value = line.substring(line.indexOf("=") + 1);

				if(key.equalsIgnoreCase("Subject"))
				{ this.emailSubject = value; continue; }
				if(key.equalsIgnoreCase("Sender"))
				{ this.emailSender = value; continue; }
				if(key.equalsIgnoreCase("SMTP"))
				{ this.emailSMTP = value; continue; }
				if(key.equalsIgnoreCase("Recipients"))
				{ this.emailRecipients = value; continue; }
				if(key.equalsIgnoreCase("PercentDifference"))
				{ this.percentDifference = Double.parseDouble(value); continue; }
				if(key.equalsIgnoreCase("Threads"))
				{ this.numThreads = Integer.parseInt(value); continue; }
				if(key.equalsIgnoreCase("Patients"))
				{ this.patientFiles = value; continue; }
				if(key.equalsIgnoreCase("UseStates"))
				{
					if(value.equalsIgnoreCase("true"))
						this.useStates=true; 
					continue; 
				}
				if(key.equalsIgnoreCase("AutoSerialization"))
				{
					this.autoSerialization=new SEScenarioAutoSerialization();
					// Parse the value
					String[] values = value.split(",");
					this.autoSerialization.setDirectory(values[0]);
					this.autoSerialization.getPeriod().setValue(Double.parseDouble(values[1]),TimeUnit.s);
					this.autoSerialization.setPeriodTimeStamps(EnumOnOff.fromValue(values[2]));		
					this.autoSerialization.setAfterActions(EnumOnOff.fromValue(values[3]));		
					this.autoSerialization.setReloadState(EnumOnOff.fromValue(values[4]));
					continue; 
				}
				if(key.equalsIgnoreCase("ExecuteTests"))
				{
					if(value.equalsIgnoreCase("false"))
						ExecuteJobs=false; 
					continue; 
				}
				if(key.equalsIgnoreCase("PlotResults"))
				{
					if(value.equalsIgnoreCase("false"))
						PlotResults=false; 
					continue; 
				}
				if(key.equalsIgnoreCase("SendEmail"))
				{ 
					if(value.equalsIgnoreCase("false"))
						this.sendEmail = false; 
					continue; 
				}
				if(key.equalsIgnoreCase("Executor"))
				{
					Class<TestDriver.Executor> clazz = null;
					try
					{
						clazz = (Class<TestDriver.Executor>)Class.forName(value); 						
						this.executors.put(clazz.getSimpleName(), clazz);
					} 
					catch(Exception e){Log.error("Could not find Executor "+value);}
					continue;
				}        

				if(key.startsWith("Macro"))
				{
					macros.put(key.substring(6), value);
					continue;
				}

				for(String macro : macros.keySet())
				{
					if(value.indexOf(macro)>-1)
						value = value.replaceAll(macro, macros.get(macro));
				}

				TestJob job = new TestJob();
				job.useState = this.useStates;
				job.autoSerialization = this.autoSerialization;
				if(!ExecuteJobs)
					job.skipExecution = true;
				if(!PlotResults)
					job.skipPlot = true;
				job2groups.put(job, currentGroup);
				if (key.charAt(0) == '*')
				{
					job.knownFailure = true;
					job.state = JobState.Complete;
					key = key.substring(1);
				}
				if (key.charAt(0) == '-')
				{
					job.skipExecution = true;
					job.skipPlot = true;
					key = key.substring(1);
				}
				this.jobs.add(job);
				job.name = key.trim();        	
				job.percentDifference = this.percentDifference;

				String[] directives = value.trim().split(" ");
				for(String directive : directives)
				{
					if(directive.indexOf("=")==-1)
					{
						if(executors.containsKey(directive))
						{
							try{job.executor = executors.get(directive).newInstance();}
							catch (Exception e)
							{
								job.knownFailure = true;
								Log.error("Could not make a new "+executors.get(directive).getName()+" executor");								
							}
							continue;
						}
						if(directive.equalsIgnoreCase("NoCompare")) 
						{ job.PlottableResults = false; continue; }
						if(directive.equalsIgnoreCase("FastPlot")) 
						{ job.PlottableResults = true; job.plotType=PlotType.FastPlot; continue; }
						if(directive.equalsIgnoreCase("FullPlot"))
						{ job.PlottableResults = true; job.plotType=PlotType.FullPlot; continue; }
						if(directive.equalsIgnoreCase("FullPlotErrors"))
						{ job.PlottableResults = true; job.plotType=PlotType.FullPlotErrors; continue; }
						if(directive.equalsIgnoreCase("FastPlotErrors"))
						{ job.PlottableResults = true; job.plotType=PlotType.FastPlotErrors; continue; }
						if(directive.equalsIgnoreCase("MemoryFastPlot"))
						{ job.PlottableResults = true; job.plotType=PlotType.MemoryFastPlot; continue; }
					}
					else
					{
						key = directive.substring(0, directive.indexOf('='));
						value = directive.substring(directive.indexOf("=") + 1);
						if(key.equalsIgnoreCase("Baseline"))
						{job.baselineDirectory = value; continue;}              
						else if(key.equalsIgnoreCase("Computed"))
						{job.computedDirectory = value; continue;}
						if(key.equalsIgnoreCase("Results"))
						{job.resultsFiles = value; continue;}   
					}
				}
				if(job.resultsFiles!=null && !job.resultsFiles.isEmpty())
				{
					for(String result : job.resultsFiles.split(","))
					{
						job.baselineFiles.add(job.baselineDirectory+"/"+result+".zip");
						job.computedFiles.add(job.computedDirectory+"/"+result+".txt");
					}
				}
				else
				{
					if(job.executor.getClass().getName().indexOf("Scenario")!=-1)//Is a Scenario File test
					{
						DeriveScenarioResultNames(job, job.name);						
					}
					else
					{
						job.baselineFiles.add(job.baselineDirectory+"/"+job.name+".zip");
						job.computedFiles.add(job.computedDirectory+"/"+job.name+".txt");
					}
				}
			}
			
			if(this.patientFiles!=null)
			{
  			// Need to copy all the jobs and speficy a particular patient file
				List<String> patientFileNames;
				
				if(patientFiles.equalsIgnoreCase("all"))
					patientFileNames = FileUtils.findFiles("./patients", ".xml", true);				
				else
				{
					String[] patientFiles = this.patientFiles.split(",");
					patientFileNames = new ArrayList<String>();
					for(String patientFile : patientFiles)
					{
						patientFile = patientFile.trim();
						if(!patientFile.endsWith(".xml"))
							patientFile += ".xml";
						patientFileNames.add(patientFile);
					}
				}
				
				List<TestJob> oldJobs = new ArrayList<TestJob>(jobs);
				jobs.clear();
				TestJob copy;
				
				for(String pFileName : patientFileNames)
				{
					String[] split = pFileName.split("[/\\\\]");
					pFileName = split[split.length-1];

					for(TestJob job : oldJobs)
					{
						if(job.executor.getClass().getName().indexOf("Scenario")==-1)
						{
							jobs.add(job);
							continue;
						}
						copy = job.clone();															
						copy.patientFile = pFileName;
						DeriveScenarioResultNames(copy, copy.name.replaceAll(".xml", "-"+pFileName));	
						jobs.add(copy);
					}
				}

			}
			
			// Let's clean out everything we are about to run
			for(TestJob job : jobs)
			{
				DeleteTestResults(job.computedDirectory+"/"+job.name,ExecuteJobs,PlotResults);
				for(String resultFile : job.computedFiles)
				{
					DeleteTestResults(resultFile,ExecuteJobs,PlotResults);
				}
			}
			br.close();
		}
		catch (IOException e)
		{
			Log.error("Ouch",e);
		}
	}
	
	protected void DeriveScenarioResultNames(TestJob job, String baseName)
	{
		job.baselineFiles.clear();
		job.computedFiles.clear();
		
		String[] dirs = baseName.substring(0, baseName.indexOf(".xml")).split("[/\\\\]");
		String baseline = job.baselineDirectory;
		for(int i=0; i<dirs.length-1; i++)
			baseline+="/"+dirs[i];
		baseline+="/Current Baseline/"+dirs[dirs.length-1]+"Results.zip";
		job.baselineFiles.add(baseline);
		//example : ..\verification\Scenarios\Validation\Current Baseline\Patient-ValidationResults.zip
		String output = job.computedDirectory;
		for(int i=0; i<dirs.length; i++)
			output+="/"+dirs[i];
		output+="Results.txt";
		//example : ./Scenarios/Validation/Patient-ValidationResults.txt
		job.computedFiles.add(output);
	}

	public class ExecuteThread extends Thread
	{
		public TestJob job;
		public void run()
		{  		  	
			for(String toCompute : job.computedFiles)
			{
				if(job.PlottableResults)
				{
					if(job.name.endsWith(".xml"))//This should be a scenario file, different naming convention
					{
						String[] dirs = toCompute.substring(0,toCompute.indexOf(".txt")).split("[/\\\\]");
						String report = toCompute.substring(0,toCompute.indexOf(".txt"))+"/"+dirs[dirs.length-1]+"Report.xml";
						job.reportFiles.add(report);
					}
					else
					{
						String path = toCompute.substring(0, toCompute.lastIndexOf("."));
						path = path + path.substring(path.lastIndexOf("/")) + "Report.xml";
						job.reportFiles.add(path);
					}
				}
				else
					job.reportFiles.add(toCompute.substring(0, toCompute.lastIndexOf("."))+"Report.xml");
			}

			if(!job.skipExecution)
			{
				if(job.executor!=null)
				{
					Log.info("Executing Job : "+job.name);
					try
					{
						if(!job.executor.ExecuteTest(job))
							job.skipPlot = true;
					}
					catch(Exception ex)
					{
						Log.error("Could not exeucute job : "+job.name, ex);
						job.skipPlot = true;
					}
					catch(Throwable t)
					{
						Log.error("Could not exeucute job : "+job.name, t);
						job.skipPlot = true;
					}
				}
				else  		
					Log.error("No executor for job "+job.name);
			}
			System.gc();
			job.state = JobState.Executed;
		}
	}

	public class CompareThread extends Thread
	{
		public TestJob job;
		public void run()
		{
			if(job.PlottableResults && !job.skipPlot)
			{  				
				for(int i=0; i<job.baselineFiles.size(); i++)
				{
					CSVComparison compare = new CSVComparison();
					compare.limit = job.percentDifference;
					compare.reportDifferences = false;					
					compare.setFullReportPath(job.reportFiles.get(i)); 
					if(new File(job.computedFiles.get(i)).exists())
					{
						Set<String> failures = null;
						if(new File(job.baselineFiles.get(i)).exists())
						{
							failures = compare.compare(job.baselineFiles.get(i), job.computedFiles.get(i));
							if(failures==null)// Something bad happened in running this test...
							{
								compare.createErrorSuite("Could not compare these files for some reason: "+job.baselineFiles.get(i)+" and " + job.computedFiles.get(i));
								compare.write();
								continue;
							}
							compare.write();
						}
						if((job.plotType == PlotType.FastPlotErrors || job.plotType == PlotType.FullPlotErrors) && failures.isEmpty())
						{
							Log.info("No plots for "+job.computedFiles.get(i));
						}
						else
						{
							CSVComparePlotter plotter = new CSVComparePlotter();
							plotter.plotType = job.plotType;
							plotter.plot(job.baselineFiles.get(i), job.computedFiles.get(i), failures);
							/*
							PlotDriver driver = new PlotDriver();
							if(job.plotType == PlotType.FastPlot)
							{
								driver.preload = true;
								driver.onlyPlotFailures = false;
								driver.abbreviateContents = 5;
							}
							else if(job.plotType == PlotType.FullPlot)
							{
								driver.preload = true;
								driver.onlyPlotFailures = false;
								driver.abbreviateContents = 0;
							}
							else if(job.plotType == PlotType.FastPlotErrors)
							{
								driver.preload = true;
								driver.onlyPlotFailures = true;
								driver.abbreviateContents = 5;
							}
							else if(job.plotType == PlotType.FullPlotErrors)
							{
								driver.preload = true;
								driver.onlyPlotFailures = true;
								driver.abbreviateContents = 0;
							}
							else if(job.plotType == PlotType.MemoryFastPlot)
							{
								driver.preload = false;
								driver.onlyPlotFailures = false;
								driver.abbreviateContents = 0;
							}
							if(driver.preload)
							{
								if(!driver.preloadData(job.baselineFiles.get(i), job.computedFiles.get(i)))
									continue;
							}
							if(job.executor.getClass().getName().indexOf("Scenario")!=-1)
								driver.isScenario = true;
							
							driver.generateCompareJobs(job.baselineFiles.get(i), job.computedFiles.get(i), failures);
							driver.execute();
							*/
						}
					}
					else
					{
						Log.error("Couldn't read file "+job.computedFiles.get(i));
					}
				}
			}
			System.gc();
			job.state = JobState.Complete;
		}
	}

	public void createReport()
	{
		List<String> currentGroup;
		TestReport report = new TestReport();  	
		report.setFullReportPath(".\\"+this.name+"Report.xml");  	
		for(TestJob job : this.jobs)
		{
			String group = job2groups.get(job);
			if(!groups.containsKey(group))
				groups.put(group,new ArrayList<String>());
			currentGroup = groups.get(group);

			if (job.knownFailure)
			{
				report.addKnownSuiteFailure(job.name);
				continue;
			}  	
			if(job.reportFiles.isEmpty())
			{
				report.createErrorSuite("No reports found for "+job.name+" to summarize");
				continue;
			}
			for(String reportFile : job.reportFiles)
			{
				if(!FileUtils.fileExists(reportFile))
				{
					report.createErrorSuite("Unable to find file "+reportFile+" to summarize");
					continue;
				}
				Object o = CDMSerializer.readFile(reportFile);
				if(o instanceof TestReportData)
				{
					TestSuite s;
					TestReportData summarize = (TestReportData)o;        
					for(TestReportData.TestSuite suite : summarize.getTestSuite())
					{
						s=new TestSuite();
						s.load(suite);
						report.addSuite(s);
						currentGroup.add(s.getName());
					}
				}
				else
				{
					report.createErrorSuite(reportFile);
					Log.error("Need file with TestReportData object");
				}
			}
		}  	
		report.write();
		if(this.sendEmail)
			email(report);
	}

	protected void email(TestReport report)
	{
		if(this.emailSubject==null||this.emailRecipients==null||
				this.emailSender==null||this.emailSMTP==null)
		{
			Log.error("Not sending email since all nessesary email info was not provided");
			return;
		}

		//Get the name of the computer
		String hostname = "Unknown";
		try
		{
			InetAddress addr = InetAddress.getLocalHost();
			hostname = addr.getHostName();
		}
		catch(Exception ex)
		{
			System.out.println("Hostname can not be resolved");
		}  
		//Get revision number if found add it to the email and make a copy of the bin
		String commit= "";
		if (commitHash != null && !commitHash.equals("") && !commitHash.equals("00000000"))
		{
			commit= " Commit " + commitHash.substring(0, 8);
		}

		String environmentString = "";
		if (environment != null && !environment.equals(""))
		{
			environmentString = environment + " " + architecture + " - ";
		}

		// Email out a summary of all the reports we made
		EmailUtil email = new EmailUtil();
		email.setSubject(environmentString + this.emailSubject + " from "+hostname+commit);
		email.setSender(this.emailSender);
		email.setSMTP(this.emailSMTP);
		for(String recipient : this.emailRecipients.split(","))
			email.addRecipient(recipient.trim());
		email.sendHTML(report.toHTML(groups));
	}

	static void DeleteTestResults(String hint, boolean executeJobs, boolean plotResults)
	{// Try and delete all files related to this test
		// There are lots of various naming conventions, and this should support all of them

		// Get rid of any extensions
		hint = hint.replaceAll(".xml", "");
		hint = hint.replaceAll(".txt", "");

		File file;
		
		if(executeJobs)
		{
			file = new File(hint+".log");
			FileUtils.delete(file);

			file = new File(hint+"Test.log");
			FileUtils.delete(file);

			file = new File(hint+".txt");
			FileUtils.delete(file);

			file = new File(hint+".xml");
			FileUtils.delete(file);

			file = new File(hint+"Test.xml");
			FileUtils.delete(file);

			file = new File(hint+"Report.xml");
			FileUtils.delete(file);

			file = new File(hint+"Results.txt");
			FileUtils.delete(file);
		}
		
		if(plotResults)
		{
			file = new File(hint+"/");
			FileUtils.delete(file);

			file = new File(hint+"Results");
			FileUtils.delete(file);
		}
	}
}


