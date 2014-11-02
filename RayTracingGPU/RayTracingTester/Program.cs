using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Configuration;
using System.Diagnostics;
using System.Windows.Forms;

namespace RayTracingTester
{
	class Program
	{
		static string RayTracerFilePath = System.Configuration.ConfigurationManager.AppSettings["RayTracerFilePath"];
		static string SceneConfigFilePath = System.Configuration.ConfigurationManager.AppSettings["SceneConfigFilePath"];
		static int StartupWarmupTime = int.Parse(System.Configuration.ConfigurationManager.AppSettings["StartupWarmupTime"]);
		static int RTDepthStart = 0;
		static int RTDepthEnd = 5;

		static void Main(string[] args)
		{
			//TestRTDepthGPUAndCPU(@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfig-Bunny3.json", 3000, 0, RTDepthStart, RTDepthEnd);

			//TestRTWindowsSize(@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfig-Sphere.json", 20000, 100, 50, 20);

			//TestRTSet(new string[] {
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-1.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-2.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-3.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-4.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-5.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-6.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-7.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-8.json",
			//		@"C:\Users\Mateusz\Desktop\mgr\proj\Qt\git\RayTracingGPU\RayTracingGPU\SceneConfigs\Elements\SceneConfig-Sphere2-9.json"
			//	},
			//	15000);

			TestRTOne("", 30000);
		}
		static void TestRTOne(string sceneConfigFilePath, int gpuTimeInterval)
		{
			Process proc = null;
			try
			{
				System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo(RayTracerFilePath);
				si.WorkingDirectory = System.IO.Path.GetDirectoryName(RayTracerFilePath);
				si.UseShellExecute = false;

				si.Arguments = string.Empty;
				//ustawianie pliku konfiguracyjnego
				if (!string.IsNullOrEmpty(sceneConfigFilePath))
					si.Arguments = "-c " + sceneConfigFilePath;
				//ustawianie wielkosci ekranu
				si.Arguments = string.Format("{0} -w {1} -h {1}", si.Arguments, 400);

				proc = Process.Start(si);

				//czas ładowania 
				Thread.Sleep(StartupWarmupTime);

				//przełączenie na GPU
				SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F1);

				//test
				_TestRTOneProbe(proc, gpuTimeInterval);

				//zamkniecie procesu
				proc.Kill();
				Thread.Sleep(2000);
				proc = null;
			}
			finally
			{
				if (proc != null)
				{
					proc.CloseMainWindow();
				}
			}
		}

