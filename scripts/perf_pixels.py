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

def line_plot(xs, ys, names, xlabel, ylabel, title, x_log=False):
    sns.set_theme()
    plt.title(title)
    for (x, y, name) in zip(xs, ys, names):
        plt.plot(x, y, marker='^', linestyle='dashed', label=name)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend()
    plt.tight_layout()

    if x_log:
        plt.xscale('log')
        ax = plt.gca()
        ticks = np.arange(4, 24, 2)
        ax.set_xticks(2 ** ticks)
        ax.xaxis.set_major_locator(FixedLocator(2**ticks))
        ax.xaxis.set_minor_locator(NullLocator())
        ax.set_xticklabels([f"$2^{{{j:.0f}}}$" for j in ticks])


    plt.show()

#plot_init_settings()

data_dir = "../output/"

names = [ "scalar_10", "sse2_10", "avx2_10", "avx512_10"]
lane_width = [ 1, 16, 32, 64]
# names = names[1:]
# lane_width = lane_width[1:]

# names = [ "avx2", "avx2_blocking_1024", "avx2_blocking_512", "avx2_blocking_256"]#, "avx2_unrolled_2"]
# lane_width = [ 32, 32, 32, 32]
# 
# names = [ "avx2", "avx2_unrolled_2", "avx2_unrolled_3"]
# lane_width = [ 32, 32, 32]

compilers = [ "" ]

threshold = 25
#names = names[1:]

xs = []
ys = []
ns = []

for comp in compilers:
    for n, lw in zip(names, lane_width):
        prefix = os.path.join(data_dir, f"{n}_{threshold}_")
        cycles = [ int(l) for l in open(prefix + f"cycles{comp}.dat").readlines() ]
        sizes = []
        counts = []
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
        ns.append(f"{n} {comp}")

        if "avx512" in n:
            xs[-1] = xs[-1][1:]
            ys[-1] = ys[-1][1:]

line_plot(xs, ys, ns, "n", "pixels per cycle", f"Fast 10, t={threshold}  n x n image (clang 13.0.1 -O3 -march=native)")

