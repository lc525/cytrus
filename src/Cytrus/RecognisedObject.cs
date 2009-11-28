using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace cytrus.managed
{
    public class RecognisedObject : INotifyPropertyChanged
    {
        private int _recognitionCertainty;
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

        public int RecognitionCertainty {
            get
            {
                return _recognitionCertainty;
            }
            set
            {
                _recognitionCertainty = value;
                NotifyPropertyChanged("RecognitionCertainty");
            }
        }

        public RecognisedObject(string name, int certainty)
        {
            ObjectName = name;
            RecognitionCertainty = certainty;
        }

        private void NotifyPropertyChanged(String info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }


        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion
    }
}
