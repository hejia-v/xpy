using System;
using System.Text;
using System.Runtime.InteropServices;
using UnityEngine;

namespace XPython
{
    public class PyDLL
    {

#if UNITY_IPHONE && !UNITY_EDITOR
        const string DLL = "__Internal";
#elif UNITY_EDITOR
        const string DLL = "xpy_d";
#else
        const string DLL = "xpy";
#endif

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void DebugCallback(int level, string message);

        private static void DebugMethod(int level, string message)
        {
            switch (level)
            {
                case 1:
                case 2:
                    Debug.Log("xpy: " + message);
                    break;
                case 3:
                    Debug.LogWarning("xpy: " + message);
                    break;
                case 4:
                    Debug.LogError("xpy: " + message);
                    break;
                default:
                    Debug.Log("xpy: " + message);
                    break;
            }
        }

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void RegisterDebugCallback(IntPtr callback);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Native_ReleaseMemory(IntPtr pbuffer);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Native_GetCurrentPath();

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Python_RegisterModule();

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Python_Start([MarshalAs(UnmanagedType.LPStr)] string program, [MarshalAs(UnmanagedType.LPStr)] string home);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Python_CheckInterpreter([MarshalAs(UnmanagedType.LPStr)] string program);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Python_Finalize();

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Python_InitScript([MarshalAs(UnmanagedType.LPStr)] string pythonfile);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Python_RunString([MarshalAs(UnmanagedType.LPStr)] string script);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Python_RunFunction([MarshalAs(UnmanagedType.LPStr)] string pythonfile,
            [MarshalAs(UnmanagedType.LPStr)] string funcname, [MarshalAs(UnmanagedType.LPStr)] string args);

        public static string GetPath()
        {
            IntPtr strPtr = Native_GetCurrentPath();
            string str = Marshal.PtrToStringAnsi(strPtr);
            Native_ReleaseMemory(strPtr);
            return str;
        }

        public static void Init()
        {
            Debug.Log("PyDLL Init .................");

            // register c++ log
            DebugCallback callback_delegate = new DebugCallback(DebugMethod);
            // Convert callback_delegate into a function pointer that can be used in unmanaged code.
            IntPtr intptr_delegate = Marshal.GetFunctionPointerForDelegate(callback_delegate);
            // Call the API passing along the function pointer.
            RegisterDebugCallback(intptr_delegate);

            string path = GetPath();
            Debug.Log(path);

            string program = "python36_xpy";
#if UNITY_EDITOR
            string python_home = path + "/native/xpy/external/Python-3.6.0";
            string scriptroot = path + "/Assets/Script";
#else
            // TODO:
            string python_home = "";
            string scriptroot = "";
#endif
            Python_RegisterModule();
            Python_Start(program, python_home);
            bool isEmbedded = Python_CheckInterpreter(program);
            Python_InitScript(scriptroot);
            Python_RunFunction("main", "main", "");
            Debug.Log("Python is embedded: " + isEmbedded);
        }

        public static void Destroy()
        {
            Python_Finalize();
        }
    }
}
