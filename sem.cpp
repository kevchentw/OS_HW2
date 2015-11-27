#include <iostream>
#include <pthread.h>
#include <semaphore.h>
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

Counter x_sem;

sem_t s_mutex;


void* ThreadRunner(void *){
    for (int k = 0; k < 100000000; k++) {
        sem_wait(&s_mutex);
        x_sem.Increment();
        sem_post(&s_mutex);
    }
}

int main() {
    pthread_t tid[T];
    sem_init(&s_mutex, 0, 1);

    for (int i = 0; i < T;i++) {
        pthread_create(&tid[i], NULL, ThreadRunner, 0);
    }

    for (int i =  0; i < T; i++){
        pthread_join(tid[i], NULL);
    }
    sem_destroy(&s_mutex);
    x_sem.Print();
}
