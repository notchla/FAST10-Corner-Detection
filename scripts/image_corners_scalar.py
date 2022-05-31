import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import sys

image_filename = "../data/box0_big.png"
data_filename = "../output/scalar_10_25_checkposition.dat"

img = mpimg.imread(image_filename)
img_rgb = np.stack((img,)*3, axis=-1)

lane_len = int(sys.argv[1])

with open(data_filename) as f:
    lines = f.readlines()
    info = [int(x) for x in lines[0].split()]
    width = info[0]
    height = info[1]
    num_corners = info[2]
    
    colors = np.zeros((height,width,3))
    corners_x = []
    corners_y = []
    
    for i in range(1, 1 + num_corners):
        corner = [int(x) for x in lines[i].split()]
        #colors[corner[0], corner[1], :] = [0, 0, 1]
        corners_y.append(corner[0])
        corners_x.append(corner[1])
        
    # Start positions for lanes in a row
    first = lane_len if lane_len > 3 else 3
    lane_start = [x for x in range(first, width - 3, lane_len)]
    if lane_start[0] > 3:
        lane_start.insert(0, 3)
    lane_start.append(width - 3)
    
    for i in range(3, height - 3):
        row = [int(x) for x in lines[1 + num_corners + i].split()]
        assert len(row) == width
        
        for k in range(len(lane_start) - 1):
            lane_max = max(row[lane_start[k]:lane_start[k+1]])
            col = [lane_max / 9, 1 - lane_max / 9, 0]
            for j in range(lane_start[k], lane_start[k+1]):
                colors[i, j, :] = col

result = 0.5 * img_rgb[:height, :width,:] + 0.5 * colors
imgplot = plt.imshow(result)
plt.plot(corners_x, corners_y, '*', color='yellow', markersize=1)
plt.show()