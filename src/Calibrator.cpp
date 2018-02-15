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
  recalibrate_ = false;
}

void Calibrator::setConfigurationRepositoryPath(boost::filesystem::path path)
{
  configuration_repository_path_ = path;
}

void Calibrator::recalibrate()
{
  recalibrate_ = true;
}

bool Calibrator::getHomographyImageToStage(cv::Mat & homography_image_to_stage)
{
  const bool got_calibration = true;

  boost::filesystem::path calibration_path = configuration_repository_path_;
  calibration_path /= "calibration/calibration.yml";

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

  if (recalibrate_)
  {
    bool success = calibrate(calibration_path);
    if (!success)
    {
      return !got_calibration;
    }
  }

  cv::FileStorage calibration_fs(calibration_path.string(), cv::FileStorage::READ);
  calibration_fs["homography_image_to_stage"] >> homography_image_to_stage;
  calibration_fs.release();
  return got_calibration;
}

// private

bool Calibrator::calibrate(const boost::filesystem::path calibration_path)
{
  boost::filesystem::path chessboard_path = configuration_repository_path_;
  chessboard_path /= "images/chessboard.png";

  //////
  cv::Mat chessboard = cv::imread(chessboard_path.string(),cv::IMREAD_GRAYSCALE);
  if(!chessboard.data)
  {
    std::cout <<  "Could not open or find the chessboard image!" << std::endl;
    return false;
  }
  else
  {
    std::cout <<  "Found chessboard image!" << std::endl;
  }

  size_t pattern_count_col = 6;
  size_t pattern_count_row = 8;
  cv::Size patternsize(pattern_count_col,pattern_count_row);
  std::vector<cv::Point2f> corners;

  bool patternfound = cv::findChessboardCorners(chessboard,
                                                patternsize,
                                                corners,
                                                cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE + cv::CALIB_CB_FAST_CHECK);

  if(!patternfound)
  {
    std::cout <<  "Could not find the chessboard corners!" << std::endl;
    return false;
  }
  if(patternfound)
  {
    std::cout <<  "Found the chessboard corners!" << std::endl;
    cv::cornerSubPix(chessboard,
                     corners,
                     cv::Size(11, 11),
                     cv::Size(-1, -1),
                     cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
  }

  std::vector<cv::Point2f> image_points;
  for (size_t row=0; row<pattern_count_row; ++row)
  {
    for (size_t col=0; col<pattern_count_col; ++col)
    {
      if ((col % (pattern_count_col - 1)) == 0)
      {
        image_points.push_back(corners[row*pattern_count_col + col]);
      }
    }
  }
  std::vector<cv::Point2i> image_points_i;
  cv::Mat(image_points).copyTo(image_points_i);
  boost::filesystem::path image_points_path = configuration_repository_path_;
  image_points_path /= "calibration/image_points.yml";
  cv::FileStorage image_points_fs(image_points_path.string(), cv::FileStorage::WRITE);
  image_points_fs << "image_points" << image_points_i;
  image_points_fs.release();


  float font_scale = 0.55;
  int font_thickness = 2;

  cv::Mat chessboard_image_points;
  cv::cvtColor(chessboard,chessboard_image_points,CV_GRAY2BGR);
  cv::drawChessboardCorners(chessboard_image_points,patternsize,cv::Mat(corners),patternfound);
  size_t index = 0;
  for (std::vector<cv::Point2i>::iterator it = image_points_i.begin(); it != image_points_i.end(); ++it)
  {
    cv::Point2i image_pt = *it;
    std::stringstream image_pt_ss;
    image_pt_ss << "i" << index << image_pt;
    cv::Point2i text_location;
    if (((index % 4) == 0) || (((index - 1) % 4) == 0))
    {
      text_location = cv::Point2i(image_pt.x,image_pt.y - 10);
    }
    else
    {
      text_location = cv::Point2i(image_pt.x,image_pt.y + 30);
    }
    cv::putText(chessboard_image_points,
                image_pt_ss.str(),
                text_location,
                cv::FONT_HERSHEY_SIMPLEX,
                font_scale,
                cv::Scalar(0,0,255),
                font_thickness);
    ++index;
  }
  boost::filesystem::path chessboard_image_points_path = configuration_repository_path_;
  chessboard_image_points_path /= "images/chessboard_image_points.png";
  cv::imwrite(chessboard_image_points_path.string(),chessboard_image_points);

  boost::filesystem::path stage_points_path = configuration_repository_path_;
  stage_points_path /= "calibration/stage_points.yml";
  cv::FileStorage stage_points_fs(stage_points_path.string(), cv::FileStorage::READ);
  std::vector<cv::Point2f> stage_points;
  stage_points_fs["stage_points"] >> stage_points;
  stage_points_fs.release();

  std::vector<cv::Point2i> stage_points_i;
  cv::Mat(stage_points).copyTo(stage_points_i);

  cv::Mat chessboard_stage_points;
  cv::cvtColor(chessboard,chessboard_stage_points,CV_GRAY2BGR);
  cv::drawChessboardCorners(chessboard_stage_points,patternsize,cv::Mat(corners),patternfound);
  for (size_t i=0; i<stage_points_i.size(); ++i)
  {
    cv::Point2i stage_pt = stage_points_i[i];
    cv::Point2i image_pt = image_points_i[i];
    std::stringstream stage_pt_ss;
    stage_pt_ss << "s" << i << stage_pt;
    cv::Point2i text_location;
    if (((i % 4) == 0) || (((i - 1) % 4) == 0))
    {
      text_location = cv::Point2i(image_pt.x,image_pt.y - 10);
    }
    else
    {
      text_location = cv::Point2i(image_pt.x,image_pt.y + 30);
    }
    cv::putText(chessboard_stage_points,
                stage_pt_ss.str(),
                text_location,
                cv::FONT_HERSHEY_SIMPLEX,
                font_scale,
                cv::Scalar(0,0,255),
                font_thickness);
  }
  boost::filesystem::path chessboard_stage_points_path = configuration_repository_path_;
  chessboard_stage_points_path /= "images/chessboard_stage_points.png";
  cv::imwrite(chessboard_stage_points_path.string(),chessboard_stage_points);

  cv::FileStorage calibration_fs(calibration_path.string(), cv::FileStorage::WRITE);
  cv::Mat homography_image_to_stage = cv::findHomography(image_points,stage_points,CV_RANSAC);
  cv::Mat homography_stage_to_image = cv::findHomography(stage_points,image_points,CV_RANSAC);
  calibration_fs << "homography_image_to_stage" << homography_image_to_stage;
  calibration_fs << "homography_stage_to_image" << homography_stage_to_image;

  calibration_fs.release();

  cv::Mat calibrated;
  cv::cvtColor(chessboard,calibrated,CV_GRAY2BGR);

  std::vector<cv::Point2f> stage_points_in;
  std::vector<cv::Point2f> image_points_out;

  cv::Point2f stage_offset(15000,15000);
  cv::Point2f stage_limit(100000,100000);

  stage_points_in.push_back(stage_offset);
  stage_points_in.push_back(cv::Point2f((stage_limit.x - stage_offset.x),stage_offset.y));
  stage_points_in.push_back(cv::Point2f(stage_offset.x,(stage_limit.y - stage_offset.y)));
  stage_points_in.push_back(cv::Point2f((stage_limit.x - stage_offset.x),(stage_limit.y - stage_offset.y)));

  cv::perspectiveTransform(stage_points_in,image_points_out,homography_stage_to_image);
  std::vector<cv::Point2i> stage_points_in_i;
  cv::Mat(stage_points_in).copyTo(stage_points_in_i);
  std::vector<cv::Point2i> image_points_out_i;
  cv::Mat(image_points_out).copyTo(image_points_out_i);

  cv::line(calibrated,
           image_points_out[0],
           image_points_out[1],
           cv::Scalar(255,0,0),
           4);
  cv::line(calibrated,
           image_points_out[0],
           image_points_out[2],
           cv::Scalar(0,255,0),
           4);
  cv::line(calibrated,
           image_points_out[1],
           image_points_out[3],
           cv::Scalar(255,255,0),
           4);
  cv::line(calibrated,
           image_points_out[2],
           image_points_out[3],
           cv::Scalar(255,255,0),
           4);

  std::stringstream image_out_ss_0;
  image_out_ss_0 << "s" << stage_points_in_i[0];
  cv::Point2f image_out_0(image_points_out[0].x + 20,image_points_out[0].y - 20);
  cv::putText(calibrated,
              image_out_ss_0.str(),
              image_out_0,
              cv::FONT_HERSHEY_SIMPLEX,
              font_scale,
              cv::Scalar(0,0,255),
              font_thickness);

  std::stringstream image_out_ss_1;
  image_out_ss_1 << "s" << stage_points_in_i[1];
  cv::Point2f image_out_1(image_points_out[1].x + 20,image_points_out[1].y + 30);
  cv::putText(calibrated,
              image_out_ss_1.str(),
              image_out_1,
              cv::FONT_HERSHEY_SIMPLEX,
              font_scale,
              cv::Scalar(0,0,255),
              font_thickness);

  std::stringstream image_out_ss_2;
  image_out_ss_2 << "s" << stage_points_in_i[2];
  cv::Point2f image_out_2(image_points_out[2].x,image_points_out[2].y + 20);
  cv::putText(calibrated,
              image_out_ss_2.str(),
              image_out_2,
              cv::FONT_HERSHEY_SIMPLEX,
              font_scale,
              cv::Scalar(0,0,255),
              font_thickness);

  std::stringstream image_out_ss_3;
  image_out_ss_3 << "s" << stage_points_in_i[3];
  cv::Point2f image_out_3(image_points_out[3].x,image_points_out[3].y + 20);
  cv::putText(calibrated,
              image_out_ss_3.str(),
              image_out_3,
              cv::FONT_HERSHEY_SIMPLEX,
              font_scale,
              cv::Scalar(0,0,255),
              font_thickness);

  boost::filesystem::path calibrated_path = configuration_repository_path_;
  calibrated_path /= "images/calibrated.png";
  cv::imwrite(calibrated_path.string(),calibrated);

  return true;
}
