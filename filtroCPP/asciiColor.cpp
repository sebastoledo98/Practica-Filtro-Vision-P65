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

Mat quantizacion(Mat imagen) {
    Mat quantizado = Mat::zeros(imagen.size(), CV_8UC1);
    for (int i = 0; i < imagen.rows; i++) {
        for (int j = 0; j < imagen.cols; j++) {
            int pixel = imagen.at<uchar>(i, j);
            float delta = (float)255 / (LEVEL);
            int Q = (floor(pixel / delta) * delta) + (delta / 2);
            quantizado.at<uchar>(i, j) = Q;
        }
    }
    return quantizado;
}

void ASCII(Mat imagen, Mat caracteres, Mat resultado) {
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

Mat convAscii(Mat imagen, Mat caracteres){
    //Matrices donde se guarda la imagen en ASCII
    Mat ascii;
    Mat asciiColor;

    //Matrices para la conversion a ascii
    Mat iluminacion;
    Mat downscale;
    Mat upscale;
    Mat hls, bgr;
    Mat desaturada;
    //pasamos a HLS para trabajar con valores de iluminacion
    cvtColor(imagen, hls, COLOR_BGR2HLS);
    //Ecualizamos la imagen para realzar el contraste
    //equalizeHist(hls, hls);


    //reducimos la imagen para eliminar informacion
    resize(hls, downscale, (imagen.size() / 8), INTER_LINEAR);
    resize(downscale, upscale, imagen.size(), INTER_LINEAR);

    //desaturamos la imagen para trabajar solo con la iluminacion
    desaturada = upscale.clone();
    for(int i = 0; i < hls.rows; i++){
        for(int j = 0; j < hls.cols; j++){
            desaturada.at<Vec3b>(i,j)[2] = 0;
        }
    }

    cvtColor(upscale, bgr, COLOR_HLS2BGR);

    vector<Mat> canalesImagen;
    split(desaturada, canalesImagen);

    //Ecualizamos la imagen para realzar el contraste
    Ptr<CLAHE> clahe = createCLAHE(40, Size(8,8));
    clahe->apply(canalesImagen[1], canalesImagen[1]);
    //Quantizacion
    Mat LQuantizada = quantizacion(canalesImagen[1]);
    Mat canales[3] = {canalesImagen[0], LQuantizada, canalesImagen[2]};
    Mat quantizadaHLS, quantizadaBGR;
    merge(canales, 3, quantizadaHLS);
    cvtColor(quantizadaHLS, quantizadaBGR, COLOR_HLS2BGR);

    //convertimos a ASCII
    ascii = Mat::zeros(imagen.size(), imagen.type());
    ASCII(quantizadaHLS, caracteres, ascii);

    //se multiplica la imagen original por el ASCII para obtener la imagen en colores
    bitwise_and(ascii, bgr, asciiColor);

    return asciiColor;
}

int main(int argc, char *argv[]) {

    VideoCapture video("/dev/video0");
    //VideoCapture video("/home/video.mp4");
    if(video.isOpened()){
        //Matrices donde se carga la imagen y los caracteres para la imagen y los bordes en ASCII
        //Mat original = imread("imagenOriginal.jpg", IMREAD_COLOR);
        //Mat original = imread("gonza.jpeg", IMREAD_COLOR);
        Mat original;
        //Mat original = imread("prueba2.png", IMREAD_COLOR);
        Mat bordesOriginal = imread("pruebaBordes.png", IMREAD_COLOR);
        Mat caracteres = imread("caracteres.png", IMREAD_GRAYSCALE);
        Mat caracteresColor = imread("caracteres.png", IMREAD_COLOR);


        namedWindow("Original", WINDOW_AUTOSIZE);
        //namedWindow("ASCII", WINDOW_AUTOSIZE);
        namedWindow("ASCIIColor", WINDOW_AUTOSIZE);

        while(true){
            video >> original;

            Mat asciiColor = convAscii(original, caracteresColor);

            imshow("Original", original);
            //imshow("ASCII", ascii);
            imshow("ASCIIColor", asciiColor);


            if(waitKey(23)==27)
                break;
        }
    }
    video.release();
    destroyAllWindows();

    return 0;
}
