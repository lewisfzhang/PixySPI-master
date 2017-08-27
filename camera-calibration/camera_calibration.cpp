#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <string>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <math.h>
extern "C" {
#include "pixy.h"
}

#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <ctime>

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

using namespace cv;
using namespace std;

static void help() {
    cout << "This is a camera calibration sample." << endl
         << "Usage: calibration configurationFile" << endl
         << "Near the sample file you'll find the configuration file, which has detailed help of "
            "how to edit it. It may be any OpenCV supported file format XML/YAML." << endl;
}

class Settings {

public:
    Settings() : goodInput(false) {}
    enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
    enum InputType {INVALID, CAMERA, VIDEO_FILE, IMAGE_LIST};

    void write(FileStorage& fs) const { // Write serialization for this class
        fs << "{" << "BoardSize_Width" << boardSize.width
                  << "BoardSize_Height" << boardSize.height
                  << "Square_Size" << squareSize
                  << "Calibrate_Pattern" << patternToUse
                  << "Calibrate_NrOfFrameToUse" << nrFrames
                  << "Calibrate_FixAspectRatio" << aspectRatio
                  << "Calibrate_AssumeZeroTangentialDistortion" << calibZeroTangentDist
                  << "Calibrate_FixPrincipalPointAtTheCenter" << calibFixPrincipalPoint

                  << "Write_DetectedFeaturePoints" << bwritePoints
                  << "Write_extrinsicParameters" << bwriteExtrinsics
                  << "Write_outputFileName" << outputFileName

                  << "Show_UndistortedImage" << showUndistorted

                  << "Input_FlipAroundHorizontalAxis" << flipVertical
                  << "Input_Delay" << delay
                  << "Input" << input
           << "}";
    }

    void read(const FileNode& node) { // Read serialization for this class
        node["BoardSize_Width" ] >> boardSize.width;
        node["BoardSize_Height"] >> boardSize.height;
        node["Calibrate_Pattern"] >> patternToUse;
        node["Square_Size"] >> squareSize;
        node["Calibrate_NrOfFrameToUse"] >> nrFrames;
        node["Calibrate_FixAspectRatio"] >> aspectRatio;
        node["Write_DetectedFeaturePoints"] >> bwritePoints;
        node["Write_extrinsicParameters"] >> bwriteExtrinsics;
        node["Write_outputFileName"] >> outputFileName;
        node["Calibrate_AssumeZeroTangentialDistortion"] >> calibZeroTangentDist;
        node["Calibrate_FixPrincipalPointAtTheCenter"] >> calibFixPrincipalPoint;
        node["Input_FlipAroundHorizontalAxis"] >> flipVertical;
        node["Show_UndistortedImage"] >> showUndistorted;
        node["Input"] >> input;
        node["Input_Delay"] >> delay;
        interpret();
    }

