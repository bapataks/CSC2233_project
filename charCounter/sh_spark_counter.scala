/*
** Simple Host driver function
**
** This scala driver function computes the total character
** count of a file using the basic operations like map and
** reduce offered by Spark.
*/

// import required libraries
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

object SHSparkCounter {

    // core simple host driver function
    def hostCounterSimple (d: Dataset[String]) : Int = {
        // use map to compute length of each line
        // then use reduce to aggregate total count
        var numChars = d.map(line => line.length).reduce((a,b) => a+b)

        // return result
        numChars
    }

    def main(args: Array[String]) {
        val logFile = args.head

        // create a new spark session or get one that exists
        val spark = SparkSession.builder.appName("MapReduceApp").getOrCreate()

        import spark.implicits._

        // read the textfile as input
        val logData = spark.read.textFile(logFile)

        // compute total character count using simple host driver
        val numChars = logData.charCounter(hostCounterSimple)

        // print the result
        println("numChars: " + numChars)

        // stop spark session
        spark.stop()
    }
}
