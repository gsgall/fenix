import matplotlib.pyplot as plt
import pandas as pd
import os
from matplotlib.animation import FuncAnimation

from set_rcParams import set_rcParams
set_rcParams()

# Define the directory containing the CSV files
# csv_dir = './csv_files'  # Replace with your directory

# Get a sorted list of CSV files
csv_files = [f"two-stream_out_rays_{i:04d}.csv" for i in range(1, 1500, 1)]

# Prepare the figure
fig, ax = plt.subplots()

def animate(frame):
    # Clear the axis
    ax.clear()

    # Load the data for the current frame
    # csv_file = os.path.join(csv_dir, csv_files[frame])
    data = pd.read_csv(csv_files[frame])

    # Assuming the CSV has two columns: 'x' and 'y'
    scatter = ax.scatter(data['x'], data['v_x'],c=data['v_x'],cmap='jet', s=100, edgecolor='k', alpha=0.7)

    # Add title and labels
    ax.set_title(f"Simulation Time = {0.1 * (frame + 1):0.1f} (s)")
    ax.set_xlabel("$x$ (m)")
    ax.set_ylabel("$v_x$ (m/s)")
    ax.set_ylim(top=4, bottom=-4)

# Create the animation
ani = FuncAnimation(fig, animate, frames=len(csv_files), interval=200)

# Show the animation
# plt.show()
ani.save('two-stream.mp4', fps=30)
