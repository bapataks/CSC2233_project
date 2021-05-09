import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset
import scala.collection.mutable.ListBuffer

object HFilter {

    def hostFilter (d: Dataset[String]) : Dataset[String] = {
        val source = d.map(row => row.mkString).collect

        var newList = ListBuffer[String]()
        for (l <- source) {
            if (l.length < 20) {
                newList += l
            }
        }

        newList.toDS()
    }

    def main(args: Array[String]) {
        val logFile = args.head
        val spark = SparkSession.builder.appName("FilterApp").getOrCreate()

        import spark.implicits._

        val logData = spark.read.textFile(logFile)

        val filterOutput = logData.myFilter(hostFilter)
        //filterOutput.show()
        //println("remaining entries: " + filterOutput.count)

        spark.stop()
    }
}
