#include <iostream>
#include <filesystem>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "combineChunks.h"

using namespace std;
using namespace cv;
namespace fs = filesystem;

void combineChunks(string folderName, string resultName, int width, int height) {
    vector<string> chunks;
    vector<Point> chunkPositions;

    for (const auto& entry : fs::directory_iterator(folderName)) {
        // Read the name to get the position.
        string chunkName = entry.path().filename().string();
        string startPositionXString, startPositionYString;
        istringstream iss(chunkName);
        getline(iss, startPositionXString, '-');
        getline(iss, startPositionYString, '-');

        int startPositionX, startPositionY;
        if (istringstream(startPositionXString) >> startPositionX && istringstream(startPositionYString) >> startPositionY) {
            chunkPositions.push_back(Point(startPositionX, startPositionY));
        }
        else {
            cerr << "Failed to read positions for chunk." << endl;
        }

        // Push the image into the chunks vector.
        chunks.push_back(entry.path().generic_string());
    }

    Mat result = Mat(width, height, CV_8UC3, Scalar(0, 0, 0));

    for (size_t index = 0; index < chunks.size(); ++index) {
        Mat chunk = imread(chunks[index]);
        Point chunkPosition = chunkPositions[index];
        chunk.copyTo(result(Rect(chunkPosition, chunk.size())));
    }

    imwrite(resultName, result);
}