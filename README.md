# eleeye
ElephantEye - a XiangQi (Chinese Chess) Engine for XQWizard with Strong AI

中国象棋对弈程序ElephantEye(象眼)　版本：3.15

象棋百科全书网 *　2008年3月
(* Email: webmaster@xqbase.com)

一、简介

　　ElephantEye 是一款自由的中国象棋程序，在遵循《GNU宽松通用公共许可协议》(GNU Lesser General Public Licence)的前提下，广大象棋爱好者和程序设计师可以自由使用 ElephantEye 及其源程序。
　　ElephantEye 中文名称为“象眼”，它跟“马腿”和“炮架子”一起构成了中国象棋“棋盘上的第三维”。ElephantEye 通常与一个象棋棋谱编辑软件ElephantBoard 配合使用，寓意有板有眼(英文 Board 的意思是“板”)。(注：现在 ElephantBoard 已更名为“象棋巫师”。)

二、引擎协议

　　ElephantEye 支持UCCI 3.0，浅红象棋用户可通过UCCI引擎适配器(UCCI2QH)调用ElephantEye引擎。
(1) 支持的UCCI命令有：
　　ucci
　　setoption ...
　　position {fen <fen_str> | startpos} [moves <move_list>]
　　banmoves <move_list>
　　go [ponder | draw] ...
　　ponderhit [draw] | stop
　　probe {fen <fen_str> | startpos} [moves <move_list>]
　　quit
(2) 可以返回的UCCI信息有：
　　id {name <engine_name> | version <version_name> | copyright <copyright_info> | author <author_name> | user <user_name>}
　　option ...
　　ucciok
　　info ...
　　{nobestmove | bestmove <best_move> [ponder <ponder_move>] [draw | resign]}
    pophash [bestmove <best_move>] [lowerbound <value> depth <depth>] [upperbound <value> depth <depth>]
　　bye

三、参数设置

　　ElephantEye 作为UCCI引擎，有若干可以设置的参数(可以直接在<象棋巫师>中设置)。
(1) 开局库：
　　默认的开局库为 ElephantEye 程序(ELEEYE.EXE)当前目录下的 BOOK.DAT，含有10,000个对称局面的着法。
(2) 思考时间：
　　限定思考深度通常不是很好的选择，建议给定限时让程序自动分配时间。而在解杀局或分析局面时，则可让程序无限制思考，并可随时中止思考。
(3) 置换表大小：
　　尽管置换表大小对程序的运行速度影响不大，默认16MB的设置已经足够，但 ElephantEye 还是提供了设置置换表大小的功能。在内存允许的情况下，下慢棋时可以适当增加置换表的大小，但建议不要超过物理内存的一半。
(3) 裁剪程度：
　　为加快程序的运算速度，ElephantEye 默认使用空着裁剪，并且产生负面影响的可能性很小。只有最低级别会禁用空着裁剪。
(4) 知识量：
　　知识量和局面评价的准确性有关，在 ElephantEye 的知识量等级中，只有最低级别是不采用局面评价函数的(只考虑子力价值)，在解排局等不需要依靠审局知识来分析的局面时，可以尝试用这种设置。
(5) 随机性：
　　ElephantEye 设有4级随机性。随机性越大，程序越有可能走出它认为不是最好的着法，但“不是最好的着法”并非一点好处也没有，尤其在没有启用开局库时，适当增大随机性，可以避免程序在相同的局面下走出一样的着法。

四、规则

　　从2.0版开始，ElephantEye除了支持“单方面长将判负”的规则外，还支持“长打判负”，“打”包括“将”和“捉”。由于程序复杂性方面的限制，只有以下三种情况被识别成“捉”：
　　　　A. 马捉车或有根的炮兵(卒)；
　　　　B. 车捉有根的马炮兵(卒)；
　　　　C. 炮捉车或有根的马兵(卒)。
　　尽管 ElephantEye 在复杂的情况可能无法正确识别长打，但由于支持UCCI命令 banmoves ... ，一旦用户认为引擎走了“长打”的禁着，可以用<象棋巫师>的“设置禁着”功能让引擎强制变着。

五、博弈算法

