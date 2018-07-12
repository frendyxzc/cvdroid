package vip.frendy.cv

import android.app.Activity
import android.os.Bundle
import kotlinx.android.synthetic.main.activity_warp.*
import vip.frendy.cv.widget.WarpView

class WarpActivity : Activity() {

    private var warpView: WarpView? = null

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_warp)

        warpView = WarpView(this, R.drawable.image)

        content.addView(warpView)

        warp.setOnClickListener {
            toWarp(200f, 100f, 50f)
        }
    }

    private fun toWarp(startX: Float, startY: Float, strength: Float) {
        var _startX = startX
        var _startY = startY
        var _step = 1

        while (_step < 100) {
            warpView?.warp2(_startX, _startY, _startX + strength, _startY + strength)

            _startY ++
            _step ++
        }
    }

}