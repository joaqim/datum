#include "Application.h"  

#include <iostream>  
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <opencv2/opencv.hpp>
#include <tinydir.h>

using std::cout;
using std::endl;
using std::cerr;


int window_width  = 640;
int window_height = 480;

//#include <poppler/PDFDoc.h> 
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>

//using poppler::document;
//using poppler::page;
//using poppler::page_renderer;
//using poppler::image;

namespace poppler {

cv::Mat readPDFtoCV(const std::string& filename,int DPI) {
  document* mypdf = document::load_from_file(filename);
  /*
    if(mypdf == NULL) {
        cerr << "couldn't read pdf\n";
        return cv::Mat();
    }
    cout << "pdf has " << mypdf->pages() << " pages\n";
    page* mypage = mypdf->create_page(0);
 
    page_renderer renderer;
    renderer.set_render_hint(page_renderer::text_antialiasing);
    image myimage = renderer.render_page(mypage,DPI,DPI);
    cout << "created image of  " << myimage.width() << "x"<< myimage.height() << "\n";
 
    cv::Mat cvimg;
    if(myimage.format() == image::format_rgb24) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
    } else if(myimage.format() == image::format_argb32) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
    } else {
        cerr << "PDF format no good\n";
        return cv::Mat();
    }
    return cvimg;
  */
  return cv::Mat();
}
} // namespace poppler

static std::vector<cv::Rect> detectLetters(cv::Mat img)
{
    std::vector<cv::Rect> boundRect;
    cv::Mat img_gray, img_sobel, img_threshold, element;
    cvtColor(img, img_gray, CV_BGR2GRAY);
    cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
    cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3) );
    cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
    std::vector< std::vector< cv::Point> > contours;
    cv::findContours(img_threshold, contours, 0, 1); 
    std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
        if (contours[i].size()>100)
        { 
            cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true );
            cv::Rect appRect( boundingRect( cv::Mat(contours_poly[i]) ));
            if (appRect.width>appRect.height) 
                boundRect.push_back(appRect);
        }
    return boundRect;
}


// Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
static GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
            magFilter == GL_LINEAR_MIPMAP_NEAREST ||
            magFilter == GL_NEAREST_MIPMAP_LINEAR ||
            magFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << endl;
        magFilter = GL_LINEAR;
    }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1)
    {
        inputColourFormat = GL_LUMINANCE;
    }

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,            // Internal colour format to convert to
                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
            minFilter == GL_LINEAR_MIPMAP_NEAREST ||
            minFilter == GL_NEAREST_MIPMAP_LINEAR ||
            minFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return textureID;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void resize_callback(GLFWwindow* window, int new_width, int new_height) {
    glViewport(0, 0, window_width = new_width, window_height = new_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_width, window_height, 0.0, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

static void draw_frame(const cv::Mat& frame) {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

    glEnable(GL_TEXTURE_2D);
    GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

    /* Draw a quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2i(0,   0);
    glTexCoord2i(0, 1); glVertex2i(0,   window_height);
    glTexCoord2i(1, 1); glVertex2i(window_width, window_height);
    glTexCoord2i(1, 0); glVertex2i(window_width, 0);
    glEnd();

    glDeleteTextures(1, &image_tex);
    glDisable(GL_TEXTURE_2D);
}

static void init_opengl(int w, int h) {
    glViewport(0, 0, w, h); // use a screen size of WIDTH x HEIGHT

    glMatrixMode(GL_PROJECTION);     // Make a simple 2D projection on the entire window
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 100.0);

    glMatrixMode(GL_MODELVIEW);    // Set the matrix mode to object modeling

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the window
}

int main(int argc, char **argv)
{
    /*
    if (argc != 2) {
        cout << "Usage: " << argv[0] << "<path_to_image_file>" << endl;
        exit(EXIT_FAILURE);
    }

    cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        cout << "Cannot load image: " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }
    */

  int DPI(200);
    
  if( argc > 1) {
    DPI = strtol(argv[1], NULL, 10);
  }

  poppler::document *doc = poppler::document::load_from_file("../data/ReadMe.pdf");
  // poppler::document *doc = poppler::document::load_from_file("../data/BN2.pdf");
    if(doc == NULL) {
        cerr << "couldn't read pdf\n";
    }
    cout << "pdf has " << doc->pages() << " pages\n";
    poppler::page* mypage = doc->create_page(0);
 
    poppler::page_renderer renderer;
    renderer.set_render_hint(poppler::page_renderer::text_antialiasing);
    poppler::image myimage = renderer.render_page(mypage,DPI,DPI);
    cout << "created image of  " << myimage.width() << "x"<< myimage.height() << "\n";
 
    cv::Mat cvimg;
    if(myimage.format() == poppler::image::format_rgb24) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
    } else if(myimage.format() == poppler::image::format_argb32) {
      cv::Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
    } else {
        cerr << "PDF format no good\n";
    }

    //Read
    //cv::Mat img=cv::imread("../data/side_2.jpg");
    //cv::Mat img = poppler::readPDFtoCV("../data/ReadMe.pdf", 200);
    cv::Mat img(cvimg);

    //Detect
    std::vector<cv::Rect> letterBBoxes=detectLetters(img);
    //Display
    for(int i=0; i< letterBBoxes.size(); i++)
      cv::rectangle(img,letterBBoxes[i],cv::Scalar(0,255,0),3,8,0);
    cv::imwrite( "imgOut2.jpg", img);  

    
    window_width = img.cols;
    window_height = img.rows;

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
      exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(window_width, window_height, "datum", NULL, NULL);
    if (!window) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, resize_callback);

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    //  Initialise glew (must occur AFTER window creation or glew will error)
    GLenum err = glewInit();
    if (GLEW_OK != err)
      {
        cout << "GLEW initialisation error: " << glewGetErrorString(err) << endl;
        exit(-1);
      }
    cout << "GLEW okay - using version: " << glewGetString(GLEW_VERSION) << endl;

    init_opengl(window_width, window_height);

    while (!glfwWindowShouldClose(window)) {
      draw_frame(img);

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    exit(EXIT_SUCCESS);
};
