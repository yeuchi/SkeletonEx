package com.ctyeung.skeletonex

import android.content.Context
import android.graphics.Canvas
import android.util.AttributeSet

class MyImageView :androidx.appcompat.widget.AppCompatImageView {
    constructor(context: Context) : this(context, null)
    constructor(context: Context, attrs: AttributeSet?) : this(context, attrs, 0)
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int) : super(
        context,
        attrs,
        defStyleAttr
    ) {

    }

     var myWidth = 0
     var myHeight = 0

    override fun onDraw(canvas: Canvas) {
        super.onDraw(canvas)
        myWidth = this.measuredWidth
        myHeight = this.measuredHeight
    }
}