#include <iostream>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../pixy/src/host/libpixyusb/include/pixy.h"

#include "pixy_util.hpp"

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

cv::Mat renderBA81(uint8_t renderFlags, uint16_t width, uint16_t height, uint32_t frameLen, uint8_t *frame) {
    uint16_t x, y;
    uint8_t r, g, b;
    cv::Mat imageRGB;

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

    imageRGB = cv::Mat(height - 2, width - 2, CV_8UC3, data);
    delete[] data;

    return imageRGB;
}

static cv::Mat processImage(cv::Mat src) {
    cv::Mat dst;
    src.copyTo(dst);

    // cv::cvtColor(dst, dst, CV_BGR2HSV);

    inRange(dst, cv::Scalar(180, 190, 90), cv::Scalar(255, 255, 200), dst);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    for(int i = 0; i >= 0; i = hierarchy[i][0]) {
        cv::Scalar color(255, 0, 0);
        drawContours(dst, contours, i, color, CV_FILLED, 8, hierarchy);
    }

    return dst;
}

cv::Mat getImage() {
    unsigned char *pixels;
    int32_t response, fourcc;
    int8_t renderflags;
    int return_value, res;
    uint16_t rwidth, rheight;
    uint32_t numPixels;
    uint16_t height,width;
    uint16_t mode;

    return_value = pixy_command("cam_getFrame",  // String id for remote procedure
                                 0x01, 0x21,     // mode
                                 0x02,   0,      // xoffset
                                 0x02,   0,      // yoffset
                                 0x02, 320,      // width
                                 0x02, 200,      // height
                                 0,              // separator
                                 &response, &fourcc, &renderflags, &rwidth, &rheight, &numPixels, &pixels, 0);

    return processImage(renderBA81(renderflags,rwidth,rheight,numPixels,pixels));
}

cv::Mat pixy_util::nextImage() {
    cv::Mat result;

    cv::Mat view0 = getImage();
    view0.copyTo(result);

    return result;
}

bool pixy_util::init() {
    int pixy_init_status = pixy_init();
    
    if (pixy_init_status != 0) {
        pixy_error(pixy_init_status);
        return false;
    }

    return true;
}

int main() {
    if (pixy_util::init()) {
        std::cout << "Connected to Pixy over USB" << std::endl;
        
        while (cvWaitKey(1) == -1) {
            imshow("Pixy Feed", pixy_util::nextImage());
        }
    }
}