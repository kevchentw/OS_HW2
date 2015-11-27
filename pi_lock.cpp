#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>


using namespace std;

pthread_mutex_t p_mutex;


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

Counter x;

void* ThreadRunner(void *){
    for(int j=0; j<100000000; j++){
        pthread_mutex_lock(&p_mutex);
        x.Pick();
        pthread_mutex_unlock(&p_mutex);
    }
}

int main() {
    srand (time(NULL));
    pthread_t tid[3];
    pthread_mutex_init (&p_mutex, NULL);
    for (int i = 0; i < 3;i++) {
        pthread_create(&tid[i], NULL, &ThreadRunner, 0);
    }

    for (int i =  0; i < 3; i++){
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&p_mutex);
    x.Print();
}
