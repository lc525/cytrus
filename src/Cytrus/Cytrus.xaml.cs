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
using cytrus.managed;
using System.Collections.ObjectModel;
using System.Windows.Interop;
using System.IO;
using System.Windows.Threading;
using Tomers.WPF.Imaging.Demo;

namespace cytrus.managed
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        private CameraMgr cameraManager;
        static bool isCapturing;
        private FrameRenderer _frameRenderer = FrameRenderer.Null;
        private ObservableCollection<RecognisedObject> _rObj = new ObservableCollection<RecognisedObject>();
        
        public Window1()
        {
            //temporary
            _rObj.Add(new RecognisedObject("Mouse", 50));
            _rObj.Add(new RecognisedObject("Human Face", 20));
            //

            InitializeComponent();
        }

        public ObservableCollection<RecognisedObject> RecognisedObjects
        { get { return _rObj; } }

        private void CytrusMain_Loaded(object sender, RoutedEventArgs e)
        {
            cameraManager=new CameraMgr();
            cameraList.ItemsSource = cameraManager.getCameraList();
            outputM.ItemsSource = cameraManager.getOutputModesList();
            cameraManager.setActiveOutputMode(0);
            cameraManager.selectCamera(0);

            cameraManager.onNewImageAvailable += new ImageCaptureCallback(c_onNewImageAvailable);
            //cameraManager.onOutputModeChange += new OutputModeCallback(cameraManager_onOutputModeChange);
            isCapturing = false;
        }


        void c_onNewImageAvailable(byte[] pbData)
        {
            this.Dispatcher.Invoke(DispatcherPriority.Normal, (Action)(() =>
            {
                _frameRenderer.RenderFrame(pbData);
            }));
        }

        private void RefreshCameraList_Click(object sender, RoutedEventArgs e)
        {
            cameraManager.refreshCameraList();
            cameraList.SelectedIndex = 0;
        }

        private void DriverOptions_Click(object sender, RoutedEventArgs e)
        {
            HwndSource p=(HwndSource)PresentationSource.FromVisual(this);
            cameraManager.showPropertiesDialog(p.Handle);
        }

        private void cameraList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb=(ComboBox)e.Source;
            if(cb.SelectedIndex>=0)
                cameraManager.selectCamera(cb.SelectedIndex);
        }

        private void StartCapture_Click(object sender, RoutedEventArgs e)
        {
            if (!isCapturing)
            {
                isCapturing = true;
                StartCapture.Content = "Stop Capture";
                StartCapture.SetResourceReference(BackgroundProperty, "stopCaptureButtonBrush");
                No_capture.Visibility = Visibility.Collapsed;
                cameraManager.startCapture();

                PixelFormat pixelFormat = PixelFormats.Rgb48;
                // as an effect, alocates ImageInterop file mapping with sufficient space
                // if you choose a pixelformat that uses less space than another one that you'll use later,
                // this WILL fail (throws exception)!.
                _frameRenderer = new ImageInteropFrameRenderer(captureImg, cameraManager._camWidth, cameraManager._camHeight, pixelFormat);
                // change back to current pixelformat
                OutputMode om = outputM.SelectedItem as OutputMode;
                _frameRenderer.ChangePixelFormat((PixelFormat)om.pixelFormat);
            }
            else
            {
                isCapturing = false;
                StartCapture.Content = "Start Capture";
                StartCapture.SetResourceReference(BackgroundProperty, "captureOnButtonBrush");
                cameraManager.stopCapture();
            }
        }

        private void outputM_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb=(ComboBox)e.Source;
            if (cb.SelectedIndex >= 0)
            {
                cameraManager.setActiveOutputMode(cb.SelectedIndex);
                OutputMode om = outputM.SelectedItem as OutputMode;
                _frameRenderer.ChangePixelFormat((PixelFormat)om.pixelFormat);
            }
            
        }
    }
}
