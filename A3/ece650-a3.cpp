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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <string.h>

class cmd_arg {                                         // Class to store the command-line arguments
    public:
        int s,n,l,c;         

        cmd_arg(){
            s = 10;
            n = 5;
            l = 5;
            c = 20;
        }
};

char* ToStr(int input) {                                // Function to convert input to string

    std::string ip = std::to_string(input);
    char* temp = new char[ip.length() + 1];
    strncpy(temp, ip.c_str(), ip.length());
    return temp;
    
}


int rgen_cpp(cmd_arg cmd_ip){                           // Function to carry out the rgen file
    char *s,*n,*l,*c;
    s = ToStr(cmd_ip.s);
    n = ToStr(cmd_ip.n);
    l = ToStr(cmd_ip.l);
    c = ToStr(cmd_ip.c);
    
    char* args[10];
    args[0] = (char *)"./rgen";
    args[1] = (char *)"-s";
    args[2] = s;
    args[3] = (char *)"-n";
    args[4] = n;
    args[5] = (char *)"-l";
    args[6] = l;
    args[7] = (char *)"-c";
    args[8] = c;
    args[9] = nullptr;
    return execv("./rgen", args);;
}

int close_all(pid_t pid_rgen, std::vector<pid_t> child){    // Function to kill all the pipes if any failure occurs
    int status;
    kill(pid_rgen, SIGTERM);
    waitpid(pid_rgen,&status,0);
    for (pid_t k : child) {
        int status1;
        kill(k, SIGTERM);
        waitpid(k, &status1, 0);
    }
    return 0;
}

int exec_a1(){                                      // Function to carry out the ece650-a1 file
    execlp("python3", "python3", "ece650-a1.py", (char*) NULL);
    return 0;
}


int exec_a2(){                                      // Function to carry out the ece650-a2 file
    char *args[2];
    args[0] = (char *)"./ece650-a2";
    args[1] = nullptr;
    execv("./ece650-a2",args);
    return 0;
}

int parse_input(int argc, char** argv, cmd_arg *cmd_ip){        // Function to check the errors for the command line arguments
    int number;
    int check;
    opterr = 0;
    std::string value;

    // expected options are '-s', '-n', '-l', and '-c'
    while ((check = getopt(argc, argv, "s:n:l:c:")) != -1)
        switch (check) {
        case 's':                                               // Check for "s"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 2){
                cmd_ip->s = number;
            }
            else{
                return -1;
            }
            break;
        case 'n':                                               // Check for "n"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 1){
                cmd_ip->n = number;
            }
            else{
                return -1;
            }
            break;
        case 'l':                                               // Check for "l"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 5){
                cmd_ip->l = number;
            }
            else{
                return -1;
            }
            break;
        case 'c':                                               // Check for "c"
            value = optarg;
            number = atoi(value.c_str());
            if(number >= 1){
                cmd_ip->c = number;
            }
            else{
                return -1;
            }
            break;
        case '?':                                               // Check for incorrect inputs
            return -1;
        }
    return 0;
}


int main(int argc, char** argv){
    int RtoA1[2], A1toA2[2];                                    // Initialization of two pipes
    int return_val;                                             // Assists to check for incorrect command line arguments
    std::vector<pid_t> child;                                   // To store all the child processes
    pid_t p_fork, pid_rgen;
    cmd_arg cmd_ip;
    return_val = parse_input(argc, argv, &cmd_ip);              // Obtaining the command line arguments

    
    if (return_val == -1){
        std::cerr<<"Error: Invalid input. Please try again."<<std::endl;
        exit(0);
    }
    pipe(RtoA1);                                                // Creation of pipe between Rgen and A1
    p_fork = fork();
    pid_rgen = p_fork;
    if (p_fork == 0){
        dup2(RtoA1[1],STDOUT_FILENO);
        close(RtoA1[0]);
        close(RtoA1[1]);
        return rgen_cpp(cmd_ip);
    }
    if(p_fork<0){
        std::cerr<<"Error: Problem with Fork"<<std::endl;
        close_all(pid_rgen,child);
        return -1;
    }
    pipe(A1toA2);                                               // Creation of pipe between A1 and A2
    p_fork = fork();
    child.push_back(p_fork);
    if(p_fork == 0){
        close(RtoA1[1]);
        dup2(RtoA1[0],STDIN_FILENO);
        close(A1toA2[0]);
        dup2(A1toA2[1],STDOUT_FILENO);
        return exec_a1();
    }
    if(p_fork<0){
        std::cerr<<"Error: Problem with Fork"<<std::endl;
        close_all(pid_rgen,child);
        return -1;
    }
    p_fork = fork();
    if(p_fork==0){
        close(A1toA2[1]); 
        dup2(A1toA2[0],STDIN_FILENO);       
        exec_a2();
    }
    if(p_fork<0){
        std::cerr<<"Error: Problem with Fork"<<std::endl;
        close_all(pid_rgen,child);
        return -1;
    }
    dup2(A1toA2[1],STDOUT_FILENO);
    int status_rgen;
    while ((waitpid(pid_rgen, &status_rgen, WNOHANG) != pid_rgen) && !std::cin.eof()) {
        std::string line;
        std::getline(std::cin, line);
        std::cout<<line<<"\n";
    }
    close(RtoA1[0]);                                        // Closing unwanted pipes
    close(RtoA1[1]);
    close(A1toA2[0]);
    close(A1toA2[1]);
    for (pid_t k : child) {                                 // Kill all the child process
        int status1;
        kill(k, SIGTERM);
        waitpid(k, &status1, 0);
        if (!WIFEXITED(status1) || WEXITSTATUS(status1) != 0) {
            close_all(pid_rgen, child);
            exit(EXIT_FAILURE); // Use an exit code to indicate an error
        }
    }
    return 0;
}