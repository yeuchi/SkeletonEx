package com.ctyeung.skeletonex

import android.app.Application
import android.content.Context
import java.lang.reflect.Type

class App : Application() {

    init {
        instance = this
    }

    companion object {
        var lastSubActivity: Type = MainActivity::class.java
        private var instance: App? = null

        fun applicationContext() : Context {
            return instance!!.applicationContext
        }
    }

    override fun onCreate() {
        super.onCreate()
        // initialize for any
    }
}