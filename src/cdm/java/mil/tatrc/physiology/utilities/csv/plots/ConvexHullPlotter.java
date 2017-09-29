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
import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.LogarithmicAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.ValueAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.StandardXYItemRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import mil.tatrc.physiology.datamodel.substance.SESubstanceManager;
import mil.tatrc.physiology.utilities.DoubleUtils;
import mil.tatrc.physiology.utilities.FileUtils;
import mil.tatrc.physiology.utilities.Log;
import mil.tatrc.physiology.utilities.LogListener;
import mil.tatrc.physiology.utilities.csv.CSVContents;
import mil.tatrc.physiology.utilities.csv.ConvexHullMaker;
import mil.tatrc.physiology.utilities.csv.plots.PlotDriver.PlotJob;

public class ConvexHullPlotter implements Plotter
{

	protected Map<String,List<Double>> data = new HashMap<String,List<Double>>();
	
	public static void main(String[] args)
	{
		PlotDriver.main(args);
	}
	
	public List<List<Double>> splitHull(List<List<Double>> hullVals)
	{
		//The hull values should already be ordered such that the first point is the bottom-rightmost point (max X)
		//and subsequent points follow clockwise around the hull
		//We want two set of points, the ones that extend from the maximum X value to the minimum on bottom, and then the ones on the way back
		//So that our plot is continuous, we'll include the min and max X points in both sets
		
		List<Double> bottomXVals = new ArrayList<Double>();
		List<Double> bottomYVals = new ArrayList<Double>();
		List<Double> topXVals = new ArrayList<Double>();
		List<Double> topYVals = new ArrayList<Double>();
		List<Double> leftXVals = new ArrayList<Double>();
		List<Double> leftYVals = new ArrayList<Double>();
		List<Double> rightXVals = new ArrayList<Double>();
		List<Double> rightYVals = new ArrayList<Double>();
		List<List<Double>> splitVals = new ArrayList<List<Double>>();
		
		int minXindex = 0;
		
		for(int i = hullVals.get(0).size()-1; i >=0; i--)
		{
			if(hullVals.get(0).get(i) < hullVals.get(0).get(minXindex))
			{
				minXindex = i;
			}
		}
		
		bottomXVals.addAll(hullVals.get(0).subList(0, minXindex));
		bottomYVals.addAll(hullVals.get(1).subList(0, minXindex));
		
		//topXVals.add(hullVals.get(0).get(0));
		//topYVals.add(hullVals.get(1).get(0));
		topXVals.addAll(hullVals.get(0).subList(minXindex, hullVals.get(0).size()));
		topYVals.addAll(hullVals.get(1).subList(minXindex, hullVals.get(1).size()));
		
		leftXVals.add(bottomXVals.get(bottomXVals.size()-1));
		leftYVals.add(bottomYVals.get(bottomYVals.size()-1));
		leftXVals.add(topXVals.get(0));
		leftYVals.add(topYVals.get(0));
		
		rightXVals.add(bottomXVals.get(0));
		rightYVals.add(bottomYVals.get(0));
		rightXVals.add(topXVals.get(topXVals.size()-1));
		rightYVals.add(topYVals.get(topYVals.size()-1));
		
		splitVals.add(topXVals);
		splitVals.add(topYVals);
		splitVals.add(bottomXVals);
		splitVals.add(bottomYVals);
		splitVals.add(leftXVals);
		splitVals.add(leftYVals);
		splitVals.add(rightXVals);
		splitVals.add(rightYVals);
		
		return splitVals;
	}

	public void plot(LogListener listener, SESubstanceManager subMgr)
	{
		//fill PlotJob with needed data if it doesn't exist
		PlotJob job = (PlotJob)listener;
		if(job.dataPath == null || job.dataPath.isEmpty())
		{job.dataPath = "../verification/Scenarios/"+job.verificationDirectory+"/Current Baseline/";}
		if(job.dataFile == null || job.dataFile.isEmpty())
		{job.dataFile = job.name + "Results.zip";}
		
		//Get data contents for all headers
		if (data.isEmpty() || data == null)
    {
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
	    } catch (IOException e)
	    {
		    Log.error("Could not analyze file " + job.dataPath + job.dataFile);
	    }
    }
		
