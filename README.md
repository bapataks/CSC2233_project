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

In this project we evaluate two basic operations provided and used by SparkSQL and how much gains can these operations get when offloaded to a near disk processor. The first is a combination of a map followed by a reduce operation and the other is a filter operation. If the offloaded processing of these individual operations can enhance their standalone performance then offloading these operations when they are a part of a larger job will achieve performance gains for the job as well.

To evaluate a basic operation, we modify the SparkSQL code to create a new function corresponding to the operation. We then create scala driver functions that perform the actual operation. The driver functions are as follows:
1. Simple Host - This function performs the required operation as would have done by SparkSQL originally with the benefit of added optimized RDD processing.
2. Host - This function performs the required operation as a normal scala task.
3. Near Disk - This function performs the required operation by offloading the task to another process and getting the result back from it using input/output pipe streams.

Additionally, as required by the near disk scala driver function, we have a C program that performs the required offloaded operation by reading some input given by the driver function, processing it accordingly and, writing back the results to a pipe that can then be read back by the driver function.

Since, we are not actually using any active disk, the evaluation of any performance gain is done by simulating the near disk processor behavior for a task. To achieve such simulation we limit the cpu speed for the Spark cluster process using the `cpulimit` command. This results in the C program that the near disk driver function offloads the task to, to run faster than the other Spark processes. Host and Simple Host driver functions that process the whole operation as Spark jobs, should thus take more time than usual. Near disk driver function however, which uses the corresponding C program to process the operation should not be affected by limited CPU speed as much, since the C program will utilize full CPU power.

Once it is established that these basic operations offer performance benefits, same can be evaluated for full SQL queries being executed on SparkSQL by modifying the SparkSQL code to call the near disk driver function instead of the original call to the corresponding operation and then simulating active disk processor behaviour for an additional C program by limiting CPU speed for the Spark jobs.

## Included Files
In this section, we describe the contents of this repository.
1. charCounter - This folder contains files corresponding to evaluating the performance gains for map followed by reduce operation. We use character counter as a task that would use such a combination of map and reduce operations. The task is to count the total number of characters in a file.

  a. `sh_spark_counter.scala` - 
  This file defines a Simple Host spark object which uses a driver function to compute total character count of a file using the basic operations like map and reduce provided by Spark. The result is directly obtained as a sequence of map and reduce transformation on the input Dataset. Map operation converts all lines to their corresponding lengths and the reduce operation aggregates the final character count.
  
  b. `h_spark_counter.scala` - 
  This file defines a Host spark object which uses a driver function to compute total character count of a file using simple scala loops for traversing through the file. The input Dataset is first converted to an array of strings, one item for each line. Each line and character is then traversed one by one and the character count is aggregated to get the final result.
  
  c. `nc_spark_counter.scala` - 
  This file defines a Near Disk spark object which uses a driver function to compute total character count of a file by offloading the actual computation to a separate process. It communicates with the other process through read and write pipes, writing the file contents to be processed, and reading back the result. The input Dataset is first converted to an array of strings, one item for each line, and then it is written out to the write pipe for processing. The result is read back after processing as byte stream and is converted to integer format.
  
  d. `comp_disk.c` - 
  This C program will perform the computation of aggregating the total character count of a file when near disk compute driver function offloads the operation to this process. It communicates with the near disk driver function through read and write pipes, reading the input to be processed, and writing back the result after processing it. Data to be processed is read from the read pipe and is traversed byte by byte while aggregating character counts to get the final result, similar to host driver function. The result is then written back to the write pipe after processing the input.
  
