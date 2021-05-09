import matplotlib.pyplot as plt
key = ["1%", "10%", "20%", "30%", "40%"]
simpleHost1 = [1265,1208,1332,931,1249]
simpleHost2 = [1000,898,845,1264,1221]
simpleHost3 = [913,1280,1273,1301,1282]
simpleHost = [(x+y+z)/3 for x,y,z in zip(simpleHost1,simpleHost2,simpleHost3)]
host1 = [1941,2282,2799,3012,3243]
host2 = [1684,2353,3053,2889,3290]
host3 = [1876,2385,2889,3613,3579]
host = [(x+y+z)/3 for x,y,z in zip(host1,host2,host3)]
nearComp1 = [6372,7150,7316,7926,8286]
nearComp2 = [6470,6920,7894,7788,8497]
nearComp3 = [6387,6914,7342,8061,8082]
nearComp = [(x+y+z)/3 for x,y,z in zip(nearComp1,nearComp2,nearComp3)]
barwidth = 0.25
br1 = [1,2,3,4,5]
br2 = [x + barwidth for x in br1]
br3 = [x + barwidth for x in br2]
plt.bar(br1, simpleHost, color='g', width=barwidth, label='Simple Host Compute')
plt.bar(br2, host, color='r', width=barwidth, label='Host Compute')
plt.bar(br3, nearComp, color='b', width=barwidth, label='Near Compute')
plt.xlabel('Filter Selectivity', fontweight='bold')
plt.ylabel('Task completion time (ms)', fontweight='bold')
plt.xticks([r + barwidth for r in [1,2,3,4,5]], key)
plt.legend()
plt.show()
