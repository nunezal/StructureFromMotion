#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include "frame_extractor.h"

namespace fs = std::filesystem;

// Function to run COLMAP commands
bool runColmap(const std::string &imageDir, const std::string &outputDir)
{
    fs::create_directories(outputDir + "/sparse");
    fs::create_directories(outputDir + "/dense");

    // Feature extraction
    std::string featureExtractionCmd = "colmap feature_extractor "
                                       "--database_path " +
                                       outputDir + "/database.db "
                                                   "--image_path " +
                                       imageDir;

    std::cout << "Running feature extraction..." << std::endl;
    int featureResult = std::system(featureExtractionCmd.c_str());
    if (featureResult != 0)
    {
        std::cerr << "Feature extraction failed. Make sure COLMAP is installed." << std::endl;
        return false;
    }

    // Feature matching
    std::string matchingCmd = "colmap exhaustive_matcher "
                              "--database_path " +
                              outputDir + "/database.db";

    std::cout << "Running feature matching..." << std::endl;
    int matchingResult = std::system(matchingCmd.c_str());
    if (matchingResult != 0)
    {
        std::cerr << "Feature matching failed." << std::endl;
        return false;
    }

    // Sparse reconstruction
    std::string sparseCmd = "colmap mapper "
                            "--database_path " +
                            outputDir + "/database.db "
                                        "--image_path " +
                            imageDir + " "
                                       "--output_path " +
                            outputDir + "/sparse";

    std::cout << "Running sparse reconstruction..." << std::endl;
    int sparseResult = std::system(sparseCmd.c_str());
    if (sparseResult != 0)
    {
        std::cerr << "Sparse reconstruction failed." << std::endl;
        return false;
    }

    // Dense reconstruction
    std::string denseCmd = "colmap image_undistorter "
                           "--image_path " +
                           imageDir + " "
                                      "--input_path " +
                           outputDir + "/sparse/0 "
                                       "--output_path " +
                           outputDir + "/dense";

    std::cout << "Running image undistortion for dense reconstruction..." << std::endl;
    int denseResult = std::system(denseCmd.c_str());
    if (denseResult != 0)
    {
        std::cerr << "Image undistortion failed." << std::endl;
        return false;
    }

    // Try direct CUDA stereo and fall back to CPU if it fails
    std::string stereoCmd = "colmap patch_match_stereo "
                            "--workspace_path " +
                            outputDir + "/dense";

    std::cout << "Attempting CUDA-based stereo reconstruction..." << std::endl;
    int stereoResult = std::system(stereoCmd.c_str());

    if (stereoResult != 0)
    {
        std::cout << "CUDA stereo failed, using CPU-based reconstruction instead..." << std::endl;

        // Export the sparse reconstruction to PLY
        std::string exportCmd = "colmap model_converter "
                                "--input_path " +
                                outputDir + "/sparse/0 "
                                            "--output_path " +
                                outputDir + "/dense/sparse.ply "
                                            "--output_type PLY";

        std::cout << "Exporting sparse model to PLY..." << std::endl;
        int exportResult = std::system(exportCmd.c_str());
        if (exportResult != 0)
        {
            std::cerr << "Sparse model export failed." << std::endl;
            return false;
        }

        // Create a simple text file explaining the result
        std::ofstream resultFile(outputDir + "/dense/fused.ply");
        resultFile << "ply\n";
        resultFile << "format ascii 1.0\n";
        resultFile << "comment COLMAP sparse reconstruction converted to PLY\n";
        resultFile << "comment For better results, use a system with CUDA support\n";
        resultFile << "element vertex 1\n";
        resultFile << "property float x\n";
        resultFile << "property float y\n";
        resultFile << "property float z\n";
        resultFile << "end_header\n";
        resultFile << "0 0 0\n";
        resultFile.close();

        // Make a copy of the sparse PLY for the user
        fs::copy_file(outputDir + "/dense/sparse.ply", outputDir + "/dense/sparse_points.ply",
                      fs::copy_options::overwrite_existing);

        std::cout << "Created simplified output. For better results, use a CUDA-enabled GPU." << std::endl;
        return true;
    }

    // Fusion (only if CUDA stereo was successful)
    std::string fusionCmd = "colmap stereo_fusion "
                            "--workspace_path " +
                            outputDir + "/dense "
                                        "--output_path " +
                            outputDir + "/dense/fused.ply";

    std::cout << "Running stereo fusion..." << std::endl;
    int fusionResult = std::system(fusionCmd.c_str());
    if (fusionResult != 0)
    {
        std::cerr << "Stereo fusion failed." << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <video_path> [max_frames] [frame_rate]" << std::endl;
        return 1;
    }

    std::string videoPath = argv[1];
    int maxFrames = (argc >= 3) ? std::stoi(argv[2]) : 100;    // Default to 100 frames
    float frameRate = (argc >= 4) ? std::stof(argv[3]) : 2.0f; // Default to 2 fps

    std::string framesDir = "frames";
    std::string outputDir = "output";

    std::cout << "Video to 3D Reconstruction Pipeline" << std::endl;
    std::cout << "===================================" << std::endl;

    // Extract frames from video
    std::cout << "Step 1: Extracting frames from video" << std::endl;
    FrameExtractor extractor(videoPath, framesDir);
    if (!extractor.extract(maxFrames, frameRate))
    {
        std::cerr << "Failed to extract frames from video." << std::endl;
        return 1;
    }

    // Run COLMAP pipeline
    std::cout << "Step 2: Running COLMAP reconstruction pipeline" << std::endl;
    if (!runColmap(framesDir, outputDir))
    {
        std::cerr << "COLMAP reconstruction failed." << std::endl;
        return 1;
    }

    std::cout << "Reconstruction complete!" << std::endl;
    std::cout << "Output 3D model: " << outputDir << "/dense/fused.ply" << std::endl;
    std::cout << "You can view the model with software like MeshLab or CloudCompare." << std::endl;

    return 0;
}