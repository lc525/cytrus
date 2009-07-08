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
using System.ComponentModel;
using ImageAnnotationDemo;

namespace cytrus.managed
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window,INotifyPropertyChanged
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
        private ImageFileMgr pictureManager;
        static bool isCapturing;
        private FrameRenderer _frameRenderer = FrameRenderer.Null;
        private ObservableCollection<RecognisedObject> _rObj = new ObservableCollection<RecognisedObject>();
        private ObservableCollection<ImgSize> _prelSizes = new ObservableCollection<ImgSize>();
        private SelectionChangedEventHandler sizeChangeHandler;
        private DateTime _lastCapture;
        private double _fps;
        private int meanFrames;
        private int currentNoOfFrames;
        private Size imgSize;
        private PoiAdorner prevAdorner = null;
        AdornerLayer myAdornerLayer;
        private ImageCaptureCallback staticImageCallback;
        
        public Window1()
        {
            //temporary
            _rObj.Add(new RecognisedObject("Mouse", 50));
            _rObj.Add(new RecognisedObject("Human Face", 20));
            //
            Fps = 0;
            meanFrames = 100;
            currentNoOfFrames = 0;
            InitializeComponent();
            sizeChangeHandler = new SelectionChangedEventHandler(imgSizeCombo_SelectionChanged);
        }

        public ObservableCollection<RecognisedObject> RecognisedObjects
        { get { return _rObj; } }

        private void CytrusMain_Loaded(object sender, RoutedEventArgs e)
        {
            RenderOptions.SetBitmapScalingMode(captureImg, BitmapScalingMode.LowQuality);
            cameraManager=new CameraMgr();
            cameraList.ItemsSource = cameraManager.getCameraList();
            outputM.ItemsSource = cameraManager.getOutputModesList();
            cameraManager.setActiveOutputMode(0);
            cameraManager.selectCamera(0);
            cameraManager.onImageAvailableForRendering += new ImageCaptureCallback(c_onNewImageAvailable);
            //cameraManager.onOutputModeChange += new OutputModeCallback(cameraManager_onOutputModeChange);
            isCapturing = false;
            myAdornerLayer = AdornerLayer.GetAdornerLayer(captureImg);
            staticImageCallback=new ImageCaptureCallback(c_onNewStaticImageAvailable);
            pictureManager = new ImageFileMgr();
            pictureManager.onImageAvailableForRendering += staticImageCallback;
        }


        void c_onNewImageAvailable(byte[] pbData, List<Poi_m> poiData)
        {
            this.Dispatcher.Invoke(DispatcherPriority.Normal, (Action)(() =>
            {
                if (currentNoOfFrames < meanFrames)
                {
                    currentNoOfFrames++;
                }
                else
                {
                    DateTime dtCap = DateTime.Now;
                    double milliseconds = (double)((dtCap.Ticks - _lastCapture.Ticks) / TimeSpan.TicksPerMillisecond); //*1.15;
                    Fps = Math.Round((meanFrames*1000)/milliseconds,2);
                    currentNoOfFrames = 0;
                    _lastCapture = dtCap;
                }
                _frameRenderer.RenderFrame(pbData);

                //display poi's:
                int st = (int)Fps / 3 + 1;
                if (currentNoOfFrames % st == 0)
                {

                    if (prevAdorner != null) myAdornerLayer.Remove(prevAdorner);
                    ImgSize sz = (ImgSize)imgSizeCombo.SelectedItem;
                    PoiAdorner newAdorner = new PoiAdorner(captureImg, poiData, new Size((double)sz.width,(double)sz.height));
                    newAdorner.IsHitTestVisible = false;
                    myAdornerLayer.Add(newAdorner);
                    prevAdorner = newAdorner;
                }


            }));
        }


        void c_onNewStaticImageAvailable(byte[] pbData, List<Poi_m> poiData)
        {
            PixelFormat pixelFormat = PixelFormats.Rgb24;
            // as an effect, alocates ImageInterop file mapping with sufficient space
            // if you choose a pixelformat that uses less space than another one that you'll use later,
            // this WILL fail (throws exception)!.
            int width = pictureManager._imgWidth;
            int height = pictureManager._imgHeight;
            pictureManager.setProcessingSize(width, height);
            _frameRenderer = new ImageInteropFrameRenderer(captureImg, width, height, pixelFormat);
            // change back to current pixelformat
            //OutputMode om = outputM.SelectedItem as OutputMode;
            //_frameRenderer.ChangePixelFormat((PixelFormat)om.pixelFormat);
            //captureImg.tra
            this.Dispatcher.Invoke(DispatcherPriority.Normal, (Action)(() =>
            {
                _frameRenderer.RenderFrame(pbData);

                if (prevAdorner != null) myAdornerLayer.Remove(prevAdorner);
                PoiAdorner newAdorner = new PoiAdorner(captureImg, poiData, new Size(width, height));
                newAdorner.IsHitTestVisible = false;
                myAdornerLayer.Add(newAdorner);
                prevAdorner = newAdorner;
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
            imgSize = captureImg.RenderSize; // fake call, this apparently returns {0,0}
            if (!isCapturing)
            {
                pictureManager.freeResources();
                isCapturing = true;
                StartCapture.Content = "Stop Capture";
                StartCapture.SetResourceReference(BackgroundProperty, "stopCaptureButtonBrush");
                No_capture.Visibility = Visibility.Collapsed;

                cameraManager.startCapture();
                
                Binding myBinding = new Binding("Fps");
                myBinding.Source = this;
                fpsDisplay.SetBinding(TextBlock.TextProperty, myBinding);
                _lastCapture = DateTime.Now;

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
                Fps = 0;
                currentNoOfFrames = 0;
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

        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        public double Fps
        {

            get { return _fps; }

            set
            {

                _fps = value;

                // Call OnPropertyChanged whenever the property is updated

                OnPropertyChanged("Fps");

            }

        }

        protected void OnPropertyChanged(string name)
        {

            PropertyChangedEventHandler handler = PropertyChanged;

            if (handler != null)
            {

                handler(this, new PropertyChangedEventArgs(name));

            }

        }

        #endregion

        private void ldImage_Click(object sender, RoutedEventArgs e)
        {
            if (pictureManager == null)
            {
                pictureManager = new ImageFileMgr();
                pictureManager.onImageAvailableForRendering += staticImageCallback;
            }
            System.Windows.Forms.OpenFileDialog ofd = new System.Windows.Forms.OpenFileDialog();
            ofd.Filter = "JPEG Image Files(*.JPG)|*.JPG;*.JPEG;";
            System.Windows.Forms.DialogResult result = ofd.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.OK)
            {
                string fn = ofd.FileName;
                pictureManager.setImagePath(fn);
                pictureManager.startImageProcessing();
            }
            ofd.Dispose();
        }
    }
}
