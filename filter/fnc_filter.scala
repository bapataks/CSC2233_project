import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset
import scala.collection.mutable.ListBuffer

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
        var newList = ListBuffer[String]()
        var sizeByte = Array[Byte](0,0,0,0)
        var readBytes = 0
        var prevStr = ""

        in.read(sizeByte)
        val size = (((sizeByte(3) & 0xFF) << 24) | ((sizeByte(2) & 0xFF) << 16) | ((sizeByte(1) & 0xFF) << 8) | ((sizeByte(0) & 0xFF) << 0))

        while (readBytes < size) {
            val toRead = in.available()
            val res = Array.ofDim[Byte](toRead)
            in.read(res)

            val lastReadChar = res(toRead-1).toChar
            val output = (res.map(b => b.toChar)).mkString.split("\n")

            newList += (prevStr+output(0))

            for (i <- 1 to output.length-2) {
                newList += output(i)
            }

            if (lastReadChar == '\n') {
                newList += output(output.length-1)
                prevStr = ""
            } else {
                prevStr = output(output.length-1)
            }

            readBytes += toRead
        }

        newList.toDS()
    }

    def main(args: Array[String]) {
        val logFile = args.head
        val spark = SparkSession.builder.appName("FilterApp").getOrCreate()

        import spark.implicits._

        val logData = spark.read.textFile(logFile)

        val filterOutput = logData.myFilter(nearComputeFilter)
        //filterOutput.show()
        //println("remaining entries: " + filterOutput.count)

        spark.stop()
    }
}
