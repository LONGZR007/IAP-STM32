# STM32实现从0到1的IAP

以下只简单说一下这些文件/项目的功能，以下项目配套[野火F429挑战者_V2开发板](https://detail.tmall.com/item.htm?spm=a1z10.1-b.w20601089-22026361133.6.363f73f5KZlcwa&id=600121332138&skuId=4221916348817)，如果单纯的代码看不懂的话，就看看[原子的IAP教程](http://www.openedv.com/posts/list/11494.htm)吧。

# 1.通过MDK下载不同程序实现程序跳转

本文件夹下共3个项目
- Bootloader：实现从Bootloader跳转到应用程序，将3个项目下载到目标板后，可通过按键跳转到不同的应用程序；
- IAP_蜂鸣器：应用程序——蜂鸣器，下载Bootloader和IAP_蜂鸣器后可以通过按键跳转转到IAP_蜂鸣器运行；
- IAP_流水灯：应用程序——流水灯，下载Bootloader和IAP_流水灯后可以通过按键跳转转到IAP_流水灯运行；

# 2.读写内部FLASH

要学会写IAP，其中最主要的一点就是要学会写内部FLASH，所该项目就是写内部FLASH的例程。

# 3.基于串口iap

本文件夹下共3个项目
- Bootloader：Bootloader使用串口一次性接收app文件，完成后通过按KEY1将app写入内部FLASH，完成后可以通过KEY2跳转到app运行。
- IAP_蜂鸣器：应用程序——蜂鸣器，编译生成的.bin文件可以通过串口一次性下载到Bootloader，最后由Bootloader写入内部FLASH；
- IAP_流水灯：应用程序——流水灯，编译生成的.bin文件可以通过串口一次性下载到Bootloader，最后由Bootloader写入内部FLASH。

# 4.基于串口_xmodem协议的iap

本文件夹下共3个项目
- Bootloader：Bootloader使用串口[xmodem协议](https://www.firebbs.cn/forum.php?mod=viewthread&tid=29898)接收app文件，按KEY1将使用协议将app数据写入内部FLASH，完成后可以通过KEY2跳转到app运行。
- IAP_蜂鸣器：应用程序——蜂鸣器，编译生成的.bin文件可以通过支持串口xmodem协议的工具下载到Bootloader，最后由Bootloader写入内部FLASH；
- IAP_流水灯：应用程序——流水灯，编译生成的.bin文件可以通过支持串口xmodem协议的工具下载到Bootloader，最后由Bootloader写入内部FLASH。

# 5.Bootloader应用示例

本文件夹下共4个项目
- 0-刷外部FLASH程序（烧录emXGUI资源文件）-SD卡:在本次app中使用了emXGUI所有需要刷外部FLASH资源，使用该项目可以使用SD将资源烧录到外部FALSH;
- 1-刷外部FLASH程序（烧录emXGUI资源文件）-串口YMODEM：在本次app中使用了emXGUI所有需要刷外部FLASH资源，使用该项目可以使用串口[Ymodem协议](https://www.firebbs.cn/forum.php?mod=viewthread&tid=29898)将资源烧录到外部FALSH;
- Bootloader：Bootloader在上电时按住KEY1则会使用串口[xmodem协议](https://www.firebbs.cn/forum.php?mod=viewthread&tid=29898)接收app文件，将app写入内部FLASH，完成后自动跳转到app，如果没有按住KEY1则会检测保存在外部FLASH中的标志，如果有新的app就升级APP，没有时直接跳转到app运行；
- App-FreeRTOS+emXGUI综合桌面_5寸_7寸：应用程序，在**设置->系统升级**中可以使用[Ymodem协议](https://www.firebbs.cn/forum.php?mod=viewthread&tid=29898)接收新的app文件到外部FLASH中，完成后将标准写入FLASH中，方便Bootloader检测是否有新的app；
- 资源文件_5寸：**资源文件_5寸/srcdata**下的文件为资源文件，使用SD卡烧录时将**srcdata**放到SD卡的根目录，使用串口YMODEM烧录时将**资源文件_5寸/srcdata**下的全部文件发送就OK了；
- 字体原件：生成字库的原件。

# COMM-DRV(Lib)

不推荐使用的助手，推荐使用[野火多工能调试助手](https://www.firebbs.cn/forum.php?mod=viewthread&tid=29790)或者使用SercureSRT（自行搜索安装）。

# X&YMODEM-协议源代码文件

该项目下的ymodem或者xmodem文件夹只需要拷贝到自己的项目中就可以轻松移植到自己的项目中了，只需要调用或者实现.h中的函数就可以了。

# XMODEM-SD

使用XMODEM协议将接收的文件保存到SD卡中。

# YMODEM-SD

使用YMODEM协议将接收的文件保存到SD卡中。