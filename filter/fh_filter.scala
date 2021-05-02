import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

object HFilter {

    def hostFilter (d: Dataset[String]) : Dataset[String] = {
        val source = d.map(row => row.mkString).collect

        var newList = List[String]()
        for (l <- source) {
            if (l.length < 20) {
                newList = newList++List(l)
            }
        }

        newList.toDS()
    }

    def main(args: Array[String]) {
        val logFile = args.head
        val spark = SparkSession.builder.appName("MapReduceApp").getOrCreate()

        import spark.implicits._

        val logData = spark.read.textFile(logFile)

        val filterOutput = logData.myFilter(hostFilter)
        filterOutput.show()
        //println("remaining entries: " + filterOutput.count)

        spark.stop()
    }
}
