/*
** Host driver object
**
** This scala object filters a file with lines less than 20
** characters long using simple scala loops for traversing
** through the file.
*/

// import required libraries
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset
import scala.collection.mutable.ListBuffer

object HFilter {

    // core host driver function
    def hostFilter (d: Dataset[String]) : Dataset[String] = {
        // convert to array of strings
        // one item for each line
        val source = d.map(row => row.mkString).collect

        var newList = ListBuffer[String]()

        // foreach line
        for (l <- source) {
            // check for filter predicate
            if (l.length < 20) {
                // append to result
                newList += l
            }
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
        // using host driver
        val filterOutput = logData.myFilter(hostFilter)

        //filterOutput.show()
        //println("remaining entries: " + filterOutput.count)

        // stop spark session
        spark.stop()
    }
}
