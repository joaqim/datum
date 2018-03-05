#include "AppForm.h"

#include <opencv2/opencv.hpp>

using namespace licPlaterec;

int main(int argc, char **argv) {

  AppForm app;
    app.run();
    cv::waitKey(0);
  
    exit(EXIT_SUCCESS); 
};
