#include <memory>
#include "minisat/core/Solver.h"

#include <iostream>

#include <string>
#include <cstring>
#include <sstream>
#include <list>
#include <bits/stdc++.h>
#include <vector>
#include <algorithm>
#include <iterator>

class Graph {
    private:
        void remove_char(std::istringstream *command_edge,char *ch){
            // Remove unwanted Character from edge input
            *command_edge >> *ch;
        }

    public:
        int numVertices = 0;                                            // Variable storing the number of Vertices

        std::vector<int> first, second;

        int edge_parse(std::string edge_input){
            // To extract the edges from the User input
            char ch;
            int left, right, count;
            count = 0;

            std::istringstream command_edge(edge_input);
            first.clear();
            second.clear();
            remove_char(&command_edge, &ch);                     // to remove {
            do{
                remove_char(&command_edge, &ch);                 // to remove <
                command_edge >> left;
                remove_char(&command_edge, &ch);                 // to remove ,
                command_edge >> right;

                if (left > numVertices || left < 1 || right > numVertices || right < 1){
                    return -1;
                }
                first.push_back(left-1);
                second.push_back(right-1);

                remove_char(&command_edge, &ch);                 // to remove >
                remove_char(&command_edge, &ch);                 // check for , or }
                count = count + 1;

            }while (ch != '}');

            if (count < 1){
                return -1;
            }
            return 0;
        }
};

std::vector<int> VC_answer(Graph G, int k){
    Minisat::Solver solver;
    std::vector <std::vector<Minisat::Lit>> variables_list(G.numVertices);

    // Creating literals
    for (int v = 0; v < G.numVertices; ++v){
        for (int n = 0; n < k; ++n){
            Minisat::Lit literal = Minisat::mkLit(solver.newVar());
            variables_list[v].push_back(literal);
        }
    }

    // Clause 1: At least one vertex is the ith vertex in the vertex cover: ?i ? [1,k], a clause (x1,i ?x2,i ?···?xn,i)
    Minisat::vec<Minisat::Lit> literals;
    for(int n = 0; n < k; ++n){
        for(int v = 0; v < G.numVertices; ++v){
            literals.push(variables_list[v][n]);
        }
        solver.addClause(literals);
        literals.clear();
    }

    // Clause 2: No one vertex can appear twice in a vertex cover: ?m ? [1,n],?p,q ? [1,k] with p < q,a clause (¬Xm,p ? ¬Xm,q)
    if(k>1){
        for(int v = 0; v < G.numVertices; ++v){
            for(int n=0;n<k-1;n++){
                for(int p=n+1;p<k;p++){
                    solver.addClause(~variables_list[v][n], ~variables_list[v][p]);
                }
            }
        }
    }

    // Clause 3: No more than one vertex appears in the mth position of the vertex cover : ?m ? [1,k],?p,q ? [1,n] with p < q, a clause (¬Xp,m ? ¬Xq,m)
    for(int n=0;n<k;n++){
        for(int v=0;v<G.numVertices-1;v++){
            for(int p=v+1;p<G.numVertices;p++){
                solver.addClause(~variables_list[v][n], ~variables_list[p][n]);
            }
        }
    }

    // Clause 4: Every edge is incident to at least one vertex in the vertex cover
    for (int i = 0; i < int(G.first.size()); ++i) 
	{
        for (int n = 0; n < k; ++n) 
		{
            literals.push(variables_list[G.first[i]][n]);
            literals.push(variables_list[G.second[i]][n]);
        }
        solver.addClause(literals);
        literals.clear();
    }

    bool sat = solver.solve();

    if (sat)                                                            // Obtaining values of variables
	{
        std::vector<int> result;
        for (int i = 0; i < G.numVertices; i++) 
		{
            for (int j = 0; j < k; ++j) 
			{
                if (Minisat::toInt(solver.modelValue(variables_list[i][j])) == 0) 
				{
                    result.push_back(i);
                }
            }
        }
        return result;
    } else {
        return {-1};
    }

}

void Vertex_Cover(Graph G){
    int initial = 1;
    int end = G.numVertices;
    int mid;
    bool verify;                                                        // to verify the result 
    std::vector<int> answer, end_answer, check;
    check = {-1};

    while (initial<=end){                                               // Performing binary search to obtain the k value
        mid = floor((initial+end)/2);
        answer = VC_answer(G, mid);
        verify = std::equal(answer.begin(), answer.end(), check.begin());

        if (verify == 0){
            end = mid - 1;
            end_answer.clear();
            end_answer = answer;
        }else{
            initial = mid + 1;
        }
    }

    std::sort(end_answer.begin(), end_answer.end());                    // Sorting the vertices

    for(int v: end_answer){                                             // Printing the final answer
        std::cout << v+1 << " ";
    }
    std::cout<<std::endl;
}


int main(int argc, char** argv){
    char previous = 'a';                                         // Assuming random one
    std::string input, edge_input;                               // Input obtained from User
    int ret;                                                     // Variable to check invalid inputs
    int new_input = 0;
    Graph G;                                                     // Class object

    while(std::cin.eof() != true){
        std::getline(std::cin, input);                           // Obtaining the input
        if(input.size()<1){
            new_input = 1;
        }

        std::istringstream command(input);
        char letter;
        command >> letter;                                       // Obtaining the First letter from the input obtained
        if (new_input != 1){
            switch (letter)
            {
            case 'V':
                G.numVertices = 0;
                command >> G.numVertices;                       // Obtaining the number of Vertices from the User
                if (G.numVertices < 2) {
                    std::cerr << "Error: Vertices should be more than 1"<<std::endl;
                    break;
                }
                previous = 'V';
                break;
            
            case 'E':
                if (previous == 'V') {
                    command >> edge_input;
                    ret = G.edge_parse(edge_input);             // Function to obtain all the Edges from the User
                    if (ret != 0){
                        std::cerr<<"Error: Incorrect Edges"<<std::endl;
                        continue;
                    }
                    Vertex_Cover(G);                            // Performing the Vertex Cover operation
                    previous = 'E';
                    break;
                }else if (previous == 'E') {
                    break;
                }
                std::cerr<<"Error: Vetex is not received"<<std::endl;
                break;
            
            default:
                std::cerr<<"Error: Wrong Command"<<std::endl;
                break;
            }
        }
        new_input = 0;
    }
    return 0;
}