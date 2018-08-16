package vip.frendy.cv

import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.widget.SeekBar
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_test.*
import vip.frendy.cv.util.FileUtils
import vip.frendy.opencv.OpenCVManager
import java.io.File

/**
 * Created by frendy on 2018/7/6.
 */
class TestActivity: AppCompatActivity() {

    private var mSeekbarType = -1

    private var mTouchX = 35f
    private var mTouchY = 35f

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_test)

        //测试图片
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.test)
        //显示原图
        image.setImageBitmap(bitmap)
        image.setOnImageViewTouchListener { event ->
            mTouchX = event.getX()
            mTouchY = event.getY()
        }

        bw.setOnClickListener {
            //转换
            val bwBitmap = OpenCVManager.getInstance().toBW(bitmap)
            //显示图片
            image.setImageBitmap(bwBitmap)
        }

        bokeh.setOnClickListener {
            mSeekbarType = 0
            seekbar.progress = 10
            seekbar.max = 20
        }

        bokehCircle.setOnClickListener {
            mSeekbarType = 1
            seekbar.progress = 10
            seekbar.max = 20
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

        shape.setOnClickListener {
            mSeekbarType = 4
            //转换
            val shapeBitmap = OpenCVManager.getInstance().toStretch(bitmap)
            //显示图片
            image.setImageBitmap(shapeBitmap)
        }

        cylinder.setOnClickListener {
            mSeekbarType = 5
            //转换
            val newBitmap = OpenCVManager.getInstance().toCylinder(bitmap)
            //显示图片
            image.setImageBitmap(newBitmap)
        }

        warp.setOnClickListener {
            startActivity(Intent(this, WarpActivity::class.java))
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

        initHandDetector(bitmap)
    }

    private fun initHandDetector(bitmap: Bitmap) {
        val targetPath = FileUtils.getModelPath()
        runOnUiThread { Toast.makeText(this, "Copy model to " + targetPath, Toast.LENGTH_SHORT).show() }
        FileUtils.copyFileFromRawToOthers(applicationContext, R.raw.cascade, targetPath)

        finger.setOnClickListener {
            val size = OpenCVManager.getInstance().getFingerCount(bitmap, 4000)
            toast("finger size is $size")
        }
    }

    fun updateBokeh(bitmap: Bitmap, _blurSize: Int) {
        val blurSize = if(_blurSize <= 0) 0 else _blurSize
        val blurSizeTransit = blurSize * 0.5
        //转换
        val tmpBitmap = OpenCVManager.getInstance().toBokeh(bitmap,
                mTouchX.toInt(), mTouchY.toInt(), 200, 200, blurSizeTransit.toInt(), 1)
        val newBitmap = OpenCVManager.getInstance().toBokeh(tmpBitmap,
                mTouchX.toInt(), mTouchY.toInt(), 400, 400, blurSize, 1)
        //显示图片
        image.setImageBitmap(newBitmap)
    }

    fun updateBokehCircle(bitmap: Bitmap, _blurSize: Int) {
        val blurSize = if(_blurSize <= 0) 0 else _blurSize
        val blurSizeTransit = blurSize * 0.5
        //转换
        val tmpBitmap = OpenCVManager.getInstance().toBokehWithCircle(bitmap, 100, blurSizeTransit.toInt(), 1)
        val newBitmap = OpenCVManager.getInstance().toBokehWithCircle(tmpBitmap, 200, blurSize, 1)
        //显示图片
        image.setImageBitmap(newBitmap)
    }

    fun updateEnlarge(bitmap: Bitmap, strength: Int) {
        //转换
        val bigBitmap = OpenCVManager.getInstance().toEnlarge(bitmap, mTouchX.toInt(), mTouchY.toInt(), 80, strength);
        //显示图片
        image.setImageBitmap(bigBitmap)
    }

    fun toast(msg: String) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show()
    }
}