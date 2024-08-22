# satplan

1. Follow the installation instructions for instrupy and orbitpy (in that order). These are included as submodules. In this process you will create a conda virtual environment which you will keep for this visualizer.
2. Run the following commands (in the satellite_plan repo) to setup the environment.
   1. chmod +x config.sh
   2. ./config.sh
3. You can run "python src/full_mission.py" which should work out of the box if the install is correct. Let me know if there are any issues with it. You can modify the scenario settings in full_mission.py and in create_mission.py.
4. (optional) If desired, you can run the visualizer in steps (create, execute, process, plan, plot) by calling them individually with the desired settings.
5. Studies for the paper "Decentralized Reactive Satellite Planning for Event Observation" are available in the studies folder. Code for the multi-agent reinforcement learning experiments are avilable in src/multiagent_rl.
   ![Exciting gif](https://github.com/bgorr/satplan/blob/main/example.gif?raw=true)
