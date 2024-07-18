#include "header.hpp"

#include <iostream>
#include <sstream>
#include <vector>

void Shortest_dist::remove_char(std::istringstream *command_edge,char *ch){
    // Remove unwanted Character from edge input
    *command_edge >> *ch;
}

void Shortest_dist::print_short(std::vector<int> ans){
    // To print the final answer
    for (int i = ans.size() - 1; i >= 0; --i) {
        if (i == 0){
            std::cout << ans[0] << std::endl;
            break;
        }
        std::cout << ans[i] << "-";
    }
}

void Shortest_dist::resize(){
    // To resize the adjacency list according to the input vertices
    ad_list.resize(numVertices);
    ad_list.clear();
}

int Shortest_dist::shortest(int start, int end){
    // To identify the Shortest distance between start and end node
    if (start == end){
        std::cout << start << std::endl;
        return 0;
    }
    std::vector<int> queue;
    std::vector<bool> visit(numVertices);
    std::vector<int> parent(numVertices);
    queue.emplace_back(start);
    visit[start-1] = 1;
    while(queue.empty() != true){
        int front = queue[0];
        queue.erase(queue.begin());
        for (auto it: ad_list[front-1]) {
            if(visit[it-1] != true){
                visit[it-1] = 1;
                parent[it-1] = front;
                queue.emplace_back(it);
            }
        }
    }
    std::vector<int> ans;
    int currentNode = end;
    ans.push_back(end);

    if (visit[end-1] != 0){
        while(currentNode != start) {
            currentNode = parent[currentNode-1];
            ans.push_back(currentNode);
        }
        print_short(ans);
    }
    else {
        return -1;
    }
    return 0; 
}

int Shortest_dist::edge_parse(std::string edge_input){
    // To extract the edges from the User input
    char ch;
    int left, right, count;
    count = 0;

    std::istringstream command_edge(edge_input);
    remove_char(&command_edge, &ch);                     // to remove {
    do{
        remove_char(&command_edge, &ch);                 // to remove <
        command_edge >> left;
        remove_char(&command_edge, &ch);                 // to remove ,
        command_edge >> right;

        if (left > numVertices || left < 1 || right > numVertices || right < 1){
            return -1;
        }
        ad_list[left-1].push_back(right);
        ad_list[right-1].push_back(left);

        remove_char(&command_edge, &ch);                 // to remove >
        remove_char(&command_edge, &ch);                 // check for , or }
        count = count + 1;

    }while (ch != '}');

    if (count < 1){
        return -1;
    }
    return 0;
}