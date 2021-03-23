#include "config.h"
#include <UniquePtr.h>
#include <iostream>


using namespace cpp;
using namespace std;


struct MyItem {
  int x;
  MyItem(int x) : x(x) {
    cout << "construct " << x << endl;
  }
  ~MyItem() {
    cout << "destruct " << x << endl;
  }
};


int main() {
  auto ptr = MakeUnique<MyItem>(1);
  cout << ptr->x << endl;
  return 0;
}