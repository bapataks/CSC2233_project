/*
** Near Disk driver function
**
** This scala driver function computes the total character
** count of a file by offloading the actual computation to
** a separate process.
**
** It communicates with the separate process through read
** and write pipes, writing the file contents to be processed,
** and reading back the result.
*/

// import required libraries
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer

object NCSparkCounter {

    // core near disk driver function
    def nearComputeCounter(d : Dataset[String]) : Int = {
        // convert to array of strings
        // one item for each line
        val source = d.map(row => row.mkString).collect

        // input and output files used for reading and writing
        // data between the separate process and near disk driver
        val appWriter = "/tmp/app_writer"
        val appReader = "/tmp/app_reader"

        // write data to be processed to the write pipe
        var out = new FileOutputStream(appWriter)
        for (l_without_newline <- source) {
            out.write(l_without_newline.getBytes());
        }
        out.close();

        // read the result back
        val in = new FileInputStream(appReader)
        var res = Array[Byte](0,0,0,0)
        in.read(res)

        // convert the result from byte array to int
        var charCount = (((res(3) & 0xFF) << 24) | ((res(2) & 0xFF) << 16) | ((res(1) & 0xFF) << 8) | ((res(0) & 0xFF) << 0))

        // return result
        charCount
    }

    def main(args: Array[String]) {
        val logFile = args.head

        // create a new spark session or get one that exists
        val spark = SparkSession.builder.appName("MapReduceApp").getOrCreate()

        import spark.implicits._

        // read the textfile as input
        val logData = spark.read.textFile(logFile)

        // compute total character count using near disk driver
        val numChars = logData.charCounter(nearComputeCounter)

        // print the result
        println("numChars: " + numChars)

        // stop spark session
        spark.stop()
    }
}
