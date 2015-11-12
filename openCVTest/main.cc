//
//  main.cpp
//  openCVTest
//
//  Created by YotaOdaka on 11/11/15.
//  Copyright © 2015 YotaOdaka. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

void mouseCallback(int event, int x, int y, int flags, void* param);
float getDistance(cv::Point2f point1, cv::Point2f point2);

class HomographyView final {
 public:
  // Constructor and Destructor
  HomographyView(std::string window_name,
    std::string file_name,
    int output_width,
    int output_height)
    : window_name_(window_name),
      file_name_(file_name),
      point_dragged_(0),
      show_corner_mark_(false) {
    input_image_ = cv::imread("resources/" + file_name_);
    output_size_ = cv::Size(output_width, output_height);
    
    source_points_.emplace_back(cv::Point2f(0, 0));
    source_points_.emplace_back(cv::Point2f(input_image_.cols, 0));
    source_points_.emplace_back(cv::Point2f(input_image_.cols, input_image_.rows));
    source_points_.emplace_back(cv::Point2f(0, input_image_.rows));

    destination_points_.emplace_back(cv::Point2f(277, 89));
    destination_points_.emplace_back(cv::Point2f(551, 217));
    destination_points_.emplace_back(cv::Point2f(319, 399));
    destination_points_.emplace_back(cv::Point2f(39, 270));

    cv::namedWindow(window_name_);
    cv::setWindowProperty(window_name_, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    cv::setMouseCallback(window_name_, &mouseCallback, this);

    updateWindow();
    drawWindow();
  }
  ~HomographyView() { cv::destroyWindow(window_name_); }

  // Disallow copy and assign
  HomographyView(const HomographyView&) = delete;
  HomographyView& operator=(const HomographyView&) = delete;

  // Accessors
  void set_file_name(std::string file_name) { file_name_ = file_name; }
  std::string get_file_name() const { return file_name_; };
  
  // Mouse events implementation
  void mouseLPressed(int x, int y) {
    for (auto iter = destination_points_.begin();
      iter != destination_points_.end(); iter++) {
      if (getDistance(cv::Point2f(x, y), *iter) < 10) {
        point_dragged_ = static_cast<int>(
          std::distance(destination_points_.begin(), iter) + 1);
      }
    }
  }
  void mouseLReleased(int x, int y) {
    point_dragged_ = 0;
  }
  void mouseMoved(int x, int y) {
    if (point_dragged_) {
      destination_points_[point_dragged_ - 1] = cv::Point2f(x, y);
    }
    updateWindow();
    drawWindow();
  }

 private:
  // Window management
  void updateWindow() {
    homography_matrix_
      = cv::findHomography(source_points_, destination_points_);
    cv::warpPerspective(input_image_,
      output_image_,
      homography_matrix_,
      output_size_);
    if (show_corner_mark_) {
      for (auto iter = destination_points_.begin();
        iter != destination_points_.end(); iter++) {
        cv::circle(output_image_, *iter, 10, cv::Scalar(100, 100, 0), 2);
      }
    }
  }
  void drawWindow() {
    cv::imshow(window_name_, output_image_);
    show_corner_mark_
      = (cv::waitKey() == 'm')? !show_corner_mark_: show_corner_mark_;
  }

  // Properties
  std::string window_name_;
  std::string file_name_;
  int point_dragged_;
  bool show_corner_mark_;

  cv::Mat input_image_;
  cv::Mat output_image_;
  cv::Mat homography_matrix_;
  cv::Size output_size_;

  std::vector<cv::Point2f> source_points_;
  std::vector<cv::Point2f> destination_points_;
};

// Mouse callback function
void mouseCallback(int event, int x, int y, int flags, void* param) {
  HomographyView* homograph = (HomographyView*)param;
  switch (event) {
    case cv::EVENT_LBUTTONDOWN:
      homograph->mouseLPressed(x, y);
      break;

    case cv::EVENT_LBUTTONUP:
      homograph->mouseLReleased(x, y);
      break;

    case cv::EVENT_MOUSEMOVE:
      homograph->mouseMoved(x, y);
      break;

    default:
      break;
  }
}

// Calculation
float getDistance(cv::Point2f point1, cv::Point2f point2) {
  cv::Point2f difference(point1 - point2);
  return std::sqrt(difference.x*difference.x + difference.y*difference.y);
}

// Main function
int main(int argc, const char * argv[]) {
  HomographyView homography_view("homography", "Lenna.jpg", 1920, 1080);
  
  cv::waitKey();
  return 0;
}
