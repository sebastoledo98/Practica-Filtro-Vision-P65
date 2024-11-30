package ups.est.pyappfiltro;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.TextView;


import android.os.Bundle;
import android.widget.Button;
import android.widget.Toast;
import android.widget.VideoView;
import android.widget.MediaController;
import android.net.Uri;
import androidx.appcompat.app.AppCompatActivity;

import com.longdo.mjpegviewer.MjpegView;

import java.net.HttpURLConnection;
import java.net.URL;

import ups.est.pyappfiltro.databinding.ActivityMainBinding;


public class MainActivity extends AppCompatActivity {

    // Used to load the 'proyecto_camara' library on application startup.
    static {
        System.loadLibrary("pyappfiltro");
    }

    private ActivityMainBinding binding;

    private android.widget.Button btnConectar;
    private android.widget.ImageView original;
    private Button button;
    private Handler handler;

    private MjpegView mjpeg;
    private static final long CAPTURE_INTERVAL = 100; // Intervalo de captura en milisegundos

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        mjpeg = findViewById(R.id.mjpegview);
        handler = new Handler(getMainLooper());
        original = findViewById(R.id.imageView);

        button = findViewById(R.id.button);
        //button.setOnClickListener(v -> iniciarStream());
        //iniciarStream();

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                iniciarStream();
            }
        });

    }

    private void iniciarStream() {
        mjpeg.setMode(MjpegView.MODE_FIT_WIDTH);
        mjpeg.setAdjustHeight(true);
        mjpeg.setSupportPinchZoomAndPan(true);
        mjpeg.setUrl("http://192.168.62.134:81/stream");
        mjpeg.setRecycleBitmap(true);
        mjpeg.startStream();

        iniciarCapturaDeFrames();
    }

    private void iniciarCapturaDeFrames() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                Bitmap frame = capturarFrame();
                if (frame != null) {
                    procesarBitmap(frame);
                }
                handler.postDelayed(this, CAPTURE_INTERVAL);
            }
        });
    }

    //private Bitmap capturarFrame() {
    //if (mjpeg.isLaidOut()) {
    //return mjpeg.drawToBitmap();
    //} else {
    //return null; // Si la vista no está lista, retorna null
    //}
    //}

    private Bitmap capturarFrame() {
        mjpeg.setDrawingCacheEnabled(true);
        mjpeg.buildDrawingCache();
        Bitmap bitmap = Bitmap.createBitmap(mjpeg.getDrawingCache());
        mjpeg.setDrawingCacheEnabled(false);  // Desactiva para evitar consumir memoria innecesaria
        return bitmap;
    }

    private void procesarBitmap(Bitmap bitmap) {
        Bitmap copia = bitmap.copy(bitmap.getConfig(), true);
        Bitmap caracteres = BitmapFactory.decodeResource(getResources(), R.drawable.caracteres2);
        ascii(bitmap,caracteres,copia);
        original.setImageBitmap(copia);  // Mostrar la imagen capturada
    }

    @Override
    protected void onResume() {
        super.onResume();
        mjpeg.startStream();
        iniciarCapturaDeFrames();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mjpeg.stopStream();
        detenerCapturaDeFrames();
    }

    private void detenerCapturaDeFrames() {
        handler.removeCallbacksAndMessages(null);
    }


    /**
     * A native method that is implemented by the 'proyecto_camara' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native String stringButton();
    public native void ascii(android.graphics.Bitmap in, android.graphics.Bitmap caracteres, android.graphics.Bitmap out);
}