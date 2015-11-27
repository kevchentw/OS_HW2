#include <iostream>
#include <pthread.h>

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

Counter x;

pthread_spinlock_t lock;



void* ThreadRunner(void *){
    for (int k = 0; k < 100000000; k++) {
        pthread_spin_lock(&lock);
        x.Increment();
        pthread_spin_unlock(&lock);
    }
}

int main() {
    pthread_t tid[3];
    pthread_spin_init(&lock, 0);


    for (int i = 0; i < 3;i++) {
        pthread_create(&tid[i], NULL, ThreadRunner, 0);
    }

    for (int i =  0; i < 3; i++){
        pthread_join(tid[i], NULL);
    }
    pthread_spin_destroy(&lock);
    x.Print();
}