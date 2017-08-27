#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <math.h>  
#include "pixy.h" 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>

using namespace cv;
using namespace std;

void interpolateBayer(unsigned int width, unsigned int x, unsigned int y, unsigned char *pixel, unsigned int &r, unsigned int &g, unsigned int &b);
Mat renderBA81(uint8_t renderFlags, uint16_t width, uint16_t height, uint32_t frameLen, uint8_t *frame);
Mat getImage();

int main(int argc, char * argv[])
{
    int pixy_init_status;
    int return_value;
    int32_t response;

    pixy_init_status = pixy_init();

    if(!(pixy_init_status == 0))
    {
        printf("pixy_init(): ");
        pixy_error(pixy_init_status);
        return pixy_init_status;
    }

    return_value = pixy_command("stop", END_OUT_ARGS, &response, END_IN_ARGS); 
    return_value = pixy_rcs_set_position(1, 900);
    return_value = pixy_rcs_set_position(0, 500);

    return_value = pixy_command("run", END_OUT_ARGS, &response, END_IN_ARGS);   


    while(true) {
      Mat image = getImage();
      imshow("test", image );
      cvWaitKey(1);
    }
}

Mat getImage()
{
    unsigned char *pixels;
    int32_t response, fourcc;
    int8_t renderflags;
    int return_value, res;
    uint16_t rwidth, rheight;
    uint32_t  numPixels;
    uint16_t height,width;
    uint16_t mode;

    //return_value = pixy_command("stop", END_OUT_ARGS, &response, END_IN_ARGS);

    return_value = pixy_command("cam_getFrame",  // String id for remote procedure
                                 0x01, 0x21,      // mode
                                 0x02,   0,        // xoffset
                                 0x02,   0,         // yoffset
                                 0x02, 320,       // width
                                 0x02, 200,       // height
                                 0,            // separator
                                 &response, &fourcc, &renderflags, &rwidth, &rheight, &numPixels, &pixels, 0);
    // printf("width: %d, height: %d \n", width, height);
    cout << "width: " << width << ", height: " << height << endl;
    //return_value = pixy_command("run", END_OUT_ARGS, &response, END_IN_ARGS);   

    return renderBA81(renderflags,rwidth,rheight,numPixels,pixels);
}

inline void interpolateBayer(uint16_t width, uint16_t x, uint16_t y, uint8_t *pixel, uint8_t* r, uint8_t* g, uint8_t* b)
{
    if (y&1)
    {
        if (x&1)
        {
            *r = *pixel;
            *g = (*(pixel-1)+*(pixel+1)+*(pixel+width)+*(pixel-width))>>2;
            *b = (*(pixel-width-1)+*(pixel-width+1)+*(pixel+width-1)+*(pixel+width+1))>>2;
        }
        else
        {
            *r = (*(pixel-1)+*(pixel+1))>>1;
            *g = *pixel;
            *b = (*(pixel-width)+*(pixel+width))>>1;
        }
    }
    else
    {
        if (x&1)
        {
            *r = (*(pixel-width)+*(pixel+width))>>1;
            *g = *pixel;
            *b = (*(pixel-1)+*(pixel+1))>>1;
        }
        else
        {
            *r = (*(pixel-width-1)+*(pixel-width+1)+*(pixel+width-1)+*(pixel+width+1))>>2;
            *g = (*(pixel-1)+*(pixel+1)+*(pixel+width)+*(pixel-width))>>2;
            *b = *pixel;
        }
    }

}

Mat renderBA81(uint8_t renderFlags, uint16_t width, uint16_t height, uint32_t frameLen, uint8_t *frame)
{
    uint16_t x, y;
    uint8_t r, g, b;
    Mat imageRGB;

    frame += width;
    uchar data[3*((height-2)*(width-2))];

    uint m = 0;
    for (y=1; y<height-1; y++)
    {
        frame++;
        for (x=1; x<width-1; x++, frame++)
        {
            interpolateBayer(width, x, y, frame, &r, &g, &b);
            data[m++] = b;
            data[m++] = g;
            data[m++] = r;
        }
        frame++;
    }

    imageRGB =  Mat(height - 2,width -2, CV_8UC3, data);

    return imageRGB;
}