		//Catch some errors
		if(job.Y2headers.size() > 0 && job.X2header == null)
		{
			Log.error("No X2 header specified for job "+job.name+". Each Y axis must have a corresponding X axis.");
			return;
		}
		
		//Make a dataSeries for desired headers and add to collection(s)
		CSVPlotTool plotTool = new CSVPlotTool();	//to leverage existing functions
		String title = job.name + "_";
		XYSeriesCollection dataSet1 = new XYSeriesCollection();
		XYSeriesCollection dataSet2 = new XYSeriesCollection();
		double maxY1 = 0;
		double minY1 = Double.MAX_VALUE;
		double maxY2 = 0;
		double minY2 = Double.MAX_VALUE;
		for(int i = 0; i < job.Y1headers.size(); i++)
		{
			XYSeries dataSeriesTop;
			XYSeries dataSeriesBottom;
			XYSeries dataSeriesLeft;
			XYSeries dataSeriesRight;
			
			//For convex hulls, we have to reorder points before inserting into the dataset
			ConvexHullMaker maker = new ConvexHullMaker();
			List<List<Double>> newVals = new ArrayList<List<Double>>();
			List<List<Double>> splitVals = new ArrayList<List<Double>>();
			newVals = maker.make(data.get(job.X1header), data.get(job.Y1headers.get(i)));
			splitVals = splitHull(newVals);
			dataSeriesTop = plotTool.createXYSeries(job.Y1headers.get(i),splitVals.get(0),splitVals.get(1));
			dataSeriesBottom = plotTool.createXYSeries("",splitVals.get(2),splitVals.get(3));
			dataSeriesLeft = plotTool.createXYSeries("",splitVals.get(4),splitVals.get(5));
			dataSeriesRight = plotTool.createXYSeries("",splitVals.get(6),splitVals.get(7));
			
			dataSeriesBottom.setKey("REMOVE");
			dataSeriesLeft.setKey("REMOVE");
			dataSeriesRight.setKey("REMOVE");
			
			dataSet1.addSeries(dataSeriesTop);
			dataSet1.addSeries(dataSeriesBottom);
			dataSet1.addSeries(dataSeriesLeft);
			dataSet1.addSeries(dataSeriesRight);
			
			title = title + job.Y1headers.get(i) + "_";
			maxY1 = maxY1 < dataSeriesTop.getMaxY() ? dataSeriesTop.getMaxY() : maxY1;
			minY1 = minY1 > dataSeriesBottom.getMinY() ? dataSeriesBottom.getMinY() : minY1;
		}
		for(int i = 0; i < job.Y2headers.size(); i++)
		{
			XYSeries dataSeriesTop;
			XYSeries dataSeriesBottom;
			XYSeries dataSeriesLeft;
			XYSeries dataSeriesRight;
			
			ConvexHullMaker maker = new ConvexHullMaker();
			List<List<Double>> newVals = new ArrayList<List<Double>>();
			List<List<Double>> splitVals = new ArrayList<List<Double>>();
			newVals = maker.make(data.get(job.X2header), data.get(job.Y2headers.get(i)));
			splitVals = splitHull(newVals);
			dataSeriesTop = plotTool.createXYSeries(job.Y2headers.get(i),splitVals.get(0),splitVals.get(1));
			dataSeriesBottom = plotTool.createXYSeries("",splitVals.get(2),splitVals.get(3));
			dataSeriesLeft = plotTool.createXYSeries("",splitVals.get(4),splitVals.get(5));
			dataSeriesRight = plotTool.createXYSeries("",splitVals.get(6),splitVals.get(7));
			
			dataSeriesBottom.setKey("REMOVE");
			dataSeriesLeft.setKey("REMOVE");
			dataSeriesRight.setKey("REMOVE");
			
			dataSet2.addSeries(dataSeriesTop);
			dataSet2.addSeries(dataSeriesBottom);
			dataSet2.addSeries(dataSeriesLeft);
			dataSet2.addSeries(dataSeriesRight);
			
			title = title + job.Y2headers.get(i) + "_";
			maxY2 = maxY2 < dataSeriesTop.getMaxY() ? dataSeriesTop.getMaxY() : maxY2;
			minY2 = minY2 > dataSeriesBottom.getMinY() ? dataSeriesBottom.getMinY() : minY2;
		}
		title = title + "vs_" + job.X1header;
		if(job.X2header != null && !job.X1header.equalsIgnoreCase(job.X2header))
			title = title + "_" + job.X2header;
		
