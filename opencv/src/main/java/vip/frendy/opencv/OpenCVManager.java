package vip.frendy.opencv;

import android.graphics.Bitmap;

/**
 * Created by frendy on 2018/7/6.
 */

public class OpenCVManager {

    private static OpenCVManager manager;

    public static OpenCVManager getInstance() {
        if(manager == null) {
            manager = new OpenCVManager();
        }
        return manager;
    }

    private OpenCVManager() {
        System.loadLibrary("CVDroid");
    }

    public native Bitmap toBW(Bitmap bitmap);

    //TODO: 背景虚化，目前通过指定的遮罩来实现，后续可添加边缘检测抠出遮罩
    public native Bitmap toBokeh(Bitmap bitmap, int x, int y, int w, int h, int blurSize);
    public native Bitmap toBokehWithCircle(Bitmap bitmap, int r, int blurSize);

}
