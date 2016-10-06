
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

//Globals
Mat srcGray, dst, detectedEdges, imgOriginal, imgThresholded;

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
    int shapes = 0;

    //Canny thresholds
    int edgeThresh = 0;
    int lowThresh;
    int const max_lowThresh = 100;
    int ratio = 3;
    int kSizeX = 1;
    int kSizeY = 1;

    //Object detection
    int minSize = 0; //Minimum size to draw
    int minRadius = 0;

};

//createControl() uses ControlElements structure, initialize e before createControl()
ControlElements e;

//Uncomment if you want to use canny
//void cannyThresh(int, void*); //Canny thresholding function
void createControl();//Creates the control window

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

    vector<vector<Point>> contours;

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

        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        blur(imgHSV, imgHSV, Size(3, 3));

        //Threshold the image
        inRange(imgHSV, Scalar(e.iLowH, e.iLowS, e.iLowV), Scalar(e.iHighH, e.iHighS, e.iHighV), imgThresholded);

        //Size of the Morphological Shape
        int x = e.sizeX;
        int y = e.sizeY;

        //Size of Gaussian Blur
        int kernelX = e.kX;
        int kernelY = e.kY;

        //Sigma of the Gaussian blur
        int sigmaX = e.sX;
        int sigmaY = e.sY;

        //How many passes of opening and closing Erosion/Dilation
        int passes = e.passVal;

        //Kernel cannot be an even value
        if( kernelX % 2 == 0 && kernelX <= e.kMax)
            kernelX++;
        if( kernelY % 2 == 0 && kernelY <= e.kMax)
            kernelY++;

        //Size of the shape cannot be 0
        if(x <= 0)
            x = 1;
        if(y <= 0)
            y = 1;

        //Change shapes
        MorphShapes shape;
        if(e.shapes == 0)
            shape = MORPH_CROSS;
        else if(e.shapes == 1)
            shape = MORPH_RECT;
        else
            shape = MORPH_ELLIPSE;

        Mat threshClone = imgThresholded.clone();

        //Apply a Gaussian Blur to Thresholded image
        GaussianBlur(imgThresholded, imgThresholded, Size(kernelX, kernelY), sigmaX, sigmaY);

        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(shape, Size(x, y)), Point(-1, -1) , passes);
        dilate( imgThresholded, imgThresholded, getStructuringElement(shape, Size(x, y)), Point(-1, -1), passes);

        //morphological closing (fill small holes in the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(shape, Size(x, y)), Point(-1, -1), passes);
        erode(imgThresholded, imgThresholded, getStructuringElement(shape, Size(x, y)), Point(-1, -1), passes);

        //Create a matrix of the same size and type as imgOriginal
        dst.create(imgOriginal.size(), imgOriginal.type());

        //Convert imgOriginal to gray scale and output to srcGray
        cvtColor(imgOriginal, srcGray, CV_BGR2GRAY);

        //cannyThresh(0,0);

        //Find contours
        findContours(threshClone, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        //Draw contours
        double area, length;
        drawContours(imgOriginal, contours, -1, Scalar(0, 255, 0), 2);

        //Draw a bounding box on contours
        for(int i = 0; i < contours.size(); ++i)
        {
            Point2f points[4];
            Point2f center;
            float radius;
            Rect rect;
            RotatedRect rotate_rect;

            //compute the bounding rect, rotated bounding rect, minum enclosing circle.
            rect = boundingRect(contours[i]);
            rotate_rect = minAreaRect(contours[i]);
            minEnclosingCircle(contours[i], center, radius);

            rotate_rect.points(points);

            vector< vector< Point> > polylines;
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

        //Show the capture stream
        imshow("Thresholded Image", imgThresholded); //Show the thresholded image
        imshow("Contours", imgOriginal); //Show contours and bounding boxes
        imshow("HSV", imgHSV); //Show HSV

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
    cvCreateTrackbar("Shape", "Control", &e.shapes, 2); // Shape (0 - Cross | 1 - Rect | 2 - Ellipse)

    //Gaussian Blur
    cvCreateTrackbar("KernelX", "Control", &e.kX, e.kMax); //Kernel size (0 - 31);
    cvCreateTrackbar("KernelY", "Control", &e.kY, e.kMax);

    //if you want to use Sigma controllers uncomment and set sigmaX and sigmaY to e.sY and e.sY in main
    cvCreateTrackbar("SigmaX", "Control", &e.sX, e.sMax); //Sigma (0 - 10)
    cvCreateTrackbar("SigmaY", "Control", &e.sY, e.sMax);

    // Number of passes for Morphological Operators, Opening and Closing
    cvCreateTrackbar("Passes", "Control", &e.passVal, 10);

    //Canny Window
    //namedWindow("Edge Controller", CV_WINDOW_AUTOSIZE);

    //Canny Track bars
    //Threshold
    //cvCreateTrackbar("Low Thresh", "Edge Controller", &e.lowThresh, e.max_lowThresh);

    //Blur
    //cvCreateTrackbar("Kernel X", "Edge Controller", &e.kSizeX, 100);
    //cvCreateTrackbar("Kernel Y", "Edge Controller", &e.kSizeY, 100);

    //Shape detection
    namedWindow("Object Detection", CV_WINDOW_AUTOSIZE);

    //Minimum size
    cvCreateTrackbar("Min Size", "Object Detection", &e.minSize, 300000);
    cvCreateTrackbar("Min Radius", "Object Detection", &e.minRadius, 400);
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