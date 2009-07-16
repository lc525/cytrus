using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using cytrus.managed;
using System.Collections.ObjectModel;
using System.Timers;
using System.Diagnostics;

namespace CytrusModuleTesting
{
    /// <summary>
    /// Summary description for CytrusManagersTest
    /// </summary>
    [TestClass]
    public class CytrusManagersTest
    {
        public CytrusManagersTest()
        {
            //
            // TODO: Add constructor logic here
            //
            
        }

        private TestContext testContextInstance;
        private static ImageFileMgr fileMgr = new ImageFileMgr();
        private static CameraMgr mgr = new CameraMgr();
        private DateTime startTime, _lastCapture;
        private double currentNoOfFrames=0, meanFrames = 80, Fps=40;
        float v2=0;
        private PerformanceCounter mem = new PerformanceCounter("Memory", "Available MBytes");

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        #region Additional test attributes
        //
        // You can use the following additional attributes as you write your tests:
        //
        // Use ClassInitialize to run code before running the first test in the class
        [ClassInitialize()]
        public static void MyClassInitialize(TestContext testContext) { 
        }
        
        // Use ClassCleanup to run code after all tests in a class have run
        [ClassCleanup()]
        public static void MyClassCleanup() {
            fileMgr.freeResources();
            mgr.stopCapture();
        }
        
        // Use TestInitialize to run code before running each test 
        // [TestInitialize()]
        // public void MyTestInitialize() { }
        //
        // Use TestCleanup to run code after each test has run
        //[TestCleanup()]
        //public void MyTestCleanup() {
            
        //}
        
        #endregion

        [TestMethod]
        public void CameraManagerInit()
        {
            ObservableCollection<string> cameraList=mgr.getCameraList();
            Assert.AreNotEqual(0,cameraList.Count);
            mgr.selectCamera(0);
            mgr.startCapture();
            Assert.AreNotEqual(-1, mgr._camHeight);
            Assert.AreNotEqual(-1, mgr._camWidth);
            mgr.stopCapture();
        }

        [TestMethod]
        public void FileManagerInit()
        {
            fileMgr.setImagePath("lenna.jpg");
            fileMgr.startImageProcessing();
            Assert.AreNotEqual(-1, fileMgr._imgHeight);
            Assert.AreNotEqual(-1, fileMgr._imgWidth);
            fileMgr.freeResources();
        }

        [TestMethod]
        public void SURFRun_StaticImage_Lenna()
        {
            fileMgr.setImagePath("lenna.jpg");
            fileMgr.onImageAvailableForRendering += new ImageCaptureCallback(mgr_onImageAvailableForRendering);
            fileMgr.startImageProcessing();
            Assert.AreEqual(480, fileMgr._imgHeight);
            Assert.AreEqual(480, fileMgr._imgWidth);
        }

        void mgr_onImageAvailableForRendering(byte[] pbData, List<Poi_m> poiData)
        {
            Assert.AreNotEqual(0, pbData.Count());
            Assert.AreNotEqual(0, poiData.Count);
            Assert.IsTrue(poiData.Count > 50, "Algorithm error. There have been detected less than 50 poi's in the test image");
        }

        void mgrFpsTest_onImageAvailableForRendering(byte[] pbData, List<Poi_m> poiData)
        {
            if (currentNoOfFrames < meanFrames)
            {
                currentNoOfFrames++;
            }
            else
            {
                DateTime dtCap = DateTime.Now;
                double milliseconds = (double)((dtCap.Ticks - _lastCapture.Ticks) / TimeSpan.TicksPerMillisecond); //*1.15;
                Fps = Math.Round((meanFrames * 1000) / milliseconds, 2);
                currentNoOfFrames = 0;
                _lastCapture = dtCap;
            }
        }

        [TestMethod]
        public void FPSTestRun()
        {
            
            ObservableCollection<string> cameraList = mgr.getCameraList();
            Assert.AreNotEqual(0, cameraList.Count);
            mgr.selectCamera(0);
            mgr.onImageAvailableForRendering+=new ImageCaptureCallback(mgrFpsTest_onImageAvailableForRendering);
            mgr.startCapture();
            startTime = DateTime.Now;
            _lastCapture = DateTime.Now;
            while (DateTime.Now.Ticks - startTime.Ticks < 10 * TimeSpan.TicksPerSecond)
            {
                System.Threading.Thread.Sleep(100);
            }
            Assert.IsTrue(Fps > 10, "Algorithm too slow, or camera device not supporting more than 10 fps");
            mgr.stopCapture();
        }

        [TestMethod]
        public void StaticImage_ReadTest_Lenna()
        {
            fileMgr.setImagePath("lenna.jpg");
            fileMgr.onImageAvailableForRendering += new ImageCaptureCallback(mgrReadTest_onImageAvailableForRendering);
            fileMgr.startImageProcessing();
            fileMgr.freeResources();
        }

        void mgrReadTest_onImageAvailableForRendering(byte[] pbData, List<Poi_m> poiData)
        {
            System.Drawing.Bitmap bmp = new System.Drawing.Bitmap("lenna.jpg");
            Assert.AreEqual(bmp.Width, fileMgr._imgWidth);
            Assert.AreEqual(bmp.Height, fileMgr._imgHeight);
            int cElem=0;
            for(int i=0; i<bmp.Height; i++){
                for(int j=0; j<bmp.Width; j++){
                    System.Drawing.Color color = bmp.GetPixel(j, i);
                    Assert.AreEqual(color.R,pbData[cElem++]);
                    Assert.AreEqual(color.G, pbData[cElem++]);
                    Assert.AreEqual(color.B, pbData[cElem++]);
                }
            }
        }

        [TestMethod]
        public void SURFRun_StaticImage_Large()
        {
            fileMgr.setImagePath("parrot_large.jpg");
            fileMgr.onImageAvailableForRendering += new ImageCaptureCallback(mgr_onImageAvailableForRendering);
            fileMgr.startImageProcessing();
            Assert.AreEqual(2225, fileMgr._imgHeight);
            Assert.AreEqual(3337, fileMgr._imgWidth);
            fileMgr.freeResources();
        }

        [TestMethod]
        public void StaticImage_SurfRun_MemoryAlloc_Dealloc_Stress50()
        { 
            float v1=mem.NextValue();
            for (int i = 0; i < 50; i++)
            {
                fileMgr.setImagePath("parrot_large.jpg");
                fileMgr.onImageAvailableForRendering += new ImageCaptureCallback(mgr_onImageAvailableForRendering_mem);
                fileMgr.startImageProcessing();
                fileMgr.freeResources();
                System.Threading.Thread.Sleep(10);
            }
            float v3 = mem.NextValue();
            Assert.IsTrue(v1 > v2, "Processing does not allocate memory");
            Assert.IsTrue(v3 > v2, "Not freeing resources correctly");
            Assert.IsTrue(v3 - v1 < 100, "Memory leak. If you ran programs that allocated memory during this test, the report could be fake.");
        }

        void mgr_onImageAvailableForRendering_mem(byte[] pbData, List<Poi_m> poiData)
        {
            v2 = mem.NextValue();
        }
    }
}
