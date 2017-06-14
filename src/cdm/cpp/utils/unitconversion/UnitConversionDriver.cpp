// UnitConversionDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PScalar.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>


int _tmain(int argc, _TCHAR* argv[])
{
#if 1
	CCompoundUnit theUnit("MHz");
	CCompoundUnit theTestUnit("MeV");
	if (*theUnit.GetDimension() == *theTestUnit.GetDimension())
	{
		std::cout<<"Fail"<<std::endl;
	}


	CPScalar dummy2;
	bool tf;
	tf = dummy2 < 0.0;
	tf = 0.0 < dummy2;
	dummy2 *= 1.0;
	dummy2 = (2.0 * dummy2);

	// Note: Arithmetic with CPScalar objects is done just as you would do it
	// using doubles or floats. There are two ways to invoke a conversion
	// with CPScalar objects. The first is with the overloaded function
	// call syntax:
	//			myPScalar("unitstring")
	// This converts the CPScalar value in myPScalar to the units specified
	// by "unitstring", but doesn't modify the myPScalar object. It's similar to
	// how the expression "a + 1" yields a value that is one greater than what's
	// stored in the variable "a", but doesn't modify the value of "a" itself.
	// The result of this expression is a compiler-generated "temporary".
	// Contrast this to the other method of specifying conversions:
	//			myPScalar.ConvertTo("unitstring")
	// This does the conversion directly on the myPScalar object itself, modifying
	// its contents. It's akin to the expression "++a", which not only computes the
	// incremented value, but modifies "a" itself with that new value.

	// Test arithmetic involving unitless constants and non-unitless-but-dimensionless
	// PScalars that arise when dividing like quantities of different units
	CPScalar c(1.0, "c"); // speed of light
	CPScalar v(185000, "mi/s"); // close to speed of light, in different units. Gamma should be big.
	CPScalar v2 = v*v;
	CPScalar c2 = c*c;
	CPScalar v2divc2 = v2/c2;
	v2divc2.ConvertTo("m/m"); // Convert to unity-scale dimensionless
	cerr << "v2divc2 is " << v2divc2 << endl;
	cerr << "v2divc2 + 1 is" << v2divc2+1 << endl;
	CPScalar beta = v/c;
	double unitlessSpeed = v.GetValue();
	float unitlessBeta(beta);
	cerr << "beta is " << beta << endl;
	cerr << "Unitless beta is " << unitlessBeta << endl;
	CPScalar gamma = 1.0/sqrt(1.0 -(v*v)/(c*c));
	cerr << "Gamma is " << gamma << endl;
	gamma.ConvertTo("");

	cerr << "Gamma is " << gamma << endl;
	// Now lets see what happens when we do this with "v" in compatible units
	v.ConvertTo("c");
	gamma = 1.0/sqrt( 1.0 -(v*v)/(c*c));
	cerr << "Gamma is " << gamma << endl;

	// Test count units on AmountOfSubstance
	CPScalar massAmount(13, "ct kg");
	// Converting from a "count" of something to that something, should be an
	// identity transformation:
	CPScalar totMass(massAmount);
	totMass.ConvertTo("kg");
	cerr << massAmount << " should equal " << totMass << endl << endl;
	// Quick sanity check:
	CPScalar time(1, "hr");
	cerr << time << " is " << time("s") << endl;
	cerr << "Diff between 1 hr and 59 minutes is " << std::abs(CPScalar(59,"min") - time) << endl;
	if (CCompoundUnit("cm/us") == CCompoundUnit("cm/us"))
	{
		cerr << "Unit equality test passed" << endl;
	}
	else
	{
		cerr << "Unit equality test failed" << endl;
	}

	CPScalar meter(1, "m"), yard(1,"yd");
	cerr << "A meter is ";
	if (meter < yard)
		cerr << "less than";
	else if (meter > yard)
		cerr << "greater than";
	else 
		cerr << "unordered with respect to";
	cerr << " a yard" << endl;

	cerr << "A yard is ";
	if (yard < meter)
		cerr << "less than";
	else if (yard > meter)
		cerr << "greater than";
	else 
		cerr << "unordered with respect to";
	cerr << " a meter" << endl;


	//////////////////////////////////////////
	// Example 1: Temperature conversion
	//////////////////////////////////////////
	CPScalar tempF(-40,"degF"); // degrees fahrenheit

	// Convert to other temp scales
	// The following three declarations demonstrate the use of the
	// copy constructor, as well as the function call operator to yield
	// a CPScalar value in a new unit.
	CPScalar tempK(tempF("K"));
	CPScalar tempC(tempF("degC"));
	CPScalar tempR(tempF("degR"));
	cerr << "Example 1:" << endl;
	cerr << tempF << " is " << tempC << " is " << tempK << 
		" is " << tempR << endl << endl;


	///////////////////////////////////////////
	// Example 2: length/area computation
	///////////////////////////////////////////
	// We have a square plot of land, 1000 acres. How long is each side?
	CPScalar area(1000,"acre");
	CPScalar side = sqrt(area).ConvertTo("mi");
	cerr << "Example 2:" << endl;
	cerr << "Sqrt of " << area << " is " << side << endl << endl;

	///////////////////////////////////////////
	// Example 3: length/volume computation
	///////////////////////////////////////////
	cerr << "Example 3:" << endl;
	cerr << "\nWe have a cube-shaped box whose  volume is\n";
	cerr << "3 gallons and 2 liters. How high is the box in inches?\n";
	// Volume = side^3, thus side is cube root of Volume
	CPScalar vol(3,"gal");
	vol += CPScalar(2, "l");
	side = pow(vol, (1.0/3.0));
	side.ConvertTo("in");
	cerr << "Answer: " << side << endl << endl;

	//////////////////////////////////////////
	// Example 4: Unit composition
	/////////////////////////////////////////
	// Constructing from unit string, default value is 1.0
	// "lt" is unit for speed of light, "yr" is "year", a unit of time.
	CPScalar lightyear("lt yr"); 
	// Demonstrate overloaded function call operator to express a 
	// lightyear in miles
	cerr << "Example 4:" << endl;
	cerr << lightyear << " is " << lightyear("mi") << endl << endl;

	///////////////////////////////////////////////////
	// Example 5: Demonstrate "dimensionless" units
	///////////////////////////////////////////////////
	// Init file defines radian as a unit of quantity "PlaneAngle", which has dimensions of  of m/m,
	// which is therefore dimensionless. 
	CPScalar freq(.5,"Hz");
	CPScalar angfreq(freq("rad/s"));
	cerr << "Example 5:" << endl;
	cerr << freq << " is " << angfreq << endl << endl;

	/////////////////////////////////
	// Example 6: Work conversion
	/////////////////////////////////
	// Take a pound of mass, accelerate it at a ft/min^2, through a distance of a ft.
	// What is its kinetic energy in Joules?
	CPScalar energy("lb ft^2 min^-2");
	cerr << "Example 6:" << endl;
	cerr << energy << " is " << energy("J") << endl << endl;

	/////////////////////////////////
	// Example 7: Convert photon wavelength to frequency and energy and vice versa
	// These are the ELN, or E, Lambda, Nu (common variables representing energy, 
	// wavelength, and frequency) conversions
	/////////////////////////////////
	CPScalar lambda(400, "nm");
	CPScalar nu = lambda("THz");
	cerr << "Example 7:" << endl;
	cerr << "A " << lambda << " wavelength photon has a frequency of " << nu << endl;
	// convert other direction
	lambda = nu("nm");
	cerr << "A " << nu << "frequency photon has a wavelength of " << lambda <<  endl;
	// Do energy
	energy = lambda("eV");
	cerr << "A " << lambda << " wavelength photon has an energy of " << energy << endl;
	lambda = energy("nm");
	cerr << "A " << energy << " energy photon has a wavelength of " << lambda << endl;
	energy = nu("eV");
	cerr << "A " << nu << " frequency photon has an energy of " << energy << endl;
	nu = energy("THz");
	cerr << "A " << energy << " energy photon has a frequency of " << nu << endl << endl;


	/////////////////////////////////
	// Example 8: Convert mass to energy and vice versa
	/////////////////////////////////
	CPScalar mass(1,"ug"); // a microgram
	energy = mass("kW hr"); // obtain equiv energy in kilowatt-hours
	cerr << "Example 8:" << endl;
	cerr << "A mass of " << mass << " can be converted to " << energy << " of energy" << endl;
	// convert other direction
	mass = energy("ug");
	cerr << energy << " of energy can be converted to " << mass << " of mass" << endl << endl;

	/////////////////////////////////
	// Example 9: A simple chemistry problem
	/////////////////////////////////
	// Define the mass of an oxygen atom
	CPScalar OxygenAtomicMass(16,"u");
	// Define how much of it we have, say 3 moles
	CPScalar amount(3,"mol");
	// What's the mass of that much oxygen?
	CPScalar totalMass = OxygenAtomicMass * amount;
	cerr << "Example 9:" << endl;
	cerr << "If the atomic mass of oxygen is " << OxygenAtomicMass << " and we have " <<
		amount << " of it, then the mass of that much oxygen is " << totalMass("kg") << endl << endl;


	/////////////////////////////////
	// Example 10: Electric Field Strength at an antenna to Power Density
	/////////////////////////////////
	CPScalar EFieldMag(50, "kV/m");
	CPScalar PowerDensity(EFieldMag("W/cm^2"));
	cerr << "Example 10:" << endl;
	cerr << "If the RMS electric field strength at an antenna is " << EFieldMag <<
		" then the power density is " << PowerDensity << endl;
	// Confirm inverse mapping
	EFieldMag = PowerDensity("kV/m");
	cerr << "If the power density at an antenna is " << PowerDensity <<
		" then the RMS electric field strength is " << EFieldMag << endl;

	// Scale up the E field by 10, and make sure the PD goes up by 100x
	EFieldMag *= 10.0;
	PowerDensity = EFieldMag("W/cm^2");
	cerr << "If the RMS electric field strength at an antenna is " << EFieldMag <<
		" then the power density is " << PowerDensity << endl;
	// Confirm inverse mapping
	EFieldMag = PowerDensity("kV/m");
	cerr << "If the power density at an antenna is " << PowerDensity <<
		" then the RMS electric field strength is " << EFieldMag << endl << endl;

	/////////////////////////////////
	// Example 11: Fluid flow through a pipe
	/////////////////////////////////
	// Here are our inputs to the problem
	CPScalar pipeDiam(0.75,"in");
	CPScalar waterSpeed(15, "ft/s");
	cerr << "Water is being pumped through a " << pipeDiam << " diameter pipe at a speed of " <<
		waterSpeed << "." << endl;
	// Compute flow rate in GPM
	CPScalar pipeXSectArea = pow(pipeDiam/2.0, 2.0) * M_PI; 
	CPScalar flowRate = pipeXSectArea * waterSpeed;
	cerr << "The flow rate is " << flowRate("gal/min") << ". " << endl << endl;

	/////////////////////////////////
	// Example 11: Temperature and Energy quantity type conversion
	/////////////////////////////////
	CPScalar avgKE(1, "eV");
	cerr << "In a gas where the average kinetic energy of the constituent particles is " << avgKE <<
		", the temperature is " << avgKE("K") << "." << endl;


	cerr << endl;
	CPScalar photon(1, "um");
	cerr << photon << endl;
	photon.ConvertTo("MHz");
	cerr << photon << endl;
	photon.ConvertTo("eV");
	cerr << photon << endl;
	photon.ConvertTo("nm");
	cerr << photon << endl;
	photon.ConvertTo("GHz");
	cerr << photon << endl;
	photon.ConvertTo("meV");
	cerr << photon << endl;

#endif
	CPScalar current(100,"W");
	CPScalar dbCurrent(40, "dB A");
	cerr << "This should say 40 dB A: " << dbCurrent << endl;

	cerr << "A current of " << current << " is equivalent to " << current("dB W") << endl;

	CPScalar lum(1, "fL");
	cerr << "A luminance of " << lum << " is equivalent to " << lum("cd/m^2") << endl;
	string dummy;
	cin >> dummy;
	return 0;
}

