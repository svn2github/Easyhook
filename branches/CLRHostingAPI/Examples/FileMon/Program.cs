using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.Remoting;
using System.Text;
using System.IO;
using EasyHook;
using System.Windows.Forms;

namespace FileMon
{
    public class FileMonInterface : MarshalByRefObject
    {
        public void IsInstalled(Int32 InClientPID)
        {
            Console.WriteLine("FileMon has been installed in target {0}.\r\n", InClientPID);
        }

        public void OnCreateFile(Int32 InClientPID, String[] InFileNames)
        {
            for (int i = 0; i < InFileNames.Length; i++)
            {
                Console.WriteLine(InFileNames[i]);
            }
        }

        public void ReportException(Exception InInfo)
        {
            Console.WriteLine("The target process has reported an error:\r\n" + InInfo.ToString());
        }

        public void Ping()
        {
        }
    }

    class Program
    {
        static String ChannelName = null;

        static void Main(string[] args)
        {
            Int32 TargetPID = 0;

            if ((args.Length != 1) || !Int32.TryParse(args[0], out TargetPID))
            {
                Console.WriteLine();
                Console.WriteLine("Usage: FileMon %PID%");
                Console.WriteLine();

                return;
            }

            try
            {
                //try
                //{
                //    Config.Register(
                //        "A FileMon like demo application.",
                //        "FileMon.exe",
                //        "FileMonInject.dll");
                //}
                //catch (ApplicationException)
                //{
                //    MessageBox.Show("This is an administrative task!", "Permission denied...", MessageBoxButtons.OK);

                //    System.Diagnostics.Process.GetCurrentProcess().Kill();
                //}

                RemoteHooking.IpcCreateServer<FileMonInterface>(ref ChannelName, WellKnownObjectMode.SingleCall);
                
                RemoteHooking.Inject(
                    TargetPID,
                    "FileMonInject.dll",
                    "FileMonInject.dll",
                    GetSharedAssemblies(),
                    ChannelName);
                
                Console.ReadLine();
            }
            catch (Exception ExtInfo)
            {
                Console.WriteLine("There was an error while connecting to target:\r\n{0}", ExtInfo.ToString());
            }
        }

        public static RhAssemblyInfo[] GetSharedAssemblies()
        {
          // Example of a complete fullname is:
          // easyhook, version=2.5.0.0, culture=neutral, publickeytoken=4b580fca19d0b0c5, processorarchitecture=msil
          RhAssemblyInfo[] assemblies = new RhAssemblyInfo[3];
          var name = AssemblyName.GetAssemblyName("EasyHook.dll");
          assemblies[0] = new RhAssemblyInfo
          {
            FullName = name.FullName + ", ProcessorArchitecture=" + name.ProcessorArchitecture,
            AssemblyLoadPath = name.CodeBase.Substring(@"file:\\\".Length)
          };
          name = AssemblyName.GetAssemblyName("FileMonInject.dll");
          assemblies[1] = new RhAssemblyInfo
          {
            FullName = name.FullName + ", ProcessorArchitecture=" + name.ProcessorArchitecture,
            AssemblyLoadPath = name.CodeBase.Substring(@"file:\\\".Length)
          };
          name = AssemblyName.GetAssemblyName("FileMon.exe");
          assemblies[2] = new RhAssemblyInfo
          {
            FullName = name.FullName + ", ProcessorArchitecture=" + name.ProcessorArchitecture,
            AssemblyLoadPath = name.CodeBase.Substring(@"file:\\\".Length)
          };
          return assemblies;
        }
    }
}