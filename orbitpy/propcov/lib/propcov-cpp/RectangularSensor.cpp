//------------------------------------------------------------------------------
//                           RectangularSensor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Mike Stark, NASA/GSFC
// Created: 2017.04.03
// Modified: 2021.00.00 Ryan
// Modifed: 2022.02.26 Vinay
//
/**
 * Implementation of the RectangularSensor class
 */
//------------------------------------------------------------------------------

#include "RectangularSensor.hpp"
#include "RealUtilities.hpp"
#include <iostream>

using namespace GmatMathUtil; // for trig functions, and
                              // temporarily square root fn
Real PI = GmatMathConstants::PI;


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RectangularSensor(Real angleWidthIn, Real angleHeightIn)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param angleHeightIn  angle height
 * @param angleWidthIn   angle width
 */
//------------------------------------------------------------------------------
RectangularSensor::RectangularSensor(Real angleHeightIn, Real angleWidthIn) :
   Sensor()
{
   angleWidth  = angleWidthIn;
   angleHeight = angleHeightIn;

   // length great circle from origin (0,0) to (angleHeight,angleWidth)
   // angular equivalent of length of hypotenuse of triangle for computing
   // length of a rectangle's diagonal from origin to (height, width)
   maxExcursionAngle = ACos( Cos(angleHeight/2)*Cos(angleWidth/2) ); // this is also the cone angle of all the vertices of the spherical-rectangle
   std::vector<Real> clocks = getClockAngles();
   std::vector<Rvector3> corners = getCornerHeadings(clocks);
   poles = getPoleHeadings(corners);
}

//------------------------------------------------------------------------------
// RectangularSensor(const RectangularSensor &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
RectangularSensor::RectangularSensor(const RectangularSensor &copy) :
   Sensor(copy),
   angleWidth (copy.angleWidth),
   angleHeight(copy.angleHeight)
{
}

//------------------------------------------------------------------------------
// RectangularSensor& operator=(const RectangularSensor &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the RectangularSensor
 *
 * @param copy object to copy
 */
//------------------------------------------------------------------------------
RectangularSensor& RectangularSensor::operator=(const RectangularSensor &copy)
{
   if (&copy != this)
   {
      Sensor::operator=(copy);
      angleHeight = copy.angleHeight;
      angleWidth  = copy.angleWidth;
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~RectangularSensor()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
//------------------------------------------------------------------------------
RectangularSensor::~RectangularSensor()
{
}

//------------------------------------------------------------------------------
//  bool CheckTargetVisibility(Real viewConeAngle, Real viewClockAngle = 0.0)
//------------------------------------------------------------------------------
/**
 * Determines whether or not the point is in the sensor FOV
 *
 * @param viewConeAngle   the view cone angle
 * @param viewClockAngle  the view clock angle <unused for this class>
 *
 * @return true if the point is in the sensor FOV; false otherwise
 */
//------------------------------------------------------------------------------
bool RectangularSensor::CheckTargetVisibility(Real viewConeAngle,
                                              Real viewClockAngle)
{
   //if(viewConeAngle >= PI/2.0)
   //	return false;
   bool possiblyInView=true;
   if (!CheckTargetMaxExcursionAngle(viewConeAngle))
      possiblyInView = false;

   bool inView=false;
   if (!possiblyInView)
        inView = false;
   else{
      Real viewDec = PI/2.0 - viewConeAngle;
      Rvector3 viewVector = RADECtoUnitVec(viewClockAngle,viewDec);
      
      // Below condition works only when the corners (from which the poles are built) are specified in anti-clockwise order. 
      if( poles[0]*viewVector > 0.0 && poles[1]*viewVector > 0.0 && 
         poles[2]*viewVector > 0.0 && poles[3]*viewVector > 0.0 )
      {
         inView = true;
      }
   }
   
   return inView;     
}

//------------------------------------------------------------------------------
//  void SetAngleHeight(Real angleHeightIn)
//------------------------------------------------------------------------------
/**
 * Sets the angle height for the RectangularSensor
 *
 * @param angleHeightIn angle height
 */
//------------------------------------------------------------------------------
void  RectangularSensor::SetAngleHeight(Real angleHeightIn)
{
   angleHeight = angleHeightIn;
}

//------------------------------------------------------------------------------
//  Real GetAngleHeight()
//------------------------------------------------------------------------------
/**
 * Returns the angle height for the RectangularSensor
 *
 * @return the angle height
 */
//------------------------------------------------------------------------------
Real RectangularSensor::GetAngleHeight()
{
   return angleHeight;
}

//------------------------------------------------------------------------------
//  void SetAngleWidth(Real angleWidthIn)
//------------------------------------------------------------------------------
/**
 * Sets the angle width for the RectangularSensor
 *
 * @param angleWidthIn angle width
 */
//------------------------------------------------------------------------------
void  RectangularSensor::SetAngleWidth(Real angleWidthIn)
{
   angleWidth = angleWidthIn;
}

//------------------------------------------------------------------------------
//  Real GetAngleWidth()
//------------------------------------------------------------------------------
/**
 * Returns the angle width for the RectangularSensor
 *
 * @return the angle width
 */
//------------------------------------------------------------------------------
Real RectangularSensor::GetAngleWidth()
{
   return angleWidth;
}

std::vector<Real> RectangularSensor::getClockAngles()
{
	std::vector<Real> clocks(4);
   Real clock = ASin(Sin(angleHeight/2)/Sin(maxExcursionAngle));
   // anti-clockwise order. Note that it is critical that this order be the anticlockwise for the CheckTargetVisibility function.
	clocks[0] = clock;
	clocks[1] = PI - clock;
	clocks[2] = PI + clock;
	clocks[3] = 2.0*PI - clock;
	
	return clocks;
}

std::vector<Rvector3> RectangularSensor::getCornerHeadings(std::vector<Real> &clocks)
{
	std::vector<Rvector3> headings(4);
	// Declination
	Real dec = PI/2.0 - maxExcursionAngle;
	
	for(int i = 0; i < 4;i++)
	{
		headings[i] = RADECtoUnitVec(clocks[i],dec);
	}
	
	return headings;
}

std::vector<Rvector3> RectangularSensor::getPoleHeadings(std::vector<Rvector3> &corners)
{
	std::vector<Rvector3> poles(4);
	// corners are expected to be ordered
	poles[0] = Cross(corners[0],corners[1]);
	poles[1] = Cross(corners[1],corners[2]);
	poles[2] = Cross(corners[2],corners[3]);
	poles[3] = Cross(corners[3],corners[0]);
	
	return poles;
}





