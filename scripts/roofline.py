import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, NullLocator
import numpy as np
import os

uica_dir = "../uica"
out_dir = "../output"

algos = {
    'scalar': {
        'ops': [],
        'vector_size': 1,
        'relevant_ports': [0, 1, 5, 6],
    },

    'sse2': {
        'ops': [],
        'vector_size': 16,
        'relevant_ports': [0, 1],
    },

    'avx2': {
        'ops': [],
        'vector_size': 32,
        'relevant_ports': [0, 1],
    },
}

for k, v in algos.items():
    for i in range(9):
        for l in open(os.path.join(uica_dir, k, f"{i}.txt"), encoding="utf-8").readlines():
            if "Total" in l:
                vals = l.split("\u2502")[4]

                total = 0
                for j in v['relevant_ports']:
                    total += float(vals[j * 9 : j * 9 + 6])

                total = round(total)
                v['ops'].append(total)
                break
    for i in range(9):
        for j in range(i + 1, 9):
            v['ops'][j] -= v['ops'][i]
    print(k, v)

#  scalar [ 22, 16, 18, 20, 14, 18, 17, 20, 3, ]

pi_scalar =  1 * 4
pi_sse2   = 16 * 2
pi_avx2   = 32 * 2

beta = 32

left_exp = -2
left = 2 ** left_exp
right_exp = 8
right = 2 ** right_exp

bottom_exp = -1
bottom = 2 ** bottom_exp
top_exp = 8
top = 2 ** top_exp

plt.plot([0, right], [pi_scalar, pi_scalar], label = 'scalar')
plt.plot([0, right], [pi_sse2,   pi_sse2  ], label = 'sse2')
plt.plot([0, right], [pi_avx2,   pi_avx2  ], label = 'avx2')

plt.plot([0, right], [0, beta * right], label = 'bandwidth')

def compute_roofline_position(name, ops, vector_size):
    counts = np.array([int(v) for v in open(os.path.join(out_dir, f"{name}_10_25_count_8192.dat")).read().split()[2:-1]],
            np.uint64)
    cycles = int(open(os.path.join(out_dir, f"{name}_10_25_cycles_8192.dat")).read())

    w = counts.dot(ops) * vector_size
    q = counts[0] * vector_size

    print(name, w, q)

    return w / q, w / cycles

colors = [ "blue", "orange", "green" ]
markers = [ "s", "o", "^" ]

for (k, v), c, m in zip(algos.items(), colors, markers):
    x, y = compute_roofline_position(k, v["ops"], v["vector_size"])
    print(x, y)
    plt.plot(x, y, color=c, marker=m, linestyle="none", label=k)

plt.legend()

# X axis
plt.xlabel("Operational Intensity [byte ops/byte]")
plt.xscale('log')
ax = plt.gca()
ax.set_xlim(left=left, right=right)

ticks = np.arange(left_exp, right_exp, 1)
ax.set_xticks(2.0 ** ticks)
ax.xaxis.set_major_locator(FixedLocator(2.0**ticks))
ax.xaxis.set_minor_locator(NullLocator())
ticks_list = []
for j in ticks:
    if j >= 0:
        ticks_list.append(f"{2 ** j}")
    else:
        ticks_list.append(f"$\\frac{{1}}{{{2 ** -j}}}$")
ax.set_xticklabels(ticks_list)

# Y axis
plt.yscale('log')
plt.ylabel("Performance [byte ops/cycle]")
ax.set_ylim(bottom=bottom, top=top)

ticks = np.arange(max(0, bottom_exp), top_exp, 1)
ax.set_yticks(2 ** ticks)
ax.yaxis.set_major_locator(FixedLocator(2**ticks))
ax.yaxis.set_minor_locator(NullLocator())
ax.set_yticklabels([f"{2 ** j}" for j in ticks])


plt.tight_layout()
plt.show()
