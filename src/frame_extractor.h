#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class FrameExtractor
{
public:
    FrameExtractor(const std::string &videoPath, const std::string &outputDir);

    // Extract frames from video
    bool extract(int maxFrames = -1, float frameRate = -1.0);

    // Get paths to extracted frames
    const std::vector<std::string> &getFramePaths() const;

private:
    std::string videoPath_;
    std::string outputDir_;
    std::vector<std::string> framePaths_;
};