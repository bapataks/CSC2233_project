import matplotlib.pyplot as plt
key = ["4K", "256K", "1M", "10M", "50M", "100M"]
fullCPU1 = [7795,7610,7417,7283,7123,7215]
fullCPU2 = [7317,7055,7112,7028,7207,6851]
fullCPU3 = [7589,7227,7101,7036,7194,7077]
halfCPU1 = [14005,12635,13239,13318,12628,12567]
halfCPU2 = [13259,13302,12851,12701,12315,12174]
halfCPU3 = [(14005+13259)/2,13666,12968,12553,12371,12382]
fullCPU = [(x+y+z)/3 for x,y,z in zip(fullCPU1,fullCPU2,fullCPU3)]
halfCPU = [(x+y+z)/3 for x,y,z in zip(halfCPU1,halfCPU2,halfCPU3)]
barwidth = 0.25
br1 = [1,2,3,4,5,6]
br2 = [x + barwidth for x in br1]
plt.bar(br1, fullCPU, color='g', width=barwidth, label='FullCPU Near Compute')
plt.bar(br2, halfCPU, color='b', width=barwidth, label='HalfCPU Near Compute')
plt.xlabel('Buffer Size', fontweight='bold')
plt.ylabel('Task completion time (ms)', fontweight='bold')
plt.xticks([r + barwidth for r in [1,2,3,4,5,6]], key)
plt.legend()
plt.show()
