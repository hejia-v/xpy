## 编译

### windows

1. 下载[boost 1.63.0](https://sourceforge.net/projects/boost/files/boost/1.63.0/)，解压后设置环境变量`BOOST_ROOT`为boost\_1\_63\_0文件夹所在的目录。进入boost\_1\_63\_0目录后，打开命令行，输入以下命令编译boost:

   ```
   > ./bootstrap.bat
   > b2 stage --without-python --toolset=msvc-14 --stagedir="./stage" link=static runtime-link=shared threading=multi debug release address-model=64
   ```

2. 用vs2017打开xpy/Native/xpy/external/Python-3.6.0/PCbuild/pcbuild.sln，将平台设置为x64，编译debug版本的pythoncore工程。

3. 执行xpy/Native/gensln.bat，在xpy/Native/build文件夹中生成vs2015的解决方案文件。

4. 用vs2017打开xpy/Native/xpy.sln，其中sharp工程是不依赖unity，可以用来进行c#的开发和测试。编译xpy工程后，将xpy/Native/build/bin/Debug文件夹中的xpy\_d.dll和python36\_d.dll拷贝到xpy/Assets/Plugins/x86\_64文件夹。

### android

android平台的native库推荐使用Linux编译，并且通过android studio来安装sdk和ndk，这样会少很多折腾

#### 编译Python
参考python的[issue30386](https://bugs.python.org/issue30386)

#### 编译Boost
参考[Boost-for-Android](https://github.com/dec1/Boost-for-Android)

#### 编译xpy
在android studio中打开proj.android-studio文件夹下面的工程，在local.properties中配置好boost和python的路径，通过rebuild project，可以编译出xpy的native库。
