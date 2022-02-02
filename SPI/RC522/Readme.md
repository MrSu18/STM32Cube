# cubemx的设置

1.   打开SPI模式设置为：Full-Duplex Master（全双工主机模式）
     -   分频设置为8~256都行，这样随之波特率也会改变，波特率越小通信速度越快
     -   Clock Phase (CPHA) 设置为第一个上升沿**这里一定要1Edge**
     -   不开启CRC检测
2.   随后再定义两个普通的OUTPUT输出的引脚作为复位和使能引脚



![image-20211106152923621](https://gitee.com/mrsu18/pictures-of-the-warehouse/raw/master/image/image-20211106152923621.png)

均为推免输出PP