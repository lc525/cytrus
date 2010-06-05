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
using System.Reflection;
using System.Diagnostics;
using System.Configuration;
using System.Runtime.InteropServices;

namespace cytrus.managed
{
    public delegate void BatchMessageDelegate(string c);

    public enum AppModes{
        CAPTURE_MODE,
        IMAGE_MODE,
        BATCH_MODE
    }

    
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window,INotifyPropertyChanged
    {

        public Dictionary<String, AppModes> menuAppModesStrings=new Dictionary<String,AppModes>{
                                                                    { "Capture Mode", AppModes.CAPTURE_MODE},
                                                                    { "Batch Mode", AppModes.BATCH_MODE},
                                                                    { "Image Mode", AppModes.IMAGE_MODE}
                                                                };
        
        
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
        private AppModes currentMode;
        private double selRectTL_x, selRectTL_y;
        private PoiAdorner prevAdorner = null;
        AdornerLayer myAdornerLayer;
        private ImageCaptureCallback staticImageCallback;
        private ImageCaptureCallback cameraImageCallback;
        private ImageCaptureCallback batchImageCallback;
        private Point startDrag, currentPoint, objStDrag;
        //BitmapImage bpCM, bpOM;
        bool imageLoaded;
        MouseButtonEventHandler md, mu;
        MouseEventHandler mm;
        
        public Window1()
        {
            //temporary
            //RecognisedObject p = new RecognisedObject("Mouse", 20);
            //_rObj.Add(p);
            //_rObj.Add(new RecognisedObject("Human Face", 20));
            //p.RecognitionCertainty = 80;
            //
            Fps = 0;
            meanFrames = 100;
            currentNoOfFrames = 0;
            
            InitializeComponent();
            sizeChangeHandler = new SelectionChangedEventHandler(imgSizeCombo_SelectionChanged);
            //bpCM = new BitmapImage(new Uri(@"res\switch_to_camera_mode.png", UriKind.Relative));
            //bpOM = new BitmapImage(new Uri(@"res\webcam.png", UriKind.Relative));

            md = new MouseButtonEventHandler(image_MouseDown);
            mu = new MouseButtonEventHandler(image_MouseUp);
            mm = new MouseEventHandler(image_MouseMove);

            selRectTL_x = 0;
            selRectTL_y = 0;
            imageLoaded = false;

            
            InitializeComponent();
            ContextMenu myMenu = new ContextMenu();
            MenuItem objItem = new MenuItem();
            objItem.Header = "Define as object";
            objItem.Click += new RoutedEventHandler(objItem_Click);
            myMenu.Items.Add(objItem);

            rectangle.ContextMenu = myMenu;

            ContextMenu myMenu2 = new ContextMenu();
            MenuItem objItem2 = new MenuItem();
            objItem2.Header = "Delete object";
            objItem2.Click += new RoutedEventHandler(objItem2_Click);
            myMenu2.Items.Add(objItem2);

            objListDisp.ContextMenu = myMenu2;
            currentMode = AppModes.CAPTURE_MODE;
            
        }

        void objItem2_Click(object sender, RoutedEventArgs e)
        {
            //throw new NotImplementedException();
            if (objListDisp.SelectedIndex != -1)
            {
                pictureManager.removeObject(objListDisp.SelectedIndex);
                _rObj.RemoveAt(objListDisp.SelectedIndex);
            }
        }

        void objItem_Click(object sender, RoutedEventArgs e)
        {
            objNamePopup.IsOpen = true;
        }

        private void image_MouseDown(object sender, MouseButtonEventArgs e)
        {
            //Set the start point
            startDrag = e.GetPosition(captureImg);
            objStDrag = new Point(startDrag.X, startDrag.Y);
            //startDrag.X = captureImg.ActualWidth - startDrag.X;
            startDrag.Y = captureImg.ActualHeight - startDrag.Y;
            //Move the selection marquee on top of all other objects in canvas
            Canvas.SetZIndex(rectangle, imgGrid.Children.Count);
            //Capture the mouse
            if (!captureImg.IsMouseCaptured)
                captureImg.CaptureMouse();
            //captureImg.Cursor = Cursors.Cross;
            rectangle.Visibility = Visibility.Hidden;
        }

        private void image_MouseUp(object sender, MouseButtonEventArgs e)
        {
            //Release the mouse
            if (captureImg.IsMouseCaptured)
                captureImg.ReleaseMouseCapture();
            //captureImg.Cursor = Cursors.Arrow;
        }

        private void image_MouseMove(object sender, MouseEventArgs e)
        {
            if (captureImg.IsMouseCaptured)
            {
                currentPoint = e.GetPosition(captureImg);
                //currentPoint.X = captureImg.ActualWidth - currentPoint.X;
                currentPoint.Y = captureImg.ActualHeight - currentPoint.Y;

                //Calculate the top left corner of the rectangle 
                //regardless of drag direction
                double x = startDrag.X < currentPoint.X ? startDrag.X : currentPoint.X;
                double y = startDrag.Y < currentPoint.Y ? startDrag.Y : currentPoint.Y;

                if (x < 0) x = 0;
                if (y < 0) y = 0;
                selRectTL_x = x;
                selRectTL_y = y;
                //if (x > captureImg.ActualWidth) x = captureImg.ActualWidth;
                //if (y > captureImg.ActualHeight) y = captureImg.ActualHeight;

                if (rectangle.Visibility == Visibility.Hidden)
                    rectangle.Visibility = Visibility.Visible;

                //Move the rectangle to proper place
                rectangle.RenderTransform = new TranslateTransform(x, y);
                //Set its size

                double xC, yC;

                if (e.GetPosition(captureImg).X < 0) xC = 0;
                else if (e.GetPosition(captureImg).X > captureImg.ActualWidth) xC = captureImg.ActualWidth;
                else xC = e.GetPosition(captureImg).X;

                if (e.GetPosition(captureImg).Y < 0) yC = 0;
                else if (e.GetPosition(captureImg).Y > captureImg.ActualHeight) yC = captureImg.ActualHeight;
                else yC = e.GetPosition(captureImg).Y;

                //xC = captureImg.ActualWidth - xC;
                yC = captureImg.ActualHeight - yC;

                rectangle.Width = Math.Abs(xC - startDrag.X);
                rectangle.Height = Math.Abs(yC - startDrag.Y);
                
            }
        }

        private void captureImg_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (rectangle.Visibility == Visibility.Visible)
            {
                double nX, nY;
                nX = selRectTL_x * e.NewSize.Width / e.PreviousSize.Width;
                nY = selRectTL_y * e.NewSize.Height / e.PreviousSize.Height;
                
                rectangle.RenderTransform = new TranslateTransform(nX, nY);
                selRectTL_x = nX;
                selRectTL_y = nY;

                rectangle.Width = rectangle.Width * e.NewSize.Width / e.PreviousSize.Width;
                rectangle.Height = rectangle.Height * e.NewSize.Height / e.PreviousSize.Height;

            }
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
            cameraImageCallback = new ImageCaptureCallback(c_onNewImageAvailable);
            batchImageCallback = new ImageCaptureCallback(batch_onNewImageAvailable);
            cameraManager.onImageAvailableForRendering += cameraImageCallback;
            //cameraManager.onImageAvailableForRendering += new ImageCaptureCallback(c_onNewImageAvailable);
            //cameraManager.onOutputModeChange += new OutputModeCallback(cameraManager_onOutputModeChange);
            isCapturing = false;
            myAdornerLayer = AdornerLayer.GetAdornerLayer(captureImg);
            staticImageCallback=new ImageCaptureCallback(c_onNewStaticImageAvailable);
            pictureManager = new ImageFileMgr();
            pictureManager.onImageAvailableForRendering += staticImageCallback;
        }

