import matplotlib.pyplot as plt
from matplotlib.ticker import FixedLocator, NullLocator
import numpy as np
import os
import seaborn as sns

import ops

sns.set_theme()

uica_dir = "../uica"
out_dir = "../output/roofline"


algos = ops.get_algos(uica_dir)
for k, v in algos.items():
    print(k, v)

#  scalar [ 22, 16, 18, 20, 14, 18, 17, 20, 3, ]

pi_scalar =  1 * 4
pi_sse2   = 16 * 2
pi_avx2   = 32 * 2
pi_avx512 = 64 * 2

beta = (13 * 1024 * 1024 * 1024) / 2.7e9

left_exp = 1
left = 2 ** left_exp
right_exp = 7
right = 2 ** right_exp

bottom_exp = -1
bottom = 2 ** bottom_exp
top_exp = 10
top = 2 ** top_exp

plt.plot([0, right], [pi_scalar, pi_scalar])
plt.plot([0, right], [pi_sse2,   pi_sse2  ])
plt.plot([0, right], [pi_avx2,   pi_avx2  ])
plt.plot([0, right], [pi_avx512, pi_avx512])
plt.plot([0, right], [0, beta * right])

plt.text(2 ** (right_exp - 1), pi_scalar * 1.1, "scalar", color="blue")
plt.text(2 ** (right_exp - 1), pi_sse2   * 1.1, "SSE2", color="orange")
plt.text(2 ** (right_exp - 1), pi_avx2   * 1.1, "AVX2", color="green")
plt.text(2 ** (right_exp - 1), pi_avx512 * 1.1, "AVX-512", color="red")

plt.text(2 ** (right_exp - 2.5), pi_avx512 * 3, "Bandwidth 13GB/s", color="#8476b5")

def compute_roofline_position(name, ops, vector_size, size):
    counts = np.array([int(v) for v in open(os.path.join(out_dir, f"{name}_25_count_roofline_{size}.dat")).read().split()[2:-1]],
            np.uint64)
    cycles = int(open(os.path.join(out_dir, f"{name}_25_cycles_roofline_{size}.dat")).read())

    w = counts.dot(ops) * vector_size
    q = counts[0] * vector_size

    peak = 4
    if "sse2" in name:
        peak = 32
    elif "avx2" in name:
        peak = 64
    elif "avx512" in name:
        peak = 128
    
    peak_cycles = w / peak
    peak_pixels_cy = q / peak_cycles
    pixels_cy = q / cycles
    print(f"Pixels/cycle ({name} {size}x{size}): {peak_pixels_cy}, measured: {pixels_cy}, percentage {pixels_cy / peak_pixels_cy * 100 : .2f}%")

    return w / q, w / cycles

colors = [ "blue", "orange", "green", "green", "red", "red" ]
markers = [
    [ "x" ] * len(colors),
    [ "s" ] * len(colors),
]

markers[0][3] = "+"
markers[1][3] = "o"

markers[0][5] = "+"
markers[1][5] = "o"

for i, (k, v) in enumerate(algos.items()):
    for j, size in enumerate([256, 8192]):
        c = colors[i]
        m = markers[j][i]

        x, y = compute_roofline_position(k, v["ops"], v["vector_size"], size)
        #print(f"{k}: {x}, {y}")

        name_and_blocksize = k.split('_')
        label = f'{name_and_blocksize[0]} - {"small" if size == 256 else "large"}'
        if len(name_and_blocksize) > 1:
            label += f" - blocked: {name_and_blocksize[-1]}"

        plt.plot(x, y, color=c, marker=m, linestyle="none", label=label)

plt.legend(loc="lower left")

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

plt.title('Core i5 11400H Tigerlake, single core, warm cache')


plt.tight_layout()
plt.show()
