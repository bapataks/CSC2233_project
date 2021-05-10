# pyplot script to generate cluster bar graph
# to compare host and near compute for filter task
# based on changing cpu speed for Spark cluster

import matplotlib.pyplot as plt

key = ["100%", "90%", "80%", "70%", "60%", "50%"]

# host compute task time from filterCPU.csv
host1 = [1833,1941,2389,3256,3076,3443]
host2 = [1834,2176,2506,2320,3057,3532]
host = [(x+y)/2 for x,y in zip(host1,host2)]

# near compute task time from filterCPU.csv
nearComp1 = [6076,6647,6899,7214,7727,8419]
nearComp2 = [6077,6478,6691,6850,7281,8124]
nearComp = [(x+y)/2 for x,y in zip(nearComp1,nearComp2)]

barwidth = 0.25
br1 = [1,2,3,4,5,6]
br2 = [x + barwidth for x in br1]

# plot the graph
plt.bar(br1, host, color='r', width=barwidth, label='Host Compute')
plt.bar(br2, nearComp, color='b', width=barwidth, label='Near Compute')

# print labels on axes
plt.xlabel('cpulimit factor for sparkSubmit task', fontweight='bold')
plt.ylabel('Task completion time (ms)', fontweight='bold')
plt.xticks([r + barwidth for r in [1,2,3,4,5,6]], key)

# add legend
plt.legend()

# show graph
plt.show()
