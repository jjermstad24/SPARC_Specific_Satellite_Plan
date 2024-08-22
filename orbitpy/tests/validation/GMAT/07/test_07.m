%General Mission Analysis Tool(GMAT) Script
%Created: 2020-07-07 09:08:47


%----------------------------------------
%---------- User-Modified Default Celestial Bodies
%----------------------------------------

GMAT Earth.EquatorialRadius = 6378.137;
GMAT Earth.Flattening = 0;

%----------------------------------------
%---------- Spacecraft
%----------------------------------------

Create Spacecraft Test_01_SC;
GMAT Test_01_SC.DateFormat = UTCGregorian;
GMAT Test_01_SC.Epoch = '26 May 2018 12:00:00.000';
GMAT Test_01_SC.CoordinateSystem = EarthMJ2000Eq;
GMAT Test_01_SC.DisplayStateType = Keplerian;
GMAT Test_01_SC.SMA = 6999.999999999995;
GMAT Test_01_SC.ECC = 1.345154894146495e-17;
GMAT Test_01_SC.INC = 170;
GMAT Test_01_SC.RAAN = 98.8797;
GMAT Test_01_SC.AOP = 0;
GMAT Test_01_SC.TA = 353.56989;
GMAT Test_01_SC.DryMass = 850;
GMAT Test_01_SC.Cd = 2.2;
GMAT Test_01_SC.Cr = 1.8;
GMAT Test_01_SC.DragArea = 15;
GMAT Test_01_SC.SRPArea = 1;
GMAT Test_01_SC.NAIFId = -10000001;
GMAT Test_01_SC.NAIFIdReferenceFrame = -9000001;
GMAT Test_01_SC.OrbitColor = Red;
GMAT Test_01_SC.TargetColor = Teal;
GMAT Test_01_SC.OrbitErrorCovariance = [ 1e+70 0 0 0 0 0 ; 0 1e+70 0 0 0 0 ; 0 0 1e+70 0 0 0 ; 0 0 0 1e+70 0 0 ; 0 0 0 0 1e+70 0 ; 0 0 0 0 0 1e+70 ];
GMAT Test_01_SC.CdSigma = 1e+70;
GMAT Test_01_SC.CrSigma = 1e+70;
GMAT Test_01_SC.Id = 'SatId';
GMAT Test_01_SC.Attitude = CoordinateSystemFixed;
GMAT Test_01_SC.SPADSRPScaleFactor = 1;
GMAT Test_01_SC.ModelFile = 'aura.3ds';
GMAT Test_01_SC.ModelOffsetX = 0;
GMAT Test_01_SC.ModelOffsetY = 0;
GMAT Test_01_SC.ModelOffsetZ = 0;
GMAT Test_01_SC.ModelRotationX = 0;
GMAT Test_01_SC.ModelRotationY = 0;
GMAT Test_01_SC.ModelRotationZ = 0;
GMAT Test_01_SC.ModelScale = 1;
GMAT Test_01_SC.AttitudeDisplayStateType = 'Quaternion';
GMAT Test_01_SC.AttitudeRateDisplayStateType = 'AngularVelocity';
GMAT Test_01_SC.AttitudeCoordinateSystem = EarthMJ2000Eq;
GMAT Test_01_SC.EulerAngleSequence = '321';












%----------------------------------------
%---------- ForceModels
%----------------------------------------

Create ForceModel OrbitPy_Propagator_J2_ForceModel;
GMAT OrbitPy_Propagator_J2_ForceModel.CentralBody = Earth;
GMAT OrbitPy_Propagator_J2_ForceModel.PrimaryBodies = {Earth};
GMAT OrbitPy_Propagator_J2_ForceModel.Drag = None;
GMAT OrbitPy_Propagator_J2_ForceModel.SRP = Off;
GMAT OrbitPy_Propagator_J2_ForceModel.RelativisticCorrection = Off;
GMAT OrbitPy_Propagator_J2_ForceModel.ErrorControl = LargestStep;
GMAT OrbitPy_Propagator_J2_ForceModel.GravityField.Earth.Degree = 2;
GMAT OrbitPy_Propagator_J2_ForceModel.GravityField.Earth.Order = 0;
GMAT OrbitPy_Propagator_J2_ForceModel.GravityField.Earth.StmLimit = 100;
GMAT OrbitPy_Propagator_J2_ForceModel.GravityField.Earth.PotentialFile = 'JGM2.cof';
GMAT OrbitPy_Propagator_J2_ForceModel.GravityField.Earth.TideModel = 'None';

