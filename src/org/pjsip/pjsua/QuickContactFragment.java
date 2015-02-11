package org.pjsip.pjsua;

import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.TypedValue;
import android.view.Display;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.TextView;

import com.dd.CircularProgressButton;

public class QuickContactFragment extends DialogFragment {

	/*private PagerSlidingTabStrip tabs;
	private ViewPager pager;
	private ContactPagerAdapter adapter;*/

	public static QuickContactFragment newInstance() {
		QuickContactFragment f = new QuickContactFragment();
		return f;
	}

	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {

		if (getDialog() != null) {
			getDialog().getWindow().requestFeature(Window.FEATURE_NO_TITLE);
			getDialog().getWindow().setBackgroundDrawableResource(android.R.color.transparent);
		}

		/*View root = inflater.inflate(R.layout.fragment_quick_contact, container, false);*/

		/*tabs = (PagerSlidingTabStrip) root.findViewById(R.id.tabs);
		pager = (ViewPager) root.findViewById(R.id.pager);
		adapter = new ContactPagerAdapter();

		pager.setAdapter(adapter);

		tabs.setViewPager(pager);*/

		/*return root;*/
		View DialogFragment = inflater.inflate(R.layout.fragment_quick_contact, container, false);
		
		final CircularProgressButton button_login = (CircularProgressButton) DialogFragment.findViewById(R.id.button_login);
		button_login.setIndeterminateProgressMode(true);
		button_login.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (button_login.getProgress() == 0) {
                	button_login.setProgress(50);
                } else if (button_login.getProgress() == 100) {
                	button_login.setProgress(0);
                } else {
                	button_login.setProgress(100);
                }
            }
        });
		
		final CircularProgressButton button_logoff = (CircularProgressButton) DialogFragment.findViewById(R.id.button_logoff);
		button_logoff.setIndeterminateProgressMode(true);
		button_logoff.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (button_logoff.getProgress() == 0) {
                	button_logoff.setProgress(50);
                } else if (button_logoff.getProgress() == 100) {
                	button_logoff.setProgress(0);
                } else {
                	button_logoff.setProgress(100);
                }
            }
        });
		
    	return DialogFragment;
	}

	@SuppressWarnings("deprecation")
	@Override
	public void onStart() {
		super.onStart();

		// change dialog width
		if (getDialog() != null) {

			int fullWidth = getDialog().getWindow().getAttributes().width;

			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR2) {
				Display display = getActivity().getWindowManager().getDefaultDisplay();
				Point size = new Point();
				display.getSize(size);
				fullWidth = size.x;
			} else {
				Display display = getActivity().getWindowManager().getDefaultDisplay();
				fullWidth = display.getWidth();
			}

			final int padding = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 24, getResources()
					.getDisplayMetrics());

			int w = fullWidth - padding;
			int h = getDialog().getWindow().getAttributes().height;

			getDialog().getWindow().setLayout(w, h);
		}
	}

	@Override
	public void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
	}

/*	public class ContactPagerAdapter extends PagerAdapter implements IconTabProvider {

		private final int[] ICONS = { R.drawable.ic_action_user};

		public ContactPagerAdapter() {
			super();
		}

		@Override
		public int getCount() {
			return ICONS.length;
		}

		@Override
		public int getPageIconResId(int position) {
			return ICONS[position];
		}

		@Override
		public Object instantiateItem(ViewGroup container, int position) {
			// looks a little bit messy here
			TextView v = new TextView(getActivity());
			v.setBackgroundResource(R.color.background_window);
			v.setText("PAGE " + (position + 1));
			final int padding = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 16, getResources()
					.getDisplayMetrics());
			v.setPadding(padding, padding, padding, padding);
			v.setGravity(Gravity.CENTER);
			container.addView(v, 0);
			return v;
		}

		@Override
		public void destroyItem(ViewGroup container, int position, Object view) {
			container.removeView((View) view);
		}

		@Override
		public boolean isViewFromObject(View v, Object o) {
			return v == ((View) o);
		}

	}
*/
	
}
