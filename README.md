# Vision
Vision using OpenCV

Change Log

October 18, 2016 

   Added shape recognition (only can detect rectangles as of now)
   New functions:
        + getFocalLength() , will get the focal length of the camera according to rectWidth
        + distanceToObject() , will find the distance of the object depending on the focal length
        and the rectWidth
            - returned value is in inches
    Draws the distance measured in the drawBounding() method --only for rectangles as of now


   @TODO
   Transition and implement everything into object orientation
   Main.cpp will only be used for creating objects and running them
   Create separate functions in a class that will:
   + Threshold and contour one color at a time
        - Find threshold for Red, Green, Blue, etc, separately
   + Output the threshold and contour results all into one window
   Clean up functions for better readability and optimization

October 17, 2016 

	@TODO
	Transition and implement everything into object orientation
	Main.cpp will only be used for creating objects and running them 
	Create separate functions in a class that will:
	+ Threshold and contour one color at a time
     		-Find threshold for Red, Green, Blue, etc, separately
	+ Output the threshold and contour results all into one window

October 13, 2016 

	Function drawLargest() optimized for better performance
	Function drawBounding now takes in 3 parameters
	Cleaned up a lot of useless variables and optimized ControlElements structure

October 13, 2016

Applied a Median Filter before the conversion from BGR to HSV for noise reduction
Improved contours and bounding boxes
Will only draw contours and bounding boxes with the largest area
Bounding boxes now have text indicating the shape, currently only have Rectangle and Circle
Contours are improved, using RETR_EXTERNAL instead of RETR_CCOMP
Removed the Object Detection control window as well as its elements
Removed the shape track bar, now only using crosses
Removed canny
