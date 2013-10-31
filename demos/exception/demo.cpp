#include <execinfo.h>
#include <iostream>
#include <cstdlib>

using namespace std;

void nested_func(int = 10)
{
    void *call_stack[50]; 
    size_t size;

    size = backtrace(call_stack, 50);

    char **results;

    results = backtrace_symbols(call_stack, size);
    if (results == NULL) {
        exit(0);
    }

    for (int j = 0; j < size; ++j)
        cout << results[j] << endl;

    free(results);

}

void func (int = 5)
{
    nested_func();
}

int main()
{

    func();


    return 0;
}

