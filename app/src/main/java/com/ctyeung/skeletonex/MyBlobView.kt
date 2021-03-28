package com.ctyeung.skeletonex

import android.content.Context
import android.graphics.*
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import java.util.ArrayList

open class MyBlobView : View {
    constructor(context: Context) : this(context, null)
    constructor(context: Context, attrs: AttributeSet?) : this(context, attrs, 0)
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int) : super(
        context,
        attrs,
        defStyleAttr
    ) {

    }

    var listPoints = ArrayList<PointF>()

    init {
        isFocusable = true
        isFocusableInTouchMode = true
        listPoints = ArrayList()
    }

    fun clear() {
        listPoints.clear()
        invalidate() // Indicate view should be redrawn
    }

    override fun onDraw(canvas: Canvas) {
        when(listPoints.size) {
            0 -> canvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.MULTIPLY);
            1 -> drawPoint(listPoints[0], canvas)
            2 -> drawLine(listPoints[0], listPoints[1], canvas)
            else -> {   // have triangle(s) !

                var path:Path = Path()
                var p0 = listPoints[0]
                path.moveTo(p0.x, p0.y)
                for(i in 1..listPoints.size-1){
                    var p = listPoints[i]
                    path.lineTo(p.x, p.y)
                }
                path.lineTo(p0.x, p0.y)
                path.close()

                drawTriangle(path, canvas)
            }
        }
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        val point = PointF(event.x, event.y)

        // Checks for the event that occurs
        when (event.action) {
            MotionEvent.ACTION_DOWN -> {
            }

            /*case MotionEvent.ACTION_MOVE:
                path.lineTo(point.x, point.y);
                break;*/

            MotionEvent.ACTION_UP -> {
                onActionUp(point)
            }

            else -> return false
        }// Starts a new line in the path

        return true
    }

    private fun onActionUp(p: PointF) {
        listPoints?.add(p)
        invalidate()
    }

    private fun drawTriangle(path:Path,
                             canvas: Canvas) {
        val paint = Paint()
        paint.isAntiAlias = true
        paint.strokeWidth = 3f
        paint.style = Paint.Style.FILL
        paint.color = Color.GREEN
        canvas?.drawPath(path, paint)
    }

    private fun drawLine(p1: PointF,
                         p2: PointF,
                         canvas: Canvas,
                         color:Int? = Color.GREEN) {
        val paint = Paint()
        paint.isAntiAlias = true
        paint.strokeWidth = 3f
        paint.style = Paint.Style.STROKE
        paint.strokeJoin = Paint.Join.ROUND
        paint.strokeCap = Paint.Cap.ROUND
        paint.color = color!!
        canvas?.drawLine(p1.x, p1.y, p2.x, p2.y, paint)
    }

    private fun drawPoint(p: PointF, canvas: Canvas) {
        val paint = Paint()
        paint.style = Paint.Style.FILL
        paint.color = Color.GREEN
        canvas?.drawCircle(p.x, p.y, 5F, paint);
    }
}