using System;
using System.Collections.Generic;
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
using System.Windows.Threading;

namespace ImageAnnotationDemo
{

    public partial class PoiImageAnnotationControl : ContentControl
	{
		#region Data

		private readonly PoiImageAnnotation _imageAnnotation;

		#endregion // Data

		#region Constructor

		public PoiImageAnnotationControl(
            PoiImageAnnotation imageAnnotation)
        {
            InitializeComponent();

			if (imageAnnotation == null)
				throw new ArgumentNullException("imageAnnotation");

			_imageAnnotation = imageAnnotation;
            //RegisterName("imageAnnotation", _imageAnnotation);

            // Prevent the control from having a focus rect.
            base.FocusVisualStyle = null;
        }

        #endregion // Constructor

    }
}