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

