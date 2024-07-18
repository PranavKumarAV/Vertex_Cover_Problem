#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include<stdexcept>
#include<cmath>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include "minisat/core/Solver.h"
#include <unordered_set>
#include <algorithm>
#include <list>
#include<errno.h>
#include<memory>

#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)

       #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

pthread_t getinput, cnfsat, approxvc1, approxvc2;    //Threads for CNFSAT, APPROXVC1 & APPROXVC2.
pthread_mutex_t mutex;
std::vector<int> cnfsatres;
long long cnfsat_thread_time, approxvc1_thread_time, approxvc2_thread_time; //Variables to store times for the threads.
int vertex = 0;
int numVertices;
std::vector<std::vector<int>> ad_list;
std::vector<int> edge_first, edge_second;


//static void pclock(char *msg, clockid_t cid)  //Function to compute running time of the algorithms.
//{
  //  struct timespec ts;

    //printf("%s", msg);
    //if (clock_gettime(cid, &ts) == -1)
      //  handle_error("clock_gettime");

    //intmax_t microseconds = (intmax_t) ts.tv_sec * 1000000 + ts.tv_nsec / 1000;

    //printf("%" PRIdMAX ".%06ld\n", microseconds / 1000000, microseconds % 1000000);
//}

std::vector<std::vector<int>> deepCopyAdjacencyList() 
{
    std::vector<std::vector<int>> copy;

    for (int i = 0; i < numVertices; ++i) {
        copy.push_back(ad_list[i]); 
    }

    return copy;
}

std::vector<int> deepCopyVector(std::vector<int>& original) 
{
    std::vector<int> copy;

    for (std::size_t i = 0; i < edge_first.size(); ++i) {
        copy.push_back(original[i]);
    }

    return copy;
}


