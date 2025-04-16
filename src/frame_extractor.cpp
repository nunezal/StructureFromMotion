#include "frame_extractor.h"
#include <iostream>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

FrameExtractor::FrameExtractor(const std::string &videoPath, const std::string &outputDir)
    : videoPath_(videoPath), outputDir_(outputDir)
{
    // Create output directory if it doesn't exist
    if (!fs::exists(outputDir_))
    {
        fs::create_directories(outputDir_);
    }
}

bool FrameExtractor::extract(int maxFrames, float frameRate)
{
    cv::VideoCapture cap(videoPath_);

    if (!cap.isOpened())
    {
        std::cerr << "Error: Could not open video file: " << videoPath_ << std::endl;
        return false;
    }

    double vidFps = cap.get(cv::CAP_PROP_FPS);
    int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));

    std::cout << "Video FPS: " << vidFps << std::endl;
    std::cout << "Total frames: " << totalFrames << std::endl;

    // Calculate frame interval for extraction
    int frameInterval = 1;
    if (frameRate > 0 && frameRate < vidFps)
    {
        frameInterval = static_cast<int>(vidFps / frameRate);
    }

    int frameCount = 0;
    int extractedFrames = 0;
    cv::Mat frame;

    framePaths_.clear();

    while (cap.read(frame))
    {
        if (frameCount % frameInterval == 0)
        {
            std::stringstream ss;
            ss << outputDir_ << "/frame_" << std::setw(6) << std::setfill('0') << extractedFrames << ".jpg";
            std::string framePath = ss.str();

            cv::imwrite(framePath, frame);
            framePaths_.push_back(framePath);

            extractedFrames++;
            std::cout << "Extracted frame " << extractedFrames << "/"
                      << (maxFrames > 0 ? std::to_string(maxFrames) : "all") << "\r" << std::flush;

            if (maxFrames > 0 && extractedFrames >= maxFrames)
            {
                break;
            }
        }

        frameCount++;
    }

    std::cout << std::endl
              << "Extracted " << extractedFrames << " frames" << std::endl;
    cap.release();

    return !framePaths_.empty();
}

const std::vector<std::string> &FrameExtractor::getFramePaths() const
{
    return framePaths_;
}