    void interpret() {
        goodInput = true;
        if (boardSize.width <= 0 || boardSize.height <= 0) {
            cerr << "Invalid Board size: " << boardSize.width << " " << boardSize.height << endl;
            goodInput = false;
        } else {
            cout << "Board size: " << boardSize.width << "(width) by " << boardSize.height << "(height)" << endl;
        }

        if (squareSize <= 10e-6) {
            cerr << "Invalid square size " << squareSize << endl;
            goodInput = false;
        } else{
            cout << "Square size: " << squareSize << endl;
        }

        if (nrFrames <= 0){
            cerr << "Invalid number of frames " << nrFrames << endl;
            goodInput = false;
        } else {
            cout << "Number of Frames: " << nrFrames << endl;
        }

        if (input.empty()) { // Check for valid input
            inputType = INVALID;
        } else {
            if (input[0] >= '0' && input[0] <= '9') {
                /* stringstream ss(input);
                ss >> cameraID; */
                inputType = CAMERA;
                goodInput = true;
            } else {
                if (readStringList(input, imageList)) {
                        printf("Got image list of size: %lu.\n", imageList.size());
                        inputType = IMAGE_LIST;
                        nrFrames = (nrFrames < (int)imageList.size()) ? nrFrames : (int)imageList.size();
                } else {
                  inputType = VIDEO_FILE;
                }
            }

            // if (inputType == CAMERA) {
            if (inputType == VIDEO_FILE) {
                inputCapture.open(input);
            }
            if (inputType != IMAGE_LIST && inputType != VIDEO_FILE && inputType != CAMERA) {
                inputType = INVALID;
            }
        }

        if (inputType == INVALID) {
            cerr << " Inexistent input: " << inputType;
            goodInput = false;
        } else {
            cout << "Valid input: " << inputType << endl;
        }

        flag = 0;
        if(calibFixPrincipalPoint) {
          flag |= CV_CALIB_FIX_PRINCIPAL_POINT;
        }
        if(calibZeroTangentDist) {
          flag |= CV_CALIB_ZERO_TANGENT_DIST;
        }
        if(aspectRatio) {
          flag |= CV_CALIB_FIX_ASPECT_RATIO;
        }

        calibrationPattern = NOT_EXISTING;
        if (!patternToUse.compare("CHESSBOARD")) calibrationPattern = CHESSBOARD;
        if (!patternToUse.compare("CIRCLES_GRID")) calibrationPattern = CIRCLES_GRID;
        if (!patternToUse.compare("ASYMMETRIC_CIRCLES_GRID")) calibrationPattern = ASYMMETRIC_CIRCLES_GRID;
        if (calibrationPattern == NOT_EXISTING) {
          cerr << " Inexistent camera calibration mode: " << patternToUse << endl;
          goodInput = false;
        } else {
          cout << "This is the pattern: " << patternToUse << endl;
        }
        atImageList = 0;
    }

    Mat nextImage() {
        Mat result;
        // if(inputCapture.isOpened()) {
        Mat view0 = getImage();
        view0.copyTo(result);
        /*} else if (atImageList < (int)imageList.size()) {
            printf("Reading image: %s\n", imageList[atImageList].c_str());
            result = imread(imageList[atImageList++], CV_LOAD_IMAGE_COLOR);
            imshow("test", result);
            cvWaitKey(-1);
        }*/

        return result;
    }

