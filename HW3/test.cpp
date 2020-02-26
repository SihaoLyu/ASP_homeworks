#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>

using namespace std;

int main(int argc, char** argv) {
    string a;
    ifstream in;
    in.open("./input.txt", ios::in);
    getline(in, a);
    
    cout << stoi(a.substr(a.find(" ")+1, a.size())) << endl;

    // char* rcd = new char[a.size()];
    // rcd[a.size()] = '\0';
    // a.copy(rcd, a.size());
    // char* temp = strtok(rcd, " ");
    // cout << temp << endl;
    // temp = strtok(NULL, " ");
    // cout << temp << endl;
    // delete rcd;

    // vector< vector<int> > a;
    // vector<int> b(1, 1);
    // a.push_back(b);
    // cout << a[0][0] << endl;

    string cur = "Transfer 147389 48583 64";
    char* temp = new char[cur.size()];
    temp[cur.size()] = '\0';
    cur.copy(temp, cur.size());
    cout << temp << endl;
    char* rcd = strtok(temp, " ");
    rcd = strtok(NULL, " "); // ignore "Transfer" keyword
    vector<int> task;
    while (rcd != NULL) {
        task.push_back(atoi(rcd));
        rcd = strtok(NULL, " "); 
    }
    cout << temp << endl;
    delete[] temp;

    return 0;
}