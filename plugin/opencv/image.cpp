#include "opencv.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <image/format.h>

using namespace cv;
using namespace math;




// wasn't that complicated to provide these in the first place :-/
template<class Element>
const Element& get(const cv::Mat& m, natural i, natural j) {
  return m.ptr<Element>(j)[i];
}

template<class Element>
Element& get(cv::Mat& m, natural i, natural j) {
  return m.ptr<Element>(j)[i];
}



void opencv::load(image::rgb& img, const std::string& name) {
  Mat a = imread( name, CV_LOAD_IMAGE_COLOR );
  
  img.resize(a.cols, a.rows);
  
  img.each([&](natural i, natural j) {
      img(i, j) = get<image::format::bgr>(a, i, j).gl();
    });						
  
}

void opencv::load(image::gray& img, const std::string& name) {
  Mat a = imread( name, CV_LOAD_IMAGE_GRAYSCALE );
  
  img.resize(a.cols, a.rows);
  
  img.each([&](natural i, natural j) {
      img(i, j) = get<unsigned char>(a, i, j) / 256.0;
    });	
}

// TODO add params ?
void opencv::save(const std::string& name, const image::rgb& img) {
  Mat a(img.cols(), img.rows(), CV_8UC3);

  img.each([&](natural i, natural j) {
      get<image::format::bgr>(a, i, j) = image::format::bgr( img(i, j) );
    });
  
  imwrite(name, a);
}

void opencv::save(const std::string& name, const image::gray& img) {
  
  Mat a(img.cols(), img.rows(), CV_8U);

  img.each([&](natural i, natural j) {
      get<unsigned char>(a, i, j) = img(i, j) * 256;
    });
  
  imwrite(name, a);

}
  
