/* Change Log
 *

    Applied a Median Filter before the conversion from BGR to HSV for noise reduction
    Removed the shape track bar

 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

//Globals
Mat srcGray, dst, detectedEdges, imgOriginal, imgThresholded;
vector<vector<Point>> contours;

//Variables that contain values for the control window
struct ControlElements{

    //Hue, Saturation, Value
    int iLowH = 0;
    int iHighH = 179;

    int iLowS = 0;
    int iHighS = 255;

    int iLowV = 0;
    int iHighV = 255;

    //Size of the Morphological Shape
    int sizeX = 0;
    int sizeY = 0;

    //Kernel size for Gaussian Blur
    int kX = 1;
    int kY = 1;
    int kMax = 31;

    //Sigma for Gaussian blur
    int sX = 0;
    int sY = 0;
    int sMax = 10;

    //How many passes
    int passVal = 0;

    //Shapes
    //int shapes = 0;

    //Canny thresholds
    int edgeThresh = 1;
    int lowThresh;
    int const max_lowThresh = 100;
    int ratio = 3;
    int kSizeX = 1;
    int kSizeY = 1;

    //Object detection
    int minSize = 0; //Minimum size of rect to draw (Area)
    int minRadius = 0; //Minimum radius to draw

};

//createControl() uses ControlElements structure, initialize e before createControl()
ControlElements e;

//Canny thresholding function
void cannyThresh(int, void*);

void createControl();//Creates the control window
void drawBounding(); //Draws bounding boxes and circles for contours

int main(int argc, char* argv[])
{
    VideoCapture cap(0); // open the video camera no. 0

    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }

    //Creates all the control windows and track bars
    createControl();

    while (1)
    {

        //Read a video stream from a webcam and store it in imgOriginal
        bool bSuccess = cap.read(imgOriginal);

        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        Mat imgHSV;

        //Median filter to reduce noise before converting to HSV
        medianBlur(imgOriginal, imgOriginal, 3);

        //Convert frame from BGR to HSV
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

        //Apply a blur to HSV
        blur(imgHSV, imgHSV, Size(3, 3));

        //Threshold the image
        inRange(imgHSV, Scalar(e.iLowH, e.iLowS, e.iLowV), Scalar(e.iHighH, e.iHighS, e.iHighV), imgThresholded);

        //Kernel cannot be an even value
        if( e.kX % 2 == 0 && e.kX <= e.kMax)
            e.kX++;
        if( e.kY % 2 == 0 && e.kY <= e.kMax)
            e.kY++;

        //Size of the shape cannot be 0
        if(e.sizeX <= 0)
            e.sizeX = 1;
        if(e.sizeY <= 0)
            e.sizeY = 1;

        //Change shapes
        MorphShapes shape = MORPH_CROSS;

        //Clone imgThresholded into threshClone
        Mat threshClone = imgThresholded.clone();

        //Apply a Gaussian Blur to Thresholded image
        GaussianBlur(imgThresholded, imgThresholded, Size(e.kX, e.kY), e.sX, e.sY);

        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1) , e.passVal);
        dilate( imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1), e.passVal);

        //morphological closing (fill small holes in the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1), e.passVal);
        erode(imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1), e.passVal);

        //Create a matrix of the same size and type as imgOriginal
        dst.create(imgOriginal.size(), imgOriginal.type());

        //Convert imgOriginal to gray scale and output to srcGray
        cvtColor(imgOriginal, srcGray, CV_BGR2GRAY);

        //cannyThresh(0,0);

        //Find contours
        findContours(threshClone, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

        //Draw contours
        drawContours(imgOriginal, contours, -1, Scalar(0, 255, 0), 2);

        //Draw a bounding box on contours
        drawBounding();

        //Show the capture stream
        imshow("Thresholded Image", imgThresholded); //Show the thresholded image
        imshow("Contours", imgOriginal); //Show contours and bounding boxes
        //imshow("HSV", imgHSV); //Show HSV

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }

    return 0;
}

void createControl(){

    //Create track bars in "Control" window

    namedWindow("Control" ,CV_WINDOW_AUTOSIZE); //create a window called "Control"

    //Hue, Saturation, and Value
    cvCreateTrackbar("LowH", "Control", &e.iLowH, 179); //Hue (0-179)
    cvCreateTrackbar("HighH", "Control", &e.iHighH, 179);

    cvCreateTrackbar("LowS", "Control", &e.iLowS, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighS", "Control", &e.iHighH, 255);

    cvCreateTrackbar("LowV", "Control", &e.iLowV, 255); //Value (0-255)
    cvCreateTrackbar("HighV", "Control", &e.iHighV, 255);

    //Morphological Shape
    cvCreateTrackbar("SizeX", "Control", &e.sizeX, 30); //Size of Morphological Shape (0 - 30);
    cvCreateTrackbar("SizeY", "Control", &e.sizeY, 30);

    //Gaussian Blur
    cvCreateTrackbar("KernelX", "Control", &e.kX, e.kMax); //Kernel size (0 - 31);
    cvCreateTrackbar("KernelY", "Control", &e.kY, e.kMax);

    //if you want to use Sigma controllers uncomment and set sigmaX and sigmaY to e.sY and e.sY in main
    cvCreateTrackbar("SigmaX", "Control", &e.sX, e.sMax); //Sigma (0 - 10)
    cvCreateTrackbar("SigmaY", "Control", &e.sY, e.sMax);

    // Number of passes for Morphological Operators, Opening and Closing
    cvCreateTrackbar("Passes", "Control", &e.passVal, 10);

    //Canny Window
    namedWindow("Edge Controller", CV_WINDOW_AUTOSIZE);

    //Canny Track bars
    //Threshold
    cvCreateTrackbar("Low Thresh", "Edge Controller", &e.lowThresh, e.max_lowThresh);

    //Blur
    cvCreateTrackbar("Kernel X", "Edge Controller", &e.kSizeX, 100);
    cvCreateTrackbar("Kernel Y", "Edge Controller", &e.kSizeY, 100);

    //Shape detection
    namedWindow("Object Detection", CV_WINDOW_AUTOSIZE);

    //Minimum size
    cvCreateTrackbar("Min Size", "Object Detection", &e.minSize, 300000);
    cvCreateTrackbar("Min Radius", "Object Detection", &e.minRadius, 400);
}

void drawBounding(){

    for(int i = 0; i < contours.size(); ++i)
    {
        Point2f points[4];
        Point2f center;
        float radius;
        Rect rect;
        RotatedRect rotate_rect;

        //compute the bounding rect, rotated bounding rect, minimum enclosing circle.
        rect = boundingRect(contours[i]);
        rotate_rect = minAreaRect(contours[i]);
        minEnclosingCircle(contours[i], center, radius);

        rotate_rect.points(points);

        vector< vector<Point> > polylines;
        polylines.resize(1);
        for(int j = 0; j < 4; ++j)
            polylines[0].push_back(points[j]);

        //draw them on the bounding image.

        if(rect.area() >= e.minSize){

            //cv::rectangle(imgOriginal, rect, Scalar(0,255,255), 2);
            cv::polylines(imgOriginal, polylines, true, Scalar(255, 0, 0), 2);
        }

        if(radius >= e.minRadius){

            circle(imgOriginal, center, radius, Scalar(0, 0, 255), 2);
        }
    }
}

void cannyThresh(int, void*){

    //Error checking --Kernel sizes cannot be 0
    if(e.kSizeX <= 0)
        e.kSizeX = 1;
    if(e.kSizeY <= 0)
        e.kSizeY = 1;

    //Noise reduction with a size ExE kernel
    blur(srcGray, detectedEdges, Size(e.kSizeX, e.kSizeY));

    //Canny detector
    Canny(detectedEdges, detectedEdges, e.lowThresh, e.lowThresh * e.ratio, 3);

    //Output as a mask
    dst = Scalar::all(0);

    imgThresholded.copyTo(dst, detectedEdges);
    imshow("Canny", dst);

}