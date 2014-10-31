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
		static int RTDepthStart = 0;
		static int RTDepthEnd = 2;

		static void Main(string[] args)
		{
			TestRT(SceneConfigFilePath, 10000, 60000);
		}
		static void TestRT(string sceneConfigFilePath, int gpuTimeInterval, int cpuTimeInterval)
		{
			Process proc = null;

			try
			{
				//1. odpalić aplikację z wykorzystaniem pliku configa
				//2. Po odpowiednim czasie poprzez wyzwolenie klawisza zapisać: liczbę trójkątów na scenie, liczbę wierzchołków, liczbę świateł, głębogośc RT, fps, łączną liczbę klatek
				//3. Zwiększyć liczbę głębokości (klawiszem) -> zebrac wyniki
				//4. Zmienić tryb RT -> zebrac wyniki

				System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo(RayTracerFilePath);
				si.Arguments = sceneConfigFilePath;
				si.WorkingDirectory = System.IO.Path.GetDirectoryName(RayTracerFilePath);
				si.UseShellExecute = false;
				proc = Process.Start(si);

				//czas ładowania 
				Thread.Sleep(2000);


				//SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F1);
				//return;

				//przełączenie na GPU
				//SendKeyToProcess(proc.MainWindowHandle, "{F1}");
				SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F1);
				TestRTDepth(proc, gpuTimeInterval, RTDepthStart, RTDepthEnd);

				//przełączenie na CPU
				//SendKeyToProcess(proc.MainWindowHandle, "{F2}");
				SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F2);
				TestRTDepth(proc, cpuTimeInterval, RTDepthStart, RTDepthEnd);
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
				TestRTOnce(proc, timeInterval);
			}
		}
		/// <summary>
		/// Pojedynczy test.
		/// </summary>
		static void TestRTOnce(Process proc, int timeInterval)
		{
			//reset statystyk
			//SendKeyToProcess(proc.MainWindowHandle, "{DEL}");
			SendKeyToProcess(proc.MainWindowHandle, (int)Keys.Delete);
			Thread.Sleep(timeInterval);
			//wywołanie zrzutu danych
			//SendKeyToProcess(proc.MainWindowHandle, "{F11}");
			SendSpecialKeyToProcess(proc.MainWindowHandle, (int)Keys.F11);
			Thread.Sleep(timeInterval);
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
