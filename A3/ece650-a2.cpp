// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include "header.hpp"
#include <signal.h>
#include <unistd.h>

int main(int argc, char** argv){
    int start;                                                   // Start vertice of shortest distance
    int end;                                                     // Last vertice of shortest distance
    char previous = 'a';                                         // Assuming random one
    std::string input, edge_input;                               // Input obtained from User
    int ret;                                                     // Variable to check invalid inputs
    int ret2;                                                    // For shortest distance checking
    int new_input = 0;
    Shortest_dist S_dist;                                        // Class object

    while(std::cin.eof() != true){
        std::getline(std::cin, input);                           // Obtaining the input
        if(input.size()<=1){
            new_input = 1;
        }

        std::istringstream command(input);
        char letter;
        command >> letter;                                       // Obtaining the First letter from the input obtained
        if (new_input != 1){
            switch (letter)
            {
            case 'V':
                std::cout<<input<<std::endl;
                S_dist.numVertices = 0;
                command >> S_dist.numVertices;                   // Obtaining the number of Vertices from the User
                if (S_dist.numVertices < 2) {
                    std::cerr << "Error: Vertices should be more than 1"<<std::endl;
                    break;
                }
                previous = 'V';
                break;
            
            case 'E':
                std::cout<<input<<std::endl;
                if (previous == 'V') {
                    command >> edge_input;
                    S_dist.resize();                             // Function to clear and resize the adjacency list
                    ret = S_dist.edge_parse(edge_input);         // Function to obtain all the Edges from the User

                    if (ret != 0){
                        std::cerr<<"Error: Incorrect Edges"<<std::endl;
                    }
                    previous = 'E';
                    break;
                }else if (previous == 'E') {
                    break;
                }
                std::cerr<<"Error: Vetex is not received"<<std::endl;
                break;
            
            case 's':
                if (previous == 'E' and ret != 0){
                    std::cerr<<"Error: Given edge is incorrect"<<std::endl;
                    break;
                }
                if (previous == 'E') {
                    command >> start;
                    command >> end;
                    if (start > S_dist.numVertices || start < 1 || end > S_dist.numVertices || end < 1){
                        std::cerr<<"Error: Incorrect Vertices"<<std::endl;
                        break;
                    }
                    ret2 = S_dist.shortest(start, end);
                    if (ret2 != 0){
                        std::cerr<<"Error: Path doesn't exist"<<std::endl;
                    }
                    break;
                }
                std::cerr<<"Error: Vetex or Edge not received"<<std::endl;
                break;

            default:
                if (input.size() >= 1) {
                    std::cerr<<"Error: Wrong Command"<<std::endl;
                }
                break;
            }
        }
        new_input = 0;
    }
    return 0;
}