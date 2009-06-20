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

        public struct ImgSize
        {
            public int width, height;

            public ImgSize(int w, int h){
                width=w;
                height=h;
            }
            
            public override string ToString()
            {
                return width + " x " + height;
            }
        }
        
        private CameraMgr cameraManager;
        static bool isCapturing;
        private FrameRenderer _frameRenderer = FrameRenderer.Null;
        private ObservableCollection<RecognisedObject> _rObj = new ObservableCollection<RecognisedObject>();
        private ObservableCollection<ImgSize> _prelSizes = new ObservableCollection<ImgSize>();
        private SelectionChangedEventHandler sizeChangeHandler;
        
        public Window1()
        {
            //temporary
            _rObj.Add(new RecognisedObject("Mouse", 50));
            _rObj.Add(new RecognisedObject("Human Face", 20));
            //

            InitializeComponent();
            sizeChangeHandler = new SelectionChangedEventHandler(outputM_SelectionChanged);
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
                int width=cameraManager._camWidth;
                int height=cameraManager._camHeight;
                cameraManager.setProcessingSize(width, height);
                _frameRenderer = new ImageInteropFrameRenderer(captureImg, width, height, pixelFormat);
                // change back to current pixelformat
                OutputMode om = outputM.SelectedItem as OutputMode;
                _frameRenderer.ChangePixelFormat((PixelFormat)om.pixelFormat);

                _prelSizes.Clear();
                for (int i = 0; i < 4; i++)
                {
                    _prelSizes.Add(new ImgSize(width, height));
                    width = (int)Math.Floor((double)width / 2);
                    height = (int)Math.Floor((double)height / 2);
                }
                imgSizeCombo.ItemsSource = _prelSizes;
                imgSizeCombo.SelectedIndex = 0;
                imgSizeCombo.IsEnabled = true;
                imgSizeCombo.SelectionChanged += sizeChangeHandler;
            }
            else
            {
                isCapturing = false;
                imgSizeCombo.SelectionChanged -= sizeChangeHandler;
                imgSizeCombo.IsEnabled = false;
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

        private void imgSizeCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb=(ComboBox)sender;
            if (cb.SelectedItem != null && cb.IsEnabled==true)
            {
                ImgSize sz = (ImgSize)cb.SelectedItem;

                int width = sz.width;
                int height = sz.height;


                bool ok = cameraManager.setProcessingSize(sz.width, sz.height);
                if (ok)
                {
                    _frameRenderer.ChangeImageSize(sz.width, sz.height);
                }
            }

            
        }
    }
}
