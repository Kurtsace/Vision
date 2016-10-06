# Vision
Vision using OpenCV

Changes 1.0
Added more control features to track bar
Optimized Morphological operators for Thresholding
Added a Gaussian blur

Changes 1.1
Added Canny operators for edge detection
Canny uses Thresholded image for edge detection
Optimized Gaussian blur kernels along with the X & Y sigma
Added better noise reduction for Canny edge detection

Changes 1.2
Added a min radius and min area
Bounding boxes and circles wont draw anything that's less than min radius and area
Color improvements to bounding boxes and contours
No longer using Canny for edge detection, but function is still working and available for use
