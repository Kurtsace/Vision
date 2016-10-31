# Vision
Vision using OpenCV

Change Log

October 31, 2016 
	
	Old distance detection is still inaccurate. 
	Shape detection needs slight improvements. 
	Thresholding is very good but can use some slight improvements where needed.	

	@TODO
	Find an alternate algorithm for detecting the distance to object. 
	Replace old circle detection to HoughCircles. 
	Old shape detection needs improvements to be better at detecting shapes. 
	Thresholding should be improved if possible for better accuracy. 
	Noise reduction for better detection. 

October 25, 2016
	
	All functions except focal and distance functions are final.
	Still a few bugs with distance to object, innacuracy by ~5 inches the further away it is. 
	Everything is functional and has no more bugs or errors. 
	Drawing bounding boxes of certain sizes no longer crashes the program. 
	Function largestArea() will no longer draw all contours if the minimum area requirement isnt met. 
	Minor improvements to shape detection. 
	Shape detection can only detect rectangles and circles 

	@TODO
	Improve distance estimation. 
	Add more shapes to detect.
	Improve approximation --IF POSSIBLE

October 19, 2016

        Finalized most of the functions, they will no longer need optimizing or editing 
        Making slight improvements to remaining functions and algorithms 
        Cleaned up almost everything, structures, functions, variables 
    
       @TODO 
        Finish finalizing all other functions and algorithms 
        Make improvements for vertice detection (shape recognition) 
        Improve track bar usability
        Finalize all algorithms
        Check for more bugs 
    
       *Note 
        After everything is finalized I will be abandoning this main.cpp file 
        and moving it all into object orientation for easier use and optimal 
        readability 
     
        I will be regularly updating this file but mostly will be working on 
        the object orientation. 
     
	This file will be good enough to use after the next update or so.

October 18, 2016 

   	Added shape recognition (only can detect rectangles as of now)
   	New functions:
        	+ getFocalLength() , will get the focal length of the camera according to rectWidth
        	+ distanceToObject() , will find the distance of the object depending on the focal length
        	and the rectWidth
            		- returned value is in inches
    	Draws the distance measured in the drawBounding() method --only for rectangles as of now


   	@TODO
	Improve distanceToObject, one or two major bugs affecting its calculated distance
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