    static bool readStringList(const string& filename, vector<string>& l) {
        l.clear();
        FileStorage fs(filename, FileStorage::READ);

        if(!fs.isOpened()) {
          return false;
        }

        FileNode n = fs.getFirstTopLevelNode();

        if(n.type() != FileNode::SEQ) {
          return false;
        }

        FileNodeIterator it = n.begin(), it_end = n.end();

        for(; it != it_end; ++it) {
          l.push_back((string)*it);
        }

        return true;
    }

public:
    Size boardSize;             // The size of the board -> Number of items by width and height
    Pattern calibrationPattern; // One of the Chessboard, circles, or asymmetric circle pattern
    float squareSize;           // The size of a square in your defined unit (point, millimeter,etc).
    int nrFrames;               // The number of frames to use from the input for calibration
    float aspectRatio;          // The aspect ratio
    int delay;                  // In case of a video input
    bool bwritePoints;          // Write detected feature points
    bool bwriteExtrinsics;      // Write extrinsic parameters
    bool calibZeroTangentDist;  // Assume zero tangential distortion
    bool calibFixPrincipalPoint;// Fix the principal point at the center
    bool flipVertical;          // Flip the captured images around the horizontal axis
    string outputFileName;      // The name of the file where to write
    bool showUndistorted;       // Show undistorted images after calibration
    string input;               // The input ->
    int cameraID;
    vector<string> imageList;
    int atImageList;
    VideoCapture inputCapture;
    InputType inputType;
    bool goodInput;
    int flag;
private:
    string patternToUse;

//void interpolateBayer(unsigned int width, unsigned int x, unsigned int y, unsigned char *pixel, unsigned int &r, unsigned int &g, unsigned int &b);
//Mat renderBA81(uint8_t renderFlags, uint16_t width, uint16_t height, uint32_t frameLen, uint8_t *frame);

Mat getImage() {
    unsigned char *pixels;
    int32_t response, fourcc;
    int8_t renderflags;
    int return_value, res;
    uint16_t rwidth, rheight;
    uint32_t numPixels;
    uint16_t height,width;
    uint16_t mode;

    //return_value = pixy_command("stop", END_OUT_ARGS, &response, END_IN_ARGS);

    return_value = pixy_command("cam_getFrame",  // String id for remote procedure
                                 0x01, 0x21,     // mode
                                 0x02,   0,      // xoffset
                                 0x02,   0,      // yoffset
                                 0x02, 320,      // width
                                 0x02, 200,      // height
                                 0,              // separator
                                 &response, &fourcc, &renderflags, &rwidth, &rheight, &numPixels, &pixels, 0);

    // printf("width: %d, height: %d \n", width, height);

    //cout << "width: " << rwidth << ", height: " << rheight << endl;
    //return_value = pixy_command("run", END_OUT_ARGS, &response, END_IN_ARGS);

    return renderBA81(renderflags,rwidth,rheight,numPixels,pixels);
}

inline void interpolateBayer(uint16_t width, uint16_t x, uint16_t y, uint8_t *pixel, uint8_t* r, uint8_t* g, uint8_t* b) {
    if (y&1) {
        if (x&1) {
            *r = *pixel;
            *g = (*(pixel-1)+*(pixel+1)+*(pixel+width)+*(pixel-width))>>2;
            *b = (*(pixel-width-1)+*(pixel-width+1)+*(pixel+width-1)+*(pixel+width+1))>>2;
        } else {
            *r = (*(pixel-1)+*(pixel+1))>>1;
            *g = *pixel;
            *b = (*(pixel-width)+*(pixel+width))>>1;
        }
    } else {
        if (x&1) {
            *r = (*(pixel-width)+*(pixel+width))>>1;
            *g = *pixel;
            *b = (*(pixel-1)+*(pixel+1))>>1;
        } else {
            *r = (*(pixel-width-1)+*(pixel-width+1)+*(pixel+width-1)+*(pixel+width+1))>>2;
            *g = (*(pixel-1)+*(pixel+1)+*(pixel+width)+*(pixel-width))>>2;
            *b = *pixel;
        }
    }
}

Mat renderBA81(uint8_t renderFlags, uint16_t width, uint16_t height, uint32_t frameLen, uint8_t *frame) {
    uint16_t x, y;
    uint8_t r, g, b;
    Mat imageRGB;

    frame += width;
    uchar *data = new uchar[3*((height-2)*(width-2))];
    uint m = 0;

    for (y=1; y<height-1; y++) {
        frame++;
        for (x=1; x<width-1; x++, frame++) {
            interpolateBayer(width, x, y, frame, &r, &g, &b);
            data[m++] = b;
            data[m++] = g;
            data[m++] = r;
        }
        frame++;
    }

    imageRGB = Mat(height - 2,width -2, CV_8UC3, data);
    delete[] data;

    return imageRGB;
}
};

static void read(const FileNode& node, Settings& x, const Settings& default_value = Settings()) {
    if(node.empty()) {
        x = default_value;
    } else {
        x.read(node);
    }
}

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };

bool runCalibrationAndSave(Settings& s, Size imageSize, Mat& cameraMatrix, Mat& distCoeffs,
                           vector<vector<Point2f> > imagePoints, string pixyNumber);

