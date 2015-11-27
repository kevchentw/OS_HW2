#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;
#define THREAD_NUM 4

pthread_mutex_t counter_mutex[4], left_mutex[4], road_mutex[4], info, deadlock_mutex, first_lock_mutex;
int left_car[4], counter[4], go_car[4];

int get_counter(int car_id) {
    int x;
    pthread_mutex_lock(&counter_mutex[car_id]);
    x = counter[car_id];
    pthread_mutex_unlock(&counter_mutex[car_id]);
    return x;
}

void increase_counter(int car_id) {
    pthread_mutex_lock(&counter_mutex[car_id]);
    counter[car_id]++;
    pthread_mutex_unlock(&counter_mutex[car_id]);
}

int get_left(int car_id) {
    int x;
    pthread_mutex_lock(&counter_mutex[car_id]);
    x = left_car[car_id];
    pthread_mutex_unlock(&counter_mutex[car_id]);
    return x;
}

void decrease_left(int car_id) {
    pthread_mutex_lock(&left_mutex[car_id]);
    left_car[car_id]--;
    pthread_mutex_unlock(&left_mutex[car_id]);
}

bool first_lock[4];

bool is_deadlock(){
    pthread_mutex_lock(&first_lock_mutex);
    if(first_lock[0] && first_lock[1] && first_lock[2] && first_lock[3]){
        pthread_mutex_unlock(&first_lock_mutex);
        return true;
    }
    pthread_mutex_unlock(&first_lock_mutex);

    return false;
}

bool can_cross(int car_id) {
    for (int i = 0; i < 4; ++i) {
        if (get_left(i) && get_counter(car_id) > get_counter(i)) {
            return false;
        }
    }
    return true;
}

class cross_data {
public:
    cross_data() {
        mutex_1 = 0;
        mutex_2 = 0;
        direction = 'x';
        car_id = 0;
        m1 = "x";
        m2 = "x";
    }


    void update(int a, int b, char c, int d, string e, string f) {
        mutex_1 = a;
        mutex_2 = b;
        direction = c;
        car_id = d;
        m1 = e;
        m2 = f;
    }

    int mutex_1;
    int mutex_2;
    char direction;
    int car_id;
    string m1;
    string m2;
};
cross_data cross_d[4];

