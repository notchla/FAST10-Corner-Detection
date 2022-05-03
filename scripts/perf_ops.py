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
names = names[1:]
thresh = 30


data = []
sizes = list(range(128, 8192 + 1, 128))

loads = [0] * 9
loads[0] = 1

simd_ops = [0] * 9
simd_ops[0] = 2

integer_ops = [
    1,
    3,
    6,
    8,
    17,
    23,
    28,
    38,
    18,
]



for i in range(8):
    loads[i] += 2
    simd_ops[i] += 6 * 2
    integer_ops[i] += 3 * 2
    
labels = []
for t in range(10, 100, 20):
    cycles = []
    count = []
    for i in sizes:
        prefix = f"{data_dir}sse2_{i}x{i}_"
        cur_cycles = { int(l.split()[0]): float(l.split()[1]) for l in open(prefix + "cycles.dat").readlines() }
        cur_count = { int(l.split()[0]): [int(v) for v in l.split()[1:]] for l in open(prefix + "count.dat").readlines() }
        cycles.append(cur_cycles[t])
        count.append(cur_count[t])

    total_simd_ops = np.array([np.dot(np.array(simd_ops), np.array(c)) for c in count])
    total_integer_ops = np.array([np.dot(np.array(integer_ops), np.array(c)) for c in count])
    total_ops = total_simd_ops + total_integer_ops
    
    total_loads = np.array([np.dot(np.array(loads), np.array(c)) for c in count])
    total_bytes = total_loads * 16

    total_checks =  [np.array(c[:-1]).sum() for c in count]

    data.append(total_ops / np.array(cycles))
    labels.append(f"t = {t}")

line_plot([i for i in range(128, 8192 + 1, 128)], data, labels, "n", "ops per cycle", f"Fast 9 n x n image (msvc 19.31 -O2 -Ob2)")

