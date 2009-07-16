using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Documents;
using System.Windows;
using cytrus.managed;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Shapes;

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
            renderBrush.Freeze();
            Pen renderPen = new Pen(new SolidColorBrush(Colors.White), 1);
            double renderRadius = 2.0;
            renderPen.Freeze();

            StreamGeometry geometry = new StreamGeometry();
            geometry.FillRule = FillRule.EvenOdd;
            Size mySize=new Size(renderRadius,renderRadius);

            using (StreamGeometryContext ctx = geometry.Open())
            {
                foreach (Poi_m p in _poiList)
                {
                    //Update pos
                    double relPosX = p.X * renderedImgSize.Width / _captureSize.Width;
                    double relPosY = p.Y * renderedImgSize.Height / _captureSize.Height;
                    Point pct = new Point(relPosX-renderRadius, relPosY);
                    ctx.BeginFigure(pct,true,true);
                    ctx.ArcTo(new Point(relPosX+renderRadius,relPosY),mySize,0.0,false,SweepDirection.Clockwise,true,true);
                    ctx.ArcTo(new Point(relPosX-renderRadius, relPosY), mySize, 0.0, false, SweepDirection.Clockwise, true, true);
                    ctx.LineTo(new Point(relPosX + renderRadius / 2, relPosY), false, false);
                    ctx.LineTo(new Point(relPosX+ 10 * Math.Cos(p.Orientation), relPosY+ 10 * Math.Sin(p.Orientation)), true, false);
                }
            }
            geometry.Freeze();
            drawingContext.DrawGeometry(renderBrush, renderPen, geometry);
        }

    }
}
