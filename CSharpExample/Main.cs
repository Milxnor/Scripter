﻿using Scripter;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using static Scripter.Main;

namespace Example
{
    static unsafe class ExampleClass
    {   
        public static void CreateConsole()
        {
            /*
            UObject* Engine = FindObject("FortEngine_");

            // while (!Engine->IsValid())
               // Engine = FindObject("FortEngine");

            Console.WriteLine("Finding Console");
            var ConsoleClass = FindObject("Class /Script/Engine.Console");
            Console.WriteLine("Console Class found & Finding GameViewport.");
            var GameViewport = (UObject**)Engine->Member("GameViewport");
            Console.WriteLine("Found GameViewport & Finding ViewportConsole.");
            UObject** ViewportConsole = (UObject**)(*GameViewport)->Member("ViewportConsole");
            Console.WriteLine("Found ViewportConsole and creating params");

            ParamBuilder Params = new ParamBuilder(true, 8).Add(ConsoleClass).Add(*GameViewport);

            Console.WriteLine("Created Params now finding GSC");

            var GSC = FindObject("GameplayStatics /Script/Engine.Default__GameplayStatics");
            Console.WriteLine("Found GSC and now finding SpawnObject.");
            var fn = FindObject("Function /Script/Engine.GameplayStatics.SpawnObject");

            Console.WriteLine("Found SpawnObject and now calling ProcessEvent");

            GSC->ProcessEvent(fn, Params.Get());

            Console.WriteLine("Called processevent and now assigning viewportconsole.");

            *ViewportConsole = *(UObject**)Params.GetReturnValue();

            Console.WriteLine("Console created!");
            */
        } 

        public static int Main(string args)
        {
            // Logger.Log("C#: " + FindObject("FortEngine_")->GetFullName());
            Logger.Log("Hello from C#!");
            
            // CreateConsole();

            return 0;
        }
    }
}