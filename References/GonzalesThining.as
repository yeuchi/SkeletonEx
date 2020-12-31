// ==================================================================
// Module:		GonzalesThining.as
//
// Description:	Thinning algorithm as defined in the text,
//				Digital Image processing by Gonzales & Woods, 1993.
//				Pages (492-493), 8.1.5 Skeleton of a Region.
//
//				Whole thing can be done in pixelbender but let me
//				try it out in actionscript first.
//
// Input:		Image with bounding pixels of a closed shape over
//				white backgroun.
//
// Output:		skeletonized line
//
// Author(s):	Chi T. Yeung	(cty)
//
// History:
// ==================================================================
package com.ctyeung
{
	import flash.display.BitmapData;
	import flash.display.Shader;
	import flash.display.ShaderJob;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.utils.ByteArray;
	
	import mx.core.UIComponent;

	public class GonzalesThining
	{
		public static const DEFAULT_COLOR:uint = 0xFFFFFF;
		public static const OUTER_BOUND_COLOR:uint = 0xFF0000FF;
		
		protected static const STEP1:String = "step1";
		protected static const STEP2:String = "step2";

		protected static const P2:int = 0;
		protected static const P4:int = 2;
		protected static const P6:int = 4;
		protected static const P8:int = 6;

		
		protected var listNeighbors:Array;
		protected var bmdDelete:BitmapData;
		public var bmd:BitmapData;
		
		public function GonzalesThining(wid:int,
										len:int) {
			bmd = new BitmapData(wid, len, false, 0xFFFFFF);
			bmdDelete = new BitmapData(wid, len, false, 0);
		}
		
		public function dispose():void {
			if(bmd)
				bmd.dispose();
			
			if(bmdDelete)
				bmdDelete.dispose();
			
			bmdDelete = null;
			bmd = null;
		}
		
		public function clear():void {
			bmd.fillRect(bmd.rect, 0x0);
			bmdDelete.fillRect(bmd.rect, 0x0);
		}
		
		public function apply(uic:UIComponent):void {		// [in] input graphics object
			clear();
			bmd.draw(uic);
			bmd.floodFill(0,0, OUTER_BOUND_COLOR);
			var bDeleted:Boolean = true;
			
			while(bDeleted) {
				bDeleted = false;				
				if(thinning(STEP1)) {
					deleteBorder();
					bDeleted = true;
				}
				if(thinning(STEP2)) {
					deleteBorder();
					bDeleted = true;
				}
			}
		}
		
		protected function deleteBorder():void {
			for (var y:int=1; y<bmd.height; y++) {
				for(var x:int=1; x<bmd.width; x++) {
					var clr:uint = bmdDelete.getPixel(x,y);
					if(clr==0xFFFFFF)
						bmd.setPixel(x,y, 0xFF00);
				}
			}
		}
		
		protected function step(str:String):Boolean {
			switch(str) {
				case STEP1:
					return step1CD();
				
				default:
				case STEP2:
					return step2CD();
			}
		}
		
		protected function thinning(str:String):Boolean {
			var bDelete:Boolean = false;
			
			for (var y:int=1; y<bmd.height; y++) {
				for(var x:int=1; x<bmd.width; x++) {
					var clr:uint = bmd.getPixel(x,y);
					if(clr==Skeleton.BACKGROUND_COLOR) {
						getNeighbors(x,y);
						if(correctCount()) {
							if(transition()) {
								if(step(str)){
									bmdDelete.setPixel(x,y,0xFFFFFF);
									bDelete = true;
								}
							}
						}
					}
				}
			}
			return bDelete;
		}
		
		protected function getNeighbors(x:int, y:int):void {
			var p2:uint = bmd.getPixel(x, y-1);
			var p3:uint = bmd.getPixel(x+1, y-1);
			var p4:uint = bmd.getPixel(x+1, y);
			var p5:uint = bmd.getPixel(x+1, y+1);
			var p6:uint = bmd.getPixel(x, y+1);
			var p7:uint = bmd.getPixel(x-1, y+1);
			var p8:uint = bmd.getPixel(x-1, y);
			var p9:uint = bmd.getPixel(x-1, y-1);
			listNeighbors = [p2, p3, p4, p5, p6, p7, p8, p9];
		}

		// pg 492 criterior (a)
		protected function correctCount():Boolean {	// [out] meets 2<=N(p1)<=6 criterior	
			var total:int=0;
			for(var i:int=0; i<listNeighbors.length; i++) {
				if(listNeighbors[i]==Skeleton.BACKGROUND_COLOR) 
					total ++;
			}
			return(2<=total&&total<=6)?true:false;
		}
		
		// pg 492 criterior (b)
		protected function transition():Boolean {	// [out] meets S(p1)==1 criterior
			var total:int=0;
			for(var i:int=0; i<listNeighbors.length-1; i++) {
				if(listNeighbors[i]!=Skeleton.BACKGROUND_COLOR)
					if(listNeighbors[i+1]==Skeleton.BACKGROUND_COLOR)
						total ++;
			}
			if(listNeighbors[listNeighbors.length-1]!=Skeleton.BACKGROUND_COLOR)
				if(listNeighbors[0]==Skeleton.BACKGROUND_COLOR)
					total ++;
			
			return (total==1)?true:false;
		}
		
		// pg 492 criterior (c, d) step 1
		protected function step1CD():Boolean {
			if(listNeighbors[P4]!=Skeleton.BACKGROUND_COLOR)
				return true;
					
			if(listNeighbors[P6]!=Skeleton.BACKGROUND_COLOR)
				return true;
			
			if(listNeighbors[P2]!=Skeleton.BACKGROUND_COLOR&&
			   listNeighbors[P8]!=Skeleton.BACKGROUND_COLOR)
				return true;

			return false;
		}
		
		// pg 492 criterior (c, d) step 2
		protected function step2CD():Boolean {
			if(listNeighbors[P2]!=Skeleton.BACKGROUND_COLOR)
				return true;
			
			if(listNeighbors[P8]!=Skeleton.BACKGROUND_COLOR)
				return true;
			
			if(listNeighbors[P6]!=Skeleton.BACKGROUND_COLOR&&
			   listNeighbors[P4]!=Skeleton.BACKGROUND_COLOR)
				return true;
			
			return false;
		}
	}
}