using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace cytrus.managed
{
    public class RecognisedObject
    {
        public string ObjectName { get; set; }
        public string ShortName {
            get
            {
                if (ObjectName.Length >= 11)
                    return ObjectName.Substring(0, 7)+"...";
                else
                    return ObjectName;
            }
        }
        public int RecognitionCertainty { get; set; }

        public RecognisedObject(string name, int certainty)
        {
            ObjectName = name;
            RecognitionCertainty = certainty;
        }
    }
}