　　ElephantEye 属于偏向蛮力的象棋程序，使用了严谨而有效的博弈算法：
(1) 使用位行和位列的着法生成器：
　　位行(BitRanks)和位列(BitFiles)有利于滑动棋子(车和炮)的着法(尤其是吃子着法)生成，位行和位列可以用查表来代替在射线上做的循环运算。在ElephantEye 中，位行和位列的技术不仅用在着法生成器中，也用到了牵制的判断上。
(2) 静态局面搜索：
　　在做静态搜索时，ElephantEye 搜索了吃子或解将的着法，在搜索吃子着法时，ElephantEye 过滤掉不重要的吃子，例如吃不过河的兵、吃不处于防守中的士象等着法，都不在静态搜索的范围之内。
(3) 循环着法和长将检测：
　　ElephantEye 可以识别循环着法，出现循环着法时可以判断哪方为长将，并且会利用禁止长将的规则来谋求优势，但目前 ElephantEye 还无法识别长捉。
(4) 置换表：
　　ElephantEye 参考了中国象棋程序“纵马奔流”的设计思路，使用深度优先和始终覆盖的双层置换表，并采用低出(高出)边界修正的置换表更新策略。
(5) 带检验的空着裁剪：
　　ElephantEye 使用 R=2 的空着裁剪，在残局阶段使用带检验的空着裁剪。
(6) 迭代加深/吃子着法/杀手着法/历史表启发：
　　ElephantEye 的着法排序非常简单清晰，依次是迭代加深着法、好的吃子着法、杀手着法和按历史表排序的生成着法。
(7) 将军/唯一应将延伸：
　　在选择性延伸上，ElephantEye 采用了将军和唯一应将延伸。
(8) Alpha-Beta主要变例搜索：
　　ElephantEye 使用传统意义上的递归式Alpha-Beta主要变例搜索。
(9) 开局库：
　　ElephantEye 的开局库共包含了10,000个对称着法，是从1990年到2005年全国象棋个人赛、团体赛、五羊杯、联赛等8,000局顶尖比赛中提取的。
(10) 后台思考和时间分配策略：
　　ElephantEye 支持后台思考功能，同时提供了时段制和加时制两种时间分配策略，会自动合理分配时间。

六、开局库

　　ElephantEye 的开局库可由“ElephantEye 开局库制作工具”制作。运行制作工具后，首先要选择PGN棋谱所在的文件夹，然后保存为开局库文件(通常是 BOOK.DAT)。通常，用来生成开局库的棋谱数量越多，生成的开局库文件就越大。
　　为了使制作的开局库对 ElephantEye 生效，只需要把生成的开局库文件替换掉 ElephantEye 目录下的 BOOK.DAT 即可，也可以在<象棋巫师>的“引擎设置”对话框中指定开局库文件。

七、局面评价函数库

　　ElephantEye 从2.1版开始，程序的搜索部分和局面评价部分就分离了，搜索部分通过调用API函数的形式与局面评价部分耦合。
　　其他象棋程序设计师可以在 ElephantEye 的基础上更自由地发挥。根据LGPL协议，搜索和局面评价这两个部分都作为独立的程序库，运用其中任何一部分都只需要公开该部分的源程序即可。换句话说，如果局面评价部分没有使用任何开放代码，那么程序设计师就没有义务公开这部分的源程序，搜索部分也是如此。
　　ElephantEye 的局面评价API函数接口定义如下：
　　　　A. 局面评价引擎名称：const char *GetEngineName(void);
　　　　B. 局面预评价函数接口：void PreEvaluate(PositionStruct *lppos, PreEvalStruct *lpPreEval);
　　　　C. 局面评价函数接口：int Evaluate(const PositionStruct *lppos, int vlAlpha, int vlBeta);
　　其中 PositionStruct 和 PreEvalStruct 必须分别符合 position.h 和 pregen.h 中定义的结构。　　　　

八、源程序

　　ElephantEye 的源程序包括9个模块，内容大致为：
(1) ucci.h/ucci.cpp
　　UCCI命令解释模块，包括 Windows 和 Unix 下的行输入接收程序；
(2) pregen.h/pregen.cpp
　　Zobrist 数组和着法预置表的生成模块。ElephantEye 的预置表分两个部分，一是滑动棋子的着法预置表(包括不吃子、车吃子、炮吃子和隔两子吃子)，它是实现位行和位列技术的基础；二是其他棋子的着法预置表，使得着法生成时避免了烦琐的边界判断。
(3) position.h/position.cpp
　　主要描述着法和局面的数据结构及功能。局面的处理是本模块的重点，处理内容包括局面初始化、FEN串导入、棋子移动、杀手着法的合理性判断、将军判断、长将和循环检测、子力价值分调整等过程，还包括5个子力位置价值表。
(4) genmoves.cpp
　　着法生成器，包括生成吃子着法和生成不吃子着法的两个，但不能只生成解除将军的着法。在生成吃子着法的同时赋予每个着法以相应的MVV(LVA)(或称准SEE)值。该模块还有一个专门判断棋子是否有保护的函数，来计算MVV(LVA)值，对于有保护的棋子，计算MVV-LVA的值(小于零不计)，对于无保护的棋子，只计算MVV的值。因此，判断棋子是否有根的程序也包括在本模块中。
(5) hash.h/hash.cpp
　　置换表、历史表和着法列表管理模块，包括置换表的分配和存取、主要变例获取等操作。
(6) book.h/book.cpp
　　开局库读取模块。
(7) movesort.h/movesort.cpp
　　着法列表排序模块。
(8) search.h/search.cpp
　　搜索模块，除了静态搜索、完全搜索和根结点搜索这三个主要过程外，还包括迭代加深控制、后台思考、时间分配、搜索参数统计和搜索信息输出等内容。该模块是整个程序的核心模块。
(9) eleeye.cpp
　　主程序(即 main 函数)。
(10) preeval.h/preeval.cpp
　　子力位置数组预生成器，ElephantEye 根据“进攻/防守”和“开局/中局/残局”两个参数线性调整子力位置数组。
(11) evaluate.cpp
　　局面评价函数，ElephantEye 采用了四级偷懒评价的机制，最粗的层次只评价特殊棋型，进一层次评价牵制，再进一层次评价车的灵活性，最高层次还评价马的阻碍。

