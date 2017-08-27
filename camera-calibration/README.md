# How to Calibrate.

1. Make sure you have the OpenCV library and G++ compiler.
2. Plug in the desired Pixy into your laptop.
3. Go to terminal and change the directory to the folder `camera-calibration` in `FRC-2017`.
4. Enter the command `./compile.sh`
5. Enter the command `./a.out`
6. When prompted, enter the number of the Pixy you are using. It is **very** important that you do it with out spaces.
  ex: "4"
7. Click on the new window, focus the camera (by twisting the Pixy lens), and press the spacebar and then 'g'.
8. Take 8 pictures of the dot grid, from different angles, pressing the spacebar to take the picture. Make sure to only take pictures when the dot grid is recognized. You can see when the dot grid is detected by the colorful lines on the dots.
9. Once all 8 pictures are taken, it will show the undistorted image. Make sure that the calibration is working right.
10. Press Esc and close.
11. The camera matrix and distortion coefficients will be written to a `.java` file specific to the camera you calibrated.

Note: If using a certain number camera on the robot, go to `Constants.java`, and change `kPixyNumber` to the desired Pixy Number.
