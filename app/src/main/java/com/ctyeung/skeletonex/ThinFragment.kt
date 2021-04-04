package com.ctyeung.skeletonex

import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.drawable.BitmapDrawable
import android.opengl.Visibility
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.databinding.DataBindingUtil
import androidx.fragment.app.Fragment
import androidx.navigation.findNavController
import com.ctyeung.skeletonex.databinding.FragmentThinBinding

/**
 * A simple [Fragment] subclass.
 * Use the [ThinFragment.newInstance] factory method to
 * create an instance of this fragment.
 */
class ThinFragment : BaseFragment() {

    private lateinit var binding:FragmentThinBinding
    private var totalCount:Int = 0;

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        super.onCreateView(inflater, container, savedInstanceState)
        binding = DataBindingUtil.inflate(inflater, R.layout.fragment_thin, container, false)
        binding.layout = this;
        return binding!!.root;
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (activity as MainActivity).setTittle("Thinning")
    }

    override fun onBackPressed(): Boolean {
        binding!!.root.findNavController().navigate(R.id.action_thinFragment_to_mainFragment)
        return true
    }
    var bmpSrc:Bitmap? = null

    fun onClickThinOnce() {

        if(bmpSrc == null) {
            val v: View = binding.blobView as View
            bmpSrc = Bitmap.createBitmap(v.layoutParams.width, v.layoutParams.height, Bitmap.Config.ARGB_8888)
            var c = Canvas(bmpSrc!!)
            v.layout(v.getLeft(), v.getTop(), v.getRight(), v.getBottom());
            v.draw(c);

            binding.bmpView.setImageBitmap(bmpSrc)
            binding.blobView.clear()
            totalCount = 0
        }

        bmpSrc?.let {
            var bmpResult:Bitmap = Bitmap.createBitmap(bmpSrc!!)
            val count = (this.activity as MainActivity).thinOnce(bmpSrc!!, bmpResult)
            totalCount += count
            binding.txtCount.text = "Total: ${totalCount} Count: ${count}"

            // copy bmpResult into bmpSrc
            binding.bmpView.setImageBitmap(bmpResult)
            bmpSrc = bmpResult.copy(bmpResult.config, true)
            binding.blobView.visibility = View.GONE
        }
    }

    fun onClickClear() {
        binding.blobView.visibility = View.VISIBLE
        binding.blobView.clear()
        binding.bmpView.setImageBitmap(null)
        bmpSrc = null
        totalCount = 0
    }
}