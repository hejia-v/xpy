#include "resmgr.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <iostream>

namespace fs = boost::filesystem;
using namespace std;

namespace xpy
{
ResMgr::ResMgr()
{
}

ResMgr::~ResMgr()
{
}

void ResMgr::setResPath(std::string path)
{
    m_ResPath = path;
}

std::string ResMgr::getResPath()
{
    if (m_ResPath.empty())
    {
        fs::path currpath = boost::filesystem::current_path();
        currpath += "/../../../../Assets/Res";
        string respath = currpath.lexically_normal().make_preferred().string();
        boost::replace_all(respath, "\\", "/");
        return respath;
    } 
    else
    {
        return m_ResPath;
    }
}

std::string ResMgr::readFileText(std::string filename)
{
    std::ifstream infile;
    infile.open(filename);
    if (!infile.is_open())
        return "";

    std::string text = "";
    std::string sl;
    while (getline(infile, sl))
    {
        text += sl + "\n";
    }
    infile.close();
    return text;
}
}