using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#if !STANDALONE
using UnityEngine;
#endif

namespace XPython
{
    class logger
    {
        public static void log(int level, string msg)
        {
            switch (level)
            {
                case 1:
                    debug(msg);
                    break;
                case 2:
                    info(msg);
                    break;
                case 3:
                    warning(msg);
                    break;
                case 4:
                    error(msg);
                    break;
                default:
                    debug(msg);
                    break;
            }
        }

        public static void debug(string msg)
        {
#if STANDALONE
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.DarkGray;
            Console.WriteLine(msg);
            Console.ResetColor();
#else
            Debug.Log("xpy: " + msg);
#endif
        }

        public static void info(string msg)
        {
#if STANDALONE
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.DarkGreen;
            Console.WriteLine(msg);
            Console.ResetColor();
#else
            Debug.Log("xpy: " + msg);
#endif
        }

        public static void warning(string msg)
        {
#if STANDALONE
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(msg);
            Console.ResetColor();
#else
            Debug.LogWarning("xpy: " + msg);
#endif
        }

        public static void error(string msg)
        {
#if STANDALONE
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine(msg);
            Console.ResetColor();
#else
            Debug.LogError("xpy: " + msg);
#endif
        }
    }
}
