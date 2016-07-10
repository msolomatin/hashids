#include <iostream>
using namespace std;
#include "include/Hashids.h"

int main() {
    auto hashids = Hashids("this is my salt");

    cout << hashids.encode(12345) << endl;
    cout << hashids.encode({1, 2, 3, 4, 5}) << endl;

    auto numbers = hashids.decode("NkK9");

    for (int number : numbers) {
        cout << number << ' ';
    }
    cout << endl;

    numbers = hashids.decode("zoHWuNhktp");
    for (int number : numbers) {
        cout << number << ' ';
    }
    cout << endl;
    return 0;
}
