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
import java.awt.Graphics2D;
import java.awt.Paint;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
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
import org.jfree.chart.plot.Plot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.PlotRenderingInfo;
import org.jfree.chart.plot.PlotState;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.chart.title.LegendTitle;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import org.jfree.ui.RectangleEdge;
import org.jfree.ui.RectangleInsets;
import org.jfree.util.ShapeUtilities;

import mil.tatrc.physiology.datamodel.CDMSerializer;
import mil.tatrc.physiology.datamodel.bind.ScenarioData;
import mil.tatrc.physiology.datamodel.scenario.SEScenario;
import mil.tatrc.physiology.datamodel.scenario.actions.SEAction;
import mil.tatrc.physiology.datamodel.substance.SESubstanceManager;
import mil.tatrc.physiology.utilities.DoubleUtils;
import mil.tatrc.physiology.utilities.FileUtils;
import mil.tatrc.physiology.utilities.Log;
import mil.tatrc.physiology.utilities.LogListener;
import mil.tatrc.physiology.utilities.csv.CSVContents;
import mil.tatrc.physiology.utilities.csv.plots.PlotDriver.PlotJob;

public class ActionEventPlotter implements Plotter
{
	protected List<LogEvent> events = new ArrayList<LogEvent>();
	protected List<List<Double>> data = new ArrayList<List<Double>>();
	protected List<List<Double>> timeData = new ArrayList<List<Double>>();
	protected List<SEAction> actions = new ArrayList<SEAction>();
	protected SEScenario scenario;

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
			    data.add((ArrayList<Double>) headerData);
			    List<Double> timeHeaderData = new ArrayList<Double>();
			    csv.readHeader("Time(s)", timeHeaderData);
			    timeData.add((ArrayList<Double>) timeHeaderData);
		    }
		    if(job.computedDataFile != null)
		    {
		    	csv = new CSVContents(job.computedDataPath + job.computedDataFile);
		    	csv.abbreviateContents = job.resultsSkipNum;
			    for (int i = 0; i < job.headers.size(); i++)
			    {
				    List<Double> headerData = new ArrayList<Double>();
				    csv.readHeader(csv.unitUnderscoreToSpace(job.headers.get(i)), headerData);
				    data.add((ArrayList<Double>) headerData);
				    List<Double> timeHeaderData = new ArrayList<Double>();
				    csv.readHeader("Time(s)", timeHeaderData);
				    timeData.add((ArrayList<Double>) timeHeaderData);
			    }
		    }
		    
	    } catch (IOException e)
	    {
		    Log.error("Could not analyze file " + job.dataPath + job.dataFile);
	    }
    }
		
		//make the graph
		createGraph(job, timeData, data, events, actions);
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

	public void createGraph(PlotJob job, List<List<Double>>timeData, List<List<Double>> data, List<LogEvent> events, List<SEAction> actions)
	{
		CSVPlotTool plotTool = new CSVPlotTool();	//to leverage existing functions
		String title = job.name + "_";
		XYSeriesCollection dataSet = new XYSeriesCollection();
		double maxY = 0;
		double minY = Double.MAX_VALUE;
		for(int i = 0; i < timeData.size(); i++)
    {
			if(timeData.get(i)==null || data.get(i)==null)
			{
				job.bgColor = Color.white;	//This hits when we have Expected data but NOT computed data
				continue;
			}
			
			title = title + job.headers.get(i) + "_";
			XYSeries dataSeries;
			if(job.isComparePlot)
			{
				if(timeData.size()>1)
					dataSeries = plotTool.createXYSeries(i==0?"Expected":"Computed",timeData.get(i),data.get(i));
				else	//If we're comparing but only have one data list, expected is missing, so rename to computed
				{
					dataSeries = plotTool.createXYSeries("Computed",timeData.get(i),data.get(i));
				}
			}
			else
				dataSeries = plotTool.createXYSeries(job.headers.get(i),timeData.get(i),data.get(i));
			dataSet.addSeries(dataSeries);
			maxY = maxY < dataSeries.getMaxY() ? dataSeries.getMaxY() : maxY;
			minY = minY > dataSeries.getMinY() ? dataSeries.getMinY() : minY;
    }
		title = title + "vs_Time_Action_Event_Plot";
		
		//Override the constructed title if desired (usually for compare plots)
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
		
		//If we have experimental data, try to load it and create a dataset for it
		XYSeriesCollection expDataSet = new XYSeriesCollection();
		if(job.experimentalData != null && !job.experimentalData.isEmpty())
		{
			Map<String,List<Double>> expData = new HashMap<String,List<Double>>();
			List<String> expHeaders = new ArrayList<String>();
			
			try
			{
  			CSVContents csv = new CSVContents(job.experimentalData);
  	    csv.abbreviateContents = 0;
  	    csv.readAll(expData);
  	    expHeaders = csv.getHeaders();
			}
			catch(Exception e)
			{
				Log.error("Unable to read experimental data");
			}
			
			if(!expData.isEmpty() && !expHeaders.isEmpty())
			{
				List<Double> expTimeData = new ArrayList<Double>();
				expTimeData = expData.get("Time(s)");
				
				for(String h : expHeaders)	//Will assume all headers from exp file will be on same Y axis vs time
				{
					if(h.equalsIgnoreCase("Time(s)"))
							continue;
					
					expDataSet.addSeries(plotTool.createXYSeries("Experimental "+h,expTimeData,expData.get(h)));
				}
			}
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
	    rightYAxis.setRange(yAxis.getRange());
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
    
    //If we have experimental data, set up the renderer for it and add to plot
    if(expDataSet.getSeriesCount() != 0)
    {
    	XYItemRenderer renderer1 = new XYLineAndShapeRenderer(false, true);   // Shapes only
    	renderer1.setSeriesShape(0,ShapeUtilities.createDiamond(8));
    	plot.setDataset(1,expDataSet);
    	plot.setRenderer(1,renderer1);
    	plot.mapDatasetToDomainAxis(1, 0);
    	plot.mapDatasetToRangeAxis(1, 0);
    }

		formatAEPlot(job, chart);
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
    //Special color and format changes for compare plots
    if(job.isComparePlot)
    {
    	XYLineAndShapeRenderer renderer = (XYLineAndShapeRenderer) plot.getRenderer();
    	
    	for(int i=0; i < dataSet.getSeriesCount(); i++)
    	{
    		if(dataSet.getSeries(i).getKey().toString().equalsIgnoreCase("Expected"))
    		{
    			renderer.setSeriesStroke(//makes a dashed line
              i, //argument below float[]{I,K} -> alternates between solid and opaque (solid for I, opaque for K)
              new BasicStroke(2.0f, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND, 1.0f, new float[]{15.0f, 30.0f}, 0.0f)
              );
          renderer.setDrawSeriesLineAsPath(true);
          renderer.setUseFillPaint(true);  
          renderer.setBaseShapesVisible(false);
          renderer.setSeriesFillPaint(i, Color.black); 
          renderer.setSeriesPaint(i, Color.black); 
    		}
    		if(dataSet.getSeries(i).getKey().toString().equalsIgnoreCase("Computed"))
    		{
          renderer.setSeriesFillPaint(i, Color.red); 
          renderer.setSeriesPaint(i, Color.red); 
    		}
    		if(dataSet.getSeries(i).getKey().toString().startsWith("ACTION"))
    		{
          renderer.setSeriesFillPaint(i, Color.green); 
          renderer.setSeriesPaint(i, Color.green); 
    		}
    		if(dataSet.getSeries(i).getKey().toString().startsWith("EVENT"))
    		{
          renderer.setSeriesFillPaint(i, Color.blue); 
          renderer.setSeriesPaint(i, Color.blue); 
    		}
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
    
    //This is a little hacky, but if we want only the legend, just extend Plot() and remove the draw functionality so it makes a blank plot
    class legendPlot extends Plot
    {
      public void draw(Graphics2D arg0, Rectangle2D arg1, Point2D arg2,
          PlotState arg3, PlotRenderingInfo arg4)
      {
	      
      }
      public String getPlotType()
      {
	      return null;
      }
    }
    //Then add the legend to that and throw away the original plot
    if(job.legendOnly)
    {
    	chart = new JFreeChart("", null, new legendPlot(), false);
    	chart.addLegend(actionEventsLegend);
    }

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
	
	public void formatAEPlot(PlotJob job, JFreeChart chart)
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