九、程序表现

　　ElephantEye 的设计重点在搜索算法，但在知识上比较欠缺。在2.8GHz的处理器上每秒可搜索约1,000,000个结点(包括常规搜索和静态搜索)，一般的中局局面在1分钟内可搜索约11层。
　　在棋力上，ElephantEye 和“棋隐”、SaoLa (象棋挑战者)等程序具有同等水平，但由于局面评估函数上的缺陷，ElephantEye 距离顶尖的商业象棋软件(谢谢大师、象棋世家、象棋奇兵、棋天大圣等)尚有一定的差距。
　　ElephantEye 在联众、弈天等象棋对弈网站上作过测试，用等级分来衡量，联众网的战绩在2500分左右，弈天网快棋的战绩在2000分左右，慢棋在1500分左右。
　　2005年9月在台湾象棋软件爱好者施金山先生的帮助下，ElephantEye 参加了在台北举行的第10届ICGA电脑奥林匹克大赛中国象棋组比赛，战绩是7胜5和14负，在14个程序中排名第11；2006年8月 ElephantEye 参加了在北京举行的全国首届计算机博弈锦标赛，战绩是7胜2和11负，在18个程序中排名第7。

十、相关资源

　　ElephantEye的源程序发布在SourceForge的XiangQi Wizard项目中，其页面是：
　　　　http://sourceforge.net/projects/xqwizard/
　　ElephantEye的版本改进，实时同步地发布在SourceForge的SVN站点上，获取地址是：
　　　　https://xqwizard.svn.sourceforge.net/svnroot/xqwizard/
　　您可以使用 TortoiseSVN 等SVN客户端程序获取到最新的(跟开发者完全同步的)代码，TortoiseSVN 的使用介绍和下载地址是：
　　　　http://sourceforge.net/projects/tortoisesvn/

　　ElephantEye 必须在支持UCCI(如<象棋巫师>)的象棋程序下运行，<象棋巫师>安装程序包含了最近一个版本的 ElephantEye。
　　<象棋巫师>可到下列站点下载：
　　　　http://www.skycn.com/soft/24665.html (天空软件站) 
　　　　http://www.onlinedown.net/soft/38287.htm (华军软件园) 

　　ElephantEye 的源程序包除了 ElephantEye 本身的源程序外，还包括以下几个附加模块：
　　(1) 基础代码(base)：提供了汇编指令、系统函数调用等功能；
　　(2) 中国象棋规则模块(cchess)：为其他软件使用 ElephantEye 代码提供了接口；
　　(3) 开局库制作模块(BOOK)：制作开局库BOOK.DAT的代码；
　　(4) UCCI引擎联赛模拟器(LEAGUE)：为UCCI引擎测试和比赛提供了自动批量对局的平台；
　　(5) UCCI引擎搜索树分析器(TREE)：UCCI引擎(支持UCCI 2.2+)的搜索路线分析工具；
　　(6) XQF棋谱工具(XQFTOOLS)：提供XQF等多种棋谱转换为PGN的工具；
　　(7) 浅红象棋适配器(UCCI2QH)：为浅红象棋调用UCCI引擎提供了接口；
　　(8) 浅红象棋引擎支持UCCI的适配器(QH2UCCI)：为“梦入神蛋”浅红象棋加入UCCI引擎测试提供了接口；
　　(9) BBS Chess(BBSCHESS)：一个用 Visual Basic 制作的国际象棋局面设置工具，可在各高校BBS上粘贴彩色的国际象棋局面；
　　(10) 棋盘图片生成器(FEN2BMP)：一个可以把国际象棋和中国象棋的FEN文件转换成BMP文件的实用工具；
　　(11) 编码转换(codec)，包括简繁转码、UNIX文本转码、Base64转码等；
　　(12) 其他工具(MISC)：包括简易网络通讯、管道测试等工具；
　　(13) 说明文档(DOC)：即《中国象棋程序设计探索》系列连载；
　　(14) 参赛棋谱(CCGC)：ElephantEye 参加首届全国计算机博弈锦标赛(CCGC)的全部棋谱。

　　如果要获得关于 ElephantEye 的更加详细的信息，可登录象棋百科全书网：
　　　　http://www.xqbase.com/
