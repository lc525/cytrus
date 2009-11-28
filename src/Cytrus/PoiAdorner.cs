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
using System.Collections.ObjectModel;

namespace cytrus.managed
{
    public class PoiAdorner:Adorner
    {
        private List<Poi_m> _poiList;
        Size _captureSize;
        public bool showPOI, showOrientation, showVicinity;
        ObservableCollection<RecognisedObject> _rObj;

        public PoiAdorner(Image adornedElement, List<Poi_m> pois, Size captureSize, ObservableCollection<RecognisedObject> rObj):base(adornedElement){
            _poiList = pois;
            _captureSize = captureSize;
            _rObj=rObj;
            showPOI = true;
            showOrientation = true;
            showVicinity = false;
        }

        // A common way to implement an adorner's rendering behavior is to override the OnRender
        // method, which is called by the layout system as part of a rendering pass.
        protected override void OnRender(DrawingContext drawingContext)
        {
            List<float> meanDistance=new List<float>(_rObj.Count);
            List<int> noMatches=new List<int>(_rObj.Count);
            for (int k = 0; k < _rObj.Count; k++)
            {
                meanDistance.Add(1.0f);
                noMatches.Add(0);
            }
            if (showPOI)
            {
                Rect adornedElementRect = new Rect(this.AdornedElement.DesiredSize);
                Size renderedImgSize = ((Image)base.AdornedElement).RenderSize;
                // Some arbitrary drawing implements.
                SolidColorBrush renderBrush = new SolidColorBrush(Colors.Black);
                renderBrush.Opacity = 0.7;
                renderBrush.Freeze();
                Pen renderPen = new Pen(new SolidColorBrush(Colors.White), 1);
                Pen orientationPen = new Pen(new SolidColorBrush(Colors.White), 1);

                SolidColorBrush vecinityBrush = new SolidColorBrush(Colors.Blue);
                vecinityBrush.Opacity = 0.3;
                Pen vecinityPen = new Pen(vecinityBrush, 1);
                double renderRadius = 2.0;
                double renderRadius2 = 8.0;
                renderPen.Freeze();
                orientationPen.Freeze();
                vecinityPen.Freeze();


                StreamGeometry geometry = new StreamGeometry();
                StreamGeometry orientationGeom = new StreamGeometry();
                StreamGeometry vecinityGeom = new StreamGeometry();
                geometry.FillRule = FillRule.EvenOdd;
                Size mySize = new Size(renderRadius, renderRadius);
                Size mySize2 = new Size(renderRadius2, renderRadius2);

                using (StreamGeometryContext ctx = geometry.Open(), ctl = orientationGeom.Open(), ctv = vecinityGeom.Open())
                {
                    foreach (Poi_m p in _poiList)
                    {
                        //Update pos
                        double relPosX = p.X * renderedImgSize.Width / _captureSize.Width;
                        double relPosY = p.Y * renderedImgSize.Height / _captureSize.Height;
                        Point pct = new Point(relPosX - renderRadius, relPosY);
                        Point pct2 = new Point(relPosX - renderRadius2, relPosY);

                        
                        if (p.MatchesObjectNo != -1)
                        {
                            noMatches[p.MatchesObjectNo]++;
                            meanDistance[p.MatchesObjectNo] += p.MatchedDistance;
                            
                            ctx.BeginFigure(pct2, true, true);
                            ctx.ArcTo(new Point(relPosX + renderRadius2, relPosY), mySize2, 0.0, false, SweepDirection.Clockwise, true, true);
                            ctx.ArcTo(new Point(relPosX - renderRadius2, relPosY), mySize2, 0.0, false, SweepDirection.Clockwise, true, true);

                        }
                        else
                        {
                            ctx.BeginFigure(pct, true, true);
                            ctx.ArcTo(new Point(relPosX + renderRadius, relPosY), mySize, 0.0, false, SweepDirection.Clockwise, true, true);
                            ctx.ArcTo(new Point(relPosX - renderRadius, relPosY), mySize, 0.0, false, SweepDirection.Clockwise, true, true);
                        }
                        if (showOrientation)
                        {
                            //Orientation
                            Point pct_m = new Point(relPosX, relPosY);
                            ctl.BeginFigure(pct_m, false, false);
                            ctl.LineTo(new Point(relPosX + 10 * p.Scale * Math.Cos(p.Orientation), relPosY + 10 * p.Scale * Math.Sin(p.Orientation)), true, false);
                        }

                        if (showVicinity)
                        {
                            //Vecinity
                            Point pctv = new Point(relPosX - 10 * p.Scale, relPosY);
                            ctv.BeginFigure(pctv, false, false);
                            Size arcS = new Size(10 * p.Scale, 10 * p.Scale);
                            ctv.ArcTo(new Point(relPosX + 10 * p.Scale, relPosY), arcS, 0.0, false, SweepDirection.Clockwise, true, true);
                            ctv.ArcTo(new Point(relPosX - 10 * p.Scale, relPosY), arcS, 0.0, false, SweepDirection.Clockwise, true, true);
                        }
                    }

                    for (int k = 0; k < _rObj.Count; k++)
                    {
                        if (noMatches[k] > 2)
                        {
                            meanDistance[k] /= noMatches[k];
                            _rObj[k].RecognitionCertainty = (int)Math.Floor((1.0f - meanDistance[k]) * 100);
                            if (_rObj[k].RecognitionCertainty < 10) _rObj[k].RecognitionCertainty = 0;
                        }
                        else
                        {
                            _rObj[k].RecognitionCertainty = 0;
                        }
                    }
                }

                geometry.Freeze();
                orientationGeom.Freeze();
                vecinityGeom.Freeze();
                drawingContext.DrawGeometry(renderBrush, renderPen, geometry);
                if(showOrientation)
                    drawingContext.DrawGeometry(renderBrush, orientationPen, orientationGeom);
                if(showVicinity)
                    drawingContext.DrawGeometry(null, vecinityPen, vecinityGeom);
            }
        }

    }
}
