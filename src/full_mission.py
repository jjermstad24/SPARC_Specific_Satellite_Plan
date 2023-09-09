import datetime
import os

from create_mission import create_mission
from execute_mission import execute_mission
from process_mission import process_mission
from plan_mission import plan_mission
from plot_mission_cartopy import plot_mission

def main():
    settings = {
        "directory": "./missions/test_mission_6/",
        "step_size": 1,
        "duration": 1,
        "initial_datetime": datetime.datetime(2020,1,1,0,0,0),
        "grid_type": "static", # can be "event" or "static"
        "preplanned_observations": "./missions/test_mission_6/planner_outputs/accesses_2h_rew_5sat_sol_2degs.csv",
        "event_csvs": [],
        "plot_clouds": False,
        "plot_rain": False,
        "plot_obs": True
    }
    if not os.path.exists(settings["directory"]):
        os.mkdir(settings["directory"])
    if not os.path.exists(settings["directory"]+'orbit_data/'):
        os.mkdir(settings["directory"]+'orbit_data/')
    create_mission(settings)
    execute_mission(settings)
    if settings["preplanned_observations"] is None:
        plan_mission(settings) # must come before process as process expects a plan.csv in the orbit_data directory
    process_mission(settings)
    plot_mission(settings)


if __name__ == "__main__":
    main()