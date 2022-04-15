using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Scripter
{
    unsafe class Main
    {
        public static bool bHasInitialized = false; // This is so the dll doesn't inject twice.
        public const string ScripterDLL = @"Scripter.dll";

        [StructLayout(LayoutKind.Sequential)]
        public struct FName
        {
            uint ComparisonIndex;
            uint Number;
        }
        
        [DllImport(ScripterDLL, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern sbyte* GetFullName(UObject* obj);

        [DllImport(ScripterDLL, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern IntPtr Member(UObject* Object, string MemberName);

        [DllImport(ScripterDLL, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern void ProcessEvent(UObject* Object, UObject* Function, IntPtr Params);

        // TODO: Add TArray

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct UObject
        {
            void** VFTable;
            int ObjectFlags;
            int InternalIndex;
            UObject* ClassPrivate;
            FName NamePrivate;
            UObject* OuterPrivate;
            public UObject* GetPtrToSelf() // thank ender
            {
                fixed (UObject* Ptr = &this)
                    return Ptr;
            }
            public string GetFullName()
            {
                return new string(Main.GetFullName(GetPtrToSelf()));
            }

            // public T* Member<T>(string MemberName)
            public IntPtr Member(string MemberName)
            {
                // return (T*)Main.Member(GetPtrToSelf(), MemberName);
                return (IntPtr)Main.Member(GetPtrToSelf(), MemberName);
            }

            public bool IsValid()
            {
                return false; // TODO: convert to intptr and check if its intptr.zero
            }

            public void ProcessEvent(UObject* Function, IntPtr Params)
            {
                Main.ProcessEvent(GetPtrToSelf(), Function, Params);
            }
        }

        [DllImport(ScripterDLL, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern UObject* FindObject(string name);

        public static int Startup(string args) // This gets called whenever the dll is loaded.
        {
            if (bHasInitialized)
                return 0;

            // Put your startup code here.

            // mine is:
            Example.ExampleClass.Main(args);

            bHasInitialized = true;

            System.Threading.Thread.Sleep(-1);

            return 0;
        }
    }
}
