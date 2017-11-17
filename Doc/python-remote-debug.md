
这里使用 pycharm 进行远程调试

## 1. 创建Pycharm工程
点击【file】->【open】，选择PyScript文件夹，即可创建Pycharm工程。PyScript/libs文件夹下的pycharm-debug-py3k.egg是从Pycharm的安装文件夹下面获取的，如果有需要，可以更新。

## 2. 在本地开发环境的PyCharm中进行监听配置
步骤如下
1. 在菜单中点击【Run】->【Edit Configurations】，弹出配置对话框
2. 在配置对话框中，点击【Add New Configuration】->【Python Remote Debug】
3. 填写Local host name和Port，其中Local host name指的是本机开发环境的IP地址，而Port则随便填写一个10000以上的即可(但是要与远程代码中配置的端口号一致)；需要注意的是，由于远程计算机需要连接至本地开发环境，因此本地IP地址应该保证远程可以访问得到
4. 点击【Apply】and【OK】

## 3. 在本地开发环境的PyCharm中配置Mapping映射
一般本地文件路径与远程文件路径是一一对应的，就不用配置

## 4. 在PyCharm中启动Debug Server
点击【Run】->【Debug…】，选择刚创建的远程调试配置项。

## 5. 在远程启动应用程序
在远程设备启动应用程序，当执行到pydevd.settrace语句时，便会与本地开发环境中的PyCharm建立通讯连接，接下来便可以在本地IDE中进行单步调试了。

需要注意的是，本地开发环境必须保证IP地址和端口号可从远程设备访问得到，否则会无法建立连接。
