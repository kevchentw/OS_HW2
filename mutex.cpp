#include <iostream>
#include <pthread.h>
#define T 16 

using namespace std;

class Counter{
private:
    int value;
public:
    Counter(){
        value = 0;
    }
    void Increment(){
        value++;
    }
    void Print(){
        cout << value << endl;
    }
};

Counter x_mutex;

pthread_mutex_t p_mutex;



void* ThreadRunner(void *){
    for (int k = 0; k < 100000000 ; k++) {
        pthread_mutex_lock(&p_mutex);
        x_mutex.Increment();
        pthread_mutex_unlock(&p_mutex);
    }
}



int main() {
    pthread_t tid[T];
    pthread_mutex_init (&p_mutex, NULL);

    for (int i = 0; i < T;i++) {
        pthread_create(&tid[i], NULL, ThreadRunner, 0);
    }

    for (int i =  0; i < T; i++){
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&p_mutex);
    x_mutex.Print();
}
