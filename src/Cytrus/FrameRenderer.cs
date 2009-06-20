/*~External (not created by Carata Lucian as part of the B.Sc Thesis)
** 
** Original Author: Tomer Shamam
** Project: Tutorial http://blogs.microsoft.co.il/blogs/tomershamam/archive/2007/10/14/wpf-official-image-interop.aspx
** Modifications (by Carata Lucian): none
**
** -----------------------------------------------------------------
** FrameRenderer.cs : A WPF renderer for images. It creates BitmapSources for a specific Image control.
**
*/

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Controls;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Tomers.WPF.Imaging.Demo
{
	#region FrameRenderer class
	public abstract class FrameRenderer : IDisposable
	{
		public static readonly FrameRenderer Null = new NullFrameRenderer();

		private readonly Image _image;
        protected static Object syncFormatChangeLock=new Object();

        public Object SyncLock
        {
            get { return syncFormatChangeLock; }
        }

		public Image Image
		{
			get { return _image; }
		}
		
		protected int _pixelWidth;

		public int PixelWidth
		{
			get { return _pixelWidth; }
		}

		protected int _pixelHeight;

		public int PixelHeight
		{
			get { return _pixelHeight; }
		}

		protected PixelFormat _pixelFormat;

		public PixelFormat PixelFormat
		{
			get { return _pixelFormat; }
		}

		protected int _stride;

		public int Stride
		{
			get { return _stride; }
		}

		protected FrameRenderer() { }

		protected FrameRenderer(Image image, int pixelWidth, int pixelHeight, PixelFormat pixelFormat)
		{
			this._image = image;
			this._pixelWidth = pixelWidth;
			this._pixelHeight = pixelHeight;
			this._pixelFormat = pixelFormat;
			this._stride = (_pixelWidth * _pixelFormat.BitsPerPixel + 7) / 8;
		}

		public abstract void RenderFrame(byte[] source);
        public abstract void ChangePixelFormat(PixelFormat newPixelFormat);
        public abstract void ChangeImageSize(int newWidth, int newHeight);

		private sealed class NullFrameRenderer : FrameRenderer
		{
			public override void RenderFrame(byte[] source) { }
            public override void ChangePixelFormat(PixelFormat newPixelFormat){}
            public override void ChangeImageSize(int newWidth, int newHeight){}
		}

		#region IDisposable Members

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		protected virtual void Dispose(bool disposing) { }

		~FrameRenderer() { Dispose(false); }

		#endregion
	} 
	#endregion

	#region ImageInteropFrameRenderer class
	public class ImageInteropFrameRenderer : FrameRenderer
	{
		private ImageInterop _imageInterop;

		public ImageInteropFrameRenderer(Image image, int pixelWidth, int pixelHeight, PixelFormat pixelFormat)
			: base(image, pixelWidth, pixelHeight, pixelFormat)
		{
			_imageInterop = new ImageInterop(PixelWidth, PixelHeight, PixelFormat);
		}
		
		public override void RenderFrame(byte[] source)
		{
            lock (syncFormatChangeLock)
            {
                // Update Image with new source
                    Image.Source = _imageInterop.CreateBitmapSource(source);
            }
		}

        public override void ChangePixelFormat(PixelFormat newPixelFormat)
        {
            lock (syncFormatChangeLock)
            {
                this._pixelFormat = newPixelFormat;
                this._stride = (_pixelWidth * _pixelFormat.BitsPerPixel + 7) / 8;
                _imageInterop.ChangePixelFormat(newPixelFormat);
            }
        }

        public override void ChangeImageSize(int newWidth, int newHeight)
        {
            lock (syncFormatChangeLock)
            {
                this._pixelHeight = newHeight;
                this._pixelWidth = newWidth;
                this._stride = (_pixelWidth * _pixelFormat.BitsPerPixel + 7) / 8;
                _imageInterop.ChangeImageSize(newWidth, newHeight);
            }
        }

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (_imageInterop != null)
				{
					_imageInterop.Dispose();
					_imageInterop = null;
				}
			}
			base.Dispose(disposing);
		}
	} 
	#endregion
}
