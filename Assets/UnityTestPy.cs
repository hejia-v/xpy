using System.Collections;
using System.Collections.Generic;
using System;
using System.Text;
using System.Runtime.InteropServices;
using UnityEngine;
using XPython;


public class UnityTestPy : MonoBehaviour
{
    PyEnv pyEnv = null;

    class MyClass
    {

    }

    // Need code gen
    public static string FuncCallByPython(int n, PyEnv.var[] argv)
    {
        logger.info("I'm in Python :");
        for (int i = 1; i < n; i++)
        {
            logger.info(string.Format("Args {0} type {1}", i, argv[i].type));
        }

        // return string
        // argv[0].type = PyDLL.var_type.STRING;
        // return "ok";

        argv[0].type = PyEnv.var_type.INTEGER;
        argv[0].d = 123;
        return null;
    }

    // Use this for initialization
    void Start()
    {
        pyEnv = new PyEnv();
        pyEnv.Init();

        //// test
        //PyEnv.PyObject test_1 = pyEnv.GetFunction("sharp_test", "test_1");
        //PyEnv.PyObject test_2 = (PyEnv.PyObject)pyEnv.CallFunction(test_1, "aaabbb")[0];
        //MyClass myobj = new MyClass();
        //object[] result1 = pyEnv.CallFunction(test_2, myobj);

        //PyEnv.PyObject test_4 = pyEnv.GetFunction("sharp_test", "test_4");
        //object[] result2 = pyEnv.CallFunction(test_4, "Hello World 1", test_1);
        //object[] result3 = pyEnv.CallFunction(test_4, "Hello World 2", 547);
        //object[] result4 = pyEnv.CallFunction(test_4, "Hello World 3", myobj);
        //logger.info((string)result4[0]);
        //logger.info(result4[1].GetType().ToString());

        //PyEnv.PyObject gc = pyEnv.GetFunction("gc", "collect");
        //pyEnv.CallFunction(gc);  // gc.collect(generation=2), With no arguments, run a full collection.
        //pyEnv.CollectGarbage();

        //PyEnv.PyObject init = pyEnv.GetFunction("sharp_test", "init");
        //PyEnv.SharpFunction func = FuncCallByPython;
        //pyEnv.CallFunction(init, func);
        //PyEnv.PyObject callback = pyEnv.GetFunction("sharp_test", "callback");
        //pyEnv.CallFunction(callback, 1, null, "string");
    }

    // Update is called once per frame
    void Update()
    {

    }

    void OnGUI()
    {
        GUI.Label(new Rect(1, 1, 600, 400), "hello world");
    }

    void OnDestroy()
    {
        pyEnv.Destroy();
    }
}
