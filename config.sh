git submodule init
git submodule update
conda create --name satplan_test python=3.8
conda activate satplan_test
cd instrupy
make cd ..
cd orbitpy
make cd ..
pip install tqdm
pip install pyscipopt
pip install opencv-python 
pip install Basemap
pip install imageio
pip install h5py
pip install cartopy