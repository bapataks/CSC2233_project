/*
** Near Disk driver object
**
** This scala object flters a file with lines less than 20
** characters long by offloading the actual processing to a
** separate process.
**
** It communicates with the separate process through read and
** write pipes, writing the file contents to be processed, and
** reading back the result.
*/

// import required libraries
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset
import scala.collection.mutable.ListBuffer

import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer

object NCFilter {

    // core near disk driver function
    def nearComputeFilter(d: Dataset[String]) : Dataset[String] = {
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
            val l = l_without_newline + '\n'
            out.write(l.getBytes())
        }
        out.close()

        val in = new FileInputStream(appReader)
        var newList = ListBuffer[String]()
        var sizeByte = Array[Byte](0,0,0,0)
        var readBytes = 0
        var prevStr = ""

        // read the size of result and convert to int
        in.read(sizeByte)
        val size = (((sizeByte(3) & 0xFF) << 24) | ((sizeByte(2) & 0xFF) << 16) | ((sizeByte(1) & 0xFF) << 8) | ((sizeByte(0) & 0xFF) << 0))

        // while there is result to be read
        while (readBytes < size) {
            val toRead = in.available()
            val res = Array.ofDim[Byte](toRead)
            in.read(res)

            val lastReadChar = res(toRead-1).toChar

            // split read result into different lines
            val output = (res.map(b => b.toChar)).mkString.split("\n")

            // add remaining content from previous read if required
            newList += (prevStr+output(0))

            for (i <- 1 to output.length-2) {
                // append to local result
                newList += output(i)
            }

            if (lastReadChar == '\n') {
                newList += output(output.length-1)
                prevStr = ""
            } else {
                // store read content of a partial line
                prevStr = output(output.length-1)
            }

            readBytes += toRead
        }

        // return result
        newList.toDS()
    }

    def main(args: Array[String]) {
        val logFile = args.head

        // create a new spark session or get one that exists
        val spark = SparkSession.builder.appName("FilterApp").getOrCreate()

        import spark.implicits._

        // read the textfile as input
        val logData = spark.read.textFile(logFile)

        // filter the input file for lines with less than 20 characters
        // using near disk driver
        val filterOutput = logData.myFilter(nearComputeFilter)

        //filterOutput.show()
        //println("remaining entries: " + filterOutput.count)

        // stop spark session
        spark.stop()
    }
}
