using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
using System.Runtime.InteropServices;
using UnityEngine;
using XPython;


public class UnityTestDll : MonoBehaviour
{
    PyEnv pyEnv = new PyEnv();

    // Use this for initialization
    void Start()
    {
        pyEnv.Init();
    }

    // Update is called once per frame
    void Update()
    {

    }

    void OnGUI()
    {
        GUI.Label(new Rect(1, 1, 600, 400), "this dll i = 5+7, i is");
    }

    void OnDestroy()
    {
        pyEnv.Destroy();
    }
}
