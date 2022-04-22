import matplotlib.pyplot as plt
import numpy as np
import os

os.chdir("../output")

fig, axs = plt.subplots(len(os.listdir(".")))
fig.set_size_inches(18, 10)

for index, path in enumerate(os.listdir(".")):
    name = path[:-4]

    data = []
    labels = []
    for i, l in enumerate(open(path).readlines()):
        if i == 0: 
            continue
        labels.append(l.split()[0])
        data.append([float(x) for x in l.split()[2:]])

    data = np.array(data).transpose()
    x = np.arange(len(labels))   # the label locations
    count = data.shape[0]
    width = 1 / (count + 2)     # the width of the bars

    ax = axs[index]
    for i in range(count):
        offset = (i - count / 2) * width
        rects = ax.bar(x + offset, data[i,:], width, label=f"checks = {i}")

    ax.set_xlabel('threshold')
    ax.set_ylabel('proportion')
    ax.set_title(name)
    ax.set_xticks(x, labels=labels)
    ax.grid(axis="y")

#fig.tight_layout()
plt.show()
