# CSC2233 Project
# Estimating performance gains from offloading computation to active disks for SparkSQL
Active disks are storage devices that can also perform computation on the data they store. Analogous to caches active disk computation allows data to be processed near its storage point. Thus, using such near disk computation there can be potential performance gains. When large data needs to be read into memory for processing but only a part of it is actually useful and required, the performance gains would be high. We try to estimate the performance gains that can be achieved on SparkSQL by offloading some computation tasks to an on disk processor.

## Sections
1. Approach
2. Included Files
3. Setup
4. Experiments and Results
5. Conclusion
6. Future Scope

## Approach
When SQL queries are executed they are typically processed as a sequence of several operations to eventually generate a result. SparkSQL also does the same thing when executing SQL queries. The operations are small processing tasks like scan, filter etc. The results of one operation are fed into another operation unit as input to complete a sequence of operations that generates the final result. Some of these operations are good candidates to be offloaded to a near disk processor to get performance gains.

In this project we evaluate two basic operaions offered and used by SparkSQL and how much gains can these operations get when offloaded to a near disk processor. The first is a combination of a map followed by a reduce operation and other is a filter operation. If the offloaded processing of these individual operations can enhance their standalone performance then offloading these operaions when they are a part of a larger job will achieve performance gains for the job as well.

To evaluate a basic operaion, we modify the SparkSQL code to create a new function corresponding to the operation. We then create scala driver functions that perform the actual operaion. The driver functions are as follows:
1. Simple Host - This function performs the required operaion as would have done by SparkSQL originally with the benefit of added optimized RDD processing.
2. Host - This function performs the required operation as a normal scala task.
3. Near Disk - This function performs the required operation by offloading the task to another process and getting the result back from it using file input/output streams.

Additionally, as required by the near disk scala driver function, we have a C program that performs the required operaion by reading some input given by the driver function, processing it accordingly and, writing back the results to a stream that can then be read back by the driver function.

Since, we are not actually using any active disk, the evaluation of any performance gain is done by simulating the near disk processor behavior for a task. To achieve such simulation we limit the cpu speed for the Spark cluster process using the `cpulimit` command. This results in the C program that the near disk driver function offloads the task to, to run faster than the other Spark processes. Host and Simple host driver functions that process the whole operaion as Spark jobs, should thus take more time than usual. Near disk driver function however, which uses the corresponding C program to process the operation should not be affected by limited CPU speed as much, since the C program will utilize full CPU power.

Once it is established that these basic operations offer performance benefits, same can be evaluated for full SQL queries being executed on SparkSQL by modifying the SparkSQL code to call the near disk driver function instead of the original call to the corresponding operation and then simulating active disk processor behaviour for an additional C program by limiting CPU speed for the Spark jobs.