void cross(cross_data x) {
    pthread_mutex_lock(&info);
    cout << "left:" << left_car[0] << " " << left_car[1] << " " <<left_car[2] << " " <<left_car[3] << endl;
    pthread_mutex_unlock(&info);
    while (1) {
        pthread_mutex_lock(&info);
        cout << "counter:" << counter[0] << " " << counter[1] << " " << counter[2] << " " <<counter[3] << endl;
        pthread_mutex_unlock(&info);
        while (!can_cross(x.car_id));
        if (pthread_mutex_trylock(&road_mutex[x.mutex_1]) == 0) {
            pthread_mutex_lock(&first_lock_mutex);
            first_lock[x.car_id] = true;
            pthread_mutex_unlock(&first_lock_mutex);
            pthread_mutex_lock(&info);
            cout << get_counter(x.car_id) << ": " << x.direction << " " << go_car[x.car_id] << " acquire " << x.m1 << endl;
            pthread_mutex_unlock(&info);
            increase_counter(x.car_id);
            break;
        }
        else{
            pthread_mutex_lock(&info);
            cout << get_counter(x.car_id) << ": " << x.direction << " " << go_car[x.car_id] << " acquire failed " << x.m1 << endl;
            pthread_mutex_unlock(&info);
            increase_counter(x.car_id);
        }
    }

    while (1) {
        pthread_mutex_lock(&info);
        cout << "counter:" << counter[0] << " " << counter[1] << " " <<counter[2] << " " <<counter[3] << endl;
        pthread_mutex_unlock(&info);

        while (!can_cross(x.car_id));
        if (pthread_mutex_trylock(&road_mutex[x.mutex_2]) == 0) {
            pthread_mutex_lock(&info);
            cout << get_counter(x.car_id) << ": " << x.direction << " " << go_car[x.car_id] << " acquire " << x.m2 << endl;
            pthread_mutex_unlock(&info);
            increase_counter(x.car_id);
            pthread_mutex_lock(&first_lock_mutex);
            first_lock[x.car_id] = false;
            pthread_mutex_unlock(&first_lock_mutex);
            break;
        }
        else{
            pthread_mutex_lock(&info);
            cout << get_counter(x.car_id) << ": " << x.direction << " " << go_car[x.car_id] << " acquire failed " << x.m2 << endl;
            pthread_mutex_unlock(&info);
            pthread_mutex_lock(&deadlock_mutex);
            if(is_deadlock()){
                pthread_mutex_lock(&info);
                cout << x.car_id << "A DEADLOCK HAPPENS at " << get_counter(x.car_id) << endl;
                pthread_mutex_unlock(&info);
                pthread_mutex_unlock(&road_mutex[x.mutex_1]);
                pthread_mutex_unlock(&deadlock_mutex);
                pthread_mutex_lock(&first_lock_mutex);
                first_lock[x.car_id] = false;
                pthread_mutex_unlock(&first_lock_mutex);
                increase_counter(x.car_id);
                return;
            }
            pthread_mutex_unlock(&deadlock_mutex);
            increase_counter(x.car_id);
        }
    }

    while (!can_cross(x.car_id));
    increase_counter(x.car_id);
    decrease_left(x.car_id);
    pthread_mutex_lock(&info);
    cout << get_counter(x.car_id) << ": " << x.direction << " " << go_car[x.car_id] << " leaves at " << get_counter(x.car_id) << endl;
    pthread_mutex_unlock(&info);
    go_car[x.car_id]++;



    while (!can_cross(x.car_id));
    increase_counter(x.car_id);
    pthread_mutex_unlock(&road_mutex[x.mutex_1]);
    pthread_mutex_unlock(&road_mutex[x.mutex_2]);
    pthread_mutex_lock(&info);
    cout << get_counter(x.car_id) << ": " << x.direction << " " << go_car[x.car_id] << " release locks " << endl;
    pthread_mutex_unlock(&info);
}

void *ThreadRunner(void *arg) {
    int i = *((int *) arg);
    while(get_left(i)){
        cross(cross_d[i]);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 5) {
        return 0;
    }
    pthread_t tid[THREAD_NUM];
    pthread_mutex_init (&info, NULL);
    pthread_mutex_init (&deadlock_mutex, NULL);
    pthread_mutex_init (&first_lock_mutex, NULL);
    int i;
    for (i = 0; i < 4; i++) {
        pthread_mutex_init (&counter_mutex[i], NULL);
        pthread_mutex_init (&left_mutex[i], NULL);
        pthread_mutex_init (&road_mutex[i], NULL);
    }
    for (i = 0; i < 4; i++) {
        counter[i] = 1;
        go_car[i] = 0;
        first_lock[i] = false;
    }
    cross_d[3].update(2, 3, 'W', 3, "lock_2", "lock_3");
    cross_d[0].update(0, 2, 'N', 0, "lock_0", "lock_2");
    cross_d[1].update(1, 0, 'E', 1, "lock_1", "lock_0");
    cross_d[2].update(3, 1, 'S', 2, "lock_3", "lock_1");
    for (i = 0; i < 4; i++) {
        sscanf(argv[i+1], "%d", &left_car[i]);
    }
    /*
    left_car[0] = 1;
    left_car[1] = 1;
    left_car[2] = 1;
    left_car[3] = 1;
    */
    int t[THREAD_NUM];
    int id[4];
    for (i = 0; i < THREAD_NUM; i++) {
        id[i] = i;
        pthread_create(&tid[i], NULL, ThreadRunner, (void *) &id[i]);
    }

    for (i = 0; i < THREAD_NUM; i++) {
        pthread_join(tid[i], NULL);
    }
}
