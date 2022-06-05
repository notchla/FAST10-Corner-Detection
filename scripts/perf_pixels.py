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

    ax = plt.gca()
    if x_log:
        plt.xscale('log')
        ticks = np.arange(4, 24, 2)
        ax.set_xticks(2 ** ticks)
        ax.xaxis.set_major_locator(FixedLocator(2**ticks))
        ax.xaxis.set_minor_locator(NullLocator())
        ax.set_xticklabels([f"$2^{{{j:.0f}}}$" for j in ticks])


    ax.set_ylim(bottom=0)
    plt.show()

data_dir = "../output/vectorization"


names = [ f[:-14] for f in os.listdir(data_dir) if "cycles" in f]
threshold = 25

xs = []
ys = []
ns = []

for n in names:

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

    for l in open(prefix + "count.dat").readlines():
        nums = [int(x) for x in l.split()]
        counts.append(nums[2:])
        sizes.append((nums[0] - 6))# * (nums[1] - 6))
    
    xs.append(sizes)
    ys.append([ (s[0] * lw) / cy for cy, s in zip(cycles, counts)])
    ns.append(f"{n}")


    if "avx512" in n:
        xs[-1] = xs[-1][1:]
        ys[-1] = ys[-1][1:]

line_plot(xs, ys, ns, "n", "pixels per cycle", f"Fast 10 n x n image (i5 11400H Tigerlake, clang-cl 13.0.1 -O2 -arch:AVX512)")

