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
        static int RTDepthStart = 0;
        static int RTDepthEnd = 2;

        static void Main(string[] args)
        {
            TestRT("", 3000, 3000);
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


                string fullRTPath = RayTracerFilePath + " " + sceneConfigFilePath;
                System.Diagnostics.ProcessStartInfo si = new System.Diagnostics.ProcessStartInfo(fullRTPath);
                si.WorkingDirectory = System.IO.Path.GetDirectoryName(RayTracerFilePath);
                si.UseShellExecute = false;
                proc = Process.Start(si);

                //czas ładowania 
                Thread.Sleep(5000);
                //przełączenie na GPU
                SendKeyToProcess(proc.MainWindowHandle, "{F1}");
                TestRTDepth(proc, gpuTimeInterval, RTDepthStart, RTDepthEnd);

                //przełączenie na CPU
                SendKeyToProcess(proc.MainWindowHandle, "{F2}");
                TestRTDepth(proc, cpuTimeInterval, RTDepthStart, RTDepthEnd);


            }
            finally
            {
                if (proc != null)
                    proc.Kill();
            }
        }
        /// <summary>
        /// Testy RT dla każdego z poziomu głębokości.
        /// </summary>
        static void TestRTDepth(Process proc, int timeInterval, int startDepth, int endDepth)
        {
            for (int depth = startDepth; depth <= endDepth; depth++)
            {
                SendKeyToProcess(proc.MainWindowHandle, ('0' + depth).ToString());
                TestRTOnce(proc, timeInterval);
            }
        }
        /// <summary>
        /// Pojedynczy test.
        /// </summary>
        static void TestRTOnce(Process proc, int timeInterval)
        {
            //reset statystyk
            SendKeyToProcess(proc.MainWindowHandle, "{DEL}");
            Thread.Sleep(timeInterval);
            //wywołanie zrzutu danych
            SendKeyToProcess(proc.MainWindowHandle, "{F11}");
        }

        static void SendKeyToProcess(IntPtr hwnd, string keys)
        {
            User32Ext.SetForegroundWindow(hwnd);
            SendKeys.SendWait(keys);
            //User32Ext.SetForegroundWindow(System.Diagnostics.Process.GetCurrentProcess().MainWindowHandle);
        }
    }
}
