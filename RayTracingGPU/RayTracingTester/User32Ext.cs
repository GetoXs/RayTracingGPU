using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace RayTracingTester
{
    static class User32Ext
    {
        [DllImport("user32.dll")]
        public static extern int SetForegroundWindow(IntPtr hWnd);

		[DllImport("user32.dll")]
		public static extern bool PostMessage(IntPtr hWnd, UInt32 Msg, int wParam, int lParam);
		[DllImport("user32.dll")]
		public static extern bool SendMessage(IntPtr hWnd, UInt32 Msg, int wParam, int lParam);
    }
}
