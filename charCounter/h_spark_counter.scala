/*
** Host driver function
**
** This scala driver function computes the total character
** count of a file using simple scala loops for traversing
** through the file.
*/

// import required libraries.
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

object HSparkCounter {

    // core host driver function
    def hostCounter (d: Dataset[String]) : Int = {
        // convert to array of strings
        // one item for each line
        val source = d.map(row => row.mkString).collect
        var charCount = 0

        // foreach line
        for (l <- source) {
            // traverse each character
            for (c <- l) {
                if (c != '\n') {
                    // and aggregate count
                    charCount += 1
                }
            }
        }

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

        // compute total character count using host driver
        val numChars = logData.charCounter(hostCounter)

        // print the result
        println("numChars: " + numChars)

        // stop spark session
        spark.stop()
    }
}
