#ifndef HIPACC_HELPER_HPP
#define HIPACC_HELPER_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include "../VX/vx.h"

#ifdef USE_OPENCV
# include <opencv2/opencv.hpp>
#endif


// get time in milliseconds
double time_ms () {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}


std::vector<int> create_contiguous_array_from_keypoints(std::vector<vx_keypoint_t> features)
{
  std::vector<int> toReturn(features.size() * 7); //vx_keypoint_t has 7 members
  for(int i = 0; i < features.size() * 7; i += 7)
  {
    toReturn[i+0] = reinterpret_cast<int>(features[i/7].x);
    toReturn[i+1] = reinterpret_cast<int>(features[i/7].y);
    toReturn[i+2] = *reinterpret_cast<int*>(&(features[i/7].strength));
    toReturn[i+3] = *reinterpret_cast<int*>(&(features[i/7].scale));
    toReturn[i+4] = *reinterpret_cast<int*>(&(features[i/7].orientation));
    toReturn[i+5] = reinterpret_cast<int>(features[i/7].tracking_status);
    toReturn[i+6] = *reinterpret_cast<int*>(&(features[i/7].error));
  }
  return toReturn;
}


template<typename T>
void draw_cross(const unsigned int width, const unsigned int height, const T* in, std::vector<vx_keypoint_t> features, T* out, T cross_value)
{
	for(int y = 0; y != height; y++)
	{
		for(int x = 0; x != width; x++)
		{
			out[y * width + x] = in[y * width + x];
		}
	}

	for(auto& feature: features)
	{
		std::vector<std::vector<int>> cross {          {-1, 0},
											 { 0, -1}, { 0, 0}, { 0,  1},
													   { 1, 0}          };
		for (int i = 0; i < 5; i++)
		{
			int cross_y = feature.y + cross[i][0];
			int cross_x = feature.x + cross[i][1];

			if (cross_x < 0 || cross_x >= width)
				continue;
			if (cross_y < 0 || cross_y >= height)
				continue;
			out[cross_y * width + cross_x] = cross_value;
		}
    out[feature.y * width + feature.x] = 255 - cross_value;
	}
}


template<typename T>
std::vector<vx_keypoint_t> euclidian_single_feature(const unsigned int width, const unsigned int height, const T* in, float eucl_dist)
{
	std::vector<vx_keypoint_t> features;
	for(int i = 0; i < 200*300; i++)
	{
		if (in[i] == 0)
			continue;
		vx_keypoint_t keypoint;
		keypoint.x = i % 200;
		keypoint.y = i / 200;
		keypoint.strength = in[i];
		keypoint.scale = 0;
		keypoint.orientation = 0;
		keypoint.tracking_status = 1;
		keypoint.error = 0;
		features.emplace_back(keypoint);
	}


	std::sort(features.begin(), features.end(),
			  [](const vx_keypoint_t &t1, const vx_keypoint_t &t2)
		{
			return t1.strength > t2.strength;
		});


	std::vector<vx_keypoint_t> final_features;

	float eucl_dist_sq = eucl_dist*eucl_dist;
	for(int i = 0; i < features.size(); i++)
	{
		auto& keypoint = features[i];
		int my_x = keypoint.x;
		int my_y = keypoint.y;

		bool alone = true;

		for (int j = 0; j < i; j++)
		{
			auto& other_keypoint = features[j];
			int other_x = other_keypoint.x;
			int other_y = other_keypoint.y;

			if ((my_x - other_x) * (my_x - other_x) + (my_y - other_y) * (my_y - other_y) <= eucl_dist_sq)
			{
				alone = false;
				break;
			}
		}

		if (alone)
			final_features.emplace_back(keypoint);
	}
	return final_features;
}

template<typename T>
void non_max_supression(const unsigned int width, const unsigned int height, const T* in, T* out)
{
	static std::vector<std::vector<int>> offset {{-1, -1}, {-1, 0}, {-1,  1},
												 { 0, -1},          { 0,  1},
												 { 1, -1}, { 1, 0}, { 1,  1}};
	bool reset = false;
	for(int y = 0; y != height; y++)
	{
		for(int x = 0; x != width; x++)
		{

			reset = false;
			for (int i = 0; i < 8 && !reset; i++)
			{
				int neighbour_y = y + offset[i][0];
				int neighbour_x = x + offset[i][1];

				T my_value = in[y * width + x];

				if (neighbour_x < 0 || neighbour_x >= width)
					continue;
				if (neighbour_y < 0 || neighbour_y >= height)
					continue;

				if (i < 4) // >=
				{
					if (my_value <  in[neighbour_y * width + neighbour_x])
						reset = true;
				}
				else // >
				{
					if (my_value <= in[neighbour_y * width + neighbour_x])
						reset = true;
				}
			}

			if (reset)
				out[y * width + x] = 0;
			else
				out[y * width + x] = in[y * width + x];
		}
	}
}

