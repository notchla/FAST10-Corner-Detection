import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import sys

datasets = [
    "box",
#    "junk",
#    "maze",
]


fig, axs = plt.subplots(len(datasets))
axs =[ axs ]
fig.set_size_inches(12, 8)

block_sizes = [
    (1, 1), 
    (16, 1), 
    (8, 2), 
    (4, 4), 
    (32, 1),
    (16, 2),
    (8, 4),
    #(4, 8),
    (64, 1),
    (32, 2),
    (16, 4),
    (8, 8),
]

for index, name in enumerate(datasets):
    data = open(f"../output/{name}_checks.dat").readlines()
    number_of_images = int(data[0])
    data = data[1:]

    total_counts = np.zeros((10, len(block_sizes)), np.float64)

    for _ in range(number_of_images):
        w, h = [int(x) for x in data[0].split()]

        checks = np.array([ [ int(v) for v in l.split() ] for l in data[1:h+1]]);

        for block_index, (bw, bh) in enumerate(block_sizes):
            xstart = max(3, bw)
            xend = (w - 3) - (w - 3) % bw
            assert (xend - xstart) % bw == 0

            ystart = max(3, 4 if bh == 2 else bh)
            yend = (h - 3) - (h - 3) % bh
            assert (yend - ystart) % bh == 0

            checks = checks[xstart:xend, ystart:yend]

            if bw * bh == 1:
                cmp = checks
            else:
                cmp = np.full((checks.shape[0] // bw, checks.shape[1] // bh), 0, np.int32)
                for j in range(cmp.shape[1]):
                    for i in range(cmp.shape[0]):
                        cmp[i, j] = checks[i * bw: (i + 1) * bw, j * bh: (j + 1) * bh].max()

            total_blocks = cmp.shape[0] * cmp.shape[1]
            for i in range(0, 10):
                total_counts[i, block_index] += (cmp >= i).sum() / total_blocks


        data = data[2 + h:]

    data = total_counts / number_of_images

    # for i, (bw, bh) in enumerate(block_sizes):
    #     avg_checks = np.arange(1, 11).dot(data[:, i])
    #     print(f"{bw}x{bh} = {avg_checks}")
        
    data = data[1:-1,:]

    if False:
        ax = axs[index]
        ax.bar(range(len(data)), data)
        ax.set_title(name)
        ax.set_ylim(0, 0.15)
    else:
        labels = [f"{x}x{y}" for x, y in block_sizes]
        x = np.arange(len(labels))   # the label locations
        count = data.shape[0]
        width = 1 / (count + 2)     # the width of the bars

        ax = axs[index]
        for i in range(count):
            offset = (i - count / 2) * width
            rects = ax.bar(x + offset, data[i,:] * 100, width, label=f"checks = {i}")

        ax.set_xticks(x, labels=labels)
        ax.grid(axis="y")

        ax.set_xlabel("8 checks grouped by block size")
        ax.set_ylabel("Percentage of blocks")

plt.show()




