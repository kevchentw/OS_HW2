#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>


using namespace std;

class Counter{
    private:
        int circle;
        int total;
    public:
        Counter(){
            circle = 0;
            total = 0;
        }
        void Pick(){
            total++;
            double x = (double)rand()/RAND_MAX;
            double y = (double)rand()/RAND_MAX;
            if(x*x+y*y<=1){
                circle++;
            }
        }
        void Print(){
            cout << circle << "/" << total << endl;
            cout << "pi estimate = " << setprecision(6)  << (double)circle*4/total << endl;
        }
};

Counter x[3];

void* ThreadRunner(void *arg){
    int i = *(int *)arg;
    for(int j=0; j<100000000; j++){
        x[i].Pick();
    }
}

int main() {
    srand (time(NULL));
    pthread_t tid[3];
    int t_list[3];
    for (int i = 0; i < 3;i++) {
        t_list[i] = i;
        pthread_create(&tid[i], NULL, ThreadRunner, (void *)&t_list[i]);
    }

    for (int i =  0; i < 3; i++){
        pthread_join(tid[i], NULL);
    }
    x[0].Print();
    x[1].Print();
    x[2].Print();
}