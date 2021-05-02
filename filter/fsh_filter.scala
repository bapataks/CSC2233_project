import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

object SHFilter {
    def hostFilterSimple (d: Dataset[String]) : Dataset[String] = {
        var result = d.filter(row => row.length < 20)

        result    
    }

    def main(args: Array[String]) {
        val logFile = args.head
        val spark = SparkSession.builder.appName("MapReduceApp").getOrCreate()

        import spark.implicits._

        val logData = spark.read.textFile(logFile)

        val filterOutput = logData.myFilter(hostFilterSimple)
        println("remaining entries: " + filterOutput.count)

        spark.stop()
    }
}
