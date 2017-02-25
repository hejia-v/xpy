using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
using System.Runtime.InteropServices;


namespace XPython
{
    public class PyEnv
    {
        // Use this for initialization
        public void Init()
        {
            logger.info("PyEnv Init .................");
            PyDLL.Init();
        }

        public void Destroy()
        {
            logger.info("PyEnv Destroy .................");
            PyDLL.Destroy();
        }
    }
}
