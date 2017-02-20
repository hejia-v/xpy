using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
using System.Runtime.InteropServices;
using UnityEngine;


namespace XPython
{
    public class PyEnv
    {
        // Use this for initialization
        public void Init()
        {
            Debug.Log("PyEnv Init .................");
            PyDLL.Init();
        }

        public void Destroy()
        {
            Debug.Log("PyEnv Destroy .................");
            PyDLL.Destroy();
        }
    }
}
