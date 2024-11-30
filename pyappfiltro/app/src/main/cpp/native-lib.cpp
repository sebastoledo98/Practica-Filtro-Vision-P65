#include <jni.h>
#include <opencv2/core.hpp>
#include <string>
#include "android/bitmap.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

// Librerías de OpenCV
#include <opencv2/core/core.hpp> // Funciones base (representación de matrices, operaciones, etc.)
#include <opencv2/highgui/highgui.hpp> // Funciones de interfaz gráfica
#include <opencv2/imgcodecs/imgcodecs.hpp> // Cargar y manipular imágenes en distintos formatos gráficos
#include <opencv2/imgproc/imgproc.hpp> // Operaciones de procesamiento sobre imágenes
#include <opencv2/video/video.hpp>     // Manejo de vídeo
#include <opencv2/videoio/videoio.hpp> // Lectura y escritura de vídeo
//
using namespace std;
using namespace cv;

const int LEVEL = 10;
extern "C" JNIEXPORT jstring JNICALL
Java_ups_est_pyappfiltro_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hola con OpenCV";
    return env->NewStringUTF(hello.c_str());
}

void bitmapToMat(JNIEnv * env, jobject bitmap, cv::Mat &dst, jboolean needUnPremultiplyAlpha){
    AndroidBitmapInfo info;
    void* pixels = 0;
    try {
        CV_Assert( AndroidBitmap_getInfo(env, bitmap, &info) >= 0 );
        CV_Assert( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                   info.format == ANDROID_BITMAP_FORMAT_RGB_565 );
        CV_Assert( AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0 );
        CV_Assert( pixels );
        dst.create(info.height, info.width, CV_8UC4);
        if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 )
        {
            cv::Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if(needUnPremultiplyAlpha) cvtColor(tmp, dst, cv::COLOR_mRGBA2RGBA);
            else tmp.copyTo(dst);
        } else {
// info.format == ANDROID_BITMAP_FORMAT_RGB_565
            cv::Mat tmp(info.height, info.width, CV_8UC2, pixels);
            cvtColor(tmp, dst, cv::COLOR_BGR5652RGBA);
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch(const cv::Exception& e) {
        AndroidBitmap_unlockPixels(env, bitmap);
//jclass je = env->FindClass("org/opencv/core/CvException");
        jclass je = env->FindClass("java/lang/Exception");
//if(!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nBitmapToMat}");
        return;
    }
}

void matToBitmap(JNIEnv * env, cv::Mat src, jobject bitmap, jboolean needPremultiplyAlpha) {
    AndroidBitmapInfo info;
    void* pixels = 0;
    try {
        CV_Assert( AndroidBitmap_getInfo(env, bitmap, &info) >= 0 );
        CV_Assert( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGB_565 );
        CV_Assert( src.dims == 2 && info.height == (uint32_t)src.rows && info.width == (uint32_t)src.cols );
        CV_Assert( src.type() == CV_8UC1 || src.type() == CV_8UC3 || src.type() == CV_8UC4 );
        CV_Assert( AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0 );
        CV_Assert( pixels );
        if( info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 )
        {
            cv::Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if(src.type() == CV_8UC1)
            {
                cvtColor(src, tmp, cv::COLOR_GRAY2RGBA);
            } else if(src.type() == CV_8UC3){cvtColor(src, tmp, cv::COLOR_RGB2RGBA);
            } else if(src.type() == CV_8UC4){
                if(needPremultiplyAlpha) cvtColor(src, tmp, cv::COLOR_RGBA2mRGBA);
                else src.copyTo(tmp);
            }
        } else {
// info.format == ANDROID_BITMAP_FORMAT_RGB_565
            cv::Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if(src.type() == CV_8UC1)
            {
                cvtColor(src, tmp, cv::COLOR_GRAY2BGR565);
            } else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, cv::COLOR_RGB2BGR565);
            } else if(src.type() == CV_8UC4){
                cvtColor(src, tmp, cv::COLOR_RGBA2BGR565);
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return;
    } catch(const cv::Exception& e) {
        AndroidBitmap_unlockPixels(env, bitmap);
//jclass je = env->FindClass("org/opencv/core/CvException");
        jclass je = env->FindClass("java/lang/Exception");
//if(!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return;
    }
}


void quantizacion(Mat& imagen, Mat& quantizado) {
    quantizado = Mat::zeros(imagen.size(), CV_8UC1);
    for (int i = 0; i < imagen.rows; i++) {
        for (int j = 0; j < imagen.cols; j++) {
            int pixel = imagen.at<uchar>(i, j);
            float delta = (float)255 / (LEVEL);
            int Q = (floor(pixel / delta) * delta) + (delta / 2);
            quantizado.at<uchar>(i, j) = Q;
        }
    }
}

void ASCII(Mat& imagen, Mat& caracteres, Mat& resultado) {
    Mat caracter;
    for (int i = 0; i < imagen.rows - 7; i += 8) {
        for (int j = 0; j < imagen.cols - 7; j += 8) {
            int pixel = imagen.at<Vec3b>(i,j)[1];
            switch (pixel) {
                case 12:
                    caracter = caracteres(Rect(0, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 38:
                    caracter = caracteres(Rect(8, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 63:
                    caracter = caracteres(Rect(16, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 89:
                    caracter = caracteres(Rect(24, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 114:
                    caracter = caracteres(Rect(32, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 140:
                    caracter = caracteres(Rect(40, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 165:
                    caracter = caracteres(Rect(48, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 191:
                    caracter = caracteres(Rect(56, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 216:
                    caracter = caracteres(Rect(64, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                case 242:
                    caracter = caracteres(Rect(72, 0, 8, 8));
                    caracter.copyTo(resultado(Rect(j, i, 8, 8)));
                    break;
                default:
                    break;
            }
        }
    }
}

void convAscii(Mat& imagen, Mat& caracteres, Mat& resultado){
    //caracteres.convertTo(caracteres, CV_8UC3);
    resize(caracteres, caracteres, (caracteres.size() / 3));
    caracteres.convertTo(caracteres, imagen.type());
    //Matrices donde se guarda la imagen en ASCII
    Mat ascii = Mat::zeros(imagen.size(), imagen.type());
    Mat asciiColor = Mat::zeros(imagen.size(), imagen.type());

    //Matrices para la conversion a ascii
    Mat escalar;
    Mat hls, bgr;
    Mat desaturada;
    //pasamos a HLS para trabajar con valores de iluminacion
    cvtColor(imagen, hls, COLOR_BGR2HLS);


    //reducimos la imagen para eliminar informacion
    resize(hls, escalar, (imagen.size() / 8), INTER_LINEAR);
    resize(escalar, escalar, imagen.size(), INTER_LINEAR);

    //desaturamos la imagen para trabajar solo con la iluminacion
    cvtColor(escalar, bgr, COLOR_HLS2BGR);
    cvtColor(bgr, desaturada, COLOR_BGR2HLS);
    //desaturada = escalar.clone();
    for(int i = 0; i < hls.rows; i++){
        for(int j = 0; j < hls.cols; j++){
            desaturada.at<Vec3b>(i,j)[2] = 0;
        }
    }

    bgr = Mat::zeros(escalar.size(), escalar.type());

    vector<Mat> canalesImagen;
    split(desaturada, canalesImagen);

    // Ecualizamos la imagen para realzar el contraste
    //CLAHE clahe = new createCLAHE(2.0, Size(8,8));
    //Ptr<CLAHE> clahe = createCLAHE(100, Size(8,8));
    //clahe->apply(canalesImagen[1], canalesImagen[1]);
    //cl1 = clahe.apply(img);

    //Quantizacion
    Mat LQuantizada;
    quantizacion(canalesImagen[1], LQuantizada);
    Mat canales[3] = {canalesImagen[0], LQuantizada, canalesImagen[2]};
    Mat quantizado;
    merge(canales, 3, quantizado);

    //convertimos a ASCII
    ASCII(LQuantizada, caracteres, ascii);

    ascii.convertTo(ascii, asciiColor.type());
    imagen.convertTo(bgr, asciiColor.type());
    //se multiplica la imagen original por el ASCII para obtener la imagen en colores
    bitwise_and(ascii, bgr, asciiColor);
    //cvtColor(asciiColor, asciiColor, COLOR_HLS2BGR);
    //cvtColor(desaturada, asciiColor, COLOR_HLS2BGR);

    asciiColor.copyTo(imagen);
    //return asciiColor;
}

extern "C" JNIEXPORT void JNICALL Java_ups_est_pyappfiltro_MainActivity_ascii(JNIEnv* env,jobject /*this*/,jobject bitmapIn, jobject caracteres, jobject bitmapOut){
    AndroidBitmapInfo infoIn, infoChars;
    void *pixelsIn, *pixelsChar;
    AndroidBitmap_getInfo(env, bitmapIn, &infoIn);
    AndroidBitmap_lockPixels(env, bitmapIn, &pixelsIn);

    AndroidBitmap_getInfo(env, caracteres, &infoChars);
    AndroidBitmap_lockPixels(env, caracteres, &pixelsChar);

    cv::Mat frame(infoIn.height, infoIn.width, CV_8UC4, pixelsIn);

    cv::Mat chars; //(infoChars.height, infoChars.width, CV_8UC1, pixelsChar);

    AndroidBitmap_unlockPixels(env,caracteres);
    //cv::cvtColor(chars, chars, );

    bitmapToMat(env, bitmapIn, frame, false);  // bitmapToMat es una función personalizada
    bitmapToMat(env, caracteres, chars, false);  // bitmapToMat es una función personalizada
    //Mat caracteres = imread("./caracteres.png", IMREAD_UNCHANGED);

    Mat ascii;
    ascii = Mat::zeros(frame.size(), frame.type());
    convAscii(frame, chars, ascii);
    //cvtColor(frame, frame, COLOR_BGR2GRAY);

    matToBitmap(env, frame, bitmapOut, false);

}