std::vector<int> VC_answer(int k)
{ 
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());    //Function to compute CNF-SAT
    std::vector <std::vector<Minisat::Lit>> variables_list(numVertices);

    // Creating literals
    for (int v = 0; v < numVertices; ++v)
    {
        for (int n = 0; n < k; ++n)
        {
            Minisat::Lit literal = Minisat::mkLit(solver->newVar());
            variables_list[v].push_back(literal);
        }
    }
    

    // Clause 1: At least one vertex is the ith vertex in the vertex cover: ?i ? [1,k], a clause (x1,i ?x2,i ?   ?xn,i)
    Minisat::vec<Minisat::Lit> literals;
    for(int n = 0; n < k; ++n)
    {
        for(int v = 0; v < numVertices; ++v)
        {
            literals.push(variables_list[v][n]);
        }
        solver->addClause(literals);
        literals.clear();
    }
    
    bool sat1 = solver->solve();
    
    
    //Clause 4: Every edge is incident to at least one vertex in the vertex cover
    if (sat1)
    {
        for (int i = 0; i < int(edge_first.size()); ++i) 
	    {
            for (int n = 0; n < k; ++n) 
		    {
                literals.push(variables_list[edge_first[i]][n]);
                literals.push(variables_list[edge_second[i]][n]);
            }
            solver->addClause(literals);
            literals.clear();
        }
    }
    else
    {
        return {-1};
    }
  
    bool sat4 = solver->solve();
    
    
    // Clause 2: No one vertex can appear twice in a vertex cover: ?m ? [1,n],?p,q ? [1,k] with p < q,a clause ( Xm,p ?  Xm,q)
    if (sat4)
    {

        if(k>1)
        {
            for(int v = 0; v < numVertices; ++v)
            {
                if(ad_list[v].size()>0)
                {
                    for(int n=0;n<k-1;n++)
                    {
                        for(int p=n+1;p<k;p++)
                        {
                            if(ad_list[p].size()>0)
                            {
                                solver->addClause(~variables_list[v][n], ~variables_list[v][p]);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        return {-1};
    }
    
    bool sat2 = solver->solve();
    
    if (sat2)
    {

    // Clause 3: No more than one vertex appears in the mth position of the vertex cover : ?m ? [1,k],?p,q ? [1,n] with p < q, a clause ( Xp,m ?  Xq,m)
        for(int n=0;n<k;n++)
        {
            for(int v=0;v<numVertices-1;v++)
            {
                if(ad_list[v].size()>0)
                {
                    for(int p=v+1;p<numVertices;p++)
                    {
                        if(ad_list[p].size()>0)
                            {
            
                                solver->addClause(~variables_list[v][n], ~variables_list[p][n]);
                            }
                    }
                }
            }
        }
    }
    else
    {
        return {-1};
    }
    

    
    bool sat = solver->solve();
    if (sat)                                             //condition to check if all the clauses has got passed
	{
        std::vector<int> result;
        for (int i = 0; i < numVertices; i++) 
		{
            if(ad_list[i].size()>0)
            {
                for (int j = 0; j < k; ++j) 
			    {
                    if (Minisat::toInt(solver->modelValue(variables_list[i][j])) == 0) 
				    {
                        result.push_back(i);
                    }
                }
            }
        }
        return result;
    } 
    else 
    {
        return {-1};
    }
    

}



void VCusingCNFSAT()   //Function to implement binary search and perform CNF-SAT
{   
    int initial = 1;
    
    int end = numVertices;
    int mid;
    bool verify;                                                        // to verify the result .
    std::vector<int> answer, end_answer, check;
    check = {-1};
    

    while (initial<=end)
    {                                             // Performing binary search to obtain the k value
        mid = floor((initial+end)/2);
        answer = VC_answer(mid);
        verify = std::equal(answer.begin(), answer.end(), check.begin());

        if (verify == 0)
        {
            end = mid - 1;
            end_answer.clear();
            end_answer = answer;
        }
        else
        {
            initial = mid + 1;
        }
    }

    std::sort(end_answer.begin(), end_answer.end());                    // Sorting the vertices

    std::cout<<"CNF-SAT-VC: ";

    std::size_t count = 0;
    for(int v: end_answer)
    {                                             // Printing the final answer
        if(count == end_answer.size()-1)
        {
            std::cout<<v+1;
            break;
        }
        std::cout << v+1 << ",";
        count = count+1;
    }

    std::cout<<std::endl;
    

}

void VCusingapproxVC1()     //Function to compute Vertex cover using APPROXVC-1 algorithm
{        

    // Create a deep copy of the adjacency list
    std::vector<std::vector<int>> copiedAdjList = deepCopyAdjacencyList();



    std::vector<int> vertexCover;

    std::unordered_set<int> remainingEdges;
    for (int i = 0; i < numVertices; ++i) 
    {
        for (int j : copiedAdjList[i]) 
        {
            remainingEdges.insert(j);
        }
    }

    while (!remainingEdges.empty()) 
    {
        // Find a vertex with the highest degree
        int maxDegreeVertex = -1;
        int maxDegree = -1;

        for (int i = 0; i < numVertices; ++i) 
        {
            int degree = copiedAdjList[i].size();

            if (degree > maxDegree) 
            {
                maxDegree = degree;
                maxDegreeVertex = i;
            }
        }

        if (maxDegreeVertex != -1) 
        {
            // Add the selected vertex to the vertex cover
            vertexCover.push_back(maxDegreeVertex);

            // Remove all edges incident on the selected vertex
            for (int edge : copiedAdjList[maxDegreeVertex]) 
            {
                auto it = std::find(copiedAdjList[edge].begin(), copiedAdjList[edge].end(), maxDegreeVertex);
                copiedAdjList[edge].erase(it);
                if(copiedAdjList[edge].size() == 0)
                {
                    remainingEdges.erase(edge);
                }
            }
            for (int edge : copiedAdjList[maxDegreeVertex]) 
            {
                copiedAdjList[maxDegreeVertex].erase(copiedAdjList[maxDegreeVertex].begin());
            }

            remainingEdges.erase(maxDegreeVertex);
        }
    }

    std::sort(vertexCover.begin(), vertexCover.end());

    // Print the resulting vertex cover
    std::cout << "APPROX-VC-1: ";
    std::size_t v = 0;
    for (int vertex : vertexCover) 
    {
        if(v == vertexCover.size()-1)
        {
            std::cout<<vertex+1;
            break;
        }
        std::cout << vertex+1 << ",";
        v = v+1;
    }

    std::cout << std::endl;
}

void VCusingapproxVC2()    //Function to compute Vertex cover using APPROXVC-2 algorithm
{        

    // Create a deep copy 
    std::vector<int> copied_edge_first = deepCopyVector(edge_first);
    std::vector<int> copied_edge_second = deepCopyVector(edge_second);


    std::vector<int> vertexCover;

    while(!copied_edge_first.empty())
    {

        int u = copied_edge_first[0];
        int v = copied_edge_second[0];

        copied_edge_first.erase(copied_edge_first.begin());
        copied_edge_second.erase(copied_edge_second.begin());

        vertexCover.push_back(u);
        vertexCover.push_back(v);
        while(!copied_edge_first.empty()) 
        {
            auto it = std::find(copied_edge_first.begin(), copied_edge_first.end(), u);
            if (it != copied_edge_first.end())
            {
                int index = std::distance(copied_edge_first.begin(), it);
                copied_edge_first.erase(copied_edge_first.begin() + index);
                copied_edge_second.erase(copied_edge_second.begin() + index);
            }
            else
            {
                break;
            }
        }
        while(!copied_edge_first.empty()) 
        {
            auto it = std::find(copied_edge_first.begin(), copied_edge_first.end(), v);
            if (it != copied_edge_first.end())
            {
                int index = std::distance(copied_edge_first.begin(), it);
                copied_edge_first.erase(copied_edge_first.begin() + index);
                copied_edge_second.erase(copied_edge_second.begin() + index);
            }
            else
            {
                break;
            }
        }
        while(!copied_edge_first.empty()) 
        {
            auto it = std::find(copied_edge_second.begin(), copied_edge_second.end(), u);
            if (it != copied_edge_second.end())
            {
                int index = std::distance(copied_edge_second.begin(), it);
                copied_edge_first.erase(copied_edge_first.begin() + index);
                copied_edge_second.erase(copied_edge_second.begin() + index);
            }
            else
            {
                break;
            }
        }
        while(!copied_edge_first.empty()) 
        {
            auto it = std::find(copied_edge_second.begin(), copied_edge_second.end(), v);
            if (it != copied_edge_second.end())
            {
                int index = std::distance(copied_edge_second.begin(), it);
                copied_edge_first.erase(copied_edge_first.begin() + index);
                copied_edge_second.erase(copied_edge_second.begin() + index);
            }
            else
            {
                break;
            }
        }
    }
    std::sort(vertexCover.begin(), vertexCover.end());
    // Print the resulting vertex cover
    std::cout << "APPROX-VC-2: ";
    std::size_t v = 0;
    for (int vertex : vertexCover) 
    {
        if(v == vertexCover.size()-1)
        {
            std::cout<<vertex+1;
            break;
        }
        std::cout << vertex+1 << ",";
        v = v+1;
    }

    std::cout << std::endl;

  
}


void *VCusingCNFSAT_thread(void *arg)    //Thread function for CNF-SAT
{  
  clockid_t cid;
  int s;
  s = pthread_getcpuclockid(pthread_self(), &cid);
  pthread_mutex_lock(&mutex);
  VCusingCNFSAT();
  pthread_mutex_unlock(&mutex);
  if (s != 0)
    handle_error_en(s, "pthread_getcpuclockid");
  //pclock("CNFSAT-time: 1 ", cid);
  return 0;


}

void *VCusingapproxVC1_thread(void *arg)   //Thread function for APPROX-VC-1
{   
  clockid_t cid;
  int s;
  s = pthread_getcpuclockid(pthread_self(), &cid);
  pthread_mutex_lock(&mutex);
  VCusingapproxVC1();
  pthread_mutex_unlock(&mutex);
  if (s != 0)
    handle_error_en(s, "pthread_getcpuclockid");
  //pclock("ApproxVC1-time: 1 ", cid);
  return 0;


}

void *VCusingapproxVC2_thread(void *arg)   //Thread function for APPROX-VC-2
{   
  clockid_t cid;
  int s;
  s = pthread_getcpuclockid(pthread_self(), &cid);
  pthread_mutex_lock(&mutex);
  VCusingapproxVC2();
  pthread_mutex_unlock(&mutex);
  if (s != 0)
    handle_error_en(s, "pthread_getcpuclockid");
  //pclock("ApproxVC2-time: 1 ", cid);
  return 0;


}


void remove_char(std::istringstream *command_edge,char *ch)   //Parsing of edges
{  
    *command_edge >> *ch;
}

int edge_parse(std::string edge_input)
{
    char ch;
    int left, right;

    std::istringstream command_edge(edge_input);
    remove_char(&command_edge, &ch);                     // to remove {
    do
    {
        remove_char(&command_edge, &ch);                 // to remove <
        command_edge >> left;
        remove_char(&command_edge, &ch);                 // to remove ,
        command_edge >> right;

        if (left > numVertices || left < 1 || right > numVertices || right < 1)
        {
            return -1;
        }
        ad_list[left-1].push_back(right-1);
        ad_list[right-1].push_back(left-1);

        edge_first.push_back(left-1);
        edge_second.push_back(right-1);

        remove_char(&command_edge, &ch);                 // to remove >
        remove_char(&command_edge, &ch);                 // check for , or }

    }while (ch != '}');

    return 0;
}


void *inputvertexedge(void *arg)   //Thread function to get input from user.
{
    bool line_empty = false;
    char previous = 'a';        //assuming random one
    std::string input, edge_input;

    std::getline(std::cin, input);
    while(std::cin.eof() != true){
        std::istringstream command(input);
        char letter;
        command >> letter;
        if (line_empty != true)
        {
            switch (letter)
            {
            case 'V':
                command >> numVertices;
                if (numVertices < 2) 
                {
                    std::cerr<<"Error: Vertex is less than 2"<<std::endl; //To check for validation if vertex entered is less than 2
                    break;
                }
                previous = 'V';
                break;
            
            case 'E':
                if (previous == 'V') 
                {
                    command >> edge_input;
                    ad_list.resize(numVertices);
                    ad_list.clear();
                    edge_first.clear();
                    edge_second.clear();
                    int ret = edge_parse(edge_input);

                    if (ret != 0)
                    {
                        std::cerr<<"Error: Edge is greater than vertex"<<std::endl;   //To check if given edge value is greater than the given vertex
                        break;
                    }
                    
                    if(numVertices > 15){
                      std::cout<<"CNF-SAT-VC: timeout"<<std::endl;
                    }
                    else{
                    
                    if(pthread_create(&cnfsat, NULL, &VCusingCNFSAT_thread, NULL) != 0)   //Thread creation for CNF-SAT
                    {  
                        std::cerr<<"Error: Thread for CNF-SAT has been failed"<<std::endl;
                        break;

                    }
                    }

                    if(pthread_create(&approxvc1, NULL, &VCusingapproxVC1_thread, NULL) != 0)   //Thread creation for APPROX-VC-1
                    {  
                        std::cerr<<"Error: Thread for Approx-VC1 has been failed"<<std::endl;
                        break;

                    }

                    if(pthread_create(&approxvc2, NULL, &VCusingapproxVC2_thread, NULL) != 0)   //Thread creation for APPROX-VC-2
                    {  
                        std::cerr<<"Error: Thread for Approx-VC2 has been failed"<<std::endl;
                        break;

                    }
                    
                    pthread_join(cnfsat, NULL);
                    pthread_join(approxvc1, NULL);
                    pthread_join(approxvc2, NULL);
                    previous = 'E';
                    break;
                }
                //std::cerr<<"Error: Vertex is not received"<<std::endl;
                break;            
            default:
                if (input.size() >= 1) 
                {
                    std::cerr<<"Error: Wrong Command"<<std::endl;
                }
                break;
            }
        }


        std::getline(std::cin, input);
    }
    return 0;
    
}



int main()
{
    pthread_mutex_init(&mutex, NULL);
    if(pthread_create(&getinput, NULL, &inputvertexedge, NULL) != 0) 
    {
        std::cerr<<"Error: Thread for input has been failed"<<std::endl;   //To check if given edge value is greater than the given vertex

    }

    pthread_join(getinput, NULL);
    exit(EXIT_SUCCESS);         /* Terminates threads */

}