int main(int argc, char* argv[]) {
    help();
    Settings s;
    const string inputSettingsFile = ((argc > 1) ? argv[1] : "in_VID5.xml");
    // if using webcam, use commented out code
    /*int cameraIndex = 0;
    VideoCapture cap(cameraIndex); // open the default camera
    if(!cap.isOpened()) {  // check if we succeeded
        return -1;
    }
    */

    string pixyNumber = "3";
    cout << "Please enter the Pixy Number\n>";
    getline(cin, pixyNumber);
    cout << "Calibration Pixy Number " << pixyNumber << endl;

    FileStorage fs(inputSettingsFile, FileStorage::READ); // Read the settings

    if (!fs.isOpened()) {
        cout << "Could not open the configuration file: \"" << inputSettingsFile << "\"" << endl;
        return -1;
    }

    fs["Settings"] >> s;
    fs.release();                                         // close Settings file
    int pixy_init_status;
    pixy_init_status = pixy_init();
    cout << "initialized Pixy - " << pixy_init_status << endl;

    if(pixy_init_status != 0) {
      // Error initializing Pixy
      pixy_error(pixy_init_status);
      return -1;
    }

    /*if (!s.goodInput)
    {
        cout << "Invalid input detected. Application stopping. " << endl;
        return -1;
    } else {*/
        cout << "Valid Input Detected." << endl;
    // }

    vector<vector<Point2f> > imagePoints;
    Mat cameraMatrix, distCoeffs;
    Size imageSize;
    int mode = s.inputType == Settings::IMAGE_LIST ? CAPTURING : DETECTION;
    clock_t prevTimestamp = 0;
    const Scalar RED(0,0,255), GREEN(0,255,0);
    const char ESC_KEY = 27;
    cout << "Starting calibration now..." << endl;

    for(int i = 0; cvWaitKey(1) != 13; ++i) {
      Mat view;
      bool blinkOutput = false;
      char getkey = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);
      // char getkey = (char)waitKey(50);
      view = s.nextImage();
      imshow("Image View", view);
      bool isGetKeyPressed = false;

      while (!isGetKeyPressed && mode != CALIBRATED) {
        view = s.nextImage();
        imshow("Image View", view);
        vector<Point2f> pointBuf;
        bool found;

        switch(s.calibrationPattern) { // Find feature points on the input format
          case Settings::CHESSBOARD:
              found = findChessboardCorners(view, s.boardSize, pointBuf,
                  CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
              break;
          case Settings::CIRCLES_GRID:
              found = findCirclesGrid(view, s.boardSize, pointBuf);
              break;
          case Settings::ASYMMETRIC_CIRCLES_GRID:
              found = findCirclesGrid(view, s.boardSize, pointBuf, CALIB_CB_ASYMMETRIC_GRID);
              break;
          default:
              found = false;
              break;
        }

        printf("found?: %d", found);

        if (found) { // If done with success, improve the found corners' coordinate accuracy for chessboard
          if(s.calibrationPattern == Settings::CHESSBOARD) {
              Mat viewGray;
              cvtColor(view, viewGray, COLOR_BGR2GRAY);
              cornerSubPix(viewGray, pointBuf, Size(11,11),
                  Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
          }
          if(mode == CAPTURING &&  // For camera only take new samples after delay time
              (!s.inputCapture.isOpened() || clock() - prevTimestamp > s.delay*1e-3*CLOCKS_PER_SEC)) {
              imagePoints.push_back(pointBuf);
              prevTimestamp = clock();
              blinkOutput = s.inputCapture.isOpened();
          }
          // Draw the corners
          drawChessboardCorners(view, s.boardSize, Mat(pointBuf), found);
        }
        imshow("Image View", view);
        if (getkey == ' ') {
          isGetKeyPressed = true;
        }
        getkey = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);
        // getkey = (char)waitKey(50);
      }
      isGetKeyPressed = false;
      cout << "Took Pic" << endl;
      printf("Starting image\n");
      // If no more images, or got enough, then stop calibration and show result

      //if(mode == CAPTURING && imagePoints.size() >= (unsigned)s.nrFrames)
      if (mode == CAPTURING && i >= (unsigned)s.nrFrames) {
          printf("Calib1\n");
          if(runCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints, pixyNumber)) {
            mode = CALIBRATED;
          } else {
            mode = DETECTION;
          }
      }

      if(view.empty()) { // If no more images then run calibration, save and stop loop.
        printf("Calib2\n");
        if(imagePoints.size() > 0) {
            runCalibrationAndSave(s, imageSize, cameraMatrix, distCoeffs, imagePoints, pixyNumber);
        }
        break;
      }

      imageSize = view.size();  // Format input image.

      if(s.flipVertical) {
        flip(view, view, 0);
      }

        //vector<Point2f> pointBuf;

        printf("Doing image: %d.\n", i);

        /*bool found;

        switch(s.calibrationPattern) { // Find feature points on the input format
        case Settings::CHESSBOARD:
            found = findChessboardCorners(view, s.boardSize, pointBuf,
                CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
            break;
        case Settings::CIRCLES_GRID:
            found = findCirclesGrid(view, s.boardSize, pointBuf);
            break;
        case Settings::ASYMMETRIC_CIRCLES_GRID:
            found = findCirclesGrid(view, s.boardSize, pointBuf, CALIB_CB_ASYMMETRIC_GRID);
            break;
        default:
            found = false;
            break;
        }

        printf("found?: %d", found);
        if (found) { // If done with success,
              // improve the found corners' coordinate accuracy for chessboard
              if(s.calibrationPattern == Settings::CHESSBOARD) {
                  Mat viewGray;
                  cvtColor(view, viewGray, COLOR_BGR2GRAY);
                  cornerSubPix(viewGray, pointBuf, Size(11,11),
                      Size(-1,-1), TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));
              }

                if(mode == CAPTURING &&  // For camera only take new samples after delay time
                    (!s.inputCapture.isOpened() || clock() - prevTimestamp > s.delay*1e-3*CLOCKS_PER_SEC)) {
                    imagePoints.push_back(pointBuf);
                    prevTimestamp = clock();
                    blinkOutput = s.inputCapture.isOpened();
                }

                // Draw the corners
                drawChessboardCorners(view, s.boardSize, Mat(pointBuf), found);
        }*/

        //----------------------------- Output Text ------------------------------------------------

        string msg = (mode == CAPTURING) ? "100/100" :
                      (mode == CALIBRATED && s.showUndistorted) ? "Undistorted" :
                      (mode == CALIBRATED) ? "Distorted" : "Press 'g' to start";
        int baseLine = 0;
        Size textSize = getTextSize(msg, 1, 1, 1, &baseLine);
        Point textOrigin(view.cols - 2*textSize.width - 10, view.rows - 2*baseLine - 10);

        if(mode == CAPTURING) {
            if(s.showUndistorted) {
              msg = format("%d/%d Undist", i, s.nrFrames);
              // msg = format("%d/%d Undist", (int)imagePoints.size(), s.nrFrames);
            } else {
              msg = format("%d/%d Undist", i, s.nrFrames);
              // msg = format("%d/%d Dist", (int)imagePoints.size(), s.nrFrames);
            }
        }

        putText(view, msg, textOrigin, 1, 1, mode == CALIBRATED ? GREEN : RED);
        if(blinkOutput) {
          bitwise_not(view, view);
        }

        //------------------------- Undistorted video capture output ------------------------------

        if(mode == CALIBRATED && s.showUndistorted) {
            Mat temp = view.clone();
            undistort(temp, view, cameraMatrix, distCoeffs);
        }

        //------------------------------ Show image and check for input commands -------------------

        imshow("Image View", view);
        char key = (char)waitKey(s.inputCapture.isOpened() ? 50 : s.delay);

        if(key == ESC_KEY) {
          break;
        }

        if(key == 'u' && mode == CALIBRATED) {
          s.showUndistorted = !s.showUndistorted;
          if (s.showUndistorted == 1) cout << "Showing: Undistorted" << endl;
          else cout << "Showing: Distorted" << endl;
        }
        if(key == 'g') {
          mode = CAPTURING;
          imagePoints.clear();
        }
    }

    // -----------------------Show the undistorted image for the image list ------------------------

    if(s.inputType == Settings::IMAGE_LIST && s.showUndistorted) {
        Mat view, rview, map1, map2;
        initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
            getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
            imageSize, CV_16SC2, map1, map2);

        for(int i = 0; i < (int)s.imageList.size(); i++) {
            view = imread(s.imageList[i], 1);
            if(view.empty()) {
              continue;
            }

            remap(view, rview, map1, map2, INTER_LINEAR);
            imshow("Image View", rview);
            char c = (char)waitKey();

            if(c == ESC_KEY || c == 'q' || c == 'Q') {
              break;
            }
        }
    }

    return 0;
}

