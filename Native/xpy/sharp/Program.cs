using sharp.test;
using System.Collections.Generic;

namespace sharp
{
    class Program
    {
        enum TestType
        {
            BaseFunc = 1 << 0,
            MonoFake = 1 << 1,
        }

        static void Main(string[] args)
        {
            TestType testFlag = TestType.BaseFunc;

            // TODO: 用反射获取TestType的所有枚举值
            List<TestType> testTypes = new List<TestType>() { TestType.BaseFunc };

            foreach (TestType typeFlag in testTypes)
            {
                switch (typeFlag & testFlag)
                {
                    case TestType.BaseFunc:
                        BasePyTest.TestBasePyFunctions();
                        break;
                    case TestType.MonoFake:
                        MonoBehaviourFakeTest.run();
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
