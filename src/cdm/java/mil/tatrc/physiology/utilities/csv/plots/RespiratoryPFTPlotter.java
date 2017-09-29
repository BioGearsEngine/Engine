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
package mil.tatrc.physiology.utilities.csv.plots;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Font;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.LegendItemCollection;
import org.jfree.chart.LegendItemSource;
import org.jfree.chart.axis.LogarithmicAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleEdge;
import org.jfree.ui.RectangleInsets;

import mil.tatrc.physiology.datamodel.CDMSerializer;
import mil.tatrc.physiology.datamodel.bind.PulmonaryFunctionTestData;
import mil.tatrc.physiology.datamodel.bind.ScenarioData;
import mil.tatrc.physiology.datamodel.patient.assessments.SEPulmonaryFunctionTest;
import mil.tatrc.physiology.datamodel.properties.CommonUnits.TimeUnit;
import mil.tatrc.physiology.datamodel.properties.CommonUnits.VolumeUnit;
import mil.tatrc.physiology.datamodel.properties.SEFunctionVolumeVsTime;
import mil.tatrc.physiology.datamodel.scenario.SEScenario;
import mil.tatrc.physiology.datamodel.scenario.actions.SEAction;
import mil.tatrc.physiology.datamodel.substance.SESubstanceManager;
import mil.tatrc.physiology.utilities.DoubleUtils;
import mil.tatrc.physiology.utilities.FileUtils;
import mil.tatrc.physiology.utilities.Log;
import mil.tatrc.physiology.utilities.LogListener;
import mil.tatrc.physiology.utilities.csv.CSVContents;
import mil.tatrc.physiology.utilities.csv.plots.PlotDriver.PlotJob;

public class RespiratoryPFTPlotter implements Plotter
{
	protected List<LogEvent> events = new ArrayList<LogEvent>();
	protected Map<String,List<Double>> data = new HashMap<String,List<Double>>();
	protected Map<String,List<Double>> PFTData = new HashMap<String,List<Double>>();	//might be able to get rid of this
	protected List<SEAction> actions = new ArrayList<SEAction>();
	protected SEScenario scenario;
	protected SEPulmonaryFunctionTest pft;

	public static void main(String[] args)
	{
		PlotDriver.main(args);
	}
	