		//Override the constructed title if desired
		if(job.titleOverride != null && !job.titleOverride.isEmpty() && !job.titleOverride.equalsIgnoreCase("None"))
			title = job.titleOverride;
		
		//set labels
    String XAxisLabel = job.X1header;
    String YAxisLabel = job.Y1headers.get(0);

    JFreeChart chart = ChartFactory.createXYLineChart(
    		job.titleOverride!=null&&job.titleOverride.equalsIgnoreCase("None")?"":title,					 // chart title
        XAxisLabel,				 // x axis label
        YAxisLabel,				 // y axis label
        dataSet1,                   // data
        PlotOrientation.VERTICAL,  // orientation
        true,                      // include legend
        true,                      // tooltips
        false                      // urls
        );
    
    Log.info("Creating Graph "+title);   
    XYPlot plot = (XYPlot) chart.getPlot();
		
    if (!job.logAxis)
    {
	    // Determine Y1 range
	    double resMax0 = maxY1;
	    double resMin0 = minY1;
	    if (Double.isNaN(resMax0) || Double.isNaN(resMin0))
		    plot.getDomainAxis(0).setLabel("Range is NaN");
	    if (DoubleUtils.isZero(resMin0))
		    resMin0 = -0.001;
	    if (DoubleUtils.isZero(resMax0))
		    resMax0 = 0.001;
	    double rangeLength = resMax0 - resMin0;
	    ValueAxis yAxis = plot.getRangeAxis(0);
	    yAxis.setRange(resMin0 - 0.15 * rangeLength, resMax0 + 0.15 * rangeLength);//15% buffer so we can see top and bottom clearly  
	    
	    //Override the bounds if desired
	    try
      {
	      if(job.Y1LowerBound != null)
	      	yAxis.setLowerBound(job.Y1LowerBound);
	      if(job.Y1UpperBound != null)
	      	yAxis.setUpperBound(job.Y1UpperBound);
      } catch (Exception e)
      {
      	Log.error("Couldn't set Y bounds. You probably tried to set a bound on an axis that doesn't exist.");
      }
	    
	    plot.setRangeAxis(0,yAxis);
	    
	    //Add the second Y axis to the right side
	    if(!job.Y2headers.isEmpty())
	    {
  	    ValueAxis rightYAxis = new NumberAxis();
  	    // Determine Y2 range
  	    double resMax1 = maxY2;
  	    double resMin1 = minY2;
  	    if (Double.isNaN(resMax1) || Double.isNaN(resMin1))
  		    plot.getDomainAxis(1).setLabel("Range is NaN");
  	    if (DoubleUtils.isZero(resMin1))
  		    resMin1 = -0.001;
  	    if (DoubleUtils.isZero(resMax1))
  		    resMax1 = 0.001;
  	    rangeLength = resMax1 - resMin1;
  	    rightYAxis.setRange(resMin1 - 0.15 * rangeLength, resMax1 + 0.15* rangeLength);
  	    rightYAxis.setLabel(job.Y2headers.get(0));
  	    
  	    //Override the bounds if desired
  	    try
        {
  	      if(job.Y2LowerBound != null)
  	      	rightYAxis.setLowerBound(job.Y2LowerBound);
  	      if(job.Y2UpperBound != null)
  	      	rightYAxis.setUpperBound(job.Y2UpperBound);
        } catch (Exception e)
        {
        	Log.error("Couldn't set Y bounds. You probably tried to set a bound on an axis that doesn't exist.");
        }
  	    
  	    plot.setRangeAxis(1, rightYAxis);
	    }
    }
    else
    {
    	double resMin = minY1 < minY2 ? minY1 : minY2;
    	if(resMin <= 0.0)
    		resMin = .00001;
    	LogarithmicAxis yAxis = new LogarithmicAxis("Log("+YAxisLabel+")");
    	yAxis.setLowerBound(resMin);
    	
    	//Override the bounds if desired
    	try
      {
	      if(job.Y1LowerBound != null)
	      	yAxis.setLowerBound(job.Y1LowerBound);
	      if(job.Y1UpperBound != null)
	      	yAxis.setUpperBound(job.Y1UpperBound);
      } catch (Exception e)
      {
      	Log.error("Couldn't set Y bounds. You probably tried to set a bound on an axis that doesn't exist.");
      }
	    
	    plot.setRangeAxis(0, yAxis);
    	
    	if (!job.Y2headers.isEmpty())
      {
	      LogarithmicAxis rightYAxis = new LogarithmicAxis("Log("
	          + job.Y2headers.get(0)+")");
	      rightYAxis.setLowerBound(resMin);
	      
	      //Override the bounds if desired
	      try
        {
  	      if(job.Y2LowerBound != null)
  	      	rightYAxis.setLowerBound(job.Y2LowerBound);
  	      if(job.Y2UpperBound != null)
  	      	rightYAxis.setUpperBound(job.Y2UpperBound);
        } catch (Exception e)
        {
        	Log.error("Couldn't set Y bounds. You probably tried to set a bound on an axis that doesn't exist.");
        }
  	    
  	    plot.setRangeAxis(1, rightYAxis);
      }
    }
    
