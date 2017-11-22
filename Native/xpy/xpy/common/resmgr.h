/* Created on 2017.3.7
 *
 * @author: jia
 */

#pragma once

#include "singleton.h"
#include "xdefine.h"
#include <string>

NAMESPACE_XPY_BEGIN

class ResMgr : public SingleTon<ResMgr>
{
    friend class SingleTon<ResMgr>;  // SingleTon创建实例时要访问私有的ResMgr构造函数
public:
    int AddToSearchPath(std::string realpath, std::string mntpoint, int append);

    void setResPath(std::string path);
    std::string getResPath();
    std::string readFileText(std::string filename);

private:
    ResMgr();
    ~ResMgr();

private:
    std::string m_ResPath = "";
};

NAMESPACE_XPY_END
