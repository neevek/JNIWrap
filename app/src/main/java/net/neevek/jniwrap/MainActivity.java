package net.neevek.jniwrap;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
  private final static String TAG = "JNIWrap";

  static {
    System.loadLibrary("jniwrap");
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);


    StringBuilder sb = new StringBuilder();
    TextView textView = findViewById(R.id.sample_text);
    textView.setText(getString());
  }

  public native String getString();
}