2. filter - This folder contains files corresponding to evaluating the performance of filter operation. We use (length < 20) as the filter predicate for the corresponding filter operation. It returns only those lines of a file that are less than 20 characters in length while discarding all the longer lines.

  a. `fsh_filter.scala` - 
  This file defines a Simple Host spark object which uses a driver function to filter all lines less than 20 characters long from a file using basic filter operation provided by Spark. The result is directly obtained by applying a filter transformation with the given filter predicate on the input Dataset.
  
  b. `fh_filter.scala` - 
  This file defines a Host spark object which uses a driver function to filter all lines less than 20 characters long from a file using simple scala for trsaversing through the file. The input Dataset is first converted to an array of strings, one item for each line. Each line is then traversed to check the result of the filter predicate, the line is appended to the result if the predicate evaluates to true, and discarded if not.
  
  c. `fnc_filter.scala` - 
  This file defines a Near Disk scala object which uses a driver function to filter all lines less than 20 characters long from a file by offloading the actual processing to a separate process. It communicats with the other process through read and write pipes, writing the file contents to be processed, and reading back the result. The input Dataset is first converted to an array of strings, one item for each line and then it is written out to the write pipe for processing. The result is read back after processing as byte stream and converted to a local Dataset to be returned.
  
  d. `filter_compDisk.c` - 
  This C program will perform the processing of filtering a file to keep only the lines that satisfy the set filter predicate when near disk driver function offloads the operation to this process. It communicates with the near disk driver function through read and write pipes, reading the input to be processed, and writing back the result after processing it. Data to be processed is read from the read pipe and is traversed byte by byte. Each line is then identified and the filter predicate is evaluated for it, the line is appended to a local result if the predicate evaluates to true, and discarded if not. The local result is then written back to the write pipe after processing the input. The filter predicate is defined as a boolean returning function on a string and can be modified as required.

3. results - This folder contains the the csv files where the results from the several experiments that were carried out are recorded.

  a. `fileSize.csv` - 
  b. `cpuSpeed.csv` - 
  c. `bufSize.csv` - 

4. scripts - This folder contains any miscallaneous scripts that were used.

  a. `populate.sh` - 
  b. `storage_filesizeTime.py` - 
  c. `storage_cpuTime.py` - 
  d. `storage_bufsizeTime.py` - 

## Experiments
This section covers the experiments we perform and their results. We perform the experiments for two basic operations as mentioned before, charCounter and filter task. Since both of the tasks are performed over files that will be read by SparkSQL, before every run cache is cleared. We also restart spark cluster before each task.

#### charCounter Task
charCounter task counts the total character count of a file. In terms of Spark operations it's a map function followed by a reduce operation over the lines of the file. We perform the following experiments with charCounter task to assess performance gain when offloading a map-reduce task to near disk processor.

1. Time Taken Vs File Size

It is obvious that the file size would affect the total runtime of the task. Higher the size of the file it would take higher time to process it. We run the charCounter task with the corresponding 3 scala driver functions for simple host, host and near disk computation for different file sizes and record the runtime. We use files of size [306M, 202M, 104M, 57M, 9.5M]. For each combination 3 runs are done and the average is used to plot a cluster bar graph comparing the 3 types of computation functions.

