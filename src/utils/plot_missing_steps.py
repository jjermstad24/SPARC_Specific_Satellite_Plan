import time, calendar, datetime
from functools import partial
from mpl_toolkits.basemap import Basemap
import matplotlib.pyplot as plt
import urllib, os
import csv
import numpy as np
import imageio
import sys
import multiprocessing
import h5py
import matplotlib.colors as colors
from netCDF4 import Dataset
import cartopy.crs as ccrs
import cartopy.feature as cfeature
from cartopy.feature.nightshade import Nightshade
from multiprocessing import set_start_method
from src.plot_mission_cartopy import plot_step

def plot_missing(settings):
    if not os.path.exists(settings["directory"]+'plots/'):
        os.mkdir(settings["directory"]+'plots/')
    # PLOTS THE LAST 1/4th OF THE SIMULATION
    # imageio gif creation kills itself if there are too many images, is there a fix or is it just a WSL issue?
    start_frac = 0
    num_skip = 100
    steps = np.arange(int(np.floor(settings["duration"]*start_frac*86400/settings["step_size"])),int(np.floor(settings["duration"]*86400/settings["step_size"])),num_skip)
    for step in steps:
        if not os.path.exists(f'{settings["directory"]}plots/frame_{step}.png'):
            plot_step(step,settings)

if __name__ == "__main__":
    settings = {
        "directory": "./missions/test_mission_6/",
        "step_size": 1,
        "duration": 1,
        "plot_interval": 5,
        "plot_duration": 2/24,
        "initial_datetime": datetime.datetime(2020,1,1,0,0,0),
        "grid_type": "static", # can be "event" or "static"
        "preplanned_observations": "./missions/test_mission_6/planner_outputs/accesses_2h_rew_5sat_sol_2degs.csv",
        "event_csvs": [],
        "plot_clouds": False,
        "plot_rain": False,
        "plot_obs": True
    }
    plot_missing(settings)