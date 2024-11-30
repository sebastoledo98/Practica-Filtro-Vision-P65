// Librería base para flujos de entrada y salida
#include <cmath>
#include <cwctype>
#include <iostream>
// Librería para el manejo de memoria, búsqueda, ordenamiento, etc.
#include <cstdlib>
// Librería para el manejo de cadenas de texto
#include <cstring>
// Librería para el manejo de arreglos dinámicos (lista ligada)
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
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

const int LEVEL = 11;

Mat quantizacion(Mat imagen) {
    Mat quantizado = Mat::zeros(imagen.size(), CV_8UC1);
    for (int i = 0; i < imagen.rows; i++) {
        for (int j = 0; j < imagen.cols; j++) {
            // iluminacion.at<uchar>(i,j) = quantizacion(gris.at<uchar>(i,j));
            int pixel = imagen.at<uchar>(i, j);
            float delta = (float)255 / (LEVEL - 1);
            int Q = (floor(pixel / delta) * delta) + (delta / 2);
            // int Q = (floor(pixel*LEVEL))/LEVEL;
            quantizado.at<uchar>(i, j) = Q;
        }
    }
    return quantizado;
}

Mat generarEscala(int alto) {
    Mat escala = Mat::zeros(Size(256, alto), CV_8UC1);
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j <= 256; j++) {
            escala.at<uchar>(i, j) = j;
        }
    }
    return escala;
}

void ASCII(Mat imagen, Mat caracteres) {
    Mat caracter;
    for (int i = 0; i < imagen.rows - 7; i += 8) {
        for (int j = 0; j < imagen.cols - 7; j += 8) {
            switch (imagen.at<uchar>(i, j)) {
                case 12:
                    caracter = caracteres(Rect(0, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 38:
                    caracter = caracteres(Rect(8, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 63:
                    caracter = caracteres(Rect(16, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 89:
                    caracter = caracteres(Rect(24, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 114:
                    caracter = caracteres(Rect(32, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 140:
                    caracter = caracteres(Rect(40, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 165:
                    caracter = caracteres(Rect(48, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 191:
                    caracter = caracteres(Rect(56, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 216:
                    caracter = caracteres(Rect(64, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                case 242:
                    caracter = caracteres(Rect(72, 0, 8, 8));
                    caracter.copyTo(imagen(Rect(j, i, 8, 8)));
                    break;
                default:
                    break;
            }
        }
    }
}

int main(int argc, char *argv[]) {

    Mat original = imread("imagenOriginal.jpg", IMREAD_COLOR);
    Mat caracteres = imread("caracteres.png", IMREAD_GRAYSCALE);
    Mat iluminacion;
    Mat gris;
    Mat downscale;
    Mat upscale;
    Mat aux;

    namedWindow("Original", WINDOW_AUTOSIZE);
    namedWindow("Iluminacion", WINDOW_AUTOSIZE);

    cvtColor(original, gris, COLOR_RGB2GRAY);
    resize(gris, downscale, (gris.size() / 8), INTER_LINEAR_EXACT);
    resize(downscale, upscale, gris.size(), INTER_LINEAR_EXACT);

    iluminacion = quantizacion(upscale);
    ASCII(iluminacion, caracteres);
    imwrite("ascii.png", iluminacion);

    imshow("Original", original);
    imshow("Iluminacion", iluminacion);

    waitKey(0);

    return 0;
}
