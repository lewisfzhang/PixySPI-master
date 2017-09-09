#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "../pixy/src/host/libpixyusb/include/pixy.h"

void interpolateBayer(uint16_t width, uint16_t x, uint16_t y, uint8_t *pixel, uint8_t* r, uint8_t* g, uint8_t* b) {
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

    imageRGB = cv::Mat(height - 2,width -2, CV_8UC3, data);
    delete[] data;

    return imageRGB;
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

    cv::Mat img = renderBA81(renderflags,rwidth,rheight,numPixels,pixels);

    cv::cvtColor(img, img, CV_BGR2HSV);

    inRange(img, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255), img);

    return img;
}

int main() {
    int pixy_init_status = pixy_init();

    if (pixy_init_status != 0) {
        pixy_error(pixy_init_status);
        return -1;
    }

    imshow("Image View", getImage());
}
