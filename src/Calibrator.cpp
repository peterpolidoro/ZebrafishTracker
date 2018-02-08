// ----------------------------------------------------------------------------
// Calibrator.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Calibrator.h"


// public
Calibrator::Calibrator()
{
  // homography_image_to_stage_set_ = false;
}

bool Calibrator::getHomographyImageToStage(cv::Mat & homography_image_to_stage)
{
  const bool got_calibration = true;
  boost::filesystem::path calibration_path("../ZebrafishTrackerCalibration/calibration/calibration.yml");

  try
  {
    if (boost::filesystem::exists(calibration_path))
    {
      std::cout << std::endl << "zebrafish_tracker_calibration_path = " << calibration_path << std::endl;
    }
    else
    {
      std::cerr << std::endl << "zebrafish_tracker_calibration_path: " << calibration_path << " does not exist!" << std::endl;
      return !got_calibration;
    }
  }
  catch (const boost::filesystem::filesystem_error& ex)
  {
    std::cout << std::endl << ex.what() << std::endl;
    return !got_calibration;
  }

  cv::FileStorage calibration_fs(calibration_path.string(), cv::FileStorage::READ);
  calibration_fs["homography_image_to_stage"] >> homography_image_to_stage;
  calibration_fs.release();
  return got_calibration;
}

// bool Calibrator::convertImagePointToStagePoint(cv::Point & image_point, cv::Point & stage_point)
// {
//   if (homography_image_to_stage_set_)
//   {
//     std::vector<cv::Point2f> image_points;
//     image_points.push_back(image_point);
//     std::vector<cv::Point2f> stage_points;
//     cv::perspectiveTransform(image_points,stage_points,homography_image_to_stage_);
//     stage_point = stage_points[0];
//   }
//   return homography_image_to_stage_set_;
// }

// private

// ----------------------------------------------------------------------------
// ZebrafishTrackerCalibration
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
// #include <iostream>
// #include <sstream>
// #include <signal.h>

// #include <opencv2/core/core.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/video/background_segm.hpp>
// #include <opencv2/features2d.hpp>
// #include <opencv2/calib3d/calib3d.hpp>


// int main(int argc, char * argv[])
// {
//   //////
//   cv::Mat chessboard = cv::imread("images/chessboard.png",CV_LOAD_IMAGE_GRAYSCALE);
//   if(!chessboard.data)
//   {
//     std::cout <<  "Could not open or find the chessboard image!" << std::endl;
//     return -1;
//   }

//   size_t pattern_count_col = 6;
//   size_t pattern_count_row = 8;
//   cv::Size patternsize(pattern_count_col,pattern_count_row);
//   std::vector<cv::Point2f> corners;

//   bool patternfound = cv::findChessboardCorners(chessboard,
//                                                 patternsize,
//                                                 corners,
//                                                 cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

//   if(!patternfound)
//   {
//     std::cout <<  "Could not find the chessboard corners!" << std::endl;
//     return -1;
//   }
//   if(patternfound)
//   {
//     cv::cornerSubPix(chessboard,
//                      corners,
//                      cv::Size(11, 11),
//                      cv::Size(-1, -1),
//                      cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
//   }

//   std::vector<cv::Point2f> image_points;
//   for (size_t row=0; row<pattern_count_row; ++row)
//   {
//     for (size_t col=0; col<pattern_count_col; ++col)
//     {
//       if (((col % (pattern_count_col - 1)) == 0) && ((row % 2) == 0))
//       {
//         image_points.push_back(corners[row*pattern_count_col + col]);
//       }
//     }
//   }

//   float font_scale = 0.7;
//   int font_thickness = 2;

//   cv::Mat chessboard_image_points;
//   cv::cvtColor(chessboard,chessboard_image_points,CV_GRAY2BGR);
//   cv::drawChessboardCorners(chessboard_image_points,patternsize,cv::Mat(corners),patternfound);
//   for (std::vector<cv::Point2f>::iterator it = image_points.begin(); it != image_points.end(); ++it)
//   {
//     cv::Point2i image_pt = *it;
//     std::stringstream image_pt_ss;
//     image_pt_ss << "i" << image_pt;
//     cv::Point2i text_location(image_pt.x,image_pt.y - 10);
//     cv::putText(chessboard_image_points,
//                 image_pt_ss.str(),
//                 text_location,
//                 cv::FONT_HERSHEY_SIMPLEX,
//                 font_scale,
//                 cv::Scalar(0,0,255),
//                 font_thickness);
//   }
//   cv::imwrite("images/chessboard_image_points.png",chessboard_image_points);

//   // found experimentally on rig
//   std::vector<cv::Point2f> stage_points;

//   stage_points.push_back(cv::Point2f(81200,36000)); stage_points.push_back(cv::Point2f(81500,71200));

//   stage_points.push_back(cv::Point2f(67200,36000)); stage_points.push_back(cv::Point2f(67300,71200));

//   stage_points.push_back(cv::Point2f(53000,36000)); stage_points.push_back(cv::Point2f(53200,71200));

