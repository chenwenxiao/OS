#include <iostream>                // cout
#include <thread>                // thread
#include <mutex>                // mutex, unique_lock
#include <condition_variable>    // condition_variable
#include <algorithm>
#include <unistd.h>
#include <algorithm>
using namespace std;

mutex mtx_tobacco; // 全局互斥锁.
mutex mtx_paper; // 全局互斥锁.
mutex mtx_match; // 全局互斥锁.
condition_variable cv_tobacco; // 全局条件变量.
condition_variable cv_paper;
condition_variable cv_match;
bool tobacco, paper, match = false; // 全局标志位.

void A() {
    unique_lock <mutex> lck_tobacco(mtx_tobacco);
    while (true) {// 如果标志位不为 true, 则等待...
        if (tobacco) {
            cout << "A make smoke" << endl;
            sleep(2);
            break;
        } else {
            cv_tobacco.wait(lck_tobacco); // 当前线程被阻塞, 当全局标志位变为 true 之后,
        }
    }
    // 线程被唤醒, 继续往下执行打印线程编号id.
    cout << "thread A end" << endl;
}

void B() {
    unique_lock <mutex> lck_paper(mtx_paper);
    while (true) {// 如果标志位不为 true, 则等待...
        if (paper) {
            cout << "B make smoke" << endl;
            sleep(2);
            break;
        } else {
            cv_paper.wait(lck_paper); // 当前线程被阻塞, 当全局标志位变为 true 之后,
        }
    }
    // 线程被唤醒, 继续往下执行打印线程编号id.
    cout << "thread B end" << endl;
}

void C() {
    unique_lock <mutex> lck_match(mtx_match);
    while (true) {// 如果标志位不为 true, 则等待...
        if (match) {
            cout << "C make smoke" << endl;
            sleep(2);
            break;
        } else {
            cv_match.wait(lck_match); // 当前线程被阻塞, 当全局标志位变为 true 之后,
        }
    }
    // 线程被唤醒, 继续往下执行打印线程编号id.
    cout << "thread C end" << endl;
}


void Seller() {
    unique_lock <mutex> lck_tobacco(mtx_tobacco);
    unique_lock <mutex> lck_paper(mtx_paper);
    unique_lock <mutex> lck_match(mtx_match);

    int s[3] = {0, 1, 2};
    for (int i = 0; i < 3; ++i)
        swap(s[rand() % 3], s[rand() % 3]);
    for (int i = 0; i < 3; ++i) {
        sleep(2);
        if (s[i] == 0) {
            cout << "Seller put tobacco" << endl;
            tobacco = true; // 设置全局标志位为 true.
            cv_tobacco.notify_all(); // 唤醒所有线程.
        } else if (s[i] == 1) {
            cout << "Seller put paper" << endl;
            paper = true; // 设置全局标志位为 true.
            cv_paper.notify_all(); // 唤醒所有线程.
        } else {
            cout << "Seller put match" << endl;
            match = true; // 设置全局标志位为 true.
            cv_match.notify_all(); // 唤醒所有线程.
        }
    }
}

int main()
{
    thread tA(A), tB(B), tC(C), tS(Seller);
    tA.join();
    tB.join();
    tC.join();
    tS.join();


    return 0;
}
