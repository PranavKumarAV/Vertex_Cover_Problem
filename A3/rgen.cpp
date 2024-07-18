#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <random>
#include <unistd.h>
#include <cstdlib>
#include <set>
#include <utility>


class cmd_arg {                     // Class to store the command-line arguments
    public:
        int s,n,l,c;         

        cmd_arg(){
            s = 10;
            n = 5;
            l = 5;
            c = 20;
        }
};


bool are_collinear(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3) {
    // Check if three points are collinear by comparing slopes
    return std::abs((p1.second - p2.second) * (p2.first - p3.first) - (p2.second - p3.second) * (p1.first - p2.first)) < 1e-9;
}

bool doLineSegmentsOverlap(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3, std::pair<int, int> p4) {
    // Check if two line segments overlap
    if (are_collinear(p1, p2, p3) && are_collinear(p1, p2, p4)) {
        // The line segments are collinear
        if (std::max(p1.first, p2.first) >= std::min(p3.first, p4.first) &&
            std::max(p3.first, p4.first) >= std::min(p1.first, p2.first) &&
            std::max(p1.second, p2.second) >= std::min(p3.second, p4.second) &&
            std::max(p3.second, p4.second) >= std::min(p1.second, p2.second)) {
            // Line segments overlap in both x and y ranges
            return true;
        }
    }
    return false;
}

bool doLineSegmentsOverlap_withinLine(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3, std::pair<int, int> p4) {
    // Check if two line segments overlap
    if (are_collinear(p1, p2, p3) && are_collinear(p1, p2, p4)) {
        // The line segments are collinear
        if (std::max(p1.first, p2.first) >= std::min(p3.first, p4.first) &&
            std::max(p3.first, p4.first) >= std::min(p1.first, p2.first) &&
            std::max(p1.second, p2.second) >= std::min(p3.second, p4.second) &&
            std::max(p3.second, p4.second) >= std::min(p1.second, p2.second)) {
            // Line segments overlap in both x and y ranges
            return true;
        }
    }
    return false;
}

bool doLineSegments_intersect(std::pair<int, int> p1, std::pair<int, int> p2, std::pair<int, int> p3, std::pair<int, int> p4){
    double x1 = p1.first, y1 = p1.second;
    double x2 = p2.first, y2 = p2.second;
    double x3 = p3.first, y3 = p3.second;
    double x4 = p4.first, y4 = p4.second;

    double m1 = (x2 - x1 != 0) ? (y2 - y1) / (x2 - x1) : std::numeric_limits<double>::infinity();
    double m2 = (x4 - x3 != 0) ? (y4 - y3) / (x4 - x3) : std::numeric_limits<double>::infinity();
    
    // Check if the lines are parallel
    if (m1 == m2) {
        return false;
    }
    // Calculate the intersection point
    double x, y;
    if (m1 == std::numeric_limits<double>::infinity()) {
        x = x1;
        y = m2 * (x - x3) + y3;
    } else if (m2 == std::numeric_limits<double>::infinity()) {
        x = x3;
        y = m1 * (x - x1) + y1;
    } else {
        x = (y3 - y1 + m1 * x1 - m2 * x3) / (m1 - m2);
        y = m1 * (x - x1) + y1;
    }
    // Check if the intersection point is within the line segments
    if (
        std::min(x1, x2) - 0.01 <= x && x <= std::max(x1, x2) + 0.01 &&
        std::min(x3, x4) - 0.01 <= x && x <= std::max(x3, x4) + 0.01 &&
        std::min(y1, y2) - 0.01 <= y && y <= std::max(y1, y2) + 0.01 &&
        std::min(y3, y4) - 0.01 <= y && y <= std::max(y3, y4) + 0.01
    ) {
        return true;
    }
    return false;
}
int check_errors(std::vector<std::vector<std::pair<int, int>>> street_list, std::vector<std::pair<int, int>> one_street){
    // check for same variable present between streets
    for (size_t i = 0; i < one_street.size() - 1; ++i) {
        // Iterate through coordinatesList
        for (const auto& coordinates : street_list) {
            // Calculate slopes for adjacent elements of the matching coordinates
            for (size_t j = 0; j < coordinates.size() - 1; ++j) {
                if(doLineSegmentsOverlap(one_street[i], one_street[i + 1], coordinates[j], coordinates[j + 1])){
                    return -1;
                }
            }
        }
    }
    for (size_t i = 0; i < one_street.size() - 2; ++i) {
        // Iterate through coordinatesList
        for (size_t j = i+1; j < one_street.size() - 1; ++j) {
            if(doLineSegmentsOverlap_withinLine(one_street[i], one_street[i + 1], one_street[j], one_street[j + 1])){
                return -1;
            }
            if(j<one_street.size() - 2){
                if(doLineSegments_intersect(one_street[i], one_street[i + 1], one_street[j + 1], one_street[j + 2])){
                    return -1;
                }
            }
        }
    }
    return 0;
}

