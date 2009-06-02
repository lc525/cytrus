using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CytrusManagedLib;

namespace CytrusConsole
{
    class Program
    {
        static void Main(string[] args)
        {
            Class1 c=new Class1();

            Console.Write(c.getU()+1);
        }
    }
}
