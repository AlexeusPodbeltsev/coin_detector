#ifndef FINAL_TEST_COINDETECTOR_H
#define FINAL_TEST_COINDETECTOR_H

#include "opencv2/core.hpp"
#include <map>

class CoinDetector {
public:
    CoinDetector();
    explicit CoinDetector(const std::string &filepath, std::string filename);

    void findCoins(bool dirExist, int save);

    void printResult();

    std::map<std::string, double> test(const std::string& test_dir, const std::string& test_txt);


private:
    cv::Mat img;
    std::vector<cv::Vec3f> detectedCoins;
    std::string filename;
};

#endif //FINAL_TEST_COINDETECTOR_H