%----------------------------------------
%---------- Propagators
%----------------------------------------

Create Propagator OrbitPy_Propagator_J2;
GMAT OrbitPy_Propagator_J2.FM = OrbitPy_Propagator_J2_ForceModel;
GMAT OrbitPy_Propagator_J2.Type = PrinceDormand78;
GMAT OrbitPy_Propagator_J2.InitialStepSize = 1;
GMAT OrbitPy_Propagator_J2.Accuracy = 9.999999999999999e-12;
GMAT OrbitPy_Propagator_J2.MinStep = 1;
GMAT OrbitPy_Propagator_J2.MaxStep = 1;
GMAT OrbitPy_Propagator_J2.MaxStepAttempts = 50;
GMAT OrbitPy_Propagator_J2.StopIfAccuracyIsViolated = true;

%----------------------------------------
%---------- Subscribers
%----------------------------------------

Create OrbitView DefaultOrbitView;
GMAT DefaultOrbitView.SolverIterations = Current;
GMAT DefaultOrbitView.UpperLeft = [ 0.2320075757575757 0.0703883495145631 ];
GMAT DefaultOrbitView.Size = [ 0.990530303030303 0.9247572815533981 ];
GMAT DefaultOrbitView.RelativeZOrder = 11;
GMAT DefaultOrbitView.Maximized = true;
GMAT DefaultOrbitView.Add = {Test_01_SC, Earth};
GMAT DefaultOrbitView.CoordinateSystem = EarthMJ2000Eq;
GMAT DefaultOrbitView.DrawObject = [ true true ];
GMAT DefaultOrbitView.DataCollectFrequency = 1;
GMAT DefaultOrbitView.UpdatePlotFrequency = 50;
GMAT DefaultOrbitView.NumPointsToRedraw = 0;
GMAT DefaultOrbitView.ShowPlot = true;
GMAT DefaultOrbitView.MaxPlotPoints = 20000;
GMAT DefaultOrbitView.ShowLabels = true;
GMAT DefaultOrbitView.ViewPointReference = Earth;
GMAT DefaultOrbitView.ViewPointVector = [ 30000 0 0 ];
GMAT DefaultOrbitView.ViewDirection = Earth;
GMAT DefaultOrbitView.ViewScaleFactor = 1;
GMAT DefaultOrbitView.ViewUpCoordinateSystem = EarthMJ2000Eq;
GMAT DefaultOrbitView.ViewUpAxis = Z;
GMAT DefaultOrbitView.EclipticPlane = Off;
GMAT DefaultOrbitView.XYPlane = On;
GMAT DefaultOrbitView.WireFrame = Off;
GMAT DefaultOrbitView.Axes = On;
GMAT DefaultOrbitView.Grid = Off;
GMAT DefaultOrbitView.SunLine = Off;
GMAT DefaultOrbitView.UseInitialView = On;
GMAT DefaultOrbitView.StarCount = 7000;
GMAT DefaultOrbitView.EnableStars = On;
GMAT DefaultOrbitView.EnableConstellations = On;

Create GroundTrackPlot DefaultGroundTrackPlot;
GMAT DefaultGroundTrackPlot.SolverIterations = Current;
GMAT DefaultGroundTrackPlot.UpperLeft = [ 0.2320075757575757 0.0703883495145631 ];
GMAT DefaultGroundTrackPlot.Size = [ 0.990530303030303 0.9247572815533981 ];
GMAT DefaultGroundTrackPlot.RelativeZOrder = 9;
GMAT DefaultGroundTrackPlot.Maximized = true;
GMAT DefaultGroundTrackPlot.Add = {Test_01_SC};
GMAT DefaultGroundTrackPlot.DataCollectFrequency = 1;
GMAT DefaultGroundTrackPlot.UpdatePlotFrequency = 50;
GMAT DefaultGroundTrackPlot.NumPointsToRedraw = 0;
GMAT DefaultGroundTrackPlot.ShowPlot = true;
GMAT DefaultGroundTrackPlot.MaxPlotPoints = 20000;
GMAT DefaultGroundTrackPlot.CentralBody = Earth;
GMAT DefaultGroundTrackPlot.TextureMap = 'ModifiedBlueMarble.jpg';