//   stage_points.push_back(cv::Point2f(38900,36100)); stage_points.push_back(cv::Point2f(39100,71300));

//   cv::Mat chessboard_stage_points;
//   cv::cvtColor(chessboard,chessboard_stage_points,CV_GRAY2BGR);
//   cv::drawChessboardCorners(chessboard_stage_points,patternsize,cv::Mat(corners),patternfound);
//   for (size_t i=0; i<stage_points.size(); ++i)
//   {
//     cv::Point2i stage_pt = stage_points[i];
//     cv::Point2i image_pt = image_points[i];
//     std::stringstream stage_pt_ss;
//     stage_pt_ss << "s" << stage_pt;
//     cv::Point2i text_location(image_pt.x,image_pt.y - 10);
//     cv::putText(chessboard_stage_points,
//                 stage_pt_ss.str(),
//                 text_location,
//                 cv::FONT_HERSHEY_SIMPLEX,
//                 font_scale,
//                 cv::Scalar(0,0,255),
//                 font_thickness);
//   }
//   cv::imwrite("images/chessboard_stage_points.png",chessboard_stage_points);

//   cv::FileStorage fs("calibration/calibration.yml", cv::FileStorage::WRITE);
//   std::vector<cv::Point2i> image_points_i;
//   cv::Mat(image_points).copyTo(image_points_i);
//   std::vector<cv::Point2i> stage_points_i;
//   cv::Mat(stage_points).copyTo(stage_points_i);
//   cv::Mat homography_image_to_stage = cv::findHomography(image_points,stage_points,CV_RANSAC);
//   cv::Mat homography_stage_to_image = cv::findHomography(stage_points,image_points,CV_RANSAC);
//   std::vector<cv::Point2f> image_points_calculated;
//   cv::perspectiveTransform(stage_points,image_points_calculated,homography_stage_to_image);
//   std::vector<cv::Point2i> image_points_calculated_i;
//   cv::Mat(image_points_calculated).copyTo(image_points_calculated_i);
//   std::vector<cv::Point2f> stage_points_calculated;
//   cv::perspectiveTransform(image_points,stage_points_calculated,homography_image_to_stage);
//   std::vector<cv::Point2i> stage_points_calculated_i;
//   cv::Mat(stage_points_calculated).copyTo(stage_points_calculated_i);


//   fs << "image_points" << image_points_i;
//   fs << "image_points_calculated" << image_points_calculated_i;
//   fs << "stage_points" << stage_points_i;
//   fs << "stage_points_calculated" << stage_points_calculated_i;
//   fs << "homography_image_to_stage" << homography_image_to_stage;
//   fs << "homography_stage_to_image" << homography_stage_to_image;

//   fs.release();

//   cv::Mat calibrated;
//   cv::cvtColor(chessboard,calibrated,CV_GRAY2BGR);

//   std::vector<cv::Point2f> stage_points_in;
//   std::vector<cv::Point2f> image_points_out;

//   cv::Point2f stage_offset(20000,20000);
//   cv::Point2f stage_limit(100000,100000);

//   stage_points_in.push_back(stage_offset);
//   stage_points_in.push_back(cv::Point2f((stage_limit.x - stage_offset.x),stage_offset.y));
//   stage_points_in.push_back(cv::Point2f(stage_offset.x,(stage_limit.y - stage_offset.y)));
//   stage_points_in.push_back(cv::Point2f((stage_limit.x - stage_offset.x),(stage_limit.y - stage_offset.y)));

//   cv::perspectiveTransform(stage_points_in,image_points_out,homography_stage_to_image);
//   std::vector<cv::Point2i> stage_points_in_i;
//   cv::Mat(stage_points_in).copyTo(stage_points_in_i);
//   std::vector<cv::Point2i> image_points_out_i;
//   cv::Mat(image_points_out).copyTo(image_points_out_i);

//   cv::line(calibrated,
//            image_points_out[0],
//            image_points_out[1],
//            cv::Scalar(255,0,0),
//            4);
//   cv::line(calibrated,
//            image_points_out[0],
//            image_points_out[2],
//            cv::Scalar(0,255,0),
//            4);
//   cv::line(calibrated,
//            image_points_out[1],
//            image_points_out[3],
//            cv::Scalar(255,255,0),
//            4);
//   cv::line(calibrated,
//            image_points_out[2],
//            image_points_out[3],
//            cv::Scalar(255,255,0),
//            4);

//   std::stringstream image_out_ss_0;
//   image_out_ss_0 << "s" << stage_points_in_i[0];
//   cv::Point2f image_out_0(image_points_out[0].x + 20,image_points_out[0].y - 20);
//   cv::putText(calibrated,
//               image_out_ss_0.str(),
//               image_out_0,
//               cv::FONT_HERSHEY_SIMPLEX,
//               font_scale,
//               cv::Scalar(0,0,255),
//               font_thickness);

//   std::stringstream image_out_ss_1;
//   image_out_ss_1 << "s" << stage_points_in_i[1];
//   cv::Point2f image_out_1(image_points_out[1].x + 20,image_points_out[1].y + 30);
//   cv::putText(calibrated,
//               image_out_ss_1.str(),
//               image_out_1,
//               cv::FONT_HERSHEY_SIMPLEX,
//               font_scale,
//               cv::Scalar(0,0,255),
//               font_thickness);

