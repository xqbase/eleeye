XQF工具包——XQF->PGN和PGN->XQF转换器

(1) XQF->PGN转换器

　　“XQF->PGN转换器”是第一个真正意义上的XQF文件转换工具，它实现了XQF文件转换操作的快速化和批量化。在安装了象棋百科全书网(www.elephantbase.net)提供的XQF工具包后，XQF文件就自动和XQF->PGN转换器建立了联系，只要右击XQF文件，在弹出菜单中选择“Open with PGN Reader”，转换后的文件就会自动被PGN文件默认的关联程序所打开(通常是《象棋巫师》，也可以让PGN文件与普通文本编辑器关联起来)。
　　在命令行方式下，还可以实现批量XQF文件到PGN文件的转换。例如输入下列命令：

FOR %A IN (*.XQF) DO XQF2PGN "%A" "%A.PGN"

　　需要提醒的是，XQF->PGN转换器发布时，XQStudio的最新版本是1.63，因此XQF->PGN转换器不能保证以后版本的XQStudio产生的XQF文件被正常转换。

(2) PGN->XQF转换器

　　如果说加密的XQF格式是一道门槛的话，那么XQStudio的作者“过河象”在网上棋牌乐(www.qilaile.net)上公布的非加密XQF格式(即XQF1.0格式)无疑使这道门槛降低了，这使得任何版本的XQStudio可以读取任何格式的棋谱文件，只要能把棋谱转换成非加密XQF格式即可。
　　“PGN->XQF转换器”是为方便XQStudio用户而制作的程序，安装象棋百科全书网提供的XQF工具包，即可很方便地打开任何PGN文件(跟XQF->PGN转换器的用法一样，只要右击PGN文件，在弹出菜单中选择“Open with XQF Reader”即可)。
　　在命令行方式下，还可以实现批量PGN文件到XQF文件的转换。例如输入下列命令：

FOR %A IN (*.PGN) DO PGN2XQF "%A" "%A.XQF"

(3) 补充说明：

　　XQF工具包(附带XQStudio 1.63)的下载地址是：http://www.elephantbase.net/download/xqftools.exe。
　　XQF工具包的源代码是象棋程序 ElephantEye 的源代码的一部分，发布在 SourceForge 上：http://sourceforge.net/projects/elephanteye/。
　　若有使用方法上的疑问，请联系XQF工具包的发布者：webmaster@elephantbase.net。
　　感谢网友“小月儿”(xiaoyueer2000@hotmail.com)提供了XQF文件的解密算法，本程序是根据该算法设计的。
