#include <iostream>
#include <complex>
#include <future>
#include <vector>
#include <iomanip>
#include <filesystem>
#include <cassert>
#include "lodepng.h"
#include "iterationsToColor.h"
#include "getUserInput.h"
#include "generateName.h"
#include "progressBar.h"
#include "combineChunks.h"

using namespace std;
namespace fs = std::filesystem;

void generateRow(int y, int actualY, int xChunkStart, int width, int actualWidth, int actualHeight, int maxIterations, double realMin, double realMax, double complexMin, double complexMax, vector<uint8_t>& pixels) {
    for (int x = 0; x < width; ++x) {
        int actualX = x + xChunkStart;
        double xComplex = actualX * ((realMax - realMin) / (actualWidth - 1)) + realMin;
        double yComplex = actualY * (complexMax - complexMin) / (actualHeight - 1) + complexMin;
        complex<double> complexNumber(xComplex, yComplex);
        complex<double> iterationComplex(xComplex, yComplex);
        int iterations = 0;

        while (abs(iterationComplex) < 2.0 && iterations < maxIterations) {
            iterations++;
            iterationComplex = iterationComplex * iterationComplex + complexNumber;
        }

        uint8_t r, g, b;
        iterationsToColor(iterations, maxIterations, r, g, b);

        size_t index = (static_cast<size_t>(y) * width + x) * 4;
        pixels[index] = r;
        pixels[index + 1] = g;
        pixels[index + 2] = b;
        pixels[index + 3] = 255;
    }
}

void generateChunk(string resultFolder, int chunk, int chunkX, int chunkY, int width, int actualWidth, int height, int actualHeight, int maxIterations, double realMin, double realMax, double complexMin, double complexMax) {
    cout << "Chunk " << chunk << endl;
    cout << "Generating chunk..." << endl;

    int xChunkStart = width * chunkX;
    int yChunkStart = height * chunkY;
    vector<uint8_t> pixels(width * height * 4, 0);
    vector<future<void>> futures;

    for (int y = 0; y < height; ++y) {
        int actualY = y + yChunkStart;
        futures.push_back(async(launch::async, generateRow, y, actualY, xChunkStart, width, actualWidth, actualHeight, maxIterations, realMin, realMax, complexMin, complexMax, ref(pixels)));
        updateProgressBar(y, height);
    }

    cout << endl;

    // Wait for all the rendering to complete.
    for (auto& future : futures) {
        future.wait();
    }

    cout << "Encoding chunk..." << endl;

    // Next encode the image and place it in the folder.
    string resultName = resultFolder + "/" + to_string(xChunkStart) + "-" + to_string(yChunkStart) + ".png";
    cout << resultName << endl;
    unsigned error = lodepng::encode(resultName.c_str(), pixels, width, height);

    if (error) {
        cerr << "Faild to generate chunk: " << lodepng_error_text(error) << endl;
    }
    else {
        cout << "Chunk finished." << endl << endl;
    }
}

int main() {
    // Get the settings.
    int width = getUserInput("Enter width:");
    int height = getUserInput("Enter height:");
    int chunkSize = getUserInput("Enter chunk size:");
    int maxIterations = getUserInput("Enter iterations:");
    float realMin = getUserInput("Enter real min:");
    float realMax = getUserInput("Enter real max:");
    float complexMin = getUserInput("Enter complex min:");
    float complexMax = getUserInput("Enter complex max:");

    cout << endl;

    string resultFolder = generateName();
    fs::create_directory(resultFolder);

    // Determine the amount of chunks needed.
    int xChunks = round(width / chunkSize);
    int yChunks = round(height / chunkSize);
    int xChunkSize = width / xChunks;
    int yChunkSize = height / yChunks;
    int chunksGenerated = 0;

    if (xChunkSize != chunkSize || yChunkSize != chunkSize) {
        cout << "Corrected chunk size" << endl << "x:" << width / xChunks << endl << "y:" << height / yChunks << endl;
    }

    cout << "Chunks needed: " << xChunks * yChunks << endl << endl;

    for (int x = 0; x < xChunks; ++x) {
        for (int y = 0; y < yChunks; ++y) {
            chunksGenerated++;
            generateChunk(resultFolder, chunksGenerated, x, y, xChunkSize, width, yChunkSize, height, maxIterations, realMin, realMax, complexMin, complexMax);
        }
    }

    // Combine all of the chunks.
    cout << "Combing chunks..." << endl;
    string resultImage = resultFolder + "/result.png";
    combineChunks(resultFolder, resultImage, width, height);

    // Open the result.
    cout << "Finished combing chunks. Opening image..." << endl;
    system(("start " + resultImage).c_str());

    return 0;
}