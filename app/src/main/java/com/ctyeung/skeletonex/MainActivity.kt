package com.ctyeung.skeletonex

import android.graphics.Bitmap
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.MenuItem
import android.widget.TextView
import android.widget.Toast

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        supportActionBar?.setHomeButtonEnabled(true)

        // Example of a call to a native method
       // findViewById<TextView>(R.id.sample_text).text = stringFromJNI()
    }

    fun enableBackButton(show:Boolean) {
        supportActionBar?.setDisplayHomeAsUpEnabled(show)
    }

    fun setTittle(str:String) {
        supportActionBar?.setTitle(str)
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id: Int = item.getItemId()
        if (id == android.R.id.home) {
            onBackPressed()
            return true
        }
        return super.onOptionsItemSelected(item)
    }

    /*
     * Gonzales Thinning invoked once
     */
    fun thinOnce(bmpIn:Bitmap, bmpOut:Bitmap):Int {
        try {
            return imageThinOnceFromJNI(bmpIn, bmpOut)
        }
        catch (ex:java.lang.Exception) {
            Toast.makeText(this,
                ex.toString() as String,
                Toast.LENGTH_LONG).show()
        }
        return 0
    }

    external fun imageThinOnceFromJNI(bmpIn: Bitmap?,
                                      bmpOut: Bitmap?):Int

    companion object {
        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}