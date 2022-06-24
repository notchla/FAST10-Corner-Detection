import os

def get_algos(uica_dir):
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
        
        'avx2_16x2': {
            'ops': [],
            'vector_size': 32,
            'relevant_ports': [0, 1],
        },

        'avx512': {
            'ops': [],
            'vector_size': 64,
            'relevant_ports': [0, 5],
        },

        'avx512_16x4': {
            'ops': [],
            'vector_size': 64,
            'relevant_ports': [0, 5],
        },
    }

    for k, v in algos.items():
        for i in range(9):
            for l in open(os.path.join(uica_dir, k.split("_")[0], f"{i}.txt"), encoding="utf-8").readlines():
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
    
    return algos