static double computeReprojectionErrors(const vector<vector<Point3f> >& objectPoints,
                                         const vector<vector<Point2f> >& imagePoints,
                                         const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                                         const Mat& cameraMatrix , const Mat& distCoeffs,
                                         vector<float>& perViewErrors) {
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for(i = 0; i < (int)objectPoints.size(); ++i) {
        projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
                       distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float) std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }

    return std::sqrt(totalErr/totalPoints);
}

static void calcBoardCornerPositions(Size boardSize, float squareSize, vector<Point3f>& corners,
                                     Settings::Pattern patternType /*= Settings::CHESSBOARD*/) {
    corners.clear();
    switch(patternType) {
      case Settings::CHESSBOARD:
      case Settings::CIRCLES_GRID:
        for(int i = 0; i < boardSize.height; ++i)
          for(int j = 0; j < boardSize.width; ++j)
            corners.push_back(Point3f(float(j*squareSize), float(i*squareSize), 0));
        break;
      case Settings::ASYMMETRIC_CIRCLES_GRID:
        for(int i = 0; i < boardSize.height; i++)
          for(int j = 0; j < boardSize.width; j++)
            corners.push_back(Point3f(float((2*j + i % 2)*squareSize), float(i*squareSize), 0));
        break;
      default:
        break;
    }
}