		static void TestRTSet(string[] sceneConfigFilePathArray, int gpuTimeInterval)
		{
			Process proc = null;
			try
			{
				System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo(RayTracerFilePath);
				si.WorkingDirectory = System.IO.Path.GetDirectoryName(RayTracerFilePath);
				si.UseShellExecute = false;

				foreach (var sceneConfigFilePath in sceneConfigFilePathArray)
				{
					si.Arguments = string.Empty;
					//ustawianie pliku konfiguracyjnego
					if (!string.IsNullOrEmpty(sceneConfigFilePath))
						si.Arguments = "-c " + sceneConfigFilePath;
					//ustawianie wielkosci ekranu
					si.Arguments = string.Format("{0} -w {1} -h {1}", si.Arguments, 400);

					proc = Process.Start(si);

					//czas ładowania 
					Thread.Sleep(StartupWarmupTime);

					//przełączenie na GPU
					SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F1);

					//test
					_TestRTOneProbe(proc, gpuTimeInterval);

					//zamkniecie procesu
					proc.Kill();
					Thread.Sleep(2000);
				}
				proc = null;
			}
			finally
			{
				if (proc != null)
				{
					proc.CloseMainWindow();
				}
			}
		}
		static void TestRTWindowsSize(string sceneConfigFilePath, int gpuTimeInterval, int sizeStart, int sizeInterval, int sizeCount)
		{
			Process proc = null;
			try
			{
				int size = sizeStart;
				System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo(RayTracerFilePath);
				si.WorkingDirectory = System.IO.Path.GetDirectoryName(RayTracerFilePath);
				si.UseShellExecute = false;

				for (int i = 0; i < sizeCount; i++)
				{
					si.Arguments = string.Empty;
					//ustawianie pliku konfiguracyjnego
					if (!string.IsNullOrEmpty(sceneConfigFilePath))
						si.Arguments = "-c " + sceneConfigFilePath;
					//ustawianie wielkosci ekranu
					si.Arguments = string.Format("{0} -w {1} -h {1}", si.Arguments, size);

					si.WindowStyle = ProcessWindowStyle.Minimized;
					proc = Process.Start(si);

					//czas ładowania 
					Thread.Sleep(StartupWarmupTime);

					//przełączenie na GPU
					SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F1);

					//test
					_TestRTOneProbe(proc, gpuTimeInterval);

					//zamkniecie procesu
					proc.Kill();
					Thread.Sleep(2000);

					//zwiekszenie ekranu
					size += sizeInterval;
				}
				proc = null;
			}
			finally
			{
				if (proc != null)
				{
					proc.CloseMainWindow();
				}
			}
		}
		/// <summary>
		/// Test głębokości algorytmu odpalany kolejno na CPU oraz GPU.
		/// </summary>
		static void TestRTDepthGPUAndCPU(string sceneConfigFilePath, int gpuTimeInterval, int cpuTimeInterval, int startDepth, int endDepth)
		{
			Process proc = null;

			try
			{
				//1. odpalić aplikację z wykorzystaniem pliku configa
				//2. Po odpowiednim czasie poprzez wyzwolenie klawisza zapisać: liczbę trójkątów na scenie, liczbę wierzchołków, liczbę świateł, głębogośc RT, fps, łączną liczbę klatek
				//3. Zwiększyć liczbę głębokości (klawiszem) -> zebrac wyniki
				//4. Zmienić tryb RT -> zebrac wyniki

				System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo(RayTracerFilePath);
				si.Arguments = "-c " + sceneConfigFilePath;
				si.WorkingDirectory = System.IO.Path.GetDirectoryName(RayTracerFilePath);
				si.UseShellExecute = false;
				proc = Process.Start(si);

				//czas ładowania 
				Thread.Sleep(StartupWarmupTime);


				//SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F1);
				//return;

				//przełączenie na GPU
				if (gpuTimeInterval > 0)
				{
					//SendKeyToProcess(proc.MainWindowHandle, "{F1}");
					SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F1);
					TestRTDepth(proc, gpuTimeInterval, startDepth, endDepth);
				}

				if (cpuTimeInterval > 0)
				{
					//przełączenie na CPU
					//SendKeyToProcess(proc.MainWindowHandle, "{F2}");
					SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F2);
					TestRTDepth(proc, cpuTimeInterval, startDepth, endDepth);
				}
			}
			finally
			{
				if (proc != null)
				{
					proc.CloseMainWindow();
				}
			}
		}
		/// <summary>
		/// Testy RT dla każdego z poziomu głębokości.
		/// </summary>
		static void TestRTDepth(Process proc, int timeInterval, int startDepth, int endDepth)
		{
			for (int depth = startDepth; depth <= endDepth; depth++)
			{
				//SendKeyToProcess(proc.MainWindowHandle, ((char)('0' + depth)).ToString());
				SendKeyToProcess(proc.MainWindowHandle, ('0' + depth));
				_TestRTOneProbe(proc, timeInterval);
			}
		}

		/// <summary>
		/// Pojedynczy test.
		/// </summary>
		static void _TestRTOneProbe(Process proc, int timeInterval)
		{
			//reset statystyk
			//SendKeyToProcess(proc.MainWindowHandle, "{DEL}");
			SendKeyToProcess(proc.MainWindowHandle, (int)Keys.Delete);
			Thread.Sleep(timeInterval);
			//wywołanie zrzutu danych
			//SendKeyToProcess(proc.MainWindowHandle, "{F11}");
			SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F11);
			Thread.Sleep(StartupWarmupTime);
		}

		static void SendKeyToProcess(IntPtr hwnd, string keys)
		{
			User32Ext.SetForegroundWindow(hwnd);
			SendKeys.SendWait(keys);
			SendKeys.Flush();
		}
		static void SendKeyToProcess(IntPtr hwnd, int key)
		{
			const UInt32 WM_CHAR = 0x0102;
			User32Ext.SendMessage(hwnd, WM_CHAR, key, 0);
		}
		static void SendSpecialKeyToProcess(IntPtr hwnd, int key)
		{
			const UInt32 WM_KEYDOWN = 0x0100;
			User32Ext.SendMessage(hwnd, WM_KEYDOWN, key, 0);
		}
	}



}
