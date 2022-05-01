import png
import numpy as np
import os
import sys

os.chdir("../data")

for f_name in os.listdir("."):
    if os.path.isdir(f_name):
        for name in os.listdir(f_name + "/frames"):
            r = png.Reader(f_name + "/frames/" + name).asDirect()
            image_np = np.vstack(map(np.uint8, r[2]))
            image_2times = np.concatenate((image_np[:,::3], np.flip(image_np[:,::3], axis=1)), axis=1)
            image_4times = np.concatenate((image_2times, np.flip(image_2times, axis=0)), axis=0)
            new_img = np.tile(image_4times, (int(sys.argv[1]), int(sys.argv[2])))
            
            filename = "../data2/" + f_name + "/frames/" + name
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            with open(filename, "wb") as f:
                w = png.Writer(width=new_img.shape[1], height=new_img.shape[0], greyscale=True)
                w.write(f, new_img)
                f.close()
                print("Created image " + filename)
            