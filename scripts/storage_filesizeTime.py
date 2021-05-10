# pyplot script to generate cluster bar graph
# to compare host, simple host and near compute for charCounter task
# based on changing file sizes for input file

import matplotlib.pyplot as plt

key = ["306M", "202M", "104M", "57M", "9.5M"]

# simple host compute task time from fileSize.csv
simpleHost1 = [1403,1140,834,783,598]
simpleHost2 = [1425,1196,860,760,598]
simpleHost3 = [1434,1154,1014,729,568]
simpleHost = [(x+y+z)/3 for x,y,z in zip(simpleHost1,simpleHost2,simpleHost3)]

# host compute task time from fileSize.csv
host1 = [6752,4895,3232,2175,1033]
host2 = [6530,4765,2947,2062,1018]
host3 = [6565,4840,2932,2127,1042]
host = [(x+y+z)/3 for x,y,z in zip(host1,host2,host3)]

# near compute task time from fileSize.csv
nearComp1 = [7795,5410,3355,2357,927]
nearComp2 = [7317,5243,3429,2490,941]
nearComp3 = [7589,5256,3164,2270,984]
nearComp = [(x+y+z)/3 for x,y,z in zip(nearComp1,nearComp2,nearComp3)]

barwidth = 0.25
br1 = [1,2,3,4,5]
br2 = [x + barwidth for x in br1]
br3 = [x + barwidth for x in br2]

# plot the graph
plt.bar(br1, simpleHost, color='g', width=barwidth, label='Simple Host Compute')
plt.bar(br2, host, color='r', width=barwidth, label='Host Compute')
plt.bar(br3, nearComp, color='b', width=barwidth, label='Near Compute')

# print labels on axes
plt.xlabel('File Size', fontweight='bold')
plt.ylabel('Task completion time (ms)', fontweight='bold')
plt.xticks([r + barwidth for r in [1,2,3,4,5]], key)

# add legend
plt.legend()

# show graph
plt.show()
