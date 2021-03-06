#include <stdio.h>

#include <iostream>
#include <vector>

#include <opencv2/videoio.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core.hpp>
#include "calibration_class.hpp"
#include "create_chessboard.hpp"
using namespace cv;
using namespace std;

using namespace calibration;

int main(int argc, char** argv )
{
	
	
	//asks the user if they want to generate chess board or calibrate
	int generate_chess = 0;
	
	std::cout << "Would you like to calibrate (0) or generate a chessboard (1) ? : ";
	std::cin >> generate_chess;

	if (generate_chess){
		int chess_dimy, chess_dimx;
		std::cout << "How many dimensions in horizontal? :  ";
		std::cin >> chess_dimx;
		std::cout << "How many dimensions in vertical? : ";
		std::cin >> chess_dimy;
		std::cout << "What do you want to call your marker? : ";
		std::string chess_file_name;
		std::cin >> chess_file_name;
		generate_chess_board(chess_dimx, chess_dimy, chess_file_name);
		return 1;
	}


	//creating the calibration class, this class is responsible for storing the image chessboard points.
	calibration_geometry cal_class;


	
	
	


	if (cal_class.get_real_time_var() == 1){
		VideoCapture *cap_left,*cap_right;
		cap_left = new VideoCapture(1);
		cap_right = new VideoCapture(0);
		
		////set max resolution
		//cap_left->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
		//cap_left->set(CV_CAP_PROP_FRAME_HEIGHT, 720);

		//cap_right->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
		//cap_right->set(CV_CAP_PROP_FRAME_HEIGHT, 720);
		if ((cap_left->isOpened() == false) || (cap_right->isOpened() == false)){
			std::cerr << "ERROR: could not open camera for real time capture" << std::endl;
			return -1;
		}
		
		int stereo_vision_counter = 35;

		cv::Mat left_in, right_in;
		bool end_calibration= false;
		while (!end_calibration){
			*cap_left >> left_in;
			*cap_right >> right_in;


			



			imshow("left", left_in);
			imshow("right", right_in);

			char key_press = waitKey(1);


			
			if (key_press == ' '){


				/*cv::imwrite("left_logitech" + std::to_string(stereo_vision_counter) + ".png", left_in);
				cv::imwrite("right_logitech" + std::to_string(stereo_vision_counter) + ".png", right_in);*/

				left_in = cv::imread("left_logitech" + std::to_string(stereo_vision_counter) + ".png", cv::IMREAD_COLOR);
				right_in=cv::imread("right_logitech" + std::to_string(stereo_vision_counter) + ".png",cv::IMREAD_COLOR);
				stereo_vision_counter++;

				cal_class.read_in_images_camera(left_in,right_in);


				cv::Mat *left_ptr, *right_ptr;
				bool left_success = cal_class.calibrate(calibration::direction_t::left);
				bool right_success = cal_class.calibrate(calibration::direction_t::right);


				if (cal_class.corners_left.size() == cal_class.number_blocks){
					left_ptr = cal_class.get_image_pointer(calibration::direction_t::left);
					for (auto counter_o = cal_class.corners_left.begin(); counter_o != cal_class.corners_left.end(); ++counter_o){
						cv::circle(*left_ptr, *counter_o, 3, cv::Scalar(200, 100, 200), 3);
					}
				}

				if (cal_class.corners_right.size() == cal_class.number_blocks){
					right_ptr = cal_class.get_image_pointer(calibration::direction_t::right);
					for (auto counter_o = cal_class.corners_right.begin(); counter_o != cal_class.corners_right.end(); ++counter_o){
						cv::circle(*right_ptr, *counter_o, 3, cv::Scalar(200, 100, 200), 3);
					}
				}


				if (left_success&&right_success){
					

					cal_class.update_image_points(calibration::direction_t::left);
					cal_class.update_image_points(calibration::direction_t::right);
					cal_class.update_chessboard();
					cal_class.increase_sucess();
					cal_class.display_images();
				}
			}
			else if (key_press == 'e'){
				end_calibration = true;
			}
			
		}


	}
	






	if (cal_class.get_real_time_var() == 0){

		if (cal_class.type == calibration::calibration_t::stereo){

			bool end_image_list = false;

			
			//image pointers 
			cv::Mat *image_left, *image_right;


			//variable to keep track of current image
			
			while (cal_class.file_in_range()){
				cal_class.read_in_images();
				


				bool left_success=cal_class.calibrate(calibration::direction_t::left);
				bool right_success = cal_class.calibrate(calibration::direction_t::right);
				if (cal_class.corners_left.size() == cal_class.number_blocks){
					image_left = cal_class.get_image_pointer(calibration::direction_t::left);
					for (auto counter_o = cal_class.corners_left.begin(); counter_o != cal_class.corners_left.end(); ++counter_o){
						cv::circle(*image_left, *counter_o, 3, cv::Scalar(200, 100, 200), 3);
					}
				}

				if (cal_class.corners_right.size() == cal_class.number_blocks){
					image_right = cal_class.get_image_pointer(calibration::direction_t::right);
					for (auto counter_o = cal_class.corners_right.begin(); counter_o != cal_class.corners_right.end(); ++counter_o){
						cv::circle(*image_right, *counter_o, 3, cv::Scalar(200, 100, 200), 3);
					}
				}


				if (left_success&&right_success){
					cal_class.update_image_points(calibration::direction_t::left);
					cal_class.update_image_points(calibration::direction_t::right);
					cal_class.update_chessboard();
					cal_class.increase_sucess();
					cal_class.display_images();
				}
			
			}

			
		}

	}
	if (cal_class.type == calibration::calibration_t::stereo){
		cal_class.calibrate_stereo_intrinsics();
		cal_class.calibrate_stereo_parameters();
		cal_class.write_camera_properties_stereo();
	}

	////delete camera ptr if using camera
	//if (cal_class.get_real_time_var() == 1)
	//	delete cap;


    return 0;
}