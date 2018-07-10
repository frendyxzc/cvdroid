package vip.frendy.opencv.utils;

import android.graphics.Bitmap;

/**
 * Created by frendy on 2018/7/10.
 */

public class ShapeHelper {

    public static Bitmap enlarge(Bitmap bitmap, int PointX, int PointY, int Radius, int Strength) {
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int Left = PointX - Radius < 0 ? 0 : PointX - Radius; // 计算边界值
        int Top = PointY - Radius < 0 ? 0 : PointY - Radius;
        int Bottom = PointY + Radius >= height ? height - 1 : PointY + Radius;
        int Right = PointX + Radius >= width ? width - 1 : PointX + Radius;
        Bitmap newBitmap = bitmap.copy(Bitmap.Config.ARGB_8888, true);
        int PowRadius = Radius * Radius;

        for (int Y = Top; Y <= Bottom; Y++) {
            int offSetY = Y - PointY;
            for (int X = Left; X <= Right; X++) {
                int offSetX = X - PointX;
                double XY = offSetX * offSetX + offSetY * offSetY;
                if (XY <= PowRadius) {
                    double ScaleFactor = 1 - XY / PowRadius;
                    ScaleFactor = 1 - (double) Strength / 100 * ScaleFactor; // 按照这种关系计算取样点的位置
                    int PosX = (int) (offSetX * ScaleFactor + PointX);
                    int PosY = (int) (offSetY * ScaleFactor + PointY);
                    if (PosX < 0) { // 放置越界
                        PosX = 0;
                    } else if (PosX >= width) {
                        PosX = width - 1;
                    }
                    if (PosY < 0) {
                        PosY = 0;
                    } else if (PosY >= height) {
                        PosY = height - 1;
                    }
                    newBitmap.setPixel(X, Y, bitmap.getPixel(PosX, PosY));
                }
            }
        }
        return newBitmap;
    }
}
