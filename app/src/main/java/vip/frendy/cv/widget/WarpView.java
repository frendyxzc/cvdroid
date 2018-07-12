package vip.frendy.cv.widget;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.view.MotionEvent;
import android.view.View;

/**
 * Created by frendy on 2018/7/12.
 */
public class WarpView extends View {
    private Bitmap bitmap;

    //定义两个常量,这两个常量指定该图片横向,纵向上都被划分为20格
    private final int WIDTH = 20;
    private final int HEIGHT = 20;

    //记录该图片上包含441个顶点
    private final int COUNT = (WIDTH + 1) * (HEIGHT + 1);

    //定义一个数组,记录Bitmap上的21*21个点的坐标
    private final float[] verts = new float[COUNT * 2];

    //定义一个数组,记录Bitmap上的21*21个点经过扭曲后的坐标
    //对图片扭曲的关键就是修改该数组里元素的值
    private final float[] orig = new float[COUNT * 2];

    /*1.构造方法*/
    public WarpView(Context context, int drawableId) {
        super(context);
        //使当前位图获取焦点
        setFocusable(true);
        //根据指定资源加载图片
        bitmap = BitmapFactory.decodeResource(getResources(), drawableId);
        float bitmapWidth = bitmap.getWidth();
        float bitmapHeight = bitmap.getHeight();
        int index = 0;
        for (int y = 0; y <= HEIGHT; y++) {
            float fy = bitmapHeight * y / HEIGHT;
            for (int x = 0; x <= WIDTH; x++) {
                float fx = bitmapWidth * x / WIDTH;
                //初始化orig,verts数组
                //初始化,orig,verts两个数组均匀地保存了21 * 21个点的x,y坐标　
                orig[index * 2 + 0] = verts[index * 2 + 0] = fx;
                orig[index * 2 + 1] = verts[index * 2 + 1] = fy;
                index += 1;
            }
        }
        setBackgroundColor(Color.WHITE);    //设置背景颜色
    }

    /**
     * 2.绘图方法
     * 功能：绘制图形。对bitmap按verts数组进行扭曲
     * 从第一个点(由第5个参数0控制)开始扭曲
     */
    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawBitmapMesh(bitmap, WIDTH, HEIGHT, verts, 0, null, 0, null);
    }

    /**
     * 3.工具方法
     * 功能：用于根据触摸事件的位置计算verts数组里各元素的值
     */
    private void warp(float cx, float cy) {
        for (int i = 0; i < COUNT * 2; i += 2) {
            float dx = cx - orig[i + 0];
            float dy = cy - orig[i + 1];
            float dd = dx * dx + dy * dy;
            //计算每个坐标点与当前(cx,cy)之间的距离
            float d = (float) Math.sqrt(dd);
            //计算扭曲度，距离当前点(cx,cy)越远，扭曲度越小
            float pull = 80000 / (dd * d);
            //对verts数组(保存bitmap上21*21个点经过扭曲后的坐标)重新赋值
            if (pull >= 1) {
                verts[i + 0] = cx;
                verts[i + 1] = cy;
            } else {
                //控制各顶点向触摸事件发生点偏移
                verts[i + 0] = orig[i + 0] + dx * pull;
                verts[i + 1] = orig[i + 1] + dy * pull;
            }
            invalidate();    //通知View组件重绘
        }
    }

    /**
     * 4.触摸屏监听器
     * 功能：触碰事件响应-调用warp方法根据触摸屏事件的坐标来扭曲verts数组
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        warp(event.getX(), event.getY());
        return true;
    }

    //作用范围半径
    private int r = 100;
    public void warp2(float startX, float startY, float endX, float endY) {
        //计算拖动距离
        float ddPull = (endX - startX) * (endX - startX) + (endY - startY) * (endY - startY);
        float dPull = (float) Math.sqrt(ddPull);
        //文献中提到的算法，并不能很好的实现拖动距离 MC 越大变形效果越明显的功能，下面这行代码则是我对该算法的优化
        //dPull = screenWidth - dPull >= 0.0001f ? screenWidth - dPull : 0.0001f;

        for (int i = 0; i < COUNT * 2; i += 2) {
            //计算每个坐标点与触摸点之间的距离
            float dx = orig[i] - startX;
            float dy = orig[i + 1] - startY;
            float dd = dx * dx + dy * dy;
            float d = (float) Math.sqrt(dd);

            //文献中提到的算法同样不能实现只有圆形选区内的图像才进行变形的功能，这里需要做一个距离的判断
            if (d < r) {
                //变形系数，扭曲度
                double e = (r * r - dd) * (r * r - dd) / ((r * r - dd + dPull * dPull) * (r * r - dd + dPull * dPull));
                double pullX = e * (endX - startX);
                double pullY = e * (endY - startY);
                verts[i] = (float) (orig[i] + pullX);
                verts[i + 1] = (float) (orig[i + 1] + pullY);
            }
        }
        invalidate();
    }
}
