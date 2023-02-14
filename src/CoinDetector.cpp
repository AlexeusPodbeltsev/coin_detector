#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "numeric"
#include <fstream>
#include <regex>

#include "CoinDetector.h"

using namespace cv;
using namespace std;
CoinDetector::CoinDetector() {}

CoinDetector::CoinDetector(const std::string &filepath, std::string filename)
        : filename(filename) {
    Mat input;
    try {
        input = imread(samples::findFile(filepath));
        this->img = input;
    } catch (const cv::Exception &e) {
        cerr << "Image not found" << endl;
        exit(1);
    }
}

void CoinDetector::findCoins(bool dirExist, int save) {
    detectedCoins.clear();
    if (!dirExist && save == 1) {
        system("exec mkdir ../output");
    }
    Mat imgGray, imgBlur, thresh, edges, kernel, closing, imgPrint;
    vector<Vec3f> circles;

    imgPrint = img.clone();

    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(imgGray, imgBlur, Size(15, 15), 1);
    double otsuTresh = threshold(imgBlur, thresh, 0, 255, THRESH_BINARY | THRESH_OTSU);

    if (otsuTresh > 150) {
        kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    } else {
        kernel = getStructuringElement(MORPH_RECT, Size(6, 6));

    }

    double highThreshVal = otsuTresh,
            lowerThreshVal = otsuTresh / 2;

    morphologyEx(imgBlur, closing, MORPH_ERODE, kernel, Point(-1, -1));
    morphologyEx(closing, closing, MORPH_DILATE, kernel, Point(-1, -1), 5);

    Canny(closing, edges, lowerThreshVal + 20, highThreshVal, 3);


    int minDist = min(img.size().width, img.size().height) / 8;
    int maxRadius = min(img.size().width, img.size().height) / 4;
    int minRadius = cvRound(minDist * 0.5);

    HoughCircles(edges, circles, HOUGH_GRADIENT, 1, minDist, highThreshVal * 2, 20, minRadius, maxRadius);
    if (circles.empty()) {
        cout << "\tNo circles found!" << endl;
        exit(1);
    }


    double avgRadius = std::accumulate(circles.begin(), circles.end(), 0.0,
                                       [](double accum, Vec3f circle) { return accum + circle[2]; }) /
                       circles.size();

    for (auto &circleLoop: circles) {
        Point center(cvRound(circleLoop[0]), cvRound(circleLoop[1]));
        int radius = cvRound(circleLoop[2]);

        if (radius <= avgRadius + 50 && radius > avgRadius - 50) {
            circle(imgPrint, center, 3, Scalar(0, 255, 0), 3, 8, 0);
            circle(imgPrint, center, radius, Scalar(0, 0, 255), 3, 8, 0);
            detectedCoins.push_back(circleLoop);
        }
    }

    if (save == 1) {
        imwrite("../output/output_" + filename, imgPrint);
        imshow("Detected Coins", imgPrint);
        waitKey(0);
    }
}

void CoinDetector::printResult() {
    for (auto &circle: detectedCoins) {
        Point center(cvRound(circle[0]), cvRound(circle[1]));
        int radius = cvRound(circle[2]);
        cout << "x=" << center.x << " y=" << center.y << " radius=" << radius << endl;
    }
    cout << "Number of coins: " << detectedCoins.size() << endl;
}

std::map<std::string, double> CoinDetector::test(const std::string &test_dir, const std::string &test_txt) {
    int t_p = 0; // True Positive
    int f_p = 0; // False Positive
    int f_n = 0; // False Negative

    ifstream in(test_txt);
    std::string line;
    std::vector<Vec3i> expectedCircles;
    while (true) {
        if (in.is_open()) {
            getline(in, line); //read name of image
            if (std::equal(line.begin(), line.end(), "end")) { // check whether the end of the txt file is reached
                break;
            }
            this->img = imread(test_dir + line); // read input image
            findCoins(true, 0); //find coins in image, to disable writing mode set flags dirExist=true and save=0
            Vec3i circle;
            while (getline(in, line)) {
                if (line.empty() ||
                    std::equal(line.begin(), line.end(), "end")) { // check whether the end of image data is reached
                    break;
                }
                std::regex rgx(R"(x=(\d+) y=(\d+) radius=(\d+))"); // extract num values from line
                std::smatch match;
                std::regex_search(line, match, rgx);
                circle = {stoi(match[1]), stoi(match[2]), stoi(match[3])}; //collect data about correct circles in image
                expectedCircles.push_back(circle);
            }

            bool found = false;
            for (auto itrExpected = expectedCircles.begin(); itrExpected != expectedCircles.end();) {
                Vec3i expectedCircle = *itrExpected;
                Point expectedCenter(expectedCircle[0], expectedCircle[1]);
                int expectedRadius = expectedCircle[2];
                for (auto itrDetected = detectedCoins.begin(); itrDetected != detectedCoins.end(); itrDetected++) {
                    Vec3f predictedCircle = *itrDetected;
                    Point predictedCenter(cvRound(predictedCircle[0]), cvRound(predictedCircle[1]));
                    int predictedRadius = cvRound(predictedCircle[2]);

                    if ((expectedCenter.x + 5 > predictedCenter.x && predictedCenter.x >= expectedCenter.x - 5) &&
                        (expectedCenter.y + 5 > predictedCenter.y && predictedCenter.y >= expectedCenter.y - 5) &&
                        (expectedRadius + 5 > predictedRadius && predictedRadius >= expectedRadius - 5)) {
                        t_p++;
                        detectedCoins.erase(itrDetected);
                        expectedCircles.erase(itrExpected);
                        found = true;
                        break;
                    } else {
                        found = false;
                    }
                }
                if (!found) {
                    itrExpected++;
                }
            }
            f_p += detectedCoins.size();
            f_n += expectedCircles.size();
            detectedCoins.clear();
            expectedCircles.clear();
        }
    }
    in.close();

    double precision = 1.0 * t_p / (t_p + f_p);
    double recall = 1.0 * t_p / (t_p + f_n);
    double f1 = 2 * ((precision * recall) / (precision + recall));


    std::map<std::string, double> result = {{"Precision", precision},
                                            {"Recall",    recall},
                                            {"F1",        f1}};;
    return result;
}


