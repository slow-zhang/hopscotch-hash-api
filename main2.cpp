#include<iostream>
#include"api.cpp"
//#include "hopscotch.h"
//#include "hopscotch.cpp"

using namespace std;
int main(){
	HopscotchHashing te;
	//printf("Insert 20:\n");
	
	

    te.Insert("12233", 1);
    te.Insert("12233", 1);
    
    te.Insert("23", 2);
    if (te.Contains(1)) {
        te.Delete(1);
    }


	getchar();
	return 0;
}
