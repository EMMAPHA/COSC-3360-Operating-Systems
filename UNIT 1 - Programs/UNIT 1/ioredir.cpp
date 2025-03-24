#include <iostream>
#include <string>

using namespace std;

int main()
{
    string line;
    int i=0;
    while(getline(cin,line))
    {
        cout << line << endl;
        i++;
    }
    cout << endl << "I = " << i << endl;
    return 0;
}