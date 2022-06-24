#!/usr/bin/env python3

import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, NullLocator
import numpy as np
import os

# Basic Plotting Utilities
def plot_init_settings():
    plt.rcParams['font.family'] = 'DejaVu Sans'
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.linewidth'] = 2

def line_plot(xs, ys, names, xlabel, ylabel, title, colors, x_log=False):
    sns.set_theme()
    plt.title(title)

    cols = sns.color_palette()

    for (x, y, name, c) in zip(xs, ys, names, colors):
        plt.plot(x, y, marker='^', linestyle='dashed', label=name, color=cols[c])
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)

    ax = plt.gca()
    ax.legend(loc='lower right')
    plt.tight_layout()

    if x_log:
        plt.xscale('log')
        ticks = np.arange(4, 24, 2)
        ax.set_xticks(2 ** ticks)
        ax.xaxis.set_major_locator(FixedLocator(2**ticks))
        ax.xaxis.set_minor_locator(NullLocator())
        ax.set_xticklabels([f"$2^{{{j:.0f}}}$" for j in ticks])

    plt.show()

#plot_init_settings()

data_dir = "../output/peeling"


#names = [ "avx512", "avx512_bc" ]

names = [ 
    "scalar",
    "avx2_vecpeeling",
    "avx2_16x2",
    "avx512",
    "avx512_16x4",
    "libcvd_if",
    "libcvd_sse2",
]

cute_names = [ 
    "scalar",
    "avx2",
    "avx2 - blocked: 16x2",
    "avx512",
    "avx512 - blocked: 16x4",
    "libcvd - nested ifs",
    "libcvd - sse2",
]

colors = [
    0,
    2,
    5,
    3,
    4,
    6,
    7,
]

# names = names[1:]
# lane_width = lane_width[1:]

threshold = 25
#names = names[1:]

xs = []
ys = []
ns = []

for cn, n in zip(cute_names, names):
    lw = 1
    if "sse" in n:
        lw = 16
    elif "avx2" in n:
        lw = 32
    elif "avx512" in n:
        lw = 64

    prefix = os.path.join(data_dir, f"{n}_{threshold}_")
    cycles = [ int(l) for l in open(prefix + f"cycles.dat").readlines() ]
    sizes = []
    counts = []

    if False:
        for l in open(prefix + "count.dat").readlines():
            nums = [int(x) for x in l.split()]
            counts.append(nums[2:])
            sizes.append((nums[0] - 6))# * (nums[1] - 6))
        
        xs.append(sizes)
        #ys.append([ cy / s for cy, s in zip(cycles, sizes)])
        #ys.append([ s[0] * ((16 if "sse2" in n else 32) if not "512" in n else 64) / cy for cy, s in zip(cycles, counts)])
        ys.append([ (s[0] * lw) / cy for cy, s in zip(cycles, counts)])
        #ys.append(np.array([ cy for cy, s in zip(cycles, counts)]))
        #ys.append([ s / cy for cy, s in zip(cycles, sizes)])
        ns.append(f"{n}")
    else:
        sizes = np.arange(128, 128 * len(cycles) + 1, 128)
        xs.append(sizes)
        print(sizes)
        #ys.append([ cy / s for cy, s in zip(cycles, sizes)])
        #ys.append([ s[0] * ((16 if "sse2" in n else 32) if not "512" in n else 64) / cy for cy, s in zip(cycles, counts)])
        y = []
        for cy, s in zip(cycles,sizes):
            target_size = 128 * 1024 * 1024;
            width = s
            height = s + 6
            count = max(1, (target_size + (width * height - 1)) // (width * height));
            y.append(((s - 6) * s) / cy * count)

        ys.append(y)

    if n == "avx2_vecpeeling":
        n = "avx2"
    ns.append(cn)
    

        #if "avx512" in n:
        #    xs[-1] = xs[-1][1:]
        #    ys[-1] = ys[-1][1:]

line_plot(xs, ys, ns, "n", "pixels per cycle", f"Fast 10 n x n image (i5 11400H Tigerlake, clang-cl 13.0.1 -O2)", colors)

