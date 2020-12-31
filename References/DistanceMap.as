// ==================================================================
// Module:		DistanceMap.as
//
// Description:	Skeletonization - erosion by distance map technique.
//				Follows the formulation as described in reference except
//				I am using the nearest boundary point to calculate my
//				distance.  Quad-tree comes to mind for optimization.
//
// Reference:	http://www.inf.u-szeged.hu/~palagyi/skel/skel.html
//
// Input:		close-loop shape (perimeter)
// Output:		bmd - containing grayscale image of distance from border.
//				bmdThres - threshold image from bmd.
//
// Author:		C.T. Yeung	cty
//
// History:
// 28Sep11		1st functional but too inefficient to be useful.	cty
// ==================================================================
package com.ctyeung.distanceMap
{
	import flash.display.BitmapData;
	import flash.display.Shader;
	import flash.display.ShaderJob;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	
	import mx.core.UIComponent;

	public class DistanceMap
	{
		public static const DEFAULT_COLOR:uint = 0xFFFFFF;
		public static const OUTER_BOUND_COLOR:uint = 0xFF0000FF;

		public var bmd:BitmapData;
		public var bmdThres:BitmapData;

		[Embed(source="assets/BinaryThreshold.pbj", mimeType="application/octet-stream")]
		protected var thresholdClass:Class;
		
		protected var listBound:Array;
		protected var multiplier:Number;
		
		public function DistanceMap(wid:int,
									len:int) {
			bmd = new BitmapData(wid, len, false, 0xFFFFFF);
			bmdThres = new BitmapData(wid, len, true, 0x0);
		}
		
		public function dispose():void {
			if(bmd)
				bmd.dispose();
			
			if(bmdThres)
				bmdThres.dispose();
			
			bmd = null;
			bmdThres = null;
			listBound = null;
		}
		
		public function clear():void {
			bmd.fillRect(bmd.rect, DEFAULT_COLOR);
			bmdThres.fillRect(bmd.rect, 0x0);
		}
		
		public function apply(uic:UIComponent,				// [in] input graphics object
							  chkThreshold:Boolean,			// [in] threshold selected ?
							  value:int)					// [in] threshold level
							  :void {
			bmd.draw(uic);
			if(collectBoundPoints()) {
				bmd.floodFill(0,0, OUTER_BOUND_COLOR);
				map();
				adjThreshold(chkThreshold, value);
			}
		}
		
		protected var rect:Rectangle;
		
		protected function collectBoundPoints():Boolean {	// [in] any shape exists ?
			//var r:Rectangle = findShape();
			//if(r) {
				//scaleThresholdRange(r);
				listBound = [];
				
				rect = new Rectangle(1000, 1000, 0,0);
				for(var y:int=0; y<bmd.height; y+=2) {
					for(var x:int=0; x<bmd.width; x+=2) {
						var clr:uint = bmd.getPixel(x,y);
						if(clr!=Skeleton.BACKGROUND_COLOR) {
							listBound.push(new Point(x,y));
							if(x<rect.x)
								rect.x = x;
							if(x>rect.x)
								rect.width = x;
							if(y<rect.y)
								rect.y = y;
							if(y>rect.y)
								rect.height = y;
						}
					}
				}
				if(rect.x==1000||rect.y==1000)
					return false;
				
				rect.height -= rect.y;
				rect.width -= rect.x;
				scaleThresholdRange(rect);
				return true;
		}
		
		private function findShape():Rectangle {
			// not working well.  Buggy.
			var mask:uint = DEFAULT_COLOR;
			var color:uint = Skeleton.SHAPE_BOUND_COLOR;
			return bmd.getColorBoundsRect(mask, color, true);
		}
		
		private function scaleThresholdRange(rect:Rectangle):void {
			// scale for distance value so threshold range is nearest to (0 - 255)
			var w:Number = Number(rect.width)/2.0;
			var h:Number = Number(rect.height)/2.0;
			multiplier = ((w<h)?h:w);
			multiplier = 255.0/multiplier;
		}
		
		protected function map():void {
			// would a quad tree be faster?  
			// need data structure here for performance.
			
			// also help if I only evaluate pixels within rect.
			var ph:Point = new Point();
			for(var y:int=0; y<bmd.height; y++) {
				for(var x:int=0; x<bmd.width; x++) {
					var clr:uint = bmd.getPixel(x,y);
					if(clr==Skeleton.BACKGROUND_COLOR){
						ph.x = x;
						ph.y = y;
						var min:Number = 1000;
						
						for each(var p:Point in listBound) {
							var dis:Number = Point.distance(p, ph);
							min = (min>dis)?dis:min;
						}
						min*=multiplier;
						clr = (min<<16)+(min<<8)+min;
						bmd.setPixel(x,y,clr);
					}
				}
			}
		}
		
		public function adjThreshold(chk:Boolean,	// [in] threhold ?
									 value:int)		// [in] water level
									 :void {		// [out] output

			if(chk) {
				var shader:Shader = new Shader();
				shader.byteCode = pixelBenderFilter;
				shader.data.src.input = bmd;
				shader.data.threshold.value = [Number(value)/255.0];
				
				var job:ShaderJob = new ShaderJob(shader); 
				job.target = bmdThres; 
				job.start();
			}
			else
				bmdThres.fillRect(bmdThres.rect, 0x0);
		}
		
		private function get pixelBenderFilter():ByteArray {
			return new thresholdClass() as ByteArray;
		}
	}
}