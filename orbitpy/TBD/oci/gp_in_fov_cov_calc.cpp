//------------------------------------------------------------------------------
//                           TATC coverage calculator
//------------------------------------------------------------------------------
//
// Author: Vinay Ravindra
// Created: 2020.09.22
//
/**
 * Run coverage calculations. Coverage calculation at the time-steps given in the input satellite state file.
 * 
 * The pointing of the satellite is fixed to be Nadir-pointing nominally. When the 'yaw180_flag' is set,
 * the satellite is rotated about 180 deg about yaw axis and additional coverage is calculated.
 * 
 * Latitudes must be in the range -pi/2 to pi/2, while longitudes must be in the range -pi to pi.
 * 
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip> 
#include <ctime>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "Earth.hpp"
#include "KeyValueStatistics.hpp"
#include "VisiblePOIReport.hpp"
#include "OrbitState.hpp"
#include "PointGroup.hpp"
#include "Propagator.hpp"
#include "ConicalSensor.hpp"
#include "RectangularSensor.hpp"
#include "CustomSensor.hpp"
#include "CoverageChecker.hpp"
#include "NadirPointingAttitude.hpp"
#include "LagrangeInterpolator.hpp"
#include "TimeTypes.hpp"

#include "oci_utils.h"

#define DEBUG_CONSISE
//#define DEBUG_CHK_INPS
// #define COMPUTE_AND_STORE_POI_GEOMETRY

using namespace std;
using namespace GmatMathUtil;
using namespace GmatMathConstants;


/**
 * @param satStateFp Filename, path to write the satellite ECI states
 * @param covGridFp coverage grid file path and name
 * @param fovGeom sensor FOV geometry type
 * @param senOrien sensor orientation (euler angles in degrees and sequence)
 * @param fovClock sensor clock angles in degrees
 * @param fovCone sensor cone angles in degrees
 * @param yaw180_flag 
 * @param satAccFp Filename, path to write the computed satellite access data
 *
 */
