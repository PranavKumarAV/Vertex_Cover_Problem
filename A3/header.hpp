#pragma once

#include <iostream>
#include <sstream>
#include <vector>

class Shortest_dist {
    private:
        std::vector<std::vector<int>> ad_list;

        void print_short(std::vector<int> ans);                         // To print the final answer

        void remove_char(std::istringstream *command_edge,char *ch);    // Remove unwanted Character from edge input

    public:
        int numVertices = 0;                                            // Variable storing the number of Vertices

        void resize();                                                  // To resize the adjacency list according to the input vertices

        int shortest(int start, int end);                               // To identify the Shortest distance between start and end node

        int edge_parse(std::string edge_input);                         // To extract the edges from the User input
};