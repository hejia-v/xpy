#include <iostream>
#include <iostream>
#include <boost/filesystem.hpp>
#include "xpy.h"

namespace fs = boost::filesystem;
using namespace std;

const char *char_array_test1()
{
    string t1 = "wert";
    const size_t len = t1.length() + 1;
    const char *sd = t1.c_str();
    return sd;
}

void char_array_test()
{
    char *data = new char[128];
    string s = "I come back!";
    strcpy(data, s.c_str());
    cout << data << endl;

    cout << char_array_test1() << endl;
}

void python_embedding_test()
{
    cout << Native_GetCurrentPath() << endl;
    char *program = "python36_xpy";
    Python_RegisterModule();
    Python_Start(program, "G:/GameDev/unity/xpy/native/xpy/external/Python-3.6.0");
    std::cout << Python_CheckInterpreter(program) << "\n";
    Python_InitScript("G:/GameDev/unity/xpy/Assets/Script");
    Python_RunFunction("main", "main", "");
    Python_Finalize();
}

int main(int argc, char *argv[])
{
    python_embedding_test();
    return 0;
}
