#include <iostream>
#include <pthread.h>
#define T 16 
void homemade_spin_lock(int *spinlock_addr) {
    asm(
    "spin_lock: \n\t"
            "xorl %%ecx, %%ecx \n\t"
            "incl %%ecx \n\t"
            "spin_lock_retry: \n\t"
            "xorl %%eax, %%eax \n\t"
            "lock; cmpxchgl %%ecx, (%0) \n\t"
            "jnz spin_lock_retry \n\t"
    : : "r" (spinlock_addr) : "ecx", "eax" );
}

void homemade_spin_unlock(int *spinlock_addr) {
    asm(
    "spin_unlock: \n\t"
            "movl $0, (%0) \n\t"
    : : "r" (spinlock_addr) : );
}


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

Counter x;

int s;

void* ThreadRunner(void *){
    for (int k = 0; k < 100000000; k++) {
        homemade_spin_lock(&s);
        x.Increment();
        homemade_spin_unlock(&s);
    }
}

int main() {
    pthread_t tid[T];
    s=0;
    for (int i = 0; i < T;i++) {
        pthread_create(&tid[i], NULL, ThreadRunner, 0);
    }

    for (int i =  0; i < T; i++){
        pthread_join(tid[i], NULL);
    }
    x.Print();
}