int parse_input(int argc, char** argv, cmd_arg *cmd_ip){
    int number;
    int check;
    opterr = 0;
    std::string value;

    // expected options are '-s', '-n', '-l', and '-c'
    while ((check = getopt(argc, argv, "s:n:l:c:")) != -1)
        switch (check) {
        case 's':                                       // Check for "s"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 2){
                cmd_ip->s = number;
            }
            else{
                return -1;
            }
            break;
        case 'n':                                       // Check for "n"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 1){
                cmd_ip->n = number;
            }
            else{
                return -1;
            }
            break;
        case 'l':                                       // Check for "l"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 5){
                cmd_ip->l = number;
            }
            else{
                return -1;
            }
            break;
        case 'c':                                       // Check for "c"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 1){
                cmd_ip->c = number;
            }
            else{
                return -1;
            }
            break;
        case '?':                                       // Check for incorrect inputs
            std::cerr << "Error: Unknown option" << std::endl;
            return 1;
        }
    return 0;
}

int random_gen(int lower_bound, int upper_bound){       // Function to generate random inputs
    std::uniform_int_distribution<int> distribution(lower_bound, upper_bound);
    std::random_device gen("/dev/urandom");

    return distribution(gen);
}

void rm_street(std::vector<std::vector<std::pair<int, int>>> street_list){      // Function to produce print statements to remove streets
    for (size_t st_cnt = 0; st_cnt < street_list.size(); ++st_cnt) {
        std::cout << "rm \"Street_"<<st_cnt + 1<<"\"" << std::endl;
    }
}

void add_street(std::vector<std::vector<std::pair<int, int>>> street_list){     // Function to produce print statements to add streets
    for (size_t st_cnt = 0; st_cnt < street_list.size(); ++st_cnt) {
        std::cout << "add \"Street_"<<st_cnt+1<<"\" ";
        for (const auto& coord : street_list[st_cnt]) {
            std::cout << "(" << coord.first << "," << coord.second << ") ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char** argv){
    int ret_val = 0;
    int count = 0;                                                              // To count the number of streets created
    int streets_rand,one_street_rand;                                           // Random s value between ranges
    int x,y;                                                                    // x,y coord
    int retry_count = 0;                                                        // Check if they are retrying within the limit

    std::vector<std::vector<std::pair<int, int>>> street_list;                  // Stores the entire graph
    std::vector<std::pair<int, int>> one_street;                                // Stores just one street
    cmd_arg cmd_ip;
    ret_val = parse_input(argc, argv, &cmd_ip);
    
    if (ret_val == -1){
        std::cerr<<"Error: Invalid input. Please try again." << std::endl;;
        return -1;
    }
    
    while(true){
        retry_count = 0;
        if(street_list.size()>0){
            rm_street(street_list);
            street_list.clear();
        }
        count = 0;
        streets_rand = random_gen(2,cmd_ip.s);
        while (count<streets_rand) {                                            //creating multiple streets
            one_street.clear();
            one_street_rand = random_gen(1,cmd_ip.n);
            int i = 0;
            while (i<one_street_rand+1){                                        //creating multiple line segments within a street
                x = random_gen(-cmd_ip.c,cmd_ip.c);
                y = random_gen(-cmd_ip.c,cmd_ip.c);
                if(i>0 && i<one_street_rand){
                    if(one_street[i-1].first != x || one_street[i-1].second != y){
                        one_street.push_back(std::make_pair(x, y));
                        i = i+1;
                    }
                }else if(i == one_street_rand){
                    if((one_street[i-1].first != x || one_street[i-1].second != y) && (one_street[0].first != x || one_street[0].second != y)){
                        one_street.push_back(std::make_pair(x, y));
                        i = i+1;
                    }
                }
                else{
                    one_street.push_back(std::make_pair(x, y));
                    i = i+1;
                }
            }
            ret_val = check_errors(street_list, one_street);                    // Function to check for incorrect street
            if(ret_val == 0){
                street_list.push_back(one_street);                              // If correct street, push to the graph
                count = count + 1;
            }
            else{
                retry_count = retry_count + 1;
                if(retry_count > 25){
                    std::cerr<<"Error: Failed to generate valid input for 25 simultaneous attempts" << std::endl;
                    exit(1);
                    return -1;
                }
            }
        }
        add_street(street_list);
        std::cout << "gg" << std::endl;
        sleep(random_gen(5,cmd_ip.l));                                          //  Sleep to wait for rest of the process to complete
    }
    return 0;
}