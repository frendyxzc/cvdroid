package vip.frendy.cv.widget;

import android.content.Context;
import android.support.annotation.Nullable;
import android.support.v7.widget.AppCompatImageView;
import android.util.AttributeSet;
import android.view.MotionEvent;

/**
 * Created by frendy on 2018/7/13.
 */

public class TouchImageView extends AppCompatImageView {
    private OnImageViewTouchListener mListener;

    public TouchImageView(Context context) {
        super(context);
    }

    public TouchImageView(Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    public TouchImageView(Context context, @Nullable AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    public void setOnImageViewTouchListener(OnImageViewTouchListener listener) {
        mListener = listener;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if(mListener != null) {
            mListener.onImageViewTouched(event);
        }
        return super.onTouchEvent(event);
    }

    public interface OnImageViewTouchListener {
        void onImageViewTouched(MotionEvent event);
    }
}
