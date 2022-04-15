using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using static Scripter.Main;

namespace Example
{
    static unsafe class ExampleClass
    {
        /*
        
        public static void CreateConsole()
        {
            UObject* Engine = FindObject("FortEngine_");

            while (!Engine->IsValid())
                Engine = FindObject("FortEngine");

            var ConsoleClass = FindObject("Class /Script/Engine.Console");
            var GameViewport = (UObject**)Engine->Member("GameViewport");
            UObject** ViewportConsole = (UObject**)(*GameViewport)->Member("ViewportConsole");

            ParamBuilder Params = new ParamBuilder(true, 8).Add(ConsoleClass).Add(*GameViewport);

            var GSC = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
            var fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");

            GSC->ProcessEvent(fn, Params.Get());

            *ViewportConsole = *(UObject**)Params.GetReturnValue();

            Console.WriteLine("Console created!");
        } 

        */

        public static int Main(String args)
        {
            // Console.WriteLine("Engine: " + FindObject("FortEngine_")->GetFullName());

            CreateConsole();

            return 0;
        }
    }
}