	public void plot(LogListener listener, SESubstanceManager subMgr)
	{		
		//fill PlotJob with needed data if it doesn't exist
		PlotJob job = (PlotJob)listener;
		if(job.dataPath == null || job.dataPath.isEmpty())
		{job.dataPath = "../verification/Scenarios/"+job.verificationDirectory+"/Current Baseline/";}
		if(job.logPath == null || job.logPath.isEmpty())
		{job.logPath = "../verification/Scenarios/"+job.verificationDirectory+"/Current Baseline/";}
		if(job.scenarioPath == null || job.scenarioPath.isEmpty())
		{job.scenarioPath = "../verification/Scenarios/"+job.verificationDirectory+"/";}
		if(job.dataFile == null || job.dataFile.isEmpty())
		{job.dataFile = job.name + "Results.zip";}
		if(job.logFile == null || job.logFile.isEmpty())
		{job.logFile = job.name + "Results.log";}
		if(job.scenarioFile == null || job.scenarioFile.isEmpty())
		{job.scenarioFile = job.name + ".xml";}
		
		//get all events from Log file
		if(!job.skipAllEvents)
		{
	    events = getEventsFromLog(job.logPath + job.logFile);
		}
		
		if (!job.skipAllActions)
    {
	    //Get all actions from scenario file
	    Object obj = CDMSerializer.readFile(job.scenarioPath + job.scenarioFile);
	    if (obj instanceof ScenarioData)
	    {
		    this.scenario = new SEScenario(subMgr);
		    this.scenario.load((ScenarioData) obj);
		    actions = scenario.getActions();
	    } 
	    else
	    {
		    Log.error("Could not analyze scenario file " + job.scenarioPath
		        + job.scenarioFile);
	    }
    }
		
		//Look for a PFT action
		for(SEAction action : actions)
		{			
			if(action.toString().indexOf("PulmonaryFunctionTest") != -1)
			{
				double PFTtime_s = action.getScenarioTime().getValue();
				
				//extract data from file
		    Object obj = CDMSerializer.readFile(job.PFTFile);
		    if (obj instanceof PulmonaryFunctionTestData)
		    {
			    this.pft = new SEPulmonaryFunctionTest();
			    this.pft.load((PulmonaryFunctionTestData) obj);
			    
			    SEFunctionVolumeVsTime points = this.pft.getLungVolumePlot();
			    			   
			    List<Double> timeValues = new ArrayList<Double>();
			    for(double d : points.getTime(TimeUnit.s))
			    {
			    	timeValues.add(d+PFTtime_s);	
			    }
			    List<Double> volumeValues = new ArrayList<Double>();
			    for(double d : points.getVolume(VolumeUnit.mL))
			    {
			    	volumeValues.add(d);
			    }
			    
			    PFTData.put("Time", timeValues);
			    PFTData.put("Volume", volumeValues);
		    }
		    else
		    {
		    	Log.error("Couldn't read PFT file.");
		    }
			}
		}
		
		if (data.isEmpty() || data == null)
    {
	    //Get data contents for desired header(s)
	    try
	    {
		    CSVContents csv = new CSVContents(job.dataPath + job.dataFile);
		    csv.abbreviateContents = job.resultsSkipNum;
		    for (int i = 0; i < job.headers.size(); i++)
		    {
			    List<Double> headerData = new ArrayList<Double>();
			    csv.readHeader(csv.unitUnderscoreToSpace(job.headers.get(i)), headerData);
			    data.put(job.headers.get(i), headerData);
		    }		  
		    List<Double> timeHeaderData = new ArrayList<Double>();
		    csv.readHeader("Time(s)", timeHeaderData);
		    data.put("Time(s)", timeHeaderData);
	    } catch (IOException e)
	    {
		    Log.error("Could not analyze file " + job.dataPath + job.dataFile);
	    }
    }
		
		//make the graph
		createGraph(job, PFTData, data, events, actions);
	}
	
	public List<LogEvent> getEventsFromLog(String file)
	{
		List<LogEvent> events = new ArrayList<LogEvent>();
		
		try
		{
			BufferedReader br = new BufferedReader(new FileReader(file));
			String line;
			while ((line = br.readLine()) != null)
			{
				if (line.length() == 0)
					continue;
				if(line.indexOf("[Event]") == -1)
					continue;
				else
				{
					LogEvent event = new LogEvent();
					String eventText = line.substring(line.indexOf("[Event]")+7).trim();
					int endTimeIndex = eventText.indexOf("(s)");
					if(endTimeIndex == -1)
						endTimeIndex = eventText.indexOf(",");
					Double dTime = Double.parseDouble(eventText.substring(0, endTimeIndex).trim());
					event.time.setValue(dTime, "s");
					event.text = eventText.substring(eventText.indexOf(",")+1).trim();
					events.add(event);
				}
			}
			br.close();
		}
		catch (IOException e)
		{
			Log.warn("ActionEventPlotter couldn't read the log file "+file);
		}
		catch (NumberFormatException e)
		{
			Log.error("Couldn't correctly parse log file time to double");
		}
		catch (Exception e)
		{
			Log.error("Something went wrong parsing the log",e);
			return null;
		}
		
		return events;
		
	}

