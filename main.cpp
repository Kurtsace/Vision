/* Change Log
 *
 */

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

//Globals
Mat imgOriginal, imgThresholded, imgHSV;
vector<vector<Point>> cont;
vector<Point> contPoly;

//Variables that contain values for the control window
struct ControlElements{

    //Hue, Saturation, Value
    int iLowH = 162;
    int iHighH = 179;

    int iLowS = 50;
    int iHighS = 255;

    int iLowV = 40;
    int iHighV = 255;

    //Size of the Morphological Shape
    int sizeX = 3;
    int sizeY = 3;

    //Kernel size for Gaussian Blur
    int kX = 5;
    int kY = 5;
    int kMax = 31;

    //Sigma for Gaussian blur
    int sX = 3;
    int sY = 3;
    int sMax = 10;

    //How many Morphological passes
    int passVal = 2;

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

void drawThresh();

//Draws largest contours and bounding boxes
void findLargest(Scalar color);

//Draws the bounding boxes for contours
void drawBounding(int i, vector<vector<Point>> contours, Scalar c);

//Return the focal length in inches
int getFocalLength(int pxWidth, int distance, double realWidth);

//Return the distance of the object from the camera
double distanceToObject(int focalLength, double width);

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

        //Read a video stream from a web cam and store it in imgOriginal
        bool bSuccess = cap.read(imgOriginal);

        //Break loop if unsuccessful
        if (!bSuccess)
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        //Median filter to reduce noise before converting to HSV
        medianBlur(imgOriginal, imgOriginal, 3);

        //Draw the thresholded image with contours
        drawThresh();

        //Show the capture stream
        namedWindow("Threshold", WINDOW_NORMAL);
        resizeWindow("Threshold", 480, 360);
        imshow("Threshold", imgThresholded);
        imshow("Contours", imgOriginal);
        imshow("HSV", imgHSV);

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }

    return 0;
}

void drawThresh(){

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

    //find largest contours and then draw contours with bounding boxes
    findLargest(color.RED);

}

void findLargest(Scalar color) {

    //Determine the largest contour area to draw
    double largestArea = 1;
    int largestIndex = -1;

    for (int i = 0; i < cont.size(); i++) {

        double area = contourArea(cont[i]);

        if (area > largestArea) {
            largestArea = area;
            largestIndex = i;
        }
    }

    if(largestIndex != -1){

        drawBounding(largestIndex, cont, color);
    }
}

void drawBounding(int i, vector<vector<Point>> contours, Scalar c){

    Point2f points[4];
    Point2f center;
    float radius;
    Rect rect;
    RotatedRect rotate_rect;

    //Calculate polygon approximation
    approxPolyDP(Mat(contours[i]), contPoly, arcLength(contours[i], true) * 0.01, true);

    //Compute the bounding rect, rotated bounding rect, minimum enclosing circle
    rect = boundingRect(contours[i]);
    rotate_rect = minAreaRect(contours[i]);
    minEnclosingCircle(contours[i], center, radius);

    rotate_rect.points(points);

    //Calculate rectangle width
    double rectWidth = rect.br().x - rect.tl().x;

    //Calculate focal length and the distance of the object
    cout << getFocalLength(rectWidth, 10, 3.5) << endl;
    double distance;

    //Calculate center points of rectangle
    double rectCenterX = rect.x + rect.width / 2;
    double rectCenterY = rect.y + rect.height / 2;

    vector<vector<Point>> polylines;
    polylines.resize(1);
    for (int j = 0; j < 4; ++j)
        polylines[0].push_back(points[j]);

    string distString;

    //Check to see what shape it is
    bool isRect = contPoly.size() >= 4 && contPoly.size() <= 7;
    bool isCircle = contPoly.size() >= 8 && contPoly.size() <= 18;

    //Draw correct bounding shapes and contours
    if (isRect) {

        distance = distanceToObject(570, rectWidth);
        distString = to_string(distance) + " IN.";
        drawContours(imgOriginal, cont, i, c, 2);
        circle(imgOriginal, Point(rectCenterX, rectCenterY), 5, color.WHITE, 2);
        putText(imgOriginal, "Rectangle", Point(rectCenterX + 10, rectCenterY),
                FONT_HERSHEY_SIMPLEX, .5, color.BLACK, 2);
        putText(imgOriginal, distString, Point(rectCenterX + 10, rectCenterY + 20),
                FONT_HERSHEY_SIMPLEX, .40, color.BLACK, 1.5);
        cv::polylines(imgOriginal, polylines, true, c, 2);

    } else if (isCircle) {

        distance = distanceToObject(570, radius * 2);
        distString = to_string(distance) + " IN.";
        drawContours(imgOriginal, cont, i, c, 2);
        circle(imgOriginal, Point(rectCenterX, rectCenterY), 5, color.WHITE, 2);
        putText(imgOriginal,"Circle", Point(rectCenterX, rectCenterY),
                FONT_HERSHEY_SIMPLEX, .5, color.BLACK, 2);
        putText(imgOriginal, distString, Point(rectCenterX + 10, rectCenterY + 20),
                FONT_HERSHEY_SIMPLEX, .40, color.BLACK, 1.5);
        circle(imgOriginal, center, radius, c, 2);

    } else {

        drawContours(imgOriginal, cont, i, color.WHITE, 2);
        circle(imgOriginal, Point(rectCenterX, rectCenterY), 5, color.WHITE, 2);
        putText(imgOriginal, "UNDEFINED", Point(rectCenterX + 10, rectCenterY),
                FONT_HERSHEY_SIMPLEX, .5, color.BLACK, 2);
        putText(imgOriginal, "UNDEFINED", Point(rectCenterX + 10, rectCenterY + 20),
                FONT_HERSHEY_SIMPLEX, .40, color.BLACK, 1.5);
        cv::polylines(imgOriginal, polylines, true, color.BLACK, 2);
    }
}

int getFocalLength(int pxWidth, int distance, double realWidth){

    return (pxWidth * distance) / realWidth;
}

double distanceToObject(int focalLength, double width) {

    return (focalLength * 3.5) / width;
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