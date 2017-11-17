using System;
using System.Collections.Generic;
using System.Reflection;
using System.Threading;
using UnityEngine;
using XPython;

namespace sharp
{
    class Program
    {
        static bool enableLoop = true;
        const float DefaultFrameLength = (1.0f / 60.0f) * 1000;
        const float FixFrameLength = (1.0f / 10.0f) * 1000;
        static int logLevel = 0;
        static bool running = true;
        static Thread input_thread = new Thread(new ThreadStart(HandInput));
        static List<String> commands = new List<String>();
        static List<UnityEngine.MonoBehaviour> entities = new List<UnityEngine.MonoBehaviour>();
        static List<Dictionary<String, MethodInfo>> behaviour_methods = new List<Dictionary<String, MethodInfo>>();
        static List<LoopTask> task_list = new List<LoopTask>();

        public struct MethodInfo
        {
            public object obj;
            public System.Reflection.MethodInfo method;
        };

        static void CacheMonoBehaviourMethod(object obj)
        {
            String[] methodnames = { "Awake", "Start", "Update", "OnGUI", "FixedUpdate", "OnDestroy" };
            Dictionary<String, MethodInfo> methods = new Dictionary<string, MethodInfo>();
            foreach (String methodname in methodnames)
            {
                MethodInfo info = new MethodInfo();
                info.obj = obj;
                info.method = obj.GetType().GetMethod(methodname, BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.Public);
                if (info.method==null)
                {
                    info.method = obj.GetType().BaseType.GetMethod(methodname, BindingFlags.Instance | BindingFlags.NonPublic | BindingFlags.Public);
                }
                methods[methodname] = info;
            }
            behaviour_methods.Add(methods);
        }

        static void HandInput()
        {
            while (true)
            {
                string buf = Console.ReadLine();
                logger.debug("enter command: " + buf);
                Monitor.Enter(commands);
                commands.Add(buf);
                Monitor.Exit(commands);
            }
        }

        static void RunMonoBehaviourMethod(string method)
        {
            foreach (Dictionary<String, MethodInfo> methods in behaviour_methods)
            {
                methods[method].method.Invoke(methods[method].obj, null);
            }
        }

        static void Init()
        {
            MonoBehaviour clientAppPy = new ClientAppPy();
            entities.Add(clientAppPy);
            CacheMonoBehaviourMethod(clientAppPy);

            RunMonoBehaviourMethod("Awake");
            RunMonoBehaviourMethod("Start");
        }

        class LoopTask : IComparable<LoopTask>
        {
            public int max_times = 0;
            int count = 0;
            double deadline = 0;
            double frameLength = 0;

            public int GetRemainTime()
            {
                return (int)((deadline - long.Parse(DateTime.Now.ToFileTime().ToString())) / 10000000.0f * 1000.0f);
            }

            public void Update()
            {
                if (max_times>0)
                {
                    ++count;
                }
                deadline = long.Parse(DateTime.Now.ToFileTime().ToString()) + frameLength / 1000.0f * 10000000.0f;
            }

            public virtual void Do()
            {

            }

            public bool Finished()
            {
                if (max_times>0)
                {
                    return count >= max_times;
                }
                return false;
            }

            public void SetFrameLength(double fl)
            {
                frameLength = fl;
            }

            public int CompareTo(LoopTask other)
            {
                if (this.deadline == other.deadline)
                {
                    return 0;
                }
                else if (this.deadline > other.deadline)
                {
                    return 1;
                }
                else if (this.deadline < other.deadline)
                {
                    return -1;
                }
                throw new NotImplementedException();
            }
        }

        class UpdateTask:LoopTask
        {
            public UpdateTask()
            {
                SetFrameLength((1.0f / 60.0f) * 1000);
            }

            public override void Do()
            {
                //logger.info("333333333");
                RunMonoBehaviourMethod("Update");
            }
        }

        class FixUpdateTask : LoopTask
        {
            public FixUpdateTask()
            {
                SetFrameLength((1.0f / 10.0f) * 1000);
            }

            public override void Do()
            {
                //logger.info("44444");
                RunMonoBehaviourMethod("FixedUpdate");
            }
        }

        static void Loop()
        {
            if (!enableLoop)
            {
                return;
            }

            input_thread.IsBackground = true;
            input_thread.Start();

            task_list.Add(new UpdateTask());
            task_list.Add(new FixUpdateTask());

            //need optimization
            task_list.Sort();
            while (running)
            {
                Monitor.Enter(commands);
                foreach (String s in commands)
                {
                    if (s.Trim() == "exit")
                    {
                        running = false;
                    }
                    logger.info("handle commands: " + s);
                }
                commands.Clear();
                Monitor.Exit(commands);

                foreach (LoopTask task in task_list)
                {
                    if (task.GetRemainTime() <= 0)
                    {
                        task.Do();
                        task.Update();
                    }
                }
                task_list.RemoveAll(t => t.Finished());
                if (task_list.Count == 0)
                {
                    break;
                }
                task_list.Sort();
                int lag = task_list[0].GetRemainTime();
                Thread.Sleep(lag);
            }   
        }

        static void Destroy()
        {
            RunMonoBehaviourMethod("OnDestroy");
        }

        static void Main(string[] args)
        {
            bool simple_test = true;
            if (simple_test)
            {
                BasePyTest.TestBasePyFunctions();
                //BasePyTest.TestBasePyFunctions();
            }
            else
            {
                Init();
                Loop();
                Destroy();
            }
        }
    }
}