	public void createGraph(PlotJob job, Map<String, List<Double>> PFTData, Map<String, List<Double>> data, List<LogEvent> events, List<SEAction> actions)
	{
		CSVPlotTool plotTool = new CSVPlotTool();	//to leverage existing functions
		String title = job.name + "_";
		XYSeriesCollection dataSet = new XYSeriesCollection();
		double maxY = 0;
		double minY = Double.MAX_VALUE;
		for(int i = 0; i < job.headers.size(); i++)
    {			
			title = title + job.headers.get(i) + "_";
			XYSeries dataSeries;
			dataSeries = plotTool.createXYSeries(job.headers.get(i),data.get("Time(s)"),data.get(job.headers.get(i)));
			dataSet.addSeries(dataSeries);
			maxY = maxY < dataSeries.getMaxY() ? dataSeries.getMaxY() : maxY;
			minY = minY > dataSeries.getMinY() ? dataSeries.getMinY() : minY;
    }
		
		//Now make a data series for PFT data and check its max and min
		XYSeries dataSeries = plotTool.createXYSeries("PFT Total Lung Volume (mL)",PFTData.get("Time"),PFTData.get("Volume"));
		dataSet.addSeries(dataSeries);
		maxY = maxY < dataSeries.getMaxY() ? dataSeries.getMaxY() : maxY;
		minY = minY > dataSeries.getMinY() ? dataSeries.getMinY() : minY;
		
		title = title + "vs_Time";
		
		//Override the constructed title if desired
		if(job.titleOverride != null && !job.titleOverride.isEmpty() && !job.titleOverride.equalsIgnoreCase("None"))
			title = job.titleOverride;
		
		double rangeLength = maxY - minY;
		if(Math.abs(rangeLength) < 1e-6)
		{
			rangeLength = .01;
		}
		
		class AEEntry implements Comparable<AEEntry>
		{
			public String name;
			public List<Double> times = new ArrayList<Double>();
			public List<Double> YVals = new ArrayList<Double>();
			public String type = "";
			
			public int compareTo(AEEntry entry)
			{
				return times.get(0) < entry.times.get(0) ? -1
		         : times.get(0) > entry.times.get(0) ? 1
		         : 0;
			}
		}
		
		List<AEEntry> allActionsAndEvents = new ArrayList<AEEntry>();
		
		if (!job.skipAllEvents)
    {
	    //Make points for each event
	    //Treat each event like two points on the same vertical line
	    for (LogEvent event : events)
	    {
	    	boolean skip = false;
	    	
	    	for(String eventToSkip : job.eventOmissions)
	    	{
	    		if(event.text.contains(eventToSkip))
	    			skip = true;
	    	}
	    	if(skip)
	    		continue;
		    AEEntry entry = new AEEntry();

		    entry.times.add(event.time.getValue());
		    if(job.logAxis)
		    	entry.YVals.add(maxY);
		    else if(job.forceZeroYAxisBound && maxY < 0)
		    	entry.YVals.add(-.01);
		    else
		    	entry.YVals.add(maxY + 0.15 * rangeLength);

		    entry.times.add(event.time.getValue());
		    if(job.logAxis)
		    	entry.YVals.add(minY);
		    else if(job.forceZeroYAxisBound && minY > 0)
		    	entry.YVals.add(-.01);
		    else
		    	entry.YVals.add(minY - 0.15 * rangeLength);

		    entry.name = event.text + "\r\nt=" + event.time.getValue();
		    entry.type = "EVENT:";

		    allActionsAndEvents.add(entry);
	    }
    }
		
		if (!job.skipAllActions)
    {
	    //Make similar entries for actions
	    for (SEAction action : actions)
	    {
	    	boolean skip = false;
	    	
	    	for(String actionToSkip : job.actionOmissions)
	    	{
	    		if(action.toString().contains(actionToSkip))
	    			skip = true;
	    	}
	    	if(skip)
	    		continue;
	    	
		    if (action.toString().contains("Advance Time"))
			    continue;

		    AEEntry entry = new AEEntry();

		    entry.times.add(action.getScenarioTime().getValue());
		    if(job.logAxis)
		    	entry.YVals.add(maxY);
		    else if(job.forceZeroYAxisBound && maxY < 0)
		    	entry.YVals.add(-.01);
		    else
		    	entry.YVals.add(maxY + 0.15 * rangeLength);

		    entry.times.add(action.getScenarioTime().getValue());
		    if(job.logAxis)
		    	entry.YVals.add(minY);
		    else if(job.forceZeroYAxisBound && minY > 0)
		    	entry.YVals.add(-.01);
		    else
		    	entry.YVals.add(minY - 0.15 * rangeLength);

		    entry.name = action.toString() + "\r\nt="
		        + action.getScenarioTime().getValue();
		    entry.type = "ACTION:";

		    allActionsAndEvents.add(entry);
	    }
    }
		
		//Sort the list
		Collections.sort(allActionsAndEvents);
		
		//Add a series for each entry
		for(AEEntry entry : allActionsAndEvents)
		{
			dataSet.addSeries(plotTool.createXYSeries(entry.type + entry.name,entry.times,entry.YVals));
		}			
		
    //set labels
    String XAxisLabel = "Time(s)";
    String YAxisLabel = job.headers.get(0);

    JFreeChart chart = ChartFactory.createXYLineChart(
    		job.titleOverride!=null&&job.titleOverride.equalsIgnoreCase("None")?"":title,					 // chart title
        XAxisLabel,				 // x axis label
        YAxisLabel,				 // y axis label
        dataSet,                   // data
        PlotOrientation.VERTICAL,  // orientation
        true,                      // include legend
        true,                      // tooltips
        false                      // urls
        );
    
    Log.info("Creating Graph "+title);   
    XYPlot plot = (XYPlot) chart.getPlot();
    
    if (!job.logAxis)
    {
	    // Determine Y range
	    double resMax0 = maxY;
	    double resMin0 = minY;
	    if (Double.isNaN(resMax0) || Double.isNaN(resMin0))
		    plot.getDomainAxis().setLabel("Range is NaN");
	    if (DoubleUtils.isZero(resMin0))
		    resMin0 = -0.000001;
	    if (DoubleUtils.isZero(resMax0))
		    resMax0 = 0.000001;
	    if(job.forceZeroYAxisBound && resMin0>=0) resMin0 = -.000001;
	    if(job.forceZeroYAxisBound && resMax0<=0) resMax0 = .000001;
	    rangeLength = resMax0 - resMin0;
	    ValueAxis yAxis = plot.getRangeAxis();
	    if(rangeLength != 0)
	    	yAxis.setRange(resMin0 - 0.15 * rangeLength, resMax0 + 0.15 * rangeLength);//15% buffer so we can see top and bottom clearly        	
	    
	    //Add another Y axis to the right side for easier reading
	    ValueAxis rightYAxis = new NumberAxis();
	    rightYAxis.setRange(resMin0 - 0.15 * rangeLength, resMax0 + 0.15* rangeLength);
	    rightYAxis.setLabel("");
	    
	    //Override the bounds if desired
	    try
      {
	      if(job.Y1LowerBound != null)
	      {
	      	yAxis.setLowerBound(job.Y1LowerBound);
	      	rightYAxis.setLowerBound(job.Y1LowerBound);
	      }
	      if(job.Y1UpperBound != null)
	      {
	      	yAxis.setUpperBound(job.Y1UpperBound);
	      	rightYAxis.setUpperBound(job.Y1UpperBound);
	      }
      } catch (Exception e)
      {
      	Log.error("Couldn't set Y bounds. You probably tried to set a bound on an axis that doesn't exist.");
      }
	    plot.setRangeAxis(0,yAxis);
	    plot.setRangeAxis(1, rightYAxis);
	    
    }
    else
    {
    	double resMin = minY;
    	double resMax = maxY;
    	if(resMin <= 0.0)
    		resMin = .00001;
    	LogarithmicAxis yAxis = new LogarithmicAxis("Log("+YAxisLabel+")");
    	LogarithmicAxis rightYAxis = new LogarithmicAxis("");
    	yAxis.setLowerBound(resMin);
    	rightYAxis.setLowerBound(resMin);
    	yAxis.setUpperBound(resMax);
    	rightYAxis.setUpperBound(resMax);
    	
    	//Override the bounds if desired
	    try
      {
	      if(job.Y1LowerBound != null)
	      {
	      	yAxis.setLowerBound(job.Y1LowerBound);
	      	rightYAxis.setLowerBound(job.Y1LowerBound);
	      }
	      if(job.Y1UpperBound != null)
	      {
	      	yAxis.setUpperBound(job.Y1UpperBound);
	      	rightYAxis.setUpperBound(job.Y1UpperBound);
	      }
      } catch (Exception e)
      {
      	Log.error("Couldn't set Y bounds. You probably tried to set a bound on an axis that doesn't exist.");
      }
	    plot.setRangeAxis(0,yAxis);
	    plot.setRangeAxis(1, rightYAxis);
    }
    
    //Override X bounds if desired
    try
    {
      if(job.X1LowerBound != null)
      	plot.getDomainAxis(0).setLowerBound(job.X1LowerBound);
      if(job.X1UpperBound != null)
      	plot.getDomainAxis(0).setUpperBound(job.X1UpperBound);
    }
    catch(Exception e)
    {
    	Log.error("Couldn't set X bounds. You probably tried to set a bound on an axis that doesn't exist.");
    }
    
    //Override labels if desired
    if(job.X1Label != null && !plot.getDomainAxis(0).getLabel().contains("NaN"))
    	plot.getDomainAxis(0).setLabel(job.X1Label.equalsIgnoreCase("None")?"":job.X1Label);
    if(job.Y1Label != null)
    	plot.getRangeAxis(0).setLabel(job.Y1Label.equalsIgnoreCase("None")?"":job.Y1Label);

		formatRPFTPlot(job, chart);
    plot.setDomainGridlinesVisible(job.showGridLines);
    plot.setRangeGridlinesVisible(job.showGridLines);
    
    //Changing line widths and colors
    XYItemRenderer r = plot.getRenderer(); 
    BasicStroke wideLine = new BasicStroke( 2.0f ); 
    Color[] AEcolors = {Color.red, Color.green, Color.black, Color.magenta, Color.orange};
    Color[] dataColors = {Color.blue, Color.cyan, Color.gray, Color.black, Color.red};
    for(int i = 0, cIndex = 0; i < dataSet.getSeriesCount(); i++, cIndex++)
    {
      r.setSeriesStroke(i, wideLine); 
      XYLineAndShapeRenderer renderer = (XYLineAndShapeRenderer) plot.getRenderer();
      renderer.setBaseShapesVisible(false);
      if(cIndex > 4) cIndex = 0;
      if(i<job.headers.size())	//Our actual data
      {
      	renderer.setSeriesFillPaint(i, dataColors[cIndex]); 
      	renderer.setSeriesPaint(i, dataColors[cIndex]);
      }
      else	//actions and events in procession of other colors
      {
    		renderer.setSeriesFillPaint(i, AEcolors[cIndex]); 
      	renderer.setSeriesPaint(i, AEcolors[cIndex]); 
      }
    }
    
    //Split the auto-generated legend into two legends, one for data and one for actions and events
    LegendItemCollection originalLegendCollection = plot.getLegendItems();
    final LegendItemCollection dataLegendCollection = new LegendItemCollection();
    int i;
    for(i = 0; i < job.headers.size() && i < originalLegendCollection.getItemCount(); i++)
    {
    	if(originalLegendCollection.get(i).getLabel().startsWith("ACTION") || originalLegendCollection.get(i).getLabel().startsWith("EVENT"))
    		break;
    	dataLegendCollection.add(originalLegendCollection.get(i));
    }
    final LegendItemCollection remainingLegendCollection = new LegendItemCollection();
    for(; i < originalLegendCollection.getItemCount(); i++)
    {
    	remainingLegendCollection.add(originalLegendCollection.get(i));
    }
    chart.removeLegend();
    LegendItemSource source = new LegendItemSource() 
    {
      LegendItemCollection lic = new LegendItemCollection();
      {
          lic.addAll(dataLegendCollection);
      }

      public LegendItemCollection getLegendItems() 
      {
          return lic;
      }
    };
    LegendTitle dataLegend = new LegendTitle(source);
    dataLegend.setMargin(new RectangleInsets(1.0, 1.0, 1.0, 1.0));
    dataLegend.setBorder(2, 2, 2, 2);
    dataLegend.setBackgroundPaint(Color.white);
    dataLegend.setPosition(RectangleEdge.TOP);
    dataLegend.setItemFont(new Font("SansSerif", Font.PLAIN, 22));
    chart.addLegend(dataLegend);
    
    source = new LegendItemSource() 
    {
      LegendItemCollection lic = new LegendItemCollection();
      {
          lic.addAll(remainingLegendCollection);
      }

      public LegendItemCollection getLegendItems() 
      {
          return lic;
      }
    };
    LegendTitle actionEventsLegend = new LegendTitle(source);
    actionEventsLegend.setMargin(new RectangleInsets(1.0, 1.0, 1.0, 1.0));
    actionEventsLegend.setBorder(2, 2, 2, 2);
    actionEventsLegend.setBackgroundPaint(Color.white);
    actionEventsLegend.setPosition(RectangleEdge.BOTTOM);
    actionEventsLegend.setItemFont(new Font("SansSerif", Font.PLAIN, 22));
    if(!job.hideAELegend && !job.removeAllLegends)
    	chart.addLegend(actionEventsLegend);
    
    if(job.removeAllLegends)
    	chart.removeLegend();
    
    int verticalPixels = 800 + 170*(allActionsAndEvents.size()/5);

    try
    {
      FileUtils.createDirectory(job.outputDir);
      String filename = job.outputFilename==null ? job.outputDir+"/"+plotTool.MakeFileName(title)+".jpg" : job.outputDir+"/"+job.outputFilename;
      if(!filename.endsWith(".jpg"))
      	filename = filename + ".jpg";
      File JPGFile = new File(filename);
      if(job.imageHeight != null && job.imageWidth != null)
      	ChartUtilities.saveChartAsJPEG(JPGFile, chart, job.imageWidth, job.imageHeight);
      else if(!job.hideAELegend && !job.removeAllLegends)
      	ChartUtilities.saveChartAsJPEG(JPGFile, chart, 1600, verticalPixels);
      else
      	ChartUtilities.saveChartAsJPEG(JPGFile, chart, 1600, 800);
    }
    catch (IOException e)
    {
      Log.error(e.getMessage());
    }
	}
	
