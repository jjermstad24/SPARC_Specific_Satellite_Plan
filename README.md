# SPARC_Specific_Satellite_Plan

1. Make sure there are no other instances of instrupy or orbitpy. (delete previous environments)
2. Run the following commands (in the satellite_plan repo) to setup the environment.
   1. conda create -n satplan_test python=3.8
   2. conda activate satplan_test
   3. ./config.sh
3. You can run "python src/full_mission.py" which should work out of the box if the install is correct. Let me know if there are any issues with it. You can modify the scenario settings in full_mission.py and in create_mission.py.
