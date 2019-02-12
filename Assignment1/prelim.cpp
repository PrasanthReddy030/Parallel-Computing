#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <iostream>
using namespace std;
int main()
{ 
	int p;
    char hostname[HOST_NAME_MAX + 1];

    hostname[HOST_NAME_MAX] = 0;
    p=gethostname(hostname, HOST_NAME_MAX);
    if(p==0)
        cout<<"Hostname is "<<hostname<<endl;
    else
        perror("gethostname");

    return 0;
}