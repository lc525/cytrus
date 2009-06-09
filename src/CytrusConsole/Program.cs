using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CytrusManagedLib;

namespace CytrusConsole
{
    class Program
    {
        static int val;
        static void Main(string[] args)
        {
            val = 0;
            Class1 c=new Class1();
            Console.WriteLine("DEBUG: Choose one input device:");
            int i = 1;
            foreach(string s in c.cList){
                Console.Write("\t"+i+". ");
                Console.WriteLine(s);
                i++;
            }
            Console.WriteLine("");
            Console.Write("choice:>");
            string answer = Console.ReadLine();
            int u=Int32.Parse(answer);
            c.selectCamera(u);
            c.onImageAvailable += new CaptureCallbackProc(myFunc);
            c.runAlg();
            Console.ReadLine();
            Console.WriteLine(val);
            c.stopCapture();
        }

        public static void myFunc(int pbSize, byte[] pbData)
        {
            if (val == 0) Console.WriteLine("Started");
            val++;
        }
        
    }
}
