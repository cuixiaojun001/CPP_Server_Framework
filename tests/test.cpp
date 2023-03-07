#include <iostream>
#include <string>
using namespace std;

#define Test(name) \
    cout << #name << endl;
int main() {
    
    Test(DEBUG);
    return 0;
}