import matplotlib.pyplot as plt
import pandas as pd
import os
from matplotlib.animation import FuncAnimation

from set_rcParams import set_rcParams
set_rcParams()

# Define the directory containing the CSV files
# csv_dir = './csv_files'  # Replace with your directory

# Get a sorted list of CSV files
csv_files = [f"perturbation_init_out_rays_{i:04d}.csv" for i in range(1, 1002, 1)]

# Prepare the figure
fig, ax = plt.subplots()

def animate(frame):
    # Clear the axis
    ax.clear()
    
    # Load the data for the current frame
    # csv_file = os.path.join(csv_dir, csv_files[frame])
    data = pd.read_csv(csv_files[frame])
    
    # Assuming the CSV has two columns: 'x' and 'y'
    scatter = ax.scatter(data['x'], data['v_x'],s=100, edgecolor='k', alpha=0.7)
    # plt.colorbar(scatter, label="$v_x$ (m/s)")
    # ax.plot(data['x'], data['y'], label=f"Frame {frame+1}")
    
    # Add title and labels
    ax.set_title(f"Simulation Time = {0.1 * (frame + 1):0.1f} (s)")
    ax.set_xlabel("$x$ (m)")
    ax.set_ylabel("$v_x$ (m/s)")
    ax.set_ylim(top=4, bottom=-4)

# Create the animation
ani = FuncAnimation(fig, animate, frames=len(csv_files), interval=100)

# Show the animation
# plt.show()
ani.save('two-stream.mp4', fps=30)
