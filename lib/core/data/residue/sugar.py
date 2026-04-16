from pathlib import Path

sugar = set()

for file in Path('.').iterdir():
    text = file.read_text()
    mol_list = text.split("#")
    for mol in mol_list:
        lines = mol.split("\n")
        if len(lines) < 3:
            continue
        
        if lines[2] == "1":
            sugar.add( lines[0])
    # print(mol_list[1].split("\n")
    # break
    
print(sugar)