        void batch_onNewImageAvailable(byte[] pbData, List<Poi_m> poiData)
        {
            
            if (currentNoOfFrames % 100 == 0)
            {
                string message = "captured images: " + currentNoOfFrames;
                batchCommandWind.Dispatcher.BeginInvoke(new BatchMessageDelegate(changeText), message);
            }

            //this.Dispatcher.Invoke(DispatcherPriority.Normal, (Action)(() =>
            //{
            if (currentNoOfFrames > 25)
            {
                try
                {
                    System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(320, 240, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
                    System.Drawing.Imaging.BitmapData bmpData = bmp.LockBits(
                                         new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height),
                                         System.Drawing.Imaging.ImageLockMode.WriteOnly, bmp.PixelFormat);
                    Marshal.Copy(pbData, 0, bmpData.Scan0, pbData.Length);
                    bmp.UnlockBits(bmpData);
                    bmp.Save("data\\cap" + currentNoOfFrames.ToString("D5") + ".jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
                    FileStream fs = new FileStream("data\\poidata" + currentNoOfFrames.ToString("D5") + ".dat", FileMode.OpenOrCreate);
                    using (StreamWriter sw = new StreamWriter(fs))
                    {
                        foreach(Poi_m ipct in poiData){
                            sw.WriteLine(ipct.X + " " + ipct.Y + " " + ipct.Scale + " " + ipct.Orientation);
                        }
                    }
                }
                catch (Exception e)
                {
                    System.Console.WriteLine(e.StackTrace);
                }
                //}));
            }
            currentNoOfFrames++;
        }

        void changeText(string message)
        {

            if (currentNoOfFrames < 100)
            {
                batchCommandWind.Text += "#cytrus> ... " + Environment.NewLine;
            }
            else
            {
                DateTime dtCap = DateTime.Now;
                double milliseconds = (double)((dtCap.Ticks - _lastCapture.Ticks) / TimeSpan.TicksPerMillisecond); //*1.15;
                Fps = Math.Round((100 * 1000) / milliseconds, 2);
                if (currentNoOfFrames % 5000 == 0) batchCommandWind.Text = "";
                batchCommandWind.Text += "#cytrus>" + message + " (fps:" + Fps + ")" + Environment.NewLine;
                batchScroll.ScrollToVerticalOffset(batchScroll.ScrollableHeight);
            }
            _lastCapture = DateTime.Now;
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

                //if (_rObj.Count > 0)
                //    _rObj[0].RecognitionCertainty += 1;
                //display poi's:
                int st = (int)Fps / 3 + 1;
                if (currentNoOfFrames % st == 0)
                {

                    if (prevAdorner != null) myAdornerLayer.Remove(prevAdorner);
                    ImgSize sz = (ImgSize)imgSizeCombo.SelectedItem;
                    PoiAdorner newAdorner = new PoiAdorner(captureImg, poiData, new Size((double)sz.width,(double)sz.height),_rObj);
                    newAdorner.showPOI = (bool)dispPOI.IsChecked;
                    newAdorner.showOrientation = (bool)dispOrientation.IsChecked;
                    newAdorner.showVicinity = (bool)dispArea.IsChecked;
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
                PoiAdorner newAdorner = new PoiAdorner(captureImg, poiData, new Size(width, height),_rObj);
                newAdorner.showPOI = (bool)dispPOI.IsChecked;
                newAdorner.showOrientation = (bool)dispOrientation.IsChecked;
                newAdorner.showVicinity = (bool)dispArea.IsChecked;
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
                //pictureManager = null;
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
                pick.OpacityMask = Brushes.Black;
                pick.IsEnabled = true;
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
            imageLoaded = true;
            pick.IsEnabled = true;
            pick.OpacityMask = Brushes.Black;
        }

        private void modeSw_Click(object sender, RoutedEventArgs e)
        {

            
        }

        private void pick_Click(object sender, RoutedEventArgs e)
        {
            if (pick.IsChecked == true)
            {
                if (imageLoaded == true || isCapturing==true)
                {
                    captureImg.Cursor = Cursors.Cross;
                    captureImg.MouseDown += md;
                    captureImg.MouseUp += mu;
                    captureImg.MouseMove += mm;
                }
            }
            else
            {
                captureImg.Cursor = Cursors.Arrow;

                captureImg.MouseDown -= md;
                captureImg.MouseUp -= mu;
                captureImg.MouseMove -= mm;
                rectangle.Visibility = Visibility.Hidden;
            }
        }

        private void exitMenu_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown(0);
        }

        private void objName_Cancel_Click(object sender, RoutedEventArgs e)
        {
            objNamePopup.IsOpen = false;
            TextBox tb=(TextBox)objNamePopup.FindName("objName");
            tb.Text = "";
        }

        private void dispPOI_Unchecked(object sender, RoutedEventArgs e)
        {
            dispOrientation.IsEnabled = false;
            prevAdorner.showPOI = false;
            myAdornerLayer.Update();
            dispArea.IsEnabled = false;
        }

        private void dispPOI_Checked(object sender, RoutedEventArgs e)
        {
            if (dispOrientation != null && dispArea != null)
            {
                dispOrientation.IsEnabled = true;
                prevAdorner.showPOI = true;
                myAdornerLayer.Update();
                dispArea.IsEnabled = true;
            }
        }

        private void objAddButton_Click(object sender, RoutedEventArgs e)
        {
            int rWidth, rHeight, x ,y, no=0;
            if (currentMode == AppModes.IMAGE_MODE && imageLoaded == true)
            {
                x = (int)Math.Floor(objStDrag.X * pictureManager._imgWidth / captureImg.ActualWidth);
                y = (int)Math.Floor(objStDrag.Y * pictureManager._imgHeight / captureImg.ActualHeight);
                rWidth = (int)Math.Floor(rectangle.ActualWidth * pictureManager._imgWidth / captureImg.ActualWidth);
                rHeight = (int)Math.Floor(rectangle.ActualHeight * pictureManager._imgHeight / captureImg.ActualHeight);
                no = pictureManager.registerObject(x, y, rWidth, rHeight);
            }
            if (currentMode == AppModes.CAPTURE_MODE && isCapturing == true)
            {
                x = (int)Math.Floor(objStDrag.X * cameraManager._camWidth / captureImg.ActualWidth);
                y = (int)Math.Floor(objStDrag.Y * cameraManager._camHeight / captureImg.ActualHeight);
                rWidth = (int)Math.Floor(rectangle.ActualWidth * cameraManager._camWidth / captureImg.ActualWidth);
                rHeight = (int)Math.Floor(rectangle.ActualHeight * cameraManager._camHeight / captureImg.ActualHeight);
                no = cameraManager.registerObject(x, y, rWidth, rHeight);
            }
            RecognisedObject nObj = new RecognisedObject(objName.Text, no);
            //RecognisedObject nObj = new RecognisedObject(objName.Text, 0);
            _rObj.Add(nObj);
            objNamePopup.IsOpen = false;
            objName.Text = "";
        }

        private void MenuItem_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                string source = ConfigurationManager.AppSettings["helpSource"].ToString();
                Process.Start(source);
            }
            catch
            {
                MessageBox.Show("Help file not found!");
            }
        }

        private void dispOri_Checked(object sender, System.Windows.RoutedEventArgs e)
        {
            if (prevAdorner != null)
            {
                prevAdorner.showOrientation = true;
                myAdornerLayer.Update();
            }
        }

        private void dispOri_Unchecked(object sender, System.Windows.RoutedEventArgs e)
        {
            if (prevAdorner != null)
            {
                prevAdorner.showOrientation = false;
                myAdornerLayer.Update();
            }
        }

        private void dispArea_Unchecked(object sender, System.Windows.RoutedEventArgs e)
        {
            if (prevAdorner != null)
            {
                prevAdorner.showVicinity = false;
                myAdornerLayer.Update();
            }
        }

        private void dispArea_Checked(object sender, System.Windows.RoutedEventArgs e)
        {
            if (prevAdorner != null)
            {
                prevAdorner.showVicinity = true;
                myAdornerLayer.Update();
            }
        }

        private void modeSelection_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
           
            ComboBoxItem item=(ComboBoxItem)e.AddedItems[0];
            StackPanel pnl=(StackPanel)item.Content;
            IEnumerable<Label> lblList=pnl.Children.OfType<Label>();
            Label contentTitle=lblList.ElementAt<Label>(0);
            switch(menuAppModesStrings[contentTitle.Content.ToString()]){
                case AppModes.IMAGE_MODE: {
                    No_capture.Visibility = Visibility.Hidden;
                    ImageSetup.Visibility = Visibility.Collapsed;
                    Results.Visibility = Visibility.Visible;
                    Objects.Visibility = Visibility.Visible;

                    fps_bar.Visibility = Visibility.Hidden;
                    fps_text.Visibility = Visibility.Hidden;
                    fpsDisplay.Visibility = Visibility.Hidden;
                    viewbox.Visibility = Visibility.Hidden;
                    batchCommandWind.Visibility = Visibility.Hidden;
                    if (myAdornerLayer != null)
                        myAdornerLayer.Visibility = Visibility.Visible;
                    loadImage.Visibility=Visibility.Visible;
                    pick.Visibility = Visibility.Visible;
                    currentMode = AppModes.IMAGE_MODE;
                    break;
                }
                case AppModes.BATCH_MODE:{
                    Fps = 0.0;
                    if(myAdornerLayer!=null)
                        myAdornerLayer.Visibility = Visibility.Hidden;
                    No_capture.Visibility = Visibility.Hidden;
                    Results.Visibility = Visibility.Collapsed;
                    Objects.Visibility = Visibility.Collapsed;
                    ImageSetup.Visibility = Visibility.Visible;
                    batchCommandWind.Visibility = Visibility.Visible;
                    batchScroll.Visibility = Visibility.Visible;

                    fps_bar.Visibility = Visibility.Hidden;
                    fps_text.Visibility = Visibility.Hidden;
                    fpsDisplay.Visibility = Visibility.Hidden;
                    viewbox.Visibility = Visibility.Hidden;
                    currentNoOfFrames = 0;
                    cameraManager.onImageAvailableForRendering -= cameraImageCallback;
                    cameraManager.onImageAvailableForRendering += batchImageCallback;

                    loadImage.Visibility=Visibility.Collapsed;
                    pick.Visibility=Visibility.Collapsed;
                    currentMode = AppModes.BATCH_MODE;
                    break;
                }
                case AppModes.CAPTURE_MODE:{
                    if (cameraManager != null)
                    {

                        if(myAdornerLayer!=null)
                            myAdornerLayer.Visibility = Visibility.Visible;
                        currentNoOfFrames = 0;

                        cameraManager.onImageAvailableForRendering -= batchImageCallback;
                        cameraManager.onImageAvailableForRendering += cameraImageCallback;


                        No_capture.Visibility = Visibility.Visible;
                        ImageSetup.Visibility = Visibility.Visible;
                        Results.Visibility = Visibility.Visible;
                        Objects.Visibility = Visibility.Visible;
                        batchCommandWind.Visibility = Visibility.Hidden;
                        batchScroll.Visibility = Visibility.Hidden;
                        if (prevAdorner != null) myAdornerLayer.Remove(prevAdorner);
                        captureImg.Source = null;
                        imageLoaded = false;
                        pick.IsChecked = false;

                        captureImg.Cursor = Cursors.Arrow;
                        captureImg.MouseDown -= md;
                        captureImg.MouseUp -= mu;
                        captureImg.MouseMove -= mm;

                        rectangle.Visibility = Visibility.Hidden;

                        fps_bar.Visibility = Visibility.Visible;
                        fps_text.Visibility = Visibility.Visible;
                        fpsDisplay.Visibility = Visibility.Visible;
                        viewbox.Visibility = Visibility.Visible;

                        pick.Visibility = Visibility.Visible;
                        loadImage.Visibility = Visibility.Collapsed;

                        //pick.IsEnabled = false;
                        //Color p = new Color();
                        //p.A = 126;
                        //SolidColorBrush opM = new SolidColorBrush(p);
                        //pick.OpacityMask = opM;
                        pictureManager.freeResources();
                    }

                    //pictureManager = null;
                    currentMode = AppModes.CAPTURE_MODE;
                    break;
                    
                }
            }
        }

    }
}
