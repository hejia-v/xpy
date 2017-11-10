package com.android.hejia.xpy;


public class JcNativeMan {
// all Java <-> C++ traffic should go through this class

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native_tst");
    }

// (1) Java -> C++ (via JcNativeMan.cpp)
//-----------------------------------------------------
// (1a) java (native) wrapper methods. All java code should only call c++ (native) methods through these wrappers.

    // static public boolean   registerJvm()       { return cppRegisterJvm();}

    static public String infoStr(String StrIn, int num) {
        return cppInfoStr(StrIn, num);
    }


//-----------------------------------------------------

    static public native String cppInfoStr(String StrIn, int num);

// (1b) Native methods implemented in C++ (JcNativeMan.cpp) - no Java implementation

}
