using System;
using System.Text;
using System.Runtime.InteropServices;

class MonoPInvokeCallbackAttribute : System.Attribute
{
    public MonoPInvokeCallbackAttribute(Type t) { }
}

namespace XPython
{
    public class PyDLL
    {
        public enum var_type
        {
            NONE = 0,
            INTEGER = 1,
            INT64 = 2,
            REAL = 3,
            BOOLEAN = 4,
            STRING = 5,
            POINTER = 6,
            PYTHONOBJ = 7,
            SHARPOBJ = 8,
        };
        public struct var
        {
            public var_type type;
            public int d;
            public long d64;
            public double f;
            public IntPtr ptr;
        };
        public struct PyObject
        {
            public int id;
        };


#if UNITY_IPHONE && !UNITY_EDITOR
        const string DLL = "__Internal";
#elif UNITY_EDITOR
        const string DLL = "xpy_d";
#else
        const string DLL = "xpy";
#endif

        const int max_args = 256;
        static SharpObject objects = new SharpObject();	

        public delegate string SharpFunction(int n, var[] argv);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        private delegate void DebugCallback(int level, string message);

        private static void DebugMethod(int level, string message)
        {
            logger.log(level, message);
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
        static extern int Python_InitSharpCall(Callback cb);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Python_GetFunction([MarshalAs(UnmanagedType.LPStr)] string module, [MarshalAs(UnmanagedType.LPStr)] string funcname, out int id);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Python_RunFunction([MarshalAs(UnmanagedType.LPStr)] string pythonfile,
            [MarshalAs(UnmanagedType.LPStr)] string funcname, [MarshalAs(UnmanagedType.LPStr)] string args);

        delegate int Callback(int argc, [In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = max_args)] var[] argv, IntPtr sud);

        [MonoPInvokeCallback(typeof(Callback))]
        static int CallSharp(int argc, [In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = max_args)] var[] argv, IntPtr sud)
        {
            try
            {
                SharpFunction f = (SharpFunction)objects.Get(argv[0].d);
                string ret = f(argc, argv);
                if (ret != null)
                {
                    // push string into P for passing C sharp string to python.
                    //if (c_pushstring(sud, ret) == 0)
                    //{
                    //    throw new ArgumentException("Push string failed");
                    //}
                }
                return (int)argv[0].type;
            }
            catch (Exception ex)
            {
                //c_pushstring(sud, ex.ToString());
                return -1;
            }
        }

        public static string GetCurrentPath()
        {
            IntPtr strPtr = Native_GetCurrentPath();
            string str = Marshal.PtrToStringAnsi(strPtr);
            Native_ReleaseMemory(strPtr);
            return str;
        }

        public static void Init()
        {
            logger.info("PyDLL Init .................");

            // register c++ log
            DebugCallback callback_delegate = new DebugCallback(DebugMethod);
            // Convert callback_delegate into a function pointer that can be used in unmanaged code.
            IntPtr intptr_delegate = Marshal.GetFunctionPointerForDelegate(callback_delegate);
            // Call the API passing along the function pointer.
            RegisterDebugCallback(intptr_delegate);

            string path = GetCurrentPath();
            logger.info(path);

            string program = "python36_xpy";
#if STANDALONE
            string python_home = path + "/../../../xpy/external/Python-3.6.0";
            string scriptroot = path + "/../../../../Assets/Script";
#elif UNITY_EDITOR
            string python_home = path + "/Native/xpy/external/Python-3.6.0";
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
            Python_InitSharpCall(null);
            Python_RunFunction("main", "main", "");
            logger.info("Python is embedded: " + isEmbedded);

            // test
            PyObject load = GetFunction("main", "load");
        }

        public static PyObject GetFunction(string module, string funcname)
        {
            int id = 0;
            IntPtr err = Python_GetFunction(module, funcname, out id);  // err will release when call Python_GetFunction next time.
            if (id != 0)
            {
                // succ 
                return new PyObject { id = id };
            }
            else
            {
                if (err == IntPtr.Zero)
                {
                    return new PyObject(); // None
                }
                else
                {
                    throw new ArgumentException(Marshal.PtrToStringAnsi(err));
                }
            }
        }

        public static void Destroy()
        {
            Python_Finalize();
        }
    }
}
