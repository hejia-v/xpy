using System;
using System.Runtime.InteropServices;

class MonoPInvokeCallbackAttribute : System.Attribute
{
    public MonoPInvokeCallbackAttribute(Type t) { }
}

namespace XPython
{
    public class PyEnv
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
            public IntPtr str;  // 接收到字符串数据后，需要立即释放
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
        static var[] args = new var[max_args];
        static string[] strs = new string[max_args];
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
        public static extern void Native_ReleaseMemory(out IntPtr pbuffer);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Native_GetCurrentPath();

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Python_RegisterModule();

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool Python_Start([MarshalAs(UnmanagedType.LPStr)] string program, [MarshalAs(UnmanagedType.LPStr)] string home);

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
        static extern int Python_GetFunction([MarshalAs(UnmanagedType.LPStr)] string module, [MarshalAs(UnmanagedType.LPStr)] string funcname, out int id, out IntPtr err);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        static extern int Python_CallFunction(int argc, [In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = max_args)] var[] argv,
            int strc, [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 3)] string[] strs, out IntPtr err);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        static extern int Python_CreateClassInstance([MarshalAs(UnmanagedType.LPStr)] string module, [MarshalAs(UnmanagedType.LPStr)] string classname, out int id,
            int argc, [In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = max_args)] var[] argv,
            int strc, [In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 3)] string[] strs, out IntPtr err);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        static extern int Python_GetInstanceMethod([MarshalAs(UnmanagedType.LPStr)] string method,
            [In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = max_args)] var[] argv, out IntPtr err);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        static extern int Python_SharpCollectGarbage(int n, [Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] int[] result);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int Python_RunFunction([MarshalAs(UnmanagedType.LPStr)] string pythonfile,
            [MarshalAs(UnmanagedType.LPStr)] string funcname, [MarshalAs(UnmanagedType.LPStr)] string args);

        //[DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        //public static extern bool KBE_Init();

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
            Native_ReleaseMemory(out strPtr);
            return str;
        }

        public void Init()
        {
            logger.info("PyEnv Init .................");

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
            string python_home = path + "/../../../xpy/external/Python-3.6.1";
            string scriptroot = path + "/../../../../Assets/PyScript";
#elif UNITY_EDITOR
            string python_home = path + "/Native/xpy/external/Python-3.6.1";
            string scriptroot = path + "/Assets/PyScript";
#else
            // TODO:
            string python_home = "";
            string scriptroot = "";
#endif
            Python_RegisterModule();
            bool isStart = Python_Start(program, python_home);
            if (!isStart)
            {
                throw new ArgumentException("Python start failed.");
            }
            bool isEmbedded = Python_CheckInterpreter(program);
            Python_InitScript(scriptroot);
            Python_InitSharpCall(CallSharp);
            Python_RunFunction("main", "main", "");
            logger.info("Python is embedded: " + isEmbedded);
        }

        public PyObject GetFunction(string module, string funcname)
        {
            int id = 0;
            IntPtr err = IntPtr.Zero;
            int status = Python_GetFunction(module, funcname, out id, out err);
            if (status != 0)
            {
                string e = Marshal.PtrToStringAnsi(err);
                Native_ReleaseMemory(out err);
                throw new ArgumentException(e);
            }

            return new PyObject { id = id };
        }

        int pushvalue(ref var v, object arg)
        {
            if (arg == null)
            {
                v.type = (int)var_type.NONE;
            }
            else
            {
                Type t = arg.GetType();
                if (t == typeof(int))
                {
                    v.type = var_type.INTEGER;
                    v.d = (int)arg;
                }
                else if (t == typeof(long))
                {
                    v.type = var_type.INTEGER;
                    v.d64 = (long)arg;
                }
                else if (t == typeof(float))
                {
                    v.type = var_type.REAL;
                    v.f = (float)arg;
                }
                else if (t == typeof(double))
                {
                    v.type = var_type.REAL;
                    v.f = (double)arg;
                }
                else if (t == typeof(bool))
                {
                    v.type = var_type.BOOLEAN;
                    v.d = (bool)arg ? 1 : 0;
                }
                else if (t == typeof(string))
                {
                    v.type = var_type.STRING;
                    return 2;   // string
                }
                else if (t == typeof(PyObject))
                {
                    v.type = var_type.PYTHONOBJ;
                    v.d = ((PyObject)arg).id;
                }
                else if (t.IsClass)
                {
                    v.type = var_type.SHARPOBJ;
                    v.d = objects.Query(arg);
                }
                else
                {
                    return 0;   // error
                }
            }
            return 1;
        }

        public object[] CallFunction(PyObject func, params object[] arg)
        {
            int n = arg.Length;
            if (n + 1 > max_args)
            {
                throw new ArgumentException("Too many args");
            }
            args[0].type = var_type.PYTHONOBJ;
            args[0].d = func.id;

            int sn = 0;
            for (int i = 0; i < n; i++)
            {
                int r = pushvalue(ref args[i + 1], arg[i]);
                switch (r)
                {
                    case 0:
                        throw new ArgumentException(String.Format("Unsupport type : {1} at {0}", i, arg[i].GetType()));
                    case 1:
                        break;
                    case 2:
                        // string
                        args[i + 1].d = sn;
                        strs[sn] = (string)arg[i];
                        ++sn;
                        break;
                }
            }
            IntPtr err = IntPtr.Zero;
            int retn = Python_CallFunction(n + 1, args, sn, strs, out err);
            if (retn < 0)
            {
                string e = Marshal.PtrToStringAnsi(err);
                Native_ReleaseMemory(out err);
                throw new ArgumentException(e);
            }
            if (retn == 0)
            {
                return null;
            }
            object[] ret = new object[retn];
            for (int i = 0; i < retn; i++)
            {
                switch (args[i].type)
                {
                    case var_type.NONE:
                        ret[i] = null;
                        break;
                    case var_type.INTEGER:
                        ret[i] = args[i].d;
                        break;
                    case var_type.INT64:
                        ret[i] = args[i].d64;
                        break;
                    case var_type.REAL:
                        ret[i] = args[i].f;
                        break;
                    case var_type.BOOLEAN:
                        ret[i] = (args[i].d != 0) ? true : false;
                        break;
                    case var_type.STRING:
                        // todo: encoding
                        ret[i] = Marshal.PtrToStringAnsi(args[i].str);
                        Native_ReleaseMemory(out args[i].str);
                        break;
                    case var_type.POINTER:
                        ret[i] = args[i].ptr;
                        break;
                    case var_type.PYTHONOBJ:
                        ret[i] = new PyObject { id = args[i].d };
                        break;
                    case var_type.SHARPOBJ:
                        ret[i] = objects.Get(args[i].d);
                        if (ret[i] == null)
                        {
                            throw new ArgumentException("Invalid sharp object");
                        }
                        break;
                }
            }

            return ret;
        }

        public PyObject CreateClassInstance(string module, string classname, params object[] arg)
        {
            int n = arg.Length;
            if (n > max_args)
            {
                throw new ArgumentException("Too many args");
            }

            int sn = 0;
            for (int i = 0; i < n; i++)
            {
                int r = pushvalue(ref args[i], arg[i]);
                switch (r)
                {
                    case 0:
                        throw new ArgumentException(String.Format("Unsupport type : {1} at {0}", i, arg[i].GetType()));
                    case 1:
                        break;
                    case 2:
                        // string
                        args[i].d = sn;
                        strs[sn] = (string)arg[i];
                        ++sn;
                        break;
                }
            }
            int id = 0;
            IntPtr err = IntPtr.Zero;
            int status = Python_CreateClassInstance(module, classname, out id, n, args, sn, strs, out err);
            if (status != 0)
            {
                string e = Marshal.PtrToStringAnsi(err);
                Native_ReleaseMemory(out err);
                throw new ArgumentException(e);
            }

            if (args[0].type != var_type.PYTHONOBJ)
            {
                throw new ArgumentException("not python object");
            }

            PyObject ret = new PyObject { id = args[0].d };

            return ret;  // None
        }

        public PyObject GetMethod(PyObject pyobj, string method)
        {
            pushvalue(ref args[0], pyobj);

            IntPtr err = IntPtr.Zero;
            int status = Python_GetInstanceMethod(method, args, out err);

            if (status != 0)
            {
                string e = Marshal.PtrToStringAnsi(err);
                Native_ReleaseMemory(out err);
                throw new ArgumentException(e);
            }

            if (args[0].type != var_type.PYTHONOBJ)
            {
                throw new ArgumentException("not python object");
            }

            PyObject ret = new PyObject { id = args[0].d };

            return ret;  // None
        }

        public void CollectGarbage()
        {
            const int cap = 256;
            int[] result = new int[cap];    // 256 per cycle
            int n;
            do
            {
                n = Python_SharpCollectGarbage(cap, result);
                for (int i = 0; i < n; i++)
                {
                    objects.Remove(result[i]);
                }
            } while (n < cap && n > 0);
        }

        public void Destroy()
        {
            logger.info("PyEnv Destroy .................");
            bool ret = Python_Finalize();
            if (!ret)
            {
                throw new ArgumentException("Python finalize failed.");
            }
        }
    }
}