//   std::stringstream image_out_ss_2;
//   image_out_ss_2 << "s" << stage_points_in_i[2];
//   cv::Point2f image_out_2(image_points_out[2].x,image_points_out[2].y + 20);
//   cv::putText(calibrated,
//               image_out_ss_2.str(),
//               image_out_2,
//               cv::FONT_HERSHEY_SIMPLEX,
//               font_scale,
//               cv::Scalar(0,0,255),
//               font_thickness);

//   std::stringstream image_out_ss_3;
//   image_out_ss_3 << "s" << stage_points_in_i[3];
//   cv::Point2f image_out_3(image_points_out[3].x,image_points_out[3].y + 20);
//   cv::putText(calibrated,
//               image_out_ss_3.str(),
//               image_out_3,
//               cv::FONT_HERSHEY_SIMPLEX,
//               font_scale,
//               cv::Scalar(0,0,255),
//               font_thickness);

//   // std::vector<cv::Point2f> image_points_in;
//   // std::vector<cv::Point2f> stage_points_out;

//   // cv::Point2f image_offset(175,250);

//   // image_points_in.push_back(image_offset);
//   // image_points_in.push_back(cv::Point2f((background.size().width - image_offset.x),image_offset.y));
//   // image_points_in.push_back(cv::Point2f(image_offset.x,(background.size().height - image_offset.y)));
//   // image_points_in.push_back(cv::Point2f((background.size().width - image_offset.x),(background.size().height - image_offset.y)));

//   // cv::perspectiveTransform(image_points_in,stage_points_out,homography_image_to_stage);
//   // std::vector<cv::Point2i> image_points_in_i;
//   // cv::Mat(image_points_in).copyTo(image_points_in_i);
//   // std::vector<cv::Point2i> stage_points_out_i;
//   // cv::Mat(stage_points_out).copyTo(stage_points_out_i);

//   // cv::line(calibrated,
//   //          image_points_in[0],
//   //          image_points_in[1],
//   //          cv::Scalar(255,0,0),
//   //          4);
//   // cv::line(calibrated,
//   //          image_points_in[0],
//   //          image_points_in[2],
//   //          cv::Scalar(0,255,0),
//   //          4);
//   // cv::line(calibrated,
//   //          image_points_in[1],
//   //          image_points_in[3],
//   //          cv::Scalar(255,255,0),
//   //          4);
//   // cv::line(calibrated,
//   //          image_points_in[2],
//   //          image_points_in[3],
//   //          cv::Scalar(255,255,0),
//   //          4);

//   // std::stringstream image_in_ss_0;
//   // image_in_ss_0 << "i" << image_points_in_i[0] << " -> " << "s" << stage_points_out_i[0];
//   // cv::Point2f image_in_0(image_points_in_i[0].x,image_points_in_i[0].y - 20);
//   // cv::putText(calibrated,
//   //             image_in_ss_0.str(),
//   //             image_in_0,
//   //             cv::FONT_HERSHEY_SIMPLEX,
//   //             font_scale,
//   //             cv::Scalar(0,0,255),
//   //             font_thickness);

//   // std::stringstream image_in_ss_1;
//   // image_in_ss_1 << "i" << image_points_in_i[1] << " -> " << "s" << stage_points_out_i[1];
//   // cv::Point2f image_in_1(image_points_in_i[1].x - 120,image_points_in_i[1].y - 20);
//   // cv::putText(calibrated,
//   //             image_in_ss_1.str(),
//   //             image_in_1,
//   //             cv::FONT_HERSHEY_SIMPLEX,
//   //             font_scale,
//   //             cv::Scalar(0,0,255),
//   //             font_thickness);

//   // std::stringstream image_in_ss_2;
//   // image_in_ss_2 << "i" << image_points_in_i[2] << " -> " << "s" << stage_points_out_i[2];
//   // cv::Point2f image_in_2(image_points_in_i[2].x,image_points_in_i[2].y + 30);
//   // cv::putText(calibrated,
//   //             image_in_ss_2.str(),
//   //             image_in_2,
//   //             cv::FONT_HERSHEY_SIMPLEX,
//   //             font_scale,
//   //             cv::Scalar(0,0,255),
//   //             font_thickness);

//   // std::stringstream image_in_ss_3;
//   // image_in_ss_3 << "i" << image_points_in_i[3] << " -> " << "s" << stage_points_out_i[3];
//   // cv::Point2f image_in_3(image_points_in_i[3].x - 120,image_points_in_i[3].y + 30);
//   // cv::putText(calibrated,
//   //             image_in_ss_3.str(),
//   //             image_in_3,
//   //             cv::FONT_HERSHEY_SIMPLEX,
//   //             font_scale,
//   //             cv::Scalar(0,0,255),
//   //             font_thickness);

//   cv::imwrite("images/calibrated.png",calibrated);

//   return 0;
// }
