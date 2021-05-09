import matplotlib.pyplot as plt
key = ["100%", "90%", "80%", "70%", "60%", "50%"]
host1 = [6615,7619,8285,9885,10928,12873]
host2 = [6616,7474,8630,9730,10831,12012]
host = [(x+y)/2 for x,y in zip(host1,host2)]
nearComp1 = [7567,8786,9138,10228,12244,14005]
nearComp2 = [7567,8383,9533,10792,11202,13259]
nearComp = [(x+y)/2 for x,y in zip(nearComp1,nearComp2)]
barwidth = 0.25
br1 = [1,2,3,4,5,6]
br2 = [x + barwidth for x in br1]
plt.bar(br1, host, color='r', width=barwidth, label='Host Compute')
plt.bar(br2, nearComp, color='b', width=barwidth, label='Near Compute')
plt.xlabel('cpulimit factor for sparkSubmit task', fontweight='bold')
plt.ylabel('Task completion time (ms)', fontweight='bold')
plt.xticks([r + barwidth for r in [1,2,3,4,5,6]], key)
plt.legend()
plt.show()