static bool runCalibration(Settings& s, Size& imageSize, Mat& cameraMatrix, Mat& distCoeffs,
                           vector<vector<Point2f> > imagePoints, vector<Mat>& rvecs, vector<Mat>& tvecs,
                           vector<float>& reprojErrs, double& totalAvgErr) {
    cameraMatrix = Mat::eye(3, 3, CV_64F);

    if(s.flag & CV_CALIB_FIX_ASPECT_RATIO) {
        cameraMatrix.at<double>(0,0) = 1.0;
    }

    distCoeffs = Mat::zeros(8, 1, CV_64F);
    vector<vector<Point3f> > objectPoints(1);
    calcBoardCornerPositions(s.boardSize, s.squareSize, objectPoints[0], s.calibrationPattern);
    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    //Find intrinsic and extrinsic camera parameters
    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                                 distCoeffs, rvecs, tvecs, s.flag|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
    cout << "Re-projection error reported by calibrateCamera: "<< rms << endl;
    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                                             rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}

// Print camera parameters to the output file
static void saveCameraParams(Settings& s, Size& imageSize, Mat& cameraMatrix, Mat& distCoeffs,
                            const vector<Mat>& rvecs, const vector<Mat>& tvecs,
                            const vector<float>& reprojErrs, const vector<vector<Point2f> >& imagePoints,
                            double totalAvgErr, string pixyNumber) {
    FileStorage fs(s.outputFileName, FileStorage::WRITE);
    time_t tm;
    time(&tm);
    struct tm *t2 = localtime(&tm);
    char buf[1024];
    strftime(buf, sizeof(buf)-1, "%c", t2);

    fs << "calibration_Time" << buf;

    if(!rvecs.empty() || !reprojErrs.empty()) {
      fs << "nrOfFrames" << (int)std::max(rvecs.size(), reprojErrs.size());
    }

    fs << "image_Width" << imageSize.width;
    fs << "image_Height" << imageSize.height;
    fs << "board_Width" << s.boardSize.width;
    fs << "board_Height" << s.boardSize.height;
    fs << "square_Size" << s.squareSize;

    if(s.flag & CV_CALIB_FIX_ASPECT_RATIO) {
      fs << "FixAspectRatio" << s.aspectRatio;
    }

    if(s.flag) {
        sprintf(buf, "flags: %s%s%s%s",
            s.flag & CV_CALIB_USE_INTRINSIC_GUESS ? " +use_intrinsic_guess" : "",
            s.flag & CV_CALIB_FIX_ASPECT_RATIO ? " +fix_aspectRatio" : "",
            s.flag & CV_CALIB_FIX_PRINCIPAL_POINT ? " +fix_principal_point" : "",
            s.flag & CV_CALIB_ZERO_TANGENT_DIST ? " +zero_tangent_dist" : "");
        cvWriteComment(*fs, buf, 0);
    }

    fs << "flagValue" << s.flag;
    fs << "Camera_Matrix" << cameraMatrix;
    fs << "Distortion_Coefficients" << distCoeffs;
    
    string path = "../src/com/team254/frc2017/pixy/PixyConstants.java";
    ofstream myfile(path);
    myfile << "package com.team254.frc2017.pixy;\n\n";
    myfile << "public class PixyConstants {\n";
    
    myfile << "        public static double[][] cameraMatrix = {{"
        << cameraMatrix.at<double> (0, 0) << ", " << cameraMatrix.at<double> (0, 1) << ", " << cameraMatrix.at<double> (0, 2) << "}, {"
        << cameraMatrix.at<double> (1, 0) << ", " << cameraMatrix.at<double> (1, 1) << ", " << cameraMatrix.at<double> (1, 2) << "}, {"
        << cameraMatrix.at<double> (2, 0) << ", " << cameraMatrix.at<double> (2, 1) << ", " << cameraMatrix.at<double> (2, 2) << "}};\n";
    
    myfile << "        public static double[][] distCoeffs = {{"
        << distCoeffs.at<double> (0, 0) << "}, {"
        << distCoeffs.at<double> (1, 0) << "}, {"
        << distCoeffs.at<double> (2, 0) << "}, {"
        << distCoeffs.at<double> (3, 0) << "}, {"
        << distCoeffs.at<double> (4, 0) << "}};\n";
    
    myfile << "}";
    
    
    
//    myfile << "public class PixyConstants{\n";
//    myfile << "    public PixyNumber" << pixyNumber << "Constants() {\n";
//    myfile << "        fx = " << cameraMatrix.at<double> (0, 0) << ";\n";
//    myfile << "        fy = " << cameraMatrix.at<double> (1, 1) << ";\n";
//    myfile << "        cx = " << cameraMatrix.at<double> (0, 2) << ";\n";
//    myfile << "        cy = " << cameraMatrix.at<double> (1, 2) << ";\n";
//    myfile << "        k1 = " << distCoeffs.at<double> (0, 0) << ";\n";
//    myfile << "        k2 = " << distCoeffs.at<double> (1, 0) << ";\n";
//    myfile << "        k3 = " << distCoeffs.at<double> (4, 0) << ";\n";
//    myfile << "        p1 = " << distCoeffs.at<double> (2, 0) << ";\n";
//    myfile << "        p2 = " << distCoeffs.at<double> (3, 0) << ";\n";
//    myfile << "    }\n";
//    myfile << "}";
    myfile.close();

    fs << "Avg_Reprojection_Error" << totalAvgErr;
    if(!reprojErrs.empty()) {
      fs << "Per_View_Reprojection_Errors" << Mat(reprojErrs);
    }

    if(!rvecs.empty() && !tvecs.empty()) {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());

        for(int i = 0; i < (int)rvecs.size(); i++) {
            Mat r = bigmat(Range(i, i+1), Range(0,3));
            Mat t = bigmat(Range(i, i+1), Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);

            //*.t() is MatExpr (not Mat) so we can use assignment operator

            r = rvecs[i].t();
            t = tvecs[i].t();
        }

        cvWriteComment(*fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0);
        fs << "Extrinsic_Parameters" << bigmat;
    }

    if(!imagePoints.empty()) {
        Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
        for(int i = 0; i < (int)imagePoints.size(); i++) {
            Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
            Mat imgpti(imagePoints[i]);
            imgpti.copyTo(r);
        }

        fs << "Image_points" << imagePtMat;
    }
}

bool runCalibrationAndSave(Settings& s, Size imageSize, Mat& cameraMatrix, Mat& distCoeffs,vector<vector<Point2f> > imagePoints, string pixyNumber) {
    vector<Mat> rvecs, tvecs;
    vector<float> reprojErrs;
    double totalAvgErr = 0;

    bool ok = runCalibration(s,imageSize, cameraMatrix, distCoeffs, imagePoints, rvecs, tvecs,
                             reprojErrs, totalAvgErr);

    cout << (ok ? "Calibration succeeded" : "Calibration failed")
         << ". avg re projection error = " << totalAvgErr ;

    if(ok) {
        saveCameraParams(s, imageSize, cameraMatrix, distCoeffs, rvecs ,tvecs, reprojErrs,
                         imagePoints, totalAvgErr, pixyNumber);
    }

    return ok;
}
