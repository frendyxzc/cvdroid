package vip.frendy.cv

import android.app.Activity
import android.os.Bundle
import android.util.Log
import android.widget.SeekBar
import kotlinx.android.synthetic.main.activity_warp.*
import vip.frendy.cv.widget.WarpView

class WarpActivity : Activity() {

    private var warpView: WarpView? = null

    private var mTouchX = 200f
    private var mTouchY = 600f

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_warp)

        warpView = WarpView(this, R.mipmap.girl)
        content.addView(warpView)

        warpView?.setOnWarpViewTouchListener { event ->
            mTouchX = event.getX()
            mTouchY = event.getY()
        }

        seekbar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                toWarpLeft(200f, 600f, progress.toFloat())
                toWarpRight(600f, 600f, progress.toFloat())
            }
            override fun onStartTrackingTouch(seekBar: SeekBar?) {}
            override fun onStopTrackingTouch(seekBar: SeekBar?) {}
        })

        seekbar2.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                toWarpLeft(600f, 680f, progress.toFloat())
                toWarpRight(200f, 680f, progress.toFloat())
            }
            override fun onStartTrackingTouch(seekBar: SeekBar?) {}
            override fun onStopTrackingTouch(seekBar: SeekBar?) {}
        })
    }

    private fun toWarpLeft(startX: Float, startY: Float, strength: Float) {
        var _startX = startX
        var _startY = startY
        var _step = 1

        while (_step < 100) {
            val _endX = _startX + strength
            val _endY = _startY

            warpView?.warp2(_startX, _startY, _endX, _endY)
            Log.e("warp", "** warp left : ($_startX, $_startY) - ($_endX, $_endY)")

            _startY += 1
            _step ++
        }
    }

    private fun toWarpRight(startX: Float, startY: Float, strength: Float) {
        var _startX = startX
        var _startY = startY
        var _step = 1

        while (_step < 200) {
            val _endX = _startX - strength
            val _endY = _startY

            warpView?.warp2(_startX, _startY, _endX, _endY)
            Log.e("warp", "** warp right : ($_startX, $_startY) - ($_endX, $_endY)")

            _startY += 1
            _step ++
        }
    }

}