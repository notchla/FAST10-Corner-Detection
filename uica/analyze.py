import subprocess
import sys

if len(sys.argv) <= 1:
    print(f"usage: {sys.argv[0]} <file1> ...")
    exit(1)



def cmd(command):
    out = subprocess.run(command, shell=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT).stdout
    return out

for name in sys.argv[1:]:
    print(name)
    txt = open(f"{name}.asm").readlines()
    
    counter = 0
    for i, l in enumerate(txt):
        if "je" in l or "jmp" in l:
            open(f"{name}_{counter}.asm", "w").write("".join(txt[0:i+1]))
            cmd(f"as {name}_{counter}.asm -o {name}_{counter}.o")
            info = cmd(f"../uiCA/uiCA.py {name}_{counter}.o -arch TGL -TPonly")
            cmd(f"mkdir {name}")
            cmd(f"../uiCA/uiCA.py {name}_{counter}.o -arch TGL > {name}/{counter}.txt")
            cmd(f"rm {name}_{counter}.asm {name}_{counter}.o")
            
            print(f"\t{counter}: {info}", end="");
            counter += 1