template<typename T>
T* load_data(const unsigned int width, const unsigned int height,
             const unsigned int chan=1, const std::string filename="") {
  if (std::is_same<T,unsigned char>::value) {
    if (chan != 1 && chan != 4) {
      std::cerr << "Only grayscale or BRGA supported for uchar" << std::endl;
      exit(-1);
    }
  } else if (std::is_same<T,float>::value) {
    if (chan != 1) {
      std::cerr << "Only grayscale supported for float" << std::endl;
      exit(-1);
    }
  } else {
    std::cerr << "Type not supported" << std::endl;
    exit(-1);
  }

  T* data = new T[width*height*chan];

#ifdef USE_OPENCV
  if (!filename.empty())
  {
    cv::Mat image = cv::imread(filename.c_str(), 1);

    if (!image.data) {
      std::cerr << "No image data" << std::endl;
      exit(-1);
    }

    if (image.cols != width || image.rows != height) {
      std::cerr << "Image size mismatch" << std::endl;
      exit(-1);
    }

    if (image.channels() == chan) {
      // copy entire array
      std::memcpy(data, (T*)image.data, width*height*chan*sizeof(T));
    } else {
      if (chan == 1) {
        // convert to grayscale
        cv::Mat gray_image;
        cv::cvtColor(image, gray_image, CV_BGR2GRAY);
        for (unsigned int p = 0; p < width*height; ++p) {
          data[p] = (T)gray_image.data[p];
        }
      } else {
        // match BGR to BRGA
        unsigned char* d = (unsigned char*)data;
        for (unsigned int p = 0; p < width*height; ++p) {
          for (unsigned int c = 0; c < chan; ++c) {
            d[p*chan+c] = c < image.channels()
                ? image.data[p*image.channels()+c] : 0;
          }
        }
      }
    }
  }
  else
#endif
  {
    // random data, channels will be ignored
    for (unsigned int p = 0; p < width * height; ++p) {
      if (std::is_same<T,float>::value) {
        data[p] = ((T)std::rand())/RAND_MAX;
      } else {
        data[p] = (std::rand())%256;
      }
    }
  }

  return data;
}


template<typename T>
void save_data(const unsigned int width, const unsigned int height,
               const unsigned int chan, T* data, const std::string filename) {
#ifdef USE_OPENCV
  cv::Mat image;

  if (std::is_same<T,unsigned char>::value) {
    switch (chan) {
      case 1:
        image.create(height, width, CV_8UC1);
        break;
      case 4:
        image.create(height, width, CV_8UC4);
        break;
      default:
        std::cerr << "Only grayscale or RGBA supported for uchar" << std::endl;
        exit(-1);
        break;
    }
  } else if (std::is_same<T,float>::value) {
    if (chan != 1) {
      std::cerr << "Only grayscale supported for float" << std::endl;
      exit(-1);
    }
    image.create(height, width, CV_32FC1);
  } else if (std::is_same<T,short>::value) {
    if (chan != 1) {
      std::cerr << "Only grayscale supported for short" << std::endl;
      exit(-1);
    }
    image.create(height, width, CV_16SC1);
  } else {
    std::cerr << "Type not supported" << std::endl;
    exit(-1);
  }

  std::memcpy((T*)image.data, data, width*height*chan*sizeof(T));

  cv::imwrite(filename.c_str(), image);
#endif
}


template<typename T>
bool show_data(const unsigned int width, const unsigned int height,
               const unsigned int chan, T* data, const std::string title,
               const int wait=0) {
#ifdef USE_OPENCV
  cv::Mat image;

  if (std::is_same<T,unsigned char>::value) {
    switch (chan) {
      case 1:
        image.create(height, width, CV_8UC1);
        break;
      case 4:
        image.create(height, width, CV_8UC4);
        break;
      default:
        std::cerr << "Only grayscale or RGBA supported for uchar" << std::endl;
        exit(-1);
        break;
    }
  } else if (std::is_same<T,float>::value) {
    if (chan != 1) {
      std::cerr << "Only grayscale supported for float" << std::endl;
      exit(-1);
    }
    image.create(height, width, CV_32FC1);
  } else {
    std::cerr << "Type not supported" << std::endl;
    exit(-1);
  }

  std::memcpy((T*)image.data, data, width*height*chan*sizeof(T));

  if (height > 900) {
    float scale = 900/(float)height;
    cv::namedWindow(title.c_str(), cv::WINDOW_NORMAL);
    cv::resizeWindow(title.c_str(), width*scale, height*scale);
  }

  cv::imshow(title.c_str(), image);

  return cv::waitKey(wait) >= 0;
#else
  return true;
#endif
}


template<typename T>
void compare_results(const T* output, const T* reference,
                     const unsigned int width=1, const unsigned int height=1,
                     const unsigned int border_x=0,
                     const unsigned int border_y=0) {
    std::cout << "Comparing results ..." << std::endl;
    for (unsigned int y = border_y; y < height-border_y; ++y) {
        for (unsigned int x = border_x; x < width-border_x; ++x) {
            bool failed = true;

            if (std::is_same<T,float>::value) {
                float ref = reference[y*width + x];
                failed = abs(ref-output[y*width + x]) > (0.001f*ref);
            } else {
                int diff = (int)reference[y*width + x]-(int)output[y*width + x];
                failed = abs((float)diff) > 1.0f;
            }

            if (failed) {
                std::cerr << "Test FAILED, at (" << x << "," << y << "): "
                          << (float)reference[y*width + x] << " vs. "
                          << (float)output[y*width + x] << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    std::cout << "Test PASSED" << std::endl;
}


#endif // HIPACC_HELPER_HPP
