## 编译

### windows

1. 下载[boost 1.63.0](https://sourceforge.net/projects/boost/files/boost/1.63.0/)，解压后设置环境变量`BOOST_ROOT`为boost\_1\_63\_0文件夹所在的目录。进入boost\_1\_63\_0目录后，打开命令行，输入以下命令编译boost:

   ```
   > ./bootstrap.bat
   > b2 stage --without-python --toolset=msvc-14 --stagedir="./stage" link=static runtime-link=shared threading=multi debug release address-model=64
   ```

2. 用vs2015打开xpy/Native/xpy/external/Python-3.6.0/PCbuild/pcbuild.sln，将平台设置为x64，编译debug版本的pythoncore工程。

3. 执行xpy/Native/gensln.bat，在xpy/Native/build文件夹中生成vs2015的解决方案文件。

4. 用vs2015打开xpy/Native/xpy.sln，其中sharp工程是不依赖unity，可以用来进行c#的开发和测试。编译xpy工程后，将xpy/Native/build/bin/Debug文件夹中的xpy\_d.dll和python36\_d.dll拷贝到xpy/Assets/Plugins/x86\_64文件夹。

### android

android平台的native库使用Linux编译，这里以ubuntu 16.10 sever为例

1. 通过ppa安装jdk, 注意apt_pkg目前不支持python3.6, 需要留意一下python3对应的是不是python3.6
```shell
sudo apt-get update && sudo apt-get -y upgrade
sudo apt-get install software-properties-common
sudo add-apt-repository ppa:webupd8team/java
sudo apt-get update
sudo apt install oracle-java8-installer

echo "JAVA_HOME=$(which java)" | sudo tee -a /etc/environment
source /etc/environment
echo $JAVA_HOME
```
2. wget获取[android命令行工具](https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip), 解压得到tools文件夹
3. 进入 tools/bin 目录,
4. 参考[官网](https://developer.android.com/studio/command-line/sdkmanager.html), 安装sdk

#### 编译Python
参考python的[issue30386](https://bugs.python.org/issue30386)

#### 编译Boost
参考 https://github.com/dec1/Boost-for-Android

#### 编译xpy
在android studio中打开proj.android-studio文件夹下面的工程，在local.properties中配置好boost和python的路径，通过rebuild project，可以编译出xpy的native库。

