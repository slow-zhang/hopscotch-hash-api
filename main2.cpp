#include<iostream>
#include"api.cpp"
//#include "hopscotch.h"
//#include "hopscotch.cpp"

using namespace std;
int main(){
	HopscotchHashing te;
	//printf("Insert 20:\n");
	
	

    for(int i=1;i<10;i++){
        te.Insert("1233", i*2);
        
    }
    for(int i=1;i<10;i++){
        te.Insert("1233", i*2+1);
    }
   
    te.prr();
    te.Insert("23", 2+1024+256);
    if (te.Contains(1)) {
        te.Delete(1);
    }

    te.prr();
	getchar();
	return 0;
}
