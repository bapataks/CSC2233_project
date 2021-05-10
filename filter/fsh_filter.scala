/*
** Simple Host driver object
**
** This scala object filters a file with lines less than 20
** characters long using the basic filter operation provided
** by Spark.
*/

// import required libraries
import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

object SHFilter {

    // core simple host driver function
    def hostFilterSimple (d: Dataset[String]) : Dataset[String] = {
        // use filter directly to filter lines
        // discarding the ones not satisfying the predicate
        var result = d.filter(row => row.length < 20)

        // return result
        result    
    }

    def main(args: Array[String]) {
        val logFile = args.head

        // create a new spark session or get one that exists
        val spark = SparkSession.builder.appName("FilterApp").getOrCreate()

        import spark.implicits._

        // read the textfile as input
        val logData = spark.read.textFile(logFile)

        // filter the input file for lines with less than 20 characters
        // using simple host driver
        val filterOutput = logData.myFilter(hostFilterSimple)

        // print the result
        println("remaining entries: " + filterOutput.count)

        // stop spark session
        spark.stop()
    }
}
