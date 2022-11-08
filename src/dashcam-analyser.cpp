﻿#include "dashcam-analyser.hpp"
#include "car-detecter.hpp"

int main(int argc, char* argv[])
{
    cv::String pathToVideo {"d.mp4"};
    bool useCuda {true};

	cv::namedWindow("Dashcam Analyser", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("Dashcam Analyser 2", cv::WINDOW_AUTOSIZE);

    if (argc > 1)
    {
        const cv::String keys{
            "{help h usage ? |      | Print this message}"
            "{@path			 |<none>| Path of the video/camera}"
            "{cu cuda		 |      | Use CUDA}"};

        cv::CommandLineParser parser(argc, argv, keys);
        parser.about("Dashcam Analyser V0.2");

        pathToVideo = parser.get<cv::String>(0);

        if (parser.has("help"))
        {
            parser.printMessage();
            return 0;
        }

        if (parser.has("cu"))
        {
            useCuda = true;
        }
    }

    std::vector<std::string> classList;
    std::ifstream ifs("coco.names");
    std::string line;
    while (getline(ifs, line))
    {
        classList.push_back(line);
    }

    cv::VideoCapture video {pathToVideo};

    if (!video.isOpened())
    {
        std::cerr << "Error when opening the video." << std::endl;
        return -1;
    }

    auto net = cv::dnn::readNet("yolov5x6.onnx");
    dashan::configureNet(net, useCuda);

    cv::Mat frame;
   
	while (true)
	{
        video >> frame;
        if (frame.empty())
        {
            return 0;
        }

        std::vector<dashan::Detection> output;
        dashan::detect(frame, net, output, classList);

        int detections = output.size();

        for (int i = 0; i < detections; ++i)
        {
            auto detection {output[i]};
            auto box {detection.box};
            auto classId {detection.classId};
            auto confidence {detection.confidence};

            cv::rectangle(frame, box, cv::Scalar(0, 0, 255), 3);
            cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), cv::Scalar(0, 0, 255), cv::FILLED);
            cv::putText(frame, classList[classId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        }

        cv::imshow("Dashcam Analyser", frame);
        if (static_cast<int>(cv::waitKey(33)) == 27) {
            cv::destroyAllWindows();
            return 0;
        }       
	}
	return 0;
}