    //Override X bounds if desired
    try
    {
      if(job.X1LowerBound != null)
      	plot.getDomainAxis(0).setLowerBound(job.X1LowerBound);
      if(job.X1UpperBound != null)
      	plot.getDomainAxis(0).setUpperBound(job.X1UpperBound);
      if(job.X2LowerBound != null)
      	plot.getDomainAxis(1).setLowerBound(job.X2LowerBound);
      if(job.X2UpperBound != null)
      	plot.getDomainAxis(1).setUpperBound(job.X2UpperBound);
    }
    catch(Exception e)
    {
    	Log.error("Couldn't set X bounds. You probably tried to set a bound on an axis that doesn't exist.");
    }
    
    //Add the second dataset if necessary
    if(!job.Y2headers.isEmpty())
    {
    	plot.setDataset(1, dataSet2);
    	plot.mapDatasetToRangeAxis(1, 1);
    }
    
    //Override labels if desired
    if(job.X1Label != null && !plot.getDomainAxis(0).getLabel().contains("NaN"))
    	plot.getDomainAxis(0).setLabel(job.X1Label.equalsIgnoreCase("None")?"":job.X1Label);
    if(job.X2Label != null && plot.getDomainAxis(1) != null)
    	plot.getDomainAxis(1).setLabel(job.X2Label.equalsIgnoreCase("None")?"":job.X2Label);
    if(job.Y1Label != null)
    	plot.getRangeAxis(0).setLabel(job.Y1Label.equalsIgnoreCase("None")?"":job.Y1Label);
    if(job.Y2Label != null && plot.getRangeAxis(1) != null)
    	plot.getRangeAxis(1).setLabel(job.Y2Label.equalsIgnoreCase("None")?"":job.Y2Label);
		
