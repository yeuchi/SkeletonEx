package com.ctyeung.skeletonex

import android.os.Bundle
import android.view.View
import androidx.fragment.app.Fragment

open class BaseFragment : Fragment(), IOnBackPressed {
    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        (activity as MainActivity).enableBackButton(true)
    }

    override fun onBackPressed(): Boolean {
        TODO("Not yet implemented")
    }
}