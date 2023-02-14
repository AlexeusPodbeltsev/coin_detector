#include "iostream"
#include "CoinDetector.h"
#include <fstream>
#include <regex>
#include "map"


using namespace std;
using namespace cv;

bool check_output_dir();

string extract_filename(const string &filepath);


int main(int argc, char *argv[]) {
    cout << "Welcome!\nPlease choose operation:\ndetect <filepath> | test <path_to_test_dir> <path_to_test_txt>" << endl;
    string filepath;

    if(argc == 3){
        if (std::strcmp(argv[1], "detect") == 0){
            filepath = argv[2];
            CoinDetector cd(filepath, extract_filename(filepath));
            cd.findCoins(check_output_dir(), 1);
            cd.printResult();
        }else{
            cout << "Please check your input with pattern \"detect <filepath> | test <path_to_test_dir> <path_to_test_txt>\"" << endl;
        }
    }
    if(argc == 4){
        if (std::strcmp(argv[1], "test") == 0){
            string test_dir = argv[2];
            string test_txt = argv[3];
            CoinDetector cd{};
            std::map<std::string, double> res = cd.test(test_dir, test_txt);
            for (auto & entry : res) {
                cout << entry.first << ": " << entry.second << endl;
            }
        }else{
            cout << "Please check your input with pattern \"detect <filepath> | test <path_to_test_dir> <path_to_test_txt>\"" << endl;
        }
    }

    return 0;
}

bool check_output_dir() {
    const filesystem::path output_dir{"../output"};
    return filesystem::exists(output_dir);
}

string extract_filename(const string &filepath) {
    std::regex rgx("\\w+\\.(png|jpeg|jpg)$");
    std::smatch match;
    std::regex_search(filepath.begin(), filepath.end(), match, rgx);
    string filename = match[0];
    return filename;
}