    //Format lines and colors
    plotTool.formatXYPlot(chart, job.bgColor);
    plot.setDomainGridlinesVisible(job.showGridLines);
    plot.setRangeGridlinesVisible(job.showGridLines);
    formatConvexHullPlot(job, chart, dataSet1, dataSet2);
    
    //Handle legends
    if(job.removeAllLegends)
    	chart.removeLegend();
    
    //Make the file
    try
    {
      FileUtils.createDirectory(job.outputDir);
      String filename = job.outputFilename==null ? job.outputDir+"/"+plotTool.MakeFileName(title)+".jpg" : job.outputDir+"/"+job.outputFilename;
      if(!filename.endsWith(".jpg"))
      	filename = filename + ".jpg";
      File JPGFile = new File(filename);
      if(job.imageHeight != null && job.imageWidth != null)
      	ChartUtilities.saveChartAsJPEG(JPGFile, chart, job.imageWidth, job.imageHeight);
      else
      	ChartUtilities.saveChartAsJPEG(JPGFile, chart, 1600, 800);
    }
    catch (IOException e)
    {
      Log.error(e.getMessage());
    }

	}
	
	protected void formatConvexHullPlot(PlotJob job, JFreeChart chart, XYSeriesCollection dataSet1, XYSeriesCollection dataSet2)
	{
		XYPlot plot = (XYPlot) chart.getPlot();
		XYLineAndShapeRenderer renderer1 = (XYLineAndShapeRenderer) plot.getRenderer();
    BasicStroke wideLine = new BasicStroke( 2.0f ); 
    
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
    
		//If there is only one Y axis, color all datasets red (so top, bottom, left, and right will be the same)
		if(job.Y2headers == null || job.Y2headers.isEmpty())
		{
	    for(int i = 0; i < dataSet1.getSeriesCount(); i++)
	    {
	    	renderer1.setSeriesStroke(i, wideLine); 
	    	renderer1.setBaseShapesVisible(false);
	      renderer1.setSeriesFillPaint(i, Color.red); 
	      renderer1.setSeriesPaint(i, Color.red);
	      if(dataSet1.getSeries(i).getKey() != null && dataSet1.getSeries(i).getKey().toString().equalsIgnoreCase("REMOVE"))
	      	renderer1.setSeriesVisibleInLegend(i, false);
	    }
		}
		//If there are 2 Y axes, we should color the axes to correspond with the data so it isn't (as) confusing
		else
		{
			StandardXYItemRenderer renderer2 = new StandardXYItemRenderer();
			plot.setRenderer(1, renderer2);
			
			for(int i = 0; i < dataSet1.getSeriesCount(); i++)
	    {
				renderer1.setSeriesStroke(i, wideLine); 
	      renderer1.setBaseShapesVisible(false);
	      renderer1.setSeriesFillPaint(i, Color.red); 
	      renderer1.setSeriesPaint(i, Color.red);
	      if(dataSet1.getSeries(i).getKey() != null && dataSet1.getSeries(i).getKey().toString().equalsIgnoreCase("REMOVE"))
	      	renderer1.setSeriesVisibleInLegend(i, false);
	    }
			for(int i = 0; i < dataSet2.getSeriesCount(); i++)
	    {
				renderer2.setSeriesStroke(i, wideLine); 
				renderer2.setBaseShapesVisible(false);
	      renderer2.setSeriesFillPaint(i, Color.blue); 
	      renderer2.setSeriesPaint(i, Color.blue);
	      if(dataSet2.getSeries(i).getKey() != null && dataSet2.getSeries(i).getKey().toString().equalsIgnoreCase("REMOVE"))
	      	renderer2.setSeriesVisibleInLegend(i, false);
	    }
			plot.getRangeAxis(0).setLabelPaint(Color.red);
			plot.getRangeAxis(0).setTickLabelPaint(Color.red);
			plot.getRangeAxis(1).setLabelPaint(Color.blue);
			plot.getRangeAxis(1).setTickLabelPaint(Color.blue);
		}
	}


}
