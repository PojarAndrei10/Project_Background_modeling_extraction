#include "stdafx.h"
#include "common.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/utils/logger.hpp>

using namespace std;
using namespace cv;


void formulaRunningAverage(Mat src, Mat dst, float alpha)
{
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            for (int k = 0; k < src.channels(); k++)
            {
                //dst(x,y)=(1-alpha).dst(x,y)+alpha.src(x,y)
                dst.at<Vec3f>(i, j)[k] = (1 - alpha) * dst.at<Vec3f>(i, j)[k] + alpha * src.at<Vec3b>(i, j)[k];
            }
        }
    }
}
double valAbsoluta(double x) {
    if (x < 0)
        return -x;
    else
        return x;
}
void convert(Mat& src, Mat& dst)
{
    dst.create(src.size(), CV_8UC(src.channels()));
    for (int i = 0; i < src.rows; i++)
    {
        Vec3f* srcRow = src.ptr<Vec3f>(i);
        Vec3b* dstRow = dst.ptr<Vec3b>(i);
        for (int j = 0; j < src.cols; j++)
        {
            for (int k = 0; k < src.channels(); k++)
            {
                dstRow[j][k] = saturate_cast<uchar>(valAbsoluta(srcRow[j][k]));
            }
        }
    }
}
bool isInside(Mat img, int  i, int j) 
{
	if (i >= 0 && j >= 0 && i < img.rows && j < img.cols)
		return true;
	return false;
}
Mat_<uchar> dilatare(Mat_<uchar> src, Mat_<uchar> elstr)
{
    //dst este destinatia=imaginea modificata
    //src este sursa si nu se schimba
    Mat_<uchar> dst(src.size());
    dst.setTo(255);
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            if (src(i, j) == 0)
            {
                for (int u = 0; u < elstr.rows; u++)
                {
                    for (int v = 0; v < elstr.cols; v++)
                    {
                        if (elstr(u, v) == 0)
                        {
                            int i2 = i + u - elstr.rows / 2;
                            int j2 = j + v - elstr.cols / 2;
                            if (isInside(dst, i2, j2))
                            {
                                dst(i2, j2) = 0;
                            }
                        }
                    }
                }

            }
        }
    }
    return dst;
}
int calculareLungimeVector(Vec3b& pixel) {
    int sum = pixel[0] * pixel[0] + pixel[1] * pixel[1] + pixel[2] * pixel[2];
    return sqrt(sum);
}
void binarizare(Mat& frameCurent, Mat& backgroundModel, Mat& threshold, int th) 
{
    if (frameCurent.size() != backgroundModel.size()) {
        return;
    }
    Mat dif;
    absdiff(frameCurent, backgroundModel, dif);

    threshold = Mat(dif.size(), CV_8UC1);
    threshold.setTo(0);
    for (int i = 0; i < dif.rows; i++)
    {
        for (int j = 0; j < dif.cols; j++)
        {

            if (calculareLungimeVector(dif.at<Vec3b>(i, j)) > th)
            {
                threshold.at<uchar>(i, j) = 255;
            }
        }
    }
}

void afisareFrames(Mat img, Mat& val1, Mat& val2) {

    Mat runningAverage1, runningAverage2;
    Mat binarFrame, binarFrame2;
    Mat dilat1, dilat2;

    //aplicare running avg
    formulaRunningAverage(img, val1, 0.1); //putem modifica alfa
    formulaRunningAverage(img, val2, 0.01);

    convert(val1, runningAverage1);
    convert(val2, runningAverage2);
  
    binarizare(img, runningAverage1, binarFrame, 30);
    binarizare(img, runningAverage1, binarFrame2, 30);

    Mat_<uchar> elstr(3, 3);
    elstr.setTo(0);
    dilat1 = dilatare(binarFrame, elstr);
    dilat2 = dilatare(binarFrame2, elstr);

    imshow("Result1RunningAverage", runningAverage1);
    imshow("Result2RunningAverage", runningAverage2);

    imshow("BinarizareFrame1", binarFrame);
    imshow("BinarizareFrame2", binarFrame2);
    imshow("DilatFrame1", dilat1);
    imshow("DilatFrame2", dilat2);
}

void incarcareDateProcesareFrame(string& path, int n) {
    Mat img, avg1, avg2;
    string number = "000000";

    //incarc primul cadru
    img = imread(path + number + ".jpg", IMREAD_COLOR);

    avg1 = Mat(img.size(), CV_32FC3);
    avg1.setTo(0);
    avg2 = Mat(img.size(), CV_32FC3);
    avg2.setTo(0);

    // procesare fiecare cadru in parte
    int a = 1;
    while (a < n) 
    {
        string aAsString = to_string(a);
        int diferentaLungime = number.length() - aAsString.length();
        number = number.substr(0, diferentaLungime) + aAsString; //preia o subsecventa a sirului si det cati de 0 sa puna
        img = imread(path + number + ".jpg", IMREAD_COLOR);
        a++;
        afisareFrames(img, avg1, avg2);
        imshow("Secventa de imagini originala", img);
        waitKey(30);
    }
}

int main() {
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
    string path = "C:/Users/andre/Documents/AN 3 SEM 2/PI/Proiect/511/511/input/in";
    string path2 = "C:/Users/andre/Documents/AN 3 SEM 2/PI/Proiect/fluidHighway/fluidHighway/input/in";
    string path3 = "C:/Users/andre/Documents/AN 3 SEM 2/PI/Proiect/highway/highway/input/in";
    string path4 = "C:/Users/andre/Documents/AN 3 SEM 2/PI/Proiect/IPPR2/IPPR2/input/in";
    string path5 = "C:/Users/andre/Documents/AN 3 SEM 2/PI/Proiect/MPEG4_40/MPEG4_40/input/in";
    string path6 = "C:/Users/andre/Documents/AN 3 SEM 2/PI/Proiect/IntelligentRoom/IntelligentRoom/input/in";
    incarcareDateProcesareFrame(path,866);
    //incarcareDateProcesareFrame(path2,141);
    //incarcareDateProcesareFrame(path3,1700);
    //incarcareDateProcesareFrame(path4,299);
    //incarcareDateProcesareFrame(path5,463);
    //incarcareDateProcesareFrame(path6,218);


    return 0;
}