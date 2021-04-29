import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer

object NCFilter {

    def nearComputeFilter(d: Dataset[String]) : Dataset[String] = {
        val source = d.map(row => row.mkString).collect
        val appWriter = "/tmp/app_writer"
        val appReader = "/tmp/app_reader"

        var out = new FileOutputStream(appWriter)
        for (l_without_newline <- source) {
            val l = l_without_newline + '\n'
            out.write(l.getBytes())
        }
        out.close()

        val in = new FileInputStream(appReader)
        var newList = List[String]()
        var res = Array.ofDim[Byte](in.available())
        in.read(res)
        val output = (res.map(b => b.toChar)).mkString.split("\n")

        for (str <- output) {
            newList = newList++List(str)
        }

        newList.toDS()
    }

    def main(args: Array[String]) {
        val logFile = args.head
        val spark = SparkSession.builder.appName("MapReduceApp").getOrCreate()

        import spark.implicits._

        val logData = spark.read.textFile(logFile)

        val filterOutput = logData.myFilter(nearComputeFilter)
        filterOutput.show()
        //println("remaining entries: " + filterOutput.count)

        spark.stop()
    }
}
