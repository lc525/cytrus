/*~External (not created by Carata Lucian as part of the B.Sc Thesis)
** 
** Original Author: Tomer Shamam
** Project: Tutorial http://blogs.microsoft.co.il/blogs/tomershamam/archive/2007/10/14/wpf-official-image-interop.aspx
** Modifications (by Carata Lucian): none
**
** -----------------------------------------------------------------
** ImageInterop.cs : Interop between WindowsAPI and WPF. It uses System.Windows.Interop.Imaging.CreateBitmapSourceFromMemorySection
**
*/

using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Tomers.WPF.Imaging.Demo
{
	public class ImageInterop : IDisposable
	{
		#region Fields

		private  int _pixelWidth;
		private  readonly int _pixelHeight;
		private  PixelFormat _pixelFormat;
		private  int _stride;
        private  static object syncModifyPixelFormatLock=new object();

		private IntPtr _section;
		private IntPtr _pixels;

		#endregion

		#region Initialization

		public ImageInterop(int pixelWidth, int pixelHeight, PixelFormat format)
		{
			this._pixelWidth = pixelWidth;
			this._pixelHeight = pixelHeight;
			this._pixelFormat = format;
			this._stride = (_pixelWidth * _pixelFormat.BitsPerPixel + 7) / 8;

			uint numBytes = (uint)(_stride * _pixelHeight);

			// Create a new file mapping object to back the bitmap
			_section = CreateFileMapping(
				INVALID_HANDLE_VALUE,
				IntPtr.Zero,
				PAGE_READWRITE,
				0,
				numBytes,
				null);

			_pixels = MapViewOfFile(
				_section,
				FILE_MAP_ALL_ACCESS,
				0,
				0,
				numBytes);
		}

		#endregion

		#region Operations

		public BitmapSource CreateBitmapSource(byte[] pixels)
		{
			Marshal.Copy(pixels, 0, _pixels, pixels.Length);
			// Create a bitmap source using the memory section
            lock (syncModifyPixelFormatLock)
            {
                BitmapSource source = System.Windows.Interop.Imaging.CreateBitmapSourceFromMemorySection(
                    _section,
                    _pixelWidth,
                    _pixelHeight,
                    _pixelFormat,
                    _stride,
                    0);
                return source;
            }
		}


        public void ChangePixelFormat(PixelFormat newPixelFormat)
        {
            lock (syncModifyPixelFormatLock)
            {
                this._pixelFormat = newPixelFormat;
                this._stride = (_pixelWidth * _pixelFormat.BitsPerPixel + 7) / 8;
            }
        }

		#endregion

		#region Dispose Pattern

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		private void Dispose(bool disposing)
		{
			if (_pixels != IntPtr.Zero)
			{
				UnmapViewOfFile(_pixels);
				_pixels = IntPtr.Zero;
			}

			if (_section == IntPtr.Zero)
			{
				CloseHandle(_section);
				_section = IntPtr.Zero;
			}
		}

		~ImageInterop()
		{
			Dispose(false);
		}

		#endregion

		#region Win32 Interop

		[DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
		private static extern IntPtr CreateFileMapping(
			IntPtr hFile,
			IntPtr lpFileMappingAttributes,
			uint flProtect,
			uint dwMaximumSizeHigh,
			uint dwMaximumSizeLow,
			string lpName);

		[DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
		private static extern bool CloseHandle(IntPtr hObject);

		[DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
		private static extern IntPtr MapViewOfFile(
			IntPtr hFileMappingObject,
			uint dwDesiredAccess,
			uint dwFileOffsetHigh,
			uint dwFileOffsetLow,
			uint dwNumberOfBytesToMap);

		[DllImport("kernel32.dll", CallingConvention = CallingConvention.StdCall, SetLastError = true)]
		private static extern bool UnmapViewOfFile(IntPtr lpBaseAddress);

		// Windows constants
		private const uint FILE_MAP_ALL_ACCESS = 0xF001F;
		private const uint PAGE_READWRITE = 0x04;
		private static readonly IntPtr INVALID_HANDLE_VALUE = new IntPtr(-1); 

		#endregion		
	}
}