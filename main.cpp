#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <fstream>
#include <dirent.h>
#include <regex>
#include <string> 
#include <sstream> 


#include "helper/poly_helper.hpp"


std::vector<std::string> open(std::string path = ".") {

    DIR*    dir;
    dirent* pdir;
    std::vector<std::string> files;

    dir = opendir(path.c_str());

    while (pdir = readdir(dir)) {
        files.push_back(pdir->d_name);
    }
    
    return files;
}


long extract_number(std::string str) {
    std::string temp;
    long number = 0;
    
    for (unsigned int i=0; i < str.size(); i++)
    {
        if (isdigit(str[i]))
        {
            for (unsigned int a=i; a<str.size(); a++)
            {
                temp += str[a];               
            }
          break;
        }    
    }

    std::istringstream stream(temp);
    stream >> number;
    return number;
}

long skipintro_and_get_area(std::ifstream &stream) {
    long area;

    std::regex area_regex("\b([0-9]+)\b");
    std::smatch base_match;
    std::string _s;

    std::getline(stream, _s);
    std::getline(stream, _s);

    return extract_number(_s);
}

void readfile(std::ifstream &stream, std::string filepath) {
    stream = std::ifstream(filepath);
}

int main()
{

    CGAL::set_pretty_mode(std::cout);

    std::ifstream stream;
    std::string filepath = "/Users/ehudadler/Desktop/Math Research/Polygon_Optomality/challenge_instances";
    std::string filename = "/Users/ehudadler/Desktop/Math Research/Polygon_Optomality/challenge_instances/data/images/euro-night-0000010.instance";

    DIR *dpdf;
    struct dirent *epdf;

    dpdf = opendir(filepath.c_str());
    if (dpdf != NULL){
        while (epdf = readdir(dpdf)){

            // std::string filename = "/Users/ehudadler/Downloads/challenge_instances/data/images";

            // std::string f(epdf->d_name);
            // filename.append("/");
            // filename.append(f);

            readfile(stream, filename);
            long area = skipintro_and_get_area(stream);

            Points point_set = poly::make_pointset(stream);

            std::cout << filename << "\n";

            Polygon_2 polygon_ch_best = poly::fekete_half_best_ch(filename, point_set);
            // std::cout << polygon_ch_best.area() << "\n"<< polygon_ch_best.area() / area << "\n\n";
        }
    }
    closedir(dpdf);

    return 0;
}


