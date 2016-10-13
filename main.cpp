/* Change Log
 *

   Function drawLargest() optimized for better performance
   Function drawBounding now takes in 3 parameters
   Cleaned up a lot of useless variables and optimized ControlElements structure

   @TODO
   Create separate functions or classes that will:
   + Threshold and contour one color at a time
        - Find threshold for Red, Green, Blue, etc, separately
   + Output the threshold and contour results all into one window

 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

//Globals
Mat imgOriginal, imgThresholded, imgHSV;
vector<vector<Point>> cont;

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

    //How many Morphological passes
    int passVal = 0;

};

//Color structure
struct Color{

    //B, G, R
    Scalar RED = Scalar(0, 0, 255);
    Scalar GREEN = Scalar(0, 255, 0);
    Scalar BLUE = Scalar(255, 0 ,0);
    Scalar WHITE = Scalar(255, 255, 255);
    Scalar YELLOW = Scalar(0, 215, 255);
    Scalar ORANGE = Scalar(0, 165, 255);
    Scalar BLACK = Scalar(0, 0, 0);

};

//createControl() uses ControlElements structure, initialize e before createControl()
ControlElements e;
Color color;

//Creates all control windows
void createControl();

//Draws largest contours and bounding boxes
void drawLargest();

//Draws the bounding boxes for contours
void drawBounding(int i, vector<vector<Point>> contours, Scalar c);

int main(int argc, char* argv[])
{
    //Open video camera no. 0
    VideoCapture cap(0);

    if (!cap.isOpened())
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

        //Break loop if unsuccessful
        if (!bSuccess)
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        //Median filter to reduce noise before converting to HSV
        medianBlur(imgOriginal, imgOriginal, 3);

        //Convert frame from BGR to HSV
        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

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
        Mat threshClone(imgThresholded.clone());

        //Apply a Gaussian Blur to Thresholded image
        GaussianBlur(imgThresholded, imgThresholded, Size(e.kX, e.kY), e.sX, e.sY);

        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1) , e.passVal);
        dilate( imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1), e.passVal);

        //morphological closing (fill small holes in the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1), e.passVal);
        erode(imgThresholded, imgThresholded, getStructuringElement(shape, Size(e.sizeX, e.sizeY)), Point(-1, -1), e.passVal);

        //Find contours
        findContours(threshClone, cont, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        //Draw largest contours and bounding boxes
        drawLargest();

        //Show the capture stream
        imshow("Thresholded Image", imgThresholded);
        imshow("Contours", imgOriginal);
        //imshow("HSV", imgHSV);

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
    cvCreateTrackbar("HighS", "Control", &e.iHighS, 255);

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

}

void drawLargest(){

    int largestArea(10000);
    for(int i = 0; i < cont.size(); i++){

        double area = contourArea((cont[i]), false);
        if(area > largestArea){

            drawContours(imgOriginal, cont, i, color.GREEN, 2);
            drawBounding(i, cont, color.BLUE);
        }
    }
}

void drawBounding(int i, vector<vector<Point>> contours, Scalar c){

    Point2f points[4];
    Point2f center;
    float radius;
    Rect rect;
    RotatedRect rotate_rect;

    //Compute the bounding rect, rotated bounding rect, minimum enclosing circle
    rect = boundingRect(contours[i]);
    rotate_rect = minAreaRect(contours[i]);
    minEnclosingCircle(contours[i], center, radius);

    rotate_rect.points(points);

    double rectCenterX = rect.x + rect.width / 2;
    double rectCenterY = rect.y + rect.height / 2;

    vector<vector<Point> > polylines;
    polylines.resize(1);
    for (int j = 0; j < 4; ++j)
        polylines[0].push_back(points[j]);

    //Draw them on the bounding image
    if (rect.area() >= contourArea(contours[i])) {

        circle(imgOriginal, Point(rectCenterX, rectCenterY), 5, color.WHITE, 2);
        putText(imgOriginal, "Rectangle", Point(rectCenterX, rectCenterY),
                FONT_HERSHEY_SIMPLEX, .5, color.BLACK, 2);
        //cv::rectangle(imgOriginal, rect, Scalar(0,255,255), 2);
        cv::polylines(imgOriginal, polylines, true, c, 2);
    }
    if (radius >= contourArea(contours[i])) {

        putText(imgOriginal, "Circle", Point(rectCenterX, rectCenterY),
                FONT_HERSHEY_SIMPLEX, .5, color.BLACK, 2);
        circle(imgOriginal, center, radius, c, 2);
    }
}
