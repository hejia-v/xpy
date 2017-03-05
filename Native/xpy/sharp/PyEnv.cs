using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
using System.Runtime.InteropServices;


namespace XPython
{
    public class PyEnv
    {
        // Need code gen
        public static string FuncCallByPython(int n, PyDLL.var[] argv)
        {
            logger.info("I'm in Python :");
            for (int i = 1; i < n; i++)
            {
                logger.info(string.Format("Args {0} type {1}", i, argv[i].type));
            }

            // return string
            // argv[0].type = PyDLL.var_type.STRING;
            // return "ok";

            argv[0].type = PyDLL.var_type.INTEGER;
            argv[0].d = 123;
            return null;
        }

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
