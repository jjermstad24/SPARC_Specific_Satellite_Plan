//------------------------------------------------------------------------------
//                           Spacecraft
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
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.02
// Modified: 2022.01.04 by Vinay
//
/**
 * Definition of the Spacecraft class.
 *  
 * The Spacecraft class is a container for objects related to the spacecraft, including abstractions 
 * such as orbit and attitude, algorithms such as the LaGrange interpolator, or models of objects such as sensors.
 * 
 * The spacecraft class provides operations to access the state of its contained objects, and to do computations 
 * based on that state. Note that some of the containments are pointers to objects (e.g., orbitState, orbitEpoch), 
 * the objects which can be modified outside the Spacecraft object.
 * 
 * A key part of this spacecraft state that is maintained is the rotation matrix from the Nadir pointing reference frame 
 * to the body frame. This matrix is computed from user-set Euler angles & Euler sequence.
 * 
 * Another example is that  the CoverageChecker calls Spacecraft’s CheckTargetVisibility operator, 
 * which rotates an input target-vector to the sensor frame and then calls the sensor to check whether the input target is in the field of view. 
 * 
 * The current implementation of the Spacecraft class has been verified with maximum of one sensor attachment.
 */
//------------------------------------------------------------------------------
#ifndef Spacecraft_hpp
#define Spacecraft_hpp

#include "gmatdefs.hpp"
#include "OrbitState.hpp"
#include "AbsoluteDate.hpp"
#include "Sensor.hpp"
#include "Attitude.hpp"
#include "LagrangeInterpolator.hpp"
#include "Rvector6.hpp"

class Spacecraft
{
public:
   
   /// class construction/destruction
   // class methods
   Spacecraft(AbsoluteDate *epoch, OrbitState *state, Attitude *att,
              LagrangeInterpolator *interp,
              Real angle1 = 0.0, Real angle2 = 0.0, Real angle3 = 0.0,
              Integer seq1 = 1, Integer seq2 = 2, Integer seq3 = 3); // angles in degrees
   Spacecraft( const Spacecraft &copy);
   Spacecraft& operator=(const Spacecraft &copy);
   
   virtual ~Spacecraft();
   
   /// Get the orbit state
   virtual OrbitState*    GetOrbitState();
   /// Get the orbit epoch
   virtual AbsoluteDate*  GetOrbitEpoch();
   /// Get the Julian date
   virtual Real           GetJulianDate();
   /// Get the spacecraft attitude
   virtual Attitude*      GetAttitude();
   /// Get the current cartesian state (Inertial frame)
   virtual Rvector6       GetCartesianState();
   /// Get the current Keplerian state
   virtual Rvector6 GetKeplerianState();
   /// Add a sensor to the spacecraft
   virtual void           AddSensor(Sensor* sensor);
   /// Does this spacecraft have sensors?
   virtual bool           HasSensors();
   /// Set the drag area
   virtual void           SetDragArea(Real area);
   /// Set the drag coefficient
   virtual void           SetDragCoefficient(Real Cd);
   /// Set the total mass
   virtual void           SetTotalMass(Real mass);
   /// Set the attitude for the spacecraft
   virtual void           SetAttitude(Attitude *att);
   /// Get the drag area
   virtual Real           GetDragArea();
   // Get the drag coefficient
   virtual Real           GetDragCoefficient();
   /// Get the toal mass
   virtual Real           GetTotalMass();
   
   /// This method returns the interpolated MJ2000 Cartesian state at the input date
   virtual Rvector6       GetCartesianStateAtEpoch(const AbsoluteDate &atDate);
   
   /// Check the target visibility given the input cone and clock angles for
   /// the input sensor number
   virtual bool           CheckTargetVisibility(Real    targetConeAngle,
                                                Real    targetClockAngle,
                                                Integer sensorNumber);

   /// Check the target visibility given the input body fixed state and
   /// spacecraft-to-target vector, at the input time, for the input
   /// sensor number
   virtual bool           CheckTargetVisibility(const Rvector6 &bodyFixedState,
                                                const Rvector3 &satToTargetVec,
                                                Real            atTime,
                                                Integer         sensorNumber);

   /// Get the body-fixed-to-reference (Earth-fixed to Nadir) rotation matrix
   virtual Rmatrix33 GetBodyFixedToReference(const Rvector6 &bfState);
   
   /// Set orbit state (Keplerian elements) for the spacecraft at the input time t
   virtual bool           SetOrbitEpochOrbitStateKeplerian(const AbsoluteDate &t,
                                        const Rvector6 &kepl);
   /// Set orbit state (Cartesian elements) for the spacecraft at the input time t
   virtual bool           SetOrbitEpochOrbitStateCartesian(const AbsoluteDate &t,
                                        const Rvector6 &cart); 

   /// Set the body nadir offset angles for the spacecraft
   virtual void           SetBodyNadirOffsetAngles(
                              Real angle1 = 0.0, Real angle2 = 0.0,
                              Real angle3 = 0.0,
                              Integer seq1 = 1, Integer seq2 = 2,
                              Integer seq3 = 3);
   
   /// Can the orbit be interpolated - i.e. are there enough points, etc.?
   virtual bool           CanInterpolate(Real atTime);
   
   /// Is it time to interpolate?  i.e. are there enough points? if so, what is
   /// the midpoint of the independent variable lower/upper range
   virtual bool           TimeToInterpolate(Real atTime, Real &midRange);
   
   /// Interpolate the data to the input toTime
   virtual Rvector6       Interpolate(Real toTime);

   // Get the rotation matrix from Nadir pointing frame to Spacecraft body frame.
   Rmatrix33 GetNadirToBodyMatrix();
   
protected:
   
   /// Drag coefficient
	Real                 dragCoefficient;
   /// Drag area in m^2
	Real                 dragArea;
    /// Total Mass in kg
	Real                 totalMass;
	/// Pointer to the Orbit State object
	OrbitState           *orbitState;
	/// Pointer to the Orbit Epoch object
	AbsoluteDate         *orbitEpoch;
	/// Number of attached sensors
	Integer              numSensors;
	/// Vector of pointers to sensor objects (sensors regarded as attached to the spacecraft)
	std::vector<Sensor*> sensorList;
   /// Pointer to the Attitude object
   Attitude             *attitude;
   /// Pointer to the interpolator to use (for Hermite only, currently)
   LagrangeInterpolator *interpolator;
   /// Offset angles
   Real                 offsetAngle1;
   Real                 offsetAngle2;
   Real                 offsetAngle3;
   
   /// Euler sequence
   Integer              eulerSeq1;
   Integer              eulerSeq2;
   Integer              eulerSeq3;
   
   /// The rotation matrix from the nadir-pointing frame to the spacecraft-body frame
   Rmatrix33            R_Nadir2ScBody;   
   
   /// @todo - do we need to buffer states here as well??
   
   /// Convert view vector to cone and clock angles
   virtual void  VectorToConeClock(const Rvector3 &viewVec,
                                     Real &cone,
                                     Real &clock);
   /// Compute the nadir-pointing-to-spacecraft-body-matrix
   virtual void  ComputeNadirToBodyMatrix();

};
#endif // Spacecraft_hpp
