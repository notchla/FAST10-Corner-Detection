#!/usr/bin/env python3

import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, NullLocator
import numpy as np
import os
import sys


# Basic Plotting Utilities
def plot_init_settings():
    plt.rcParams['font.family'] = 'DejaVu Sans'
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.linewidth'] = 2

def line_plot(xs, ys, names, xlabel, ylabel, title, colors, loc = None, x_log=False):
    sns.set_theme()
    pal = sns.color_palette()

    plt.title(title)
    
    if colors:
        for x, y, name, col in zip(xs, ys, names, colors):
            plt.plot(x, y, marker='^', linestyle='dashed', label=name, color=pal[col])
    else:
        for x, y, name in zip(xs, ys, names):
            plt.plot(x, y, marker='^', linestyle='dashed', label=name)

    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend(loc=loc)
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

data_dir = f"../output/{sys.argv[1]}"


names = [ f[:-14] for f in os.listdir(data_dir) if "cycles" in f]
threshold = 25

xs = []
ys = []
ns = []
colors = []
for n in names:
    lw = 1
    color = 0
    if "sse" in n:
        lw = 16
        color = 1
    elif "avx2" in n:
        lw = 32
        color = 2
    elif "avx512" in n:
        lw = 64
        color = 3

    prefix = os.path.join(data_dir, f"{n}_{threshold}_")
    cycles = [ int(l) for l in open(prefix + f"cycles.dat").readlines() ]
    sizes = []
    counts = []

    for l in open(prefix + "count.dat").readlines():
        nums = [int(x) for x in l.split()]
        counts.append(nums[2:])
        sizes.append((nums[0] - 6))# * (nums[1] - 6))


    if sys.argv[1] == "avx512" and n == "avx512":
        n = "avx512 optimized"

    if sys.argv[1] == "simdblocking":
        n = " ".join(n.split("_"))
        if "gather" in n:
            n = "avx2 - gather: 4x8"
        elif len(n.split()) > 1:
            a, b = n.split()
            n = f"{a} - load + insert: {b}"

    if "_set" in n:
        ns.append("avx512 base")
        color = 4
    else:
        ns.append(n)
    
    
    xs.append(sizes)
    ys.append([ (s[0] * lw) / cy for cy, s in zip(cycles, counts)])
    colors.append(color)

    if n.startswith("avx512"):
        xs[-1] = xs[-1][1:]
        ys[-1] = ys[-1][1:]

loc = (0, 0.1)
if sys.argv[1] == "simdblocking":
    colors = None
    loc = None
line_plot(xs, ys, ns, "n", "pixels per cycle", f"Fast 10 n x n image (i5 11400H Tigerlake, warm cache, clang-cl 13.0.1 -O2)", colors, loc)

