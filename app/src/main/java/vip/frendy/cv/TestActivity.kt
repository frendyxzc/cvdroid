package vip.frendy.cv

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.SeekBar
import kotlinx.android.synthetic.main.activity_test.*
import vip.frendy.opencv.OpenCVManager

/**
 * Created by frendy on 2018/7/6.
 */
class TestActivity: AppCompatActivity() {

    private var mSeekbarType = -1

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_test)

        //测试图片
        val bitmap = BitmapFactory.decodeResource(resources, R.mipmap.ic_launcher)
        //显示原图
        image.setImageBitmap(bitmap)

        bw.setOnClickListener {
            //转换
            val bwBitmap = OpenCVManager.getInstance().toBW(bitmap)
            //显示图片
            image.setImageBitmap(bwBitmap)
        }

        bokeh.setOnClickListener {
            mSeekbarType = 0
            seekbar.progress = 10
            seekbar.max = 60
        }

        bokehCircle.setOnClickListener {
            mSeekbarType = 1
            seekbar.progress = 10
            seekbar.max = 60
        }

        enlarge.setOnClickListener {
            mSeekbarType = 2
            seekbar.progress = 50
            seekbar.max = 100
        }

        enlarge_native.setOnClickListener {
            mSeekbarType = 3
            val width = bitmap.width
            val height = bitmap.height
            val buf = IntArray(width * height)
            bitmap.getPixels(buf, 0, width, 1, 1, width - 1, height - 1)
            val result = OpenCVManager.getInstance().toEnlarge(buf, width, height, 80, 80, 40, 1f)
            val bigBitmap = Bitmap.createBitmap(result, width, height, Bitmap.Config.ARGB_8888)
            //显示图片
            image.setImageBitmap(bigBitmap)
        }

        seekbar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                if(mSeekbarType == 0) {
                    updateBokeh(bitmap, progress)
                } else if(mSeekbarType == 1) {
                    updateBokehCircle(bitmap, progress)
                } else if(mSeekbarType == 2) {
                    updateEnlarge(bitmap, 50 - progress)
                }
            }
            override fun onStartTrackingTouch(seekBar: SeekBar?) {}
            override fun onStopTrackingTouch(seekBar: SeekBar?) {}
        })
    }

    fun updateBokeh(bitmap: Bitmap, _blurSize: Int) {
        val blurSize = if(_blurSize <= 0) 0 else _blurSize
        //转换
        val bwBitmap = OpenCVManager.getInstance().toBokeh(bitmap,
                35, 35, 60, 60, blurSize)
        //显示图片
        image.setImageBitmap(bwBitmap)
    }

    fun updateBokehCircle(bitmap: Bitmap, _blurSize: Int) {
        val blurSize = if(_blurSize <= 0) 0 else _blurSize
        //转换
        val bwBitmap = OpenCVManager.getInstance().toBokehWithCircle(bitmap, 40, blurSize)
        //显示图片
        image.setImageBitmap(bwBitmap)
    }

    fun updateEnlarge(bitmap: Bitmap, strength: Int) {
        //转换
        val bigBitmap = OpenCVManager.getInstance().toEnlarge(bitmap, 80, 80, 40, strength);
        //显示图片
        image.setImageBitmap(bigBitmap)
    }
}