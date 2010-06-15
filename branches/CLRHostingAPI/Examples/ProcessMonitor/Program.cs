using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Text;
using EasyHook;

namespace ProcessMonitor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //try
            //{
            //    Config.Register(
            //        "A simple ProcessMonitor based on EasyHook!",
            //        "ProcMonInject.dll",
            //        "ProcessMonitor.exe");
            //}
            //catch (ApplicationException)
            //{
            //    MessageBox.Show("This is an administrative task!", "Permission denied...", MessageBoxButtons.OK);
			//
            //    System.Diagnostics.Process.GetCurrentProcess().Kill();
            //}

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
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
			name = AssemblyName.GetAssemblyName("ProcMonInject.dll");
			assemblies[1] = new RhAssemblyInfo
                            {
								FullName = name.FullName + ", ProcessorArchitecture=" + name.ProcessorArchitecture,
								AssemblyLoadPath = name.CodeBase.Substring(@"file:\\\".Length)
                            };
			name = AssemblyName.GetAssemblyName("ProcessMonitor.exe");
			assemblies[2] = new RhAssemblyInfo
                            {
								FullName = name.FullName + ", ProcessorArchitecture=" + name.ProcessorArchitecture,
								AssemblyLoadPath = name.CodeBase.Substring(@"file:\\\".Length)
                            };
			return assemblies;
        }
    }
}
