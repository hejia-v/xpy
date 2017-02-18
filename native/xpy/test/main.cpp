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
    cout << native_get_current_path() << endl;
    char *program = "python36_xpy";
    python_start(program, "G:/GameDev/unity/xpy/native/xpy/external/Python-3.6.0");
    std::cout << python_check_interpreter(program) << "\n";
    python_finalize();
}

int main(int argc, char *argv[])
{
    python_embedding_test();
    return 0;
}
