#include "resmgr.h"
#include "helper/log.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <iostream>

/* Define this, so the compiler doesn't complain about using old APIs. */
#define PHYSFS_DEPRECATED
#include "physfs.h"

namespace fs = boost::filesystem;
using namespace std;

NAMESPACE_XPY_BEGIN
ResMgr::ResMgr()
{
    if (!PHYSFS_init(NULL))
    {
        printf("PHYSFS_init() failed!\n  reason: %s.\n", PHYSFS_getLastError());
        return;
    } /* if */
    logger::info("ResMgr::ResMgr init." );
}

ResMgr::~ResMgr()
{
    if (!PHYSFS_deinit())
        printf("PHYSFS_deinit() failed!\n  reason: %s.\n", PHYSFS_getLastError());
}

int ResMgr::AddToSearchPath(std::string realpath, std::string mntpoint, int append)
{
    string path = fs::path(realpath).lexically_normal().make_preferred().string();
    boost::replace_all(path, "\\", "/");
    return PHYSFS_mount(path.c_str(), mntpoint.c_str(), append);
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
NAMESPACE_XPY_END