	public void formatRPFTPlot(PlotJob job, JFreeChart chart)
  {
    XYPlot plot = (XYPlot) chart.getPlot();
    
    //For Scientific notation
    NumberFormat formatter = new DecimalFormat("0.######E0");
    
    for(int i = 0; i < plot.getDomainAxisCount(); i++)
    {
    	plot.getDomainAxis(i).setLabelFont(new Font("SansSerif", Font.PLAIN, job.fontSize));
    	plot.getDomainAxis(i).setTickLabelFont(new Font("SansSerif", Font.PLAIN, 15));
    	plot.getDomainAxis(i).setLabelPaint(job.bgColor==Color.red?Color.white:Color.black);
    	plot.getDomainAxis(i).setTickLabelPaint(job.bgColor==Color.red?Color.white:Color.black);
    }
    for(int i = 0; i < plot.getRangeAxisCount(); i++)
    {
    	plot.getRangeAxis(i).setLabelFont(new Font("SansSerif", Font.PLAIN, job.fontSize));
    	plot.getRangeAxis(i).setTickLabelFont(new Font("SansSerif", Font.PLAIN, 15));
    	plot.getRangeAxis(i).setLabelPaint(job.bgColor==Color.red?Color.white:Color.black);
    	plot.getRangeAxis(i).setTickLabelPaint(job.bgColor==Color.red?Color.white:Color.black);
    	NumberAxis rangeAxis = (NumberAxis)plot.getRangeAxis(i);
      rangeAxis.setNumberFormatOverride(formatter);
    }

    //White background outside of plottable area
    chart.setBackgroundPaint(job.bgColor);

    plot.setBackgroundPaint(Color.white);
    plot.setDomainGridlinePaint(Color.black);
    plot.setRangeGridlinePaint(Color.black);

    plot.setDomainCrosshairVisible(true);
    plot.setRangeCrosshairVisible(true);

    chart.getLegend().setItemFont(new Font("SansSerif", Font.PLAIN, 15));
    chart.getTitle().setFont(new Font("SansSerif", Font.PLAIN, job.fontSize));
    chart.getTitle().setPaint(job.bgColor==Color.red?Color.white:Color.black);
  }
}
