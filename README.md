

# 运行在mac
讲本工程为c++跨平台工程，作者使用xcode开发，后移植到windows上。移植只是调整了一些头文件的差异，哪里错了改哪里。上传版本应该是window可运行的版本。
# 如何运行在windows：
编译器设计与实现
1.安装git bash。
2.让git bash支持make。参考网址https://blog.csdn.net/z69183787/article/details/96290717
3.从dev c++等编译环境中找出g++.exe的路径，配置到环境变量里。
4.把makefile放到src文件夹下，然后在git bash中使用make指令就可以编译运行了。
//5.使用./main.out运行(make会自动运行)



