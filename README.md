# ImageRotation
Fast Image Rotation algorithms

Playground for studing and implementing fast image rotation algoritms (with reasonable complexity).
The code here based on many refrences.

## For non-visual studio use
* After build,
```
$ ImageRotation [file_name] [ratio of center x position in new image] [ratio of center y position in new image] [rotation angle in degree] [0 for tiling nonzero for doing just once] 
```
* For more usage examples, see "usage.sh"


# References (and Thanks to!)

* "Fast Bitmap Rotation and Scaling" By Steven Mortimer, Dr Dobbs' Journal, July 01, 2001"<br/>
   http://www.drdobbs.com/architecture-and-design/fast-bitmap-rotation-and-scaling/184416337
* "Rotate Bitmap Any Angle Using Scanline Property" <br/>
   http://www.efg2.com/Lab/ImageProcessing/RotateScanline.htm
* "Bitmap API" Project <br/>
   https://github.com/wernsey/bitmap
