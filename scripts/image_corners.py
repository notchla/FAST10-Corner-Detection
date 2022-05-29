import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np

image_filename = "../data/box0_big.png"
data_filename = "../output/avx2_10_25_checkposition.dat"

img = mpimg.imread(image_filename)
img_rgb = np.stack((img,)*3, axis=-1)

with open(data_filename) as f:
    lines = f.readlines()
    info = [int(x) for x in lines[0].split()]
    width = info[0]
    height = info[1]
    num_corners = info[2]
    
    colors = np.zeros((height,width,3))
    corners_x = []
    corners_y = []
    
    for i in range(1 + num_corners, len(lines)):
        lane = [int(x) for x in lines[i].split()]
        y = lane[0]
        x = lane[1]
        length = lane[2]
        checks = lane[3]
        col = [checks / 8, 1 - checks / 8, 0]
        for j in range(x, x + length):
            colors[y, j, :] = col
    
    for i in range(1, 1 + num_corners):
        corner = [int(x) for x in lines[i].split()]
        #colors[corner[0], corner[1], :] = [0, 0, 1]
        corners_y.append(corner[0])
        corners_x.append(corner[1])

result = 0.5 * img_rgb[:height, :width,:] + 0.5 * colors
imgplot = plt.imshow(result)
plt.plot(corners_x, corners_y, '*', color='yellow', markersize=1)
plt.show()