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
    const char* current_path = Native_GetCurrentPath();
    string curr_path = current_path;
    Native_ReleaseMemory((void**)(&current_path));
    current_path = nullptr;
    cout << curr_path << endl;
    char *program = "python36_xpy";
    string home = curr_path + "/../../xpy/external/Python-3.6.0";
    string scriptroot = curr_path + "/../../../Assets/Script";
    Python_RegisterModule();
    Python_Start(program, home.c_str());
    std::cout << Python_CheckInterpreter(program) << "\n";
    Python_InitScript(scriptroot.c_str());
    string script_str = "import sys\n"
        "print(sys.path)\n";
    //Python_RunString(script_str.c_str());
    Python_InitSharpCall(nullptr);
    Python_RunFunction("main", "main", "");
    Python_Finalize();
}

int main(int argc, char *argv[])
{
    python_embedding_test();
    return 0;
}
