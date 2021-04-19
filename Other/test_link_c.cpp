#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
using namespace std; 

int main(int argc, char *argv[]) {
    
    string file_name = "test_link.txt";
    int num_iter = 1000;
    time_t start = time(NULL);
    for (int i = 0; i < num_iter; i++) {
        ifstream my_file(file_name);        
        string command = "python3 testLink.py ";
        command.append(std::to_string(i));
        std::system(command.c_str());
        
        string line;
        if (!my_file.is_open()) {
            std::cout << "ERROR: FAILED TO OPEN '" << file_name << "'\n";
            return -1;
        }
        getline(my_file, line);
        int sts = atoi(line.c_str());

        if (i % 100 == 0) {
            std::cout << sts << "\n";
        }  
        my_file.close();
    }
    std::cout << num_iter << " iterations takes " << time(NULL) - start << " seconds\n";
}