int main(int argc, char *argv[])
{
  /** Set up the messaging and output **/
  std::string outFormat = "%16.9f";
 
  std::string outPath = "./";
  //MessageInterface::SetLogFile(outPath + "OClog.txt");
  //MessageInterface::SetLogEnable(true);
  MessageInterface::ShowMessage("%s\n",
                                GmatTimeUtil::FormatCurrentTime().c_str());
  /** Parse input arguments **/ 
  string satStateFp;
  string covGridFp; 
  string fovGeom; 
  string _senOrien; 
  string _fovClock; 
  string _fovCone;
  bool yaw180_flag; 
  string satAccFp;

  if(argc==9){            
      covGridFp = argv[1];
      fovGeom = argv[2];
      _senOrien = argv[3];
      _fovClock = argv[4];
      _fovCone = argv[5];
      yaw180_flag = bool(stoi(argv[6]));
      satStateFp = argv[7];
      satAccFp = argv[8];
   }else{
      MessageInterface::ShowMessage("Please input right number of arguments.\n");
      exit(1);
   }  

   RealArray senOrien(oci_utils::convertStringVectortoRealVector(oci_utils::extract_dlim_str(_senOrien, ',')));
   if(senOrien.size()!=6){
      MessageInterface::ShowMessage("Sensor orientation must be specified in a set of euler angles and sequence.\n");
      exit(1);
   }

   RealArray fovClock(oci_utils::convertStringVectortoRealVector(oci_utils::extract_dlim_str(_fovClock, ',')));
   RealArray fovCone(oci_utils::convertStringVectortoRealVector(oci_utils::extract_dlim_str(_fovCone, ',')));
   if(fovCone.size()==0){
      MessageInterface::ShowMessage("Atleast one sensor cone angle must be present.\n");
      exit(1);
   }
   if(fovCone.size()!=fovClock.size()){ 
      MessageInterface::ShowMessage("The number of sensor cone and clock angles must be the same.\n");
      exit(1);
   }

   #ifdef DEBUG_CHK_INPS
      MessageInterface::ShowMessage("Satellite states file path, name is: %s \n", satStateFp.c_str());
      MessageInterface::ShowMessage("Coverage grid file path is %s \n", covGridFp.c_str());
      MessageInterface::ShowMessage("Sensor type is %s \n", fovGeom.c_str());
      MessageInterface::ShowMessage("Sensor Orientation is %16.9f, %16.9f, %16.9f,%16.9f, %16.9f, %16.9f \n", senOrien[0], senOrien[1], senOrien[2], senOrien[3], senOrien[4], senOrien[5]);
      MessageInterface::ShowMessage("Sensor cone angle vector is: ");
      for(int i =0; i<fovCone.size(); i++){
         MessageInterface::ShowMessage(" %16.9f ", fovCone[i]);
      }
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("Sensor clock angle vector is: ");
      for(int i =0; i<fovClock.size(); i++){
         MessageInterface::ShowMessage(" %16.9f ", fovClock[i]);
      }
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("yaw180_flag is %d \n", yaw180_flag);
      MessageInterface::ShowMessage("Satellite access file path, name is: %s \n", satAccFp.c_str());
   #endif
   
   #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("**** About to read in Coverage grid ******\n");
   #endif
   /** Read in the coverage grid **/
   RealArray lats, lons;
   oci_utils::readCovGridFile(covGridFp, lats, lons);  
   PointGroup               *pGroup = new PointGroup();
   pGroup->AddUserDefinedPoints(lats, lons);
   Integer numGridPoints = lats.size();
   #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("**** Finished reading in Coverage grid ******\n");
   #endif

   #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("**** About to read satellite state file header ******\n");
   #endif
   // Read the epoch and satellite state at the epoch from input satellite state file
   Real epoch, duration, stepSize;
   Rvector6 state0;
   oci_utils::readSatStateFileHeader(satStateFp,  epoch,  stepSize, duration, state0);
   #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("**** Finished reading satellite state file header ******\n");
   #endif

   // Set the global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   // Check the OS (note that this does not work correctly for Mac)
   char *buffer = NULL;
   buffer = getenv("OS");
   if (buffer  != NULL)
   {
      MessageInterface::ShowMessage("Current OS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }
   
   /// ******** Begin setting up the test
   MessageInterface::ShowMessage("*** START TEST ***\n");
  
   try
   {      
      // These are the objects needed
      AbsoluteDate             *date;
      OrbitState               *state;
      ConicalSensor            *conicalSensor;
      Spacecraft               *sat1;
      Propagator               *prop;
      CoverageChecker          *covChecker;
	  Earth                    *earth;
      CustomSensor             *customSensor;
      NadirPointingAttitude    *attitude;
      LagrangeInterpolator     *interp = new LagrangeInterpolator( // Not used really.
                                             "TATCLagrangeInterpolator", 6, 7);

      #ifdef COMPUTE_AND_STORE_POI_GEOMETRY
         // Create the container to hold the coverage events
         std::vector<IntervalEventReport> coverageEvents;
      #endif      
      
      clock_t t0 = clock(); // for timing

      // Create an Earth model
      earth = new Earth();
      
      // Create the epoch object and set the initial epoch
      date = new AbsoluteDate();
      date->SetJulianDate(epoch);
      
      // Create the spacecraft state object and set Keplerian elements
      state = new OrbitState();
      state->SetCartesianState(state0);
      
      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("**** date and state OK "
                                       "**************\n");
      #endif            
               
      // Create a spacecraft giving it a state and epoch
      attitude = new NadirPointingAttitude();

      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage(
                           "*** About to create Spacecraft!!!!\n");
      #endif
      sat1     = new Spacecraft(date, state, attitude, interp); //,0.0, 0.0, 180.0);
         
      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("*** DONE creating Spacecraft!!!!\n");
         MessageInterface::ShowMessage("**** attitude and sat1 OK "
                                       "**************\n");
      #endif
   
      MessageInterface::ShowMessage("*** About to add Sensors!!!!\n");
      // Add sensor to satellite
      if(fovGeom == "CONICAL"){
         conicalSensor = new ConicalSensor(fovCone[0]*RAD_PER_DEG);
         conicalSensor->SetSensorBodyOffsetAngles(senOrien[3], senOrien[4], senOrien[5], senOrien[0], senOrien[1], senOrien[2]); // careful: angle in degrees
         sat1->AddSensor(conicalSensor);
         #ifdef DEBUG_CONSISE
            MessageInterface::ShowMessage("*** CONICAL Sensor added.\n");
         #endif
      }else if(fovGeom == "RECTANGULAR" || fovGeom == "CUSTOM"){

         std::vector<double> senCone_r(fovCone.size()); 
         std::transform(fovCone.begin(), fovCone.end(), senCone_r.begin(),[](double i){ return i * RAD_PER_DEG; });
         std::vector<double> senClock_r(fovClock.size()); 
         std::transform(fovClock.begin(), fovClock.end(), senClock_r.begin(),[](double i){ return i * RAD_PER_DEG; });
         customSensor =  new CustomSensor(senCone_r, senClock_r);
         customSensor->SetSensorBodyOffsetAngles(senOrien[3], senOrien[4], senOrien[5], senOrien[0], senOrien[1], senOrien[2]); // careful: angle in degrees
         sat1->AddSensor(customSensor);
         #ifdef DEBUG_CONSISE
            MessageInterface::ShowMessage("*** RECTANGULAR/ CUSTOM Sensor added.\n");
         #endif
      }else{
         MessageInterface::ShowMessage("**** Warning no Sensor defined!! ****\n");
      }

      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("**** Creating and adding sensors OK **************\n");
      #endif
      
      // Create the propagator
      prop = new Propagator(sat1);
      
      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("*** DONE creating Propagator!!!!\n");
      #endif            
      
      // Initialize the coverage checker
      covChecker = new CoverageChecker(pGroup,sat1);
      
      #ifdef COMPUTE_AND_STORE_POI_GEOMETRY
         covChecker->SetComputePOIGeometryData(true); 
      #else
         covChecker->SetComputePOIGeometryData(false); 
      #endif    
      
      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("*** Coverage Checker created!!!!\n");
      #endif
      
      // Propagate for a duration and collect data
      Real           startDate   = date->GetJulianDate();
      IntegerArray   loopPoints;
      IntegerArray   loopPoints_yaw180;

      /** Write satellite states and access files **/
      const int prc = std::numeric_limits<double>::digits10 + 1; // set to maximum precision                    

      // Write the access file in matrix format with rows as the time and columns as ground-points. 
      // Each entry in a cell of the matrix corresponds to 0 (No Access) or 1 (Access).
      ofstream satAcc; 
      satAcc.open(satAccFp.c_str(),ios::binary | ios::out);
      satAcc << "Satellite states are in Earth-Centered-Inertial equatorial-plane frame.\n";
      satAcc << "Epoch[JDUT1] is "<< std::fixed << std::setprecision(prc) << startDate <<"\n";
      satAcc << "Step size [s] is "<< std::fixed << std::setprecision(prc) << stepSize <<"\n";
      satAcc << "Mission Duration [Days] is "<< duration << ".\n";
      satAcc << "TimeIndex,gpi" << "\n";

      ifstream satState(satStateFp.c_str());

      if(!satState){
          std::cerr << "Cannot open the Satellite State File : "<<satStateFp.c_str()<<std::endl;
          return -1;
      }

      string line;
      std::getline(satState,line); 
      std::getline(satState,line); 
      std::getline(satState,line);
      std::getline(satState,line);
      std::getline(satState,line);

      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage("Start looping through the states.\n");
      #endif
      int nSteps = 0;

      while(std::getline(satState,line)){

         Rvector6 _state;
         Real _date;
         RealArray e;
         int i=0;
         stringstream ss(line);
         while(ss.good()){
             string substr;
             std::getline(ss, substr, ',');
             if(i==0){
                 _date = epoch + stepSize*stoi(substr)*GmatTimeConstants::DAYS_PER_SEC;
             }
             else{
                 e.push_back(stod(substr));               
             }
             i++;
         }        
         _state.Set(e[0], e[1], e[2], e[3], e[4], e[5]);

         date->SetJulianDate(_date);
         state->SetCartesianState(_state);

         #ifdef COMPUTE_AND_STORE_POI_GEOMETRY
            loopPoints = covChecker->AccumulateCoverageData();
         #else
            loopPoints = covChecker->CheckPointCoverage();
         #endif

         if(yaw180_flag == true){
            // Rotate satellite around z-axis by 180 deg and calculate coverage
            sat1->SetBodyNadirOffsetAngles(0,0,180,1,2,3);
            #ifdef COMPUTE_AND_STORE_POI_GEOMETRY
               loopPoints_yaw180 = covChecker->AccumulateCoverageDataAtPreviousTimeIndex();
            #else
               loopPoints_yaw180 = covChecker->CheckPointCoverage();
            #endif

            sat1->SetBodyNadirOffsetAngles(0,0,0,1,2,3); // Reset the satellite attitude to Nadir-pointing
            // Add the points to the list of points seen. Sort and remove possible duplicates (in case of overlap)
            loopPoints.insert( loopPoints.end(), loopPoints_yaw180.begin(), loopPoints_yaw180.end() );
            // remove duplicates
            sort( loopPoints.begin(), loopPoints.end() );
            loopPoints.erase( unique( loopPoints.begin(), loopPoints.end() ), loopPoints.end() );
         }
        
         // Write access data         
         // Make array with '1' (Access) in the cells corresponding to indices of gp's accessed
         // and nothing with there is no access.
         if(loopPoints.size()>0){
            // If no ground-points are accessed at this time, skip writing the row altogether.
            IntegerArray accessRow(numGridPoints,0);
            for(int j = 0; j<loopPoints.size();j++){
               satAcc << std::setprecision(prc) << nSteps << "," << loopPoints[j] << "\n";
            }
         }        
         nSteps++; 
      }
      
      satAcc.close(); 
      satState.close();

      //Delete un-needed objects
      delete    covChecker;
      delete    prop;
      delete    date;
      delete    state;
      delete    attitude;
      // delete sat1; ??
      
      if(fovGeom == "Conical"){
         delete    conicalSensor;
      }else if(fovGeom == "Custom" || fovGeom=="Rectangular"){
         delete    customSensor;
      }      
      delete    earth;

      #ifdef DEBUG_CONSISE
         MessageInterface::ShowMessage(" --- Done deleting old pointers\n");
      #endif

      // check timing
      Real timeSpent = ((Real) (clock() - t0)) / CLOCKS_PER_SEC;
      MessageInterface::ShowMessage("TIME SPENT is %12.10f seconds\n",timeSpent);
      
      MessageInterface::ShowMessage("*** END ***\n");
   
   
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n",
                                    be.GetFullMessage().c_str());
   }
   
}
