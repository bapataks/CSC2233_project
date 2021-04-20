import org.apache.spark.sql.SparkSession
import org.apache.spark.sql.Dataset

import java.io.FileInputStream
import java.io.FileOutputStream
import java.nio.ByteBuffer

object HSparkCounter {

    def hostCounter (d: Dataset[String]) : Int = {
        val source = d.map(row => row.mkString).collect
        var charCount = 0

        for (l <- source) {
            for (c <- l) {
                if (c != '\n') {
                    charCount += 1
                }
            }
        }

        charCount
    }
/*
    def hostCounterSimple (d: Dataset[String]) : Int = {
        var numChars = d.map(line => line.length).reduce((a,b) => a+b)

        numChars
    }

    def nearComputeCounter(d : Dataset[String]) : Int = {
        val source = d.map(row => row.mkString).collect
        val appWriter = "/tmp/app_writer"
        val appReader = "/tmp/app_reader"

        var out = new FileOutputStream(appWriter)
        for (l_without_newline <- source) {
            val l = l_without_newline
            out.write(l.getBytes());
        }
        out.close();

        val in = new FileInputStream(appReader)
        var res = Array[Byte](0,0,0,0)
        in.read(res)

        var charCount = (((res(3) & 0xFF) << 24) | ((res(2) & 0xFF) << 16) | ((res(1) & 0xFF) << 8) | ((res(0) & 0xFF) << 0))
        charCount
    }
*/
    def main(args: Array[String]) {
        val logFile = args.head
        val spark = SparkSession.builder.appName("MapReduceApp").getOrCreate()

            import spark.implicits._

        val logData = spark.read.textFile(logFile)

        val numChars = logData.charCounter(hostCounter)
        //val numChars = logData.charCounter(hostCounterSimple)
        //val numChars = logData.charCounter(nearComputeCounter)

        //println("hostCounter: " + spark.time(logData.charCounter(hostCounter)))
        //println("hostCounterSimple: " + spark.time(logData.charCounter(hostCounterSimple)))
        //println("nearCounter: " + spark.time(logData.charCounter(nearComputeCounter)))

        println("numChars: " + numChars)
        spark.stop()
    }
}