![Time Taken Vs File Size](https://github.com/bapataks/CSC2233_project/blob/master/results/taskTimeVSfileSize.png "Time Taken Vs File Size")

As expected, the time taken for processing increases linearly with increasing file size for each of the driver functions. The simple host performs much better than the other two, we expect this to be happening because of the optimized RDD operations that it is able to leverage within Spark. Near disk computation takes slightly more time to complete the task than the host computation.

2. Time Taken Vs CPU Speed

To simulate active disk processor running the task for near disk computation driver function we use `cpulimit` to limit CPU speed for Spark cluster. This experiment evaluates the time taken to process the task for host and near disk computation when Spark cluster has limited CPU speed to [100%, 90%, 80%, 70%, 60%, 50%]. The file size used in this experiment is 306M. For each combination 2 runs are done and the average is used to plot a cluster bar graph comparing host and near disk computation.

![Time Taken Vs CPU Speed](https://github.com/bapataks/CSC2233_project/blob/master/results/taskTimeVScpuLimit.png "Time Taken Vs CPU Speed")

We expected the near disk computation to not be affected as much with the limited CPU speed of Spark cluster. This is because the near disk computation uses a separate C program to perform processing the actual operation. However, for the used file size, near disk computation does not show overall improvement over host computation. Both seem to be equally affected by the limited CPU speed. This could be happening because of multiple reasons, one is that the buffer size used in the C program that performs the operation for near disk computation is too small and thus causes unnecessary processing delay for the near disk computation as well to not show enough gains. This was evaluated in the next experiment. Second reason could be that the current file size is just too small to affect processing time. To verify this, we evaluated total time taken by the C program in its processing as a fraction of total task time. We found that the C program takes just about 10% of the total near disk computation time. So, limiting CPU speed will affect near disk computastion 90% of what host computation is affected. Thus, to achieve any useful performance gain when offloading map-reduce task to an active disk processor the operation must have been issued on a very large file.

3. Time Taken Vs Buffer Size

Since, appropraite gains were not observed for near disk computation over host computation when limiting CPU speed on Spark cluster, this experiment was carried out to evaluate the reason. One particular reason was that the buffer size that the C program uses is too small which increases the runtime for near disk computation unnecessarily. We perform near disk computation at full CPU speed and 50% CPU speed for buffer size of [4K, 256K, 1M, 10M, 50M, 100M]. For each combination 3 runs are done and the average is used to plot a cluster bar graph comparing full CPU speed and half CPU speed for Spark cluster.

![Time Taken Vs Buffer Size](https://github.com/bapataks/CSC2233_project/blob/master/results/taskTimeVSbufSize.png "Time Taken Vs Buffer Size")

We expected that with increasing buffer size the time taken would drop down for the task. For full CPU speed the time taken is almost similar for changing buffer size while for half CPU speed on Spark cluster the total time does decrease but the change is minimal.

#### filter Task
filter task filters all the lines of a file that return on a particular predicate. For our experiments we are using a filter predicate that returns true for the lines that have a length of less than 20 characters and filters out all other longer lines of the file. filter is one of the fundamental operations that Spark offers as a transformation function for its RDDs. We perform the following experiments with respect to the filter task and if it's offloading to near disk processor can achieve performance gains.

1. Time Taken Vs Filter Selectivity

Like file size in the charCounter task, the selectivity of the filter operation on the given data could affect the runtimes of the task. We therefore conduct one experiment to evaluate the total processing times for simple host, host and near disk computations with varying filter selectivity. We use different files with filter predicate sleectivity at [1%, 10%, 20%, 30%, 40%] for the filter predicate line length less than 20 characters. Each file has 1 million lines. For each combination 3 runs are done and the average is used to plot a cluster bar graph comparing simple host, host and near disk computation functions.

![Time Taken Vs Filter Selectivity](https://github.com/bapataks/CSC2233_project/blob/master/results/taskTimeVSfilterSelectivity.png "Time Taken Vs Filter Selectivity")

As the selectivty percentage increases the number of results that need to be returned increases. This does not seem to have any impact for simple host computation, possibly again due to highly optimized RDD processing and possible in place filtering operation. For host and near disk computation however, the time increases much more with the selectivity. This could be attributed to the list building that we do for each of these functions when a line needs to be appended to the result. With increasing number of records to be returned the time to build the result list one by one grows rapidly. Due to high runtimes for files with high selectivty percentage the graph is plotted with y-axis on logscale.

2. Time Taken Vs CPU Speed

Exactly like the charCounter task, this experiment for filter task does the same thing, it uses `cpulimit` command to limit the CPU speed for the Spark cluster and assesses the runtime for the host and near disk computations. The xperiment evaluates the time taken to complete the task when the Spark cluster is limited to CPU speed of [100%, 90%, 80%, 70%, 60%, 50%]. The file used in this experiment is the 1% selectivity file from the last experiment with 1 million lines. For each combination 2 runs are done and the average is used to plot a cluster bar graph comparing host and near disk computation.

![Time Taken Vs CPU Speed](https://github.com/bapataks/CSC2233_project/blob/master/results/taskTimeVSfilterCPULimit.png "Time Taken Vs CPU Speed")

Similar to charCounter task, we expected the CPU speed reduction to not affect the near disk computation as much as it would the host computation. The host computation gets affected more as compared to the near disk computation but is still overall faster. The operation part that is being offloaded to the C program in the near disk computation is only about 25% of the total computation and hence the limited CPU speed also slowes down the overall runtime for near disk computation. To gain better results the driver function part of the filter operation for host and near disk computation functions would need to be further optimized to reduce the time taken in result list building.
