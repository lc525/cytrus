using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using CytrusManagedLib;
using System.Collections.ObjectModel;
using System.Windows.Interop;
using System.IO;
using System.Windows.Threading;

namespace Cytrus
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        private Class1 c;
        static bool isCapturing;
        
        public Window1()
        {
            InitializeComponent();
        }

        private void CytrusMain_Loaded(object sender, RoutedEventArgs e)
        {
            c=new Class1();
            cameraList.ItemsSource = c.cList;
            c.selectCamera(0);
            c.onNewImageAvailable += new NewImageCallback(c_onNewImageAvailable);
            isCapturing = false;
        }

        private void RefreshCameraList_Click(object sender, RoutedEventArgs e)
        {
            c.refreshCameraList();
            cameraList.SelectedIndex = 0;
        }

        private void DriverOptions_Click(object sender, RoutedEventArgs e)
        {
            HwndSource p=(HwndSource)PresentationSource.FromVisual(this);
            c.displayDriverProperties(p.Handle);
        }

        private void cameraList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb=(ComboBox)e.Source;
            if(cb.SelectedIndex>=0)
                c.selectCamera(cb.SelectedIndex);
        }

        private void StartCapture_Click(object sender, RoutedEventArgs e)
        {
            if (!isCapturing)
            {
                isCapturing = true;
                StartCapture.Content = "Stop Capture";
                c.runAlg();
            }
            else
            {
                isCapturing = false;
                StartCapture.Content = "Start Capture";
                c.stopCapture();
            }
        }

        void c_onNewImageAvailable()
        {
            this.Dispatcher.Invoke(DispatcherPriority.Normal, (Action)(() =>
            {
                MemoryStream ms = new MemoryStream();

                c.capturedImg.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);

                System.Windows.Media.Imaging.BitmapImage bImg = new System.Windows.Media.Imaging.BitmapImage();

                bImg.BeginInit();

                bImg.StreamSource = new MemoryStream(ms.ToArray());

                bImg.EndInit();
                captureImg.Source = bImg;
                captureImg.InvalidateVisual();
            }));
            
        }
    }
}
