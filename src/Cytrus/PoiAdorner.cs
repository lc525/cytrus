using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Documents;
using System.Windows;
using cytrus.managed;
using System.Windows.Media;
using System.Windows.Controls;

namespace cytrus.managed
{
    public class PoiAdorner:Adorner
    {
        private List<Poi_m> _poiList;
        Size _captureSize;

        public PoiAdorner(Image adornedElement, List<Poi_m> pois, Size captureSize):base(adornedElement){
            _poiList = pois;
            _captureSize = captureSize;
        }

        // A common way to implement an adorner's rendering behavior is to override the OnRender
        // method, which is called by the layout system as part of a rendering pass.
        protected override void OnRender(DrawingContext drawingContext)
        {
            Rect adornedElementRect = new Rect(this.AdornedElement.DesiredSize);
            Size renderedImgSize=((Image)base.AdornedElement).RenderSize;
            // Some arbitrary drawing implements.
            SolidColorBrush renderBrush = new SolidColorBrush(Colors.Black);
            renderBrush.Opacity = 0.7;
            Pen renderPen = new Pen(new SolidColorBrush(Colors.White), 1);
            double renderRadius = 2.0;

            foreach (Poi_m p in _poiList)
            {
                //Update pos
                double relPosX = p.X * renderedImgSize.Width / _captureSize.Width;
                double relPosY = p.Y * renderedImgSize.Height / _captureSize.Height;
                Point pct = new Point(relPosX, relPosY);
                drawingContext.DrawEllipse(renderBrush, renderPen, pct, renderRadius, renderRadius);
            }
        }

    }
}