Create ReportFile EarthMJ2000Eq_States;
GMAT EarthMJ2000Eq_States.SolverIterations = Current;
GMAT EarthMJ2000Eq_States.UpperLeft = [ 0.3086734693877551 0.05225225225225225 ];
GMAT EarthMJ2000Eq_States.Size = [ 0.9948979591836735 0.9441441441441442 ];
GMAT EarthMJ2000Eq_States.RelativeZOrder = 45;
GMAT EarthMJ2000Eq_States.Maximized = true;
GMAT EarthMJ2000Eq_States.Filename = 'states.txt';
GMAT EarthMJ2000Eq_States.Precision = 17;
GMAT EarthMJ2000Eq_States.Add = {Test_01_SC.ElapsedSecs, Test_01_SC.EarthMJ2000Eq.X, Test_01_SC.EarthMJ2000Eq.Y, Test_01_SC.EarthMJ2000Eq.Z, Test_01_SC.EarthMJ2000Eq.VX, Test_01_SC.EarthMJ2000Eq.VY, Test_01_SC.EarthMJ2000Eq.VZ};
GMAT EarthMJ2000Eq_States.WriteHeaders = true;
GMAT EarthMJ2000Eq_States.LeftJustify = On;
GMAT EarthMJ2000Eq_States.ZeroFill = On;
GMAT EarthMJ2000Eq_States.FixedWidth = true;
GMAT EarthMJ2000Eq_States.Delimiter = ' ';
GMAT EarthMJ2000Eq_States.ColumnWidth = 23;
GMAT EarthMJ2000Eq_States.WriteReport = true;

Create ReportFile Keplerian_States;
GMAT Keplerian_States.SolverIterations = Current;
GMAT Keplerian_States.UpperLeft = [ 0 0 ];
GMAT Keplerian_States.Size = [ 0 0 ];
GMAT Keplerian_States.RelativeZOrder = 0;
GMAT Keplerian_States.Maximized = false;
GMAT Keplerian_States.Filename = 'kepler_states.txt';
GMAT Keplerian_States.Precision = 17;
GMAT Keplerian_States.Add = {Test_01_SC.ElapsedSecs, Test_01_SC.Earth.SMA, Test_01_SC.Earth.ECC, Test_01_SC.EarthMJ2000Eq.INC, Test_01_SC.EarthMJ2000Eq.RAAN, Test_01_SC.EarthMJ2000Eq.AOP, Test_01_SC.Earth.TA};
GMAT Keplerian_States.WriteHeaders = true;
GMAT Keplerian_States.LeftJustify = On;
GMAT Keplerian_States.ZeroFill = On;
GMAT Keplerian_States.FixedWidth = true;
GMAT Keplerian_States.Delimiter = ' ';
GMAT Keplerian_States.ColumnWidth = 23;
GMAT Keplerian_States.WriteReport = true;

Create ReportFile EarthFixed_States;
GMAT EarthFixed_States.SolverIterations = Current;
GMAT EarthFixed_States.UpperLeft = [ 0 0 ];
GMAT EarthFixed_States.Size = [ 0 0 ];
GMAT EarthFixed_States.RelativeZOrder = 0;
GMAT EarthFixed_States.Maximized = false;
GMAT EarthFixed_States.Filename = 'fixed_states.txt';
GMAT EarthFixed_States.Precision = 17;
GMAT EarthFixed_States.Add = {Test_01_SC.ElapsedSecs, Test_01_SC.EarthFixed.X, Test_01_SC.EarthFixed.Y, Test_01_SC.EarthFixed.Z, Test_01_SC.EarthFixed.VX, Test_01_SC.EarthFixed.VY, Test_01_SC.EarthFixed.VZ};
GMAT EarthFixed_States.WriteHeaders = true;
GMAT EarthFixed_States.LeftJustify = On;
GMAT EarthFixed_States.ZeroFill = On;
GMAT EarthFixed_States.FixedWidth = true;
GMAT EarthFixed_States.Delimiter = ' ';
GMAT EarthFixed_States.ColumnWidth = 23;
GMAT EarthFixed_States.WriteReport = true;


%----------------------------------------
%---------- Mission Sequence
%----------------------------------------

BeginMissionSequence;
Propagate 'Propagate_One_Day' OrbitPy_Propagator_J2(Test_01_SC) {Test_01_SC.ElapsedDays = 1};
