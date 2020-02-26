#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <string>
#include <string.h>
#include <fstream>


using namespace std;


pthread_cond_t idle_thread_pool = PTHREAD_COND_INITIALIZER;
pthread_cond_t wait_thread_pool = PTHREAD_COND_INITIALIZER;
pthread_mutex_t ck_lk = PTHREAD_MUTEX_INITIALIZER;  // check (account_avail) lock
pthread_mutex_t op_lk = PTHREAD_MUTEX_INITIALIZER;  // task operation lock
vector<bool> account_avail;
vector<int> accounts;
vector< vector<int> > task_pool;


void* transfer(void* arg) {
    while (1) {
        // when no task or end
        pthread_mutex_lock(&op_lk);
        if (task_pool.size() == 0) { 
            pthread_cond_wait(&idle_thread_pool, &op_lk); 
            pthread_mutex_unlock(&op_lk);
            continue;
        }
        if (task_pool[0][0] == -1) { 
            pthread_mutex_unlock(&op_lk);
            pthread_exit(NULL); 
        }

        int from = task_pool[0][0] - 1;
        int to = task_pool[0][1] - 1;
        int amount = task_pool[0][2];
        task_pool.erase(task_pool.begin());
        pthread_mutex_unlock(&op_lk);

        // check if both accounts avail
        bool if_ready_trans = false;
        while (! if_ready_trans) {
            pthread_mutex_lock(&ck_lk);
            if (account_avail[from] && account_avail[to]) {
                account_avail[from] = false;
                account_avail[to] = false;
                if_ready_trans = true;
                pthread_mutex_unlock(&ck_lk);
            }
            else {
                pthread_cond_wait(&wait_thread_pool, &ck_lk);
                pthread_mutex_unlock(&ck_lk);
            }
        }

        // transferring
        if (accounts[from]-amount < 0) {
            cout << "account " << from+1 << " debit is " << accounts[from] << " cannot credit " << amount << endl;
        }
        else {
            accounts[from] -= amount;
            accounts[to] += amount;
        }

        // finishing transfer
        pthread_mutex_lock(&ck_lk);
        account_avail[from] = true;
        account_avail[to] = true;
        pthread_cond_broadcast(&wait_thread_pool);
        pthread_mutex_unlock(&ck_lk);
    }
}


int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Only takes 3 arguments!\n";
        return -1;
    }
    char* file_name = argv[1];
    ifstream file(file_name);
    if (! file.is_open()) {
        cout << "FILE PATH WRONG!\n";
        return -1;
    }

    // init accounts space
    string line;
    for(int i=0;;i++) {
        getline(file, line);
        if (line[0] == 'T') {
            accounts.resize(i);
            account_avail.resize(i);
            break;
        }
    }
    file.seekg(0, ios::beg);
    
    // init accounts info
    vector<int> output_order;
    string cur;
    getline(file, cur);
    while (cur[0] != 'T') {
        int id = stoi(cur.substr(0, cur.find(' '))) - 1;
        int debit = stoi(cur.substr(cur.find(' ')+1, cur.size()));
        accounts[id] = debit;
        account_avail[id] = true;
        output_order.push_back(id+1);
        getline(file, cur);
    }
    
    // assign threads
    int thread_nums = atoi(argv[2]);
    pthread_t thread_pool[thread_nums];
    for (int i=0; i<thread_nums; i++) {
        pthread_create(&thread_pool[i], NULL, transfer, (void*)i);
    }

    //  assign task
    do{
       pthread_mutex_lock(&op_lk);

       char* temp = new char[cur.size()+1];
       temp[cur.size()] = '\0';
       cur.copy(temp, cur.size());
       char* rcd = strtok(temp, " ");
       rcd = strtok(NULL, " "); // ignore "Transfer" keyword
       vector<int> task;
       while (rcd != NULL) {
           task.push_back(atoi(rcd));
           rcd = strtok(NULL, " "); 
       }
       task_pool.push_back(task);
       delete[] temp;
       getline(file, cur);
       pthread_cond_broadcast(&idle_thread_pool);

       pthread_mutex_unlock(&op_lk);
    }   while (! file.eof());
    file.close();

    // EOF
    pthread_mutex_lock(&op_lk);
    vector<int> EOF_task(1, -1);
    task_pool.push_back(EOF_task);
    pthread_cond_broadcast(&idle_thread_pool);
    pthread_mutex_unlock(&op_lk);

    for (int i=0; i<thread_nums; i++) {
        pthread_join(thread_pool[i], NULL);
    }

    // output all accounts amount
    cout << "All accounts amount:\n";
    for (int i=0; i<output_order.size(); i++) { 
        cout << output_order[i] << " " << accounts[output_order[i]-1] << endl;
    }

    return 0;
}