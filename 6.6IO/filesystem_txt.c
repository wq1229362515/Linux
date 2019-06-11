//一.文件i/O模块的总结

//1.PCB(task_struct)(sched.h) struct files_struct *files
//2.---->  struct file{拥有各种信息}(fs.h) ---其中的一项--->fd _array[](fdtable.h)
//   struct file *fd_array[];
//   struct file __rcu * fd_array[NR_OPEN_DEFAULT];


//内核文件描述信息结构体组的下标就是fd  
//操作文件(打开,删除,写,关闭~~~~等),这些系统调用接口函数,返回的是一个整形数,返回了文件描述符(本质是文件描述表的下标)
//起到一个索引的作用,通过PCB的文件描述符找到该fd所指向的文件指针filp
//
//
//
//
//
//PCB每个进程都有,那么这个files_struct,每个进程都有了,当需要打开文件的时候,拿出来用
//struct file 中有很多关于文件的参数描写{f_count ,f_flag ,f_pos,,,,,,,}
//file_operations每一个file中都有,而这个结构体里面都是函数指针,指向了各种操作的函数
//
//进程通过open(),打开文件,实质上是获得一个文件描述符(不管这个文件存在不存在),便于进程通过文件描述符为连接文件进行
//其他操作,进程打开文件时候,会创建一个file对象,并把file对象存入进程打开文件表中(文件描述符数组)(按照最小未分配的规则),进而确定了
//所打开文件的文件描述符(这个时候其实fd并没有创建起来,open完成了功能之后,就会获得一个fd)
//那么这时候open通过内核调用的函数就会返回给外部一个整形变量
//
//
//简单归纳 :fd只是一个整数，在open时产生。起到一个索引的作用，进程通过PCB中的文件描述符表找到该fd所指向的文件指针filp。

//linux下遵循一切皆文件这个思想
//在文件描述符数组的前三个已经占用了,就是stdin,stdout,stderr,0,1,2
//所以自己打开的文件,文件描述符至少是从三开始的
//
//close (int  fd)  关闭文件操作符,使它不在指向任何一个文件和不能在新的文件中使用
//
//
//C中是将接口封装了一层使用了文件指针文件指针这个结构 (usr/include/stdio.h (头文件中只是放了各种各样的声明)  )
//
// struct _IO_FILE;
// 
// __BEGIN_NAMESPACE_STD
///* The opaque type of streams.  This is the definition used elsewhere.  */
//typedef struct _IO_FILE FILE;
//
//在这个文件中定义了文件流指针的结构
//
// int  fileno 
// 对文件流 返回系统文件描述符
// /* Return the system file descriptor for STREAM.  */
//
// extern int fileno (FILE *__stream) __THROW __wur;
//
// 经过验证fileno 就是封装起来的文件描述符
//
// 那么如何完成的重定向我的理解就是偷梁换柱了一把,fileno是个傻子,只知道被柱子顶着,但是柱子换了,方向也变了(fd被更换),都不知道,
// 那么结果就变了,
//
// 重定向的函数 
//
// int  dup2( int oldfd,int newfd );
//
// dup2,dup 用来复制文件描述符
//
// oldfd 就是旧文件描述符
//
// newfd 就是新文件描述符
//
// 将旧文件描述符复制给新文件描述符
// 可以选择性的关闭旧的文件描述符,避免资源浪费
//
// int dup(int oldfd);
//
// 这个复制成功返回的是最小的尚未被使用的文件描述符,如果有错误那么返回-1,返回的新文件的描述符和旧文件描述符指向了同一个文件
//
//
//-------------------------------------------------------------------------------------------------------------------------
//
//
//二. 文件操作系统
//
//ls -l 包括了文件元数据
//
//模式 硬链接数 文件所有者 组 大小 最后修改时间 文件名
//
//
//stat 命令可以看到更多的信息

//EXT2  Linux second extend file system ,(Ext2fs)
//
// 在根目录下的dev下存放的是外设文件,是黄色的
// 这些设备也被当做文件来对待
// 硬盘有两种传输接口一个是IDE
// 一个是SATA接口
// 整块磁盘的第一个扇区最重要,因为记录了两个重要信息
//
// 1. 主引导分区(Master Boot Record ,MBR)(可安装引导加载程序的地方)(446字节)
// 2.分区表(partition table )(记录磁盘分区状态的)(64个字节)
// 开机的时候,系统会主动的读取这个区块的内容,这样系统才知道你的程序放在那里该如何进行开机
// 开机刷磁头,就要读取第一个扇区的内容
//-------------------------------------------------------------------------------------------------------
// (基本输入输出系统)
// BIOS它是一组固化到计算机内主板上一个ROM芯片上的程序
// 它保存着计算机最重要的基本输入输出的程序、开机后自检程序和系统自启动程序
// 它可从CMOS中读写系统设置的具体信息
// CMOS
// 在计算机领域，CMOS常指保存计算机基本启动信息（如日期、时间、启动设置等）的芯片。
// 有时人们会把CMOS和BIOS混称
// 其实CMOS是主板上的一块可读写的并行或串行FLASH芯片
// 是用来保存BIOS的硬件配置和用户对某些参数的设定。
//(CMOS)是一个存储器,记录各项硬件参数且嵌入在主板当中
//
//开机流程----------------------------------------------------------------------------------------------
//1.BIOS :   开机主动执行的韧体,会认识第一个开始的设备
//2.MBR :    第一个可开机设备的第一个扇区的主引导分区块,内包含引导加载程序
//3.引导加载程序(Boot loader) : 一个可读取内核文件来执行的软件(目的是加载内核文件,并执行内核文件)
// 引导加载程序是操作系统在安装的时候提供的,所以他能够识别硬盘文件的系统格式,因此就可以
// 读取内核文件,引导完了之后,那么就是操作系统的任务了.
//
//Boot loader 的主要任务  提供菜单:用户可以选择不同的开机选项(可能这个程序的菜单有直接加载window系统,也可能是第二分区的启动扇区),这也是多重引导的重要功能
//
//
//载入内核文件: 直接指向可开机的程序区段来开始操作系统
//
//转交其他的 loader  其他分区的引导扇区也有loader(那么其他扇区的loader只认其自己的分区的可开机内核文件)
//
//4.内核文件:开始操作系统的功能
//
//
//


//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//