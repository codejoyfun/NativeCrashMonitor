package com.example.nativecrash;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.example.nativecrash.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        binding.sampleText.setOnClickListener(this);

        NativeCrashMonitor nativeCrashMonitor = new NativeCrashMonitor();
        nativeCrashMonitor.init((threadName, javaStackInfo, nativeStackInfo) -> {
            Log.i("ningtukun", "\n" +"threadName: " + threadName + "\n" + "javaStackInfo:\n" + javaStackInfo + "\n" +"nativeStackInfo:\n" + nativeStackInfo);
        });
    }

    @Override
    public void onClick(View v) {
        NativeCrashMonitor.nativeCrash();
    }
}