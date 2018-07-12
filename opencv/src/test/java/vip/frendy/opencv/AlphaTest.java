package vip.frendy.opencv;

/**
 * Created by frendy on 2018/7/12.
 */

public class AlphaTest {

    /**
     * 改变透明通道的值
     * @param fraction [0,1]透明通道的值，0为全透明，1为不透明
     * @param color 需要改变的颜色值
     * @return 改变透明通道后的值，如：alphaEvaluator（0.5f,0xFFFFFFFF） = 0x80FFFFFF
     */
    public static int alphaEvaluator(float fraction, int color){
        int a = (color >> 24) & 0xff;
        int r = (color >> 16) & 0xff;
        int g = (color >> 8) & 0xff;
        int b = color & 0xff;

        return (int)((int)(256 * fraction) << 24) | r << 16 | g << 8 | b ;
    }
}
