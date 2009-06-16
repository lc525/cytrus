using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace cytrus.managed
{
    public class RecognisedObject
    {
        public string ObjectName { get; set; }
        public int RecognitionCertainty { get; set; }

        public RecognisedObject(string name, int certainty)
        {
            ObjectName = name;
            RecognitionCertainty = certainty;
        }
    }
}
