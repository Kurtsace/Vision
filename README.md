# Vision
Vision using OpenCV

Change Log

October 13, 2016 || 1:00 PM

	Applied a Median Filter before the conversion from BGR to HSV for noise reduction
    	Improved contours and bounding boxes
   	Will only draw contours and bounding boxes with the largest area
    	Bounding boxes now have text indicating the shape, currently only have Rectangle and Circle
    	Contours are improved, using RETR_EXTERNAL instead of RETR_CCOMP
    	Removed the Object Detection control window as well as its elements
    	Removed the shape track bar, now only using crosses
    	Removed canny
