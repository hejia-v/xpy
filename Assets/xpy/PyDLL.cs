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

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern void native_release_memory(IntPtr pbuffer);

        [DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr native_get_current_path();

        public static string GetPath()
        {
            IntPtr strPtr = native_get_current_path();
            string str = Marshal.PtrToStringAnsi(strPtr);
            native_release_memory(strPtr);
            return str;
        }

        public static void Init()
        {
            Debug.Log("PyDLL Init .................");

            string str = GetPath();
            Debug.Log(str);

            //StringBuilder sb = new StringBuilder(128);
            //bool bSucc = get_path(sb, sb.Capacity);
            //while (!bSucc)
            //{
            //    if (Marshal.GetLastWin32Error() != 0x7A)
            //    {
            //        // throw
            //    }

            //    // Marshal.GetLastWin32Error() == ERROR_INSUFFICIENT_BUFFER
            //    sb.Capacity *= 2;
            //}
            //Debug.Log(bSucc);
            //Debug.Log(sb.ToString());

        }

    }
}





//[DllImport(DLL)]
//private static extern int add(int x, int y);

//[DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
//public static extern bool get_path(IntPtr buffer, [MarshalAs(UnmanagedType.U4)] int bufferSize);

//[DllImport(DLL, CallingConvention = CallingConvention.Cdecl)]
//public static extern bool get_path1(StringBuilder buffer, [MarshalAs(UnmanagedType.U4)] int bufferSize);

//public void my_function(ref string data)
//{
//    // allocate room on the stack
//    IntPtr buffer = (IntPtr)stackalloc byte[4000];
//    // convert the managed string into an ASCII byte[]
//    byte[] data_buf = Encoding.ASCII.GetBytes(data);
//    // check for out-of-bounds
//    if (data_buf.Length > (4000 - 1))
//    {
//        throw new Exception("input too large for fixed size buffer");
//    }
//    // .. then copy the bytes
//    Marshal.Copy(data_buf, 0, buffer, data_buf.Length);
//    Marshal.WriteByte(buffer + data_buf.Length, 0); // terminating null

//    get_path(buffer, 4000);
//    // after the call, marshal the bytes back out
//    data = Marshal.PtrToStringAnsi(buffer);
//}