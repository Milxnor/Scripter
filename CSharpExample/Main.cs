using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using static Scripter.Main;

namespace Example
{
    static unsafe class ExampleClass
    {

        public static int Main(String args)
        {
            Console.WriteLine("Engine: " + FindObject("FortEngine_")->GetFullName());

            return 0;
        }
    }
}