#!/usr/bin/env python3

import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, NullLocator
import numpy as np

# Basic Plotting Utilities
def plot_init_settings():
    plt.rcParams['font.family'] = 'DejaVu Sans'
    plt.rcParams['font.size'] = 12
    plt.rcParams['axes.linewidth'] = 2



def line_plot(x, ys, name, xlabel, ylabel, title, x_log=False):
    sns.set_theme()
    plt.title(title)
    for (y, name) in zip(ys, name):
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
names = [ "scalar", "sse2" ]
thresh = 30

data = []
sizes = list(range(128, 8192 + 1, 128))
    
for n in names:
    cycles = []
    count = []
    for i in sizes:
        prefix = f"{data_dir}{n}_{i}x{i}_"
        cur_cycles = { int(l.split()[0]): float(l.split()[1]) for l in open(prefix + "cycles.dat").readlines() }
        cur_count = { int(l.split()[0]): [int(v) for v in l.split()[1:]] for l in open(prefix + "count.dat").readlines() }
        cycles.append(cur_cycles[thresh])
        count.append(cur_count[thresh][0])

    arr_size = np.array(sizes)
    if n == "sse2":
        total_pixels = (arr_size - 32) * (arr_size - 6)
    else:
        total_pixels = np.square(arr_size - 6)

    data.append(total_pixels / np.array(cycles))

line_plot([i for i in range(128, 8192 + 1, 128)], data, names, "n", "pixels per cycle", f"Fast 9, t={thresh}  n x n image (msvc 19.31 -O2 -Ob2)")

