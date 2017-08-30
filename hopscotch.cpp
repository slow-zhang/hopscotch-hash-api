#include "hopscotch.h"
#include <iostream>
#include <stdlib.h>
#include <climits>
using namespace std;

Hopscotch::Hopscotch(){
	//segments_arys = new Bucket[MAX_SEGMENTS+512];
	segments_arys = (Bucket*)malloc(sizeof(Bucket) * (size));
    seed = rand() % LONG_MAX +1;
  
}
Hopscotch::Hopscotch(int Hsize){
    //segments_arys = new Bucket[MAX_SEGMENTS+512];
    size=Hsize;
    segments_arys = (Bucket*)malloc(sizeof(Bucket) * (size));
}


Hopscotch::~Hopscotch()
{

}



bool Hopscotch::remove(int key){
	//unsigned int hash = ((*key)&(MAX_SEGMENTS-1));
	unsigned int h= MurmurHash2A((const int)key, 4);
	Bucket* start_bucket = &segments_arys[h];
	//Bucket* start_bucket = segments_arys+hash;
	start_bucket->lock();

	unsigned int hop_info = start_bucket->_hop_info;
	unsigned int mask = 1;
	for(int i=0;i<HOP_RANGE;++i,mask<<=1){
		if(mask & hop_info){
			Bucket* check_bucket = start_bucket+i;
			if(key==(check_bucket->_key)){
				
				check_bucket->_key=NULL;
				check_bucket->_address=NULL;
				start_bucket->_hop_info &=~(1<<i);
				start_bucket->unlock();
				return true;
			}
		}
	}
	start_bucket->unlock();
	return false;
}

void Hopscotch::find_closer_bucket(Bucket** free_bucket,int* free_distance,int &val) {
	Bucket* move_bucket = *free_bucket -(HOP_RANGE-1);
	for(int free_dist = (HOP_RANGE -1);free_dist>0;--free_dist) {
		unsigned int start_hop_info = move_bucket->_hop_info;
		int move_free_distance = -1;
		unsigned int mask =1;
		for (int i=0;i<free_dist;++i,mask <<=1) {
			if(mask & start_hop_info){
				move_free_distance = i;
				break;
			}
		}
		if (-1 != move_free_distance) {
			move_bucket->lock();
			if(start_hop_info == move_bucket->_hop_info) {
				Bucket* new_free_bucket = move_bucket + move_free_distance;
				move_bucket->_hop_info |= (1 << free_dist);
				(*free_bucket)->_address = new_free_bucket->_address;
				(*free_bucket)->_key = new_free_bucket->_key;
				++(move_bucket->_timestamp);
				new_free_bucket->_key =0;
				new_free_bucket->_address =0;
				move_bucket->_hop_info &= ~(1<<move_free_distance);
				*free_bucket = new_free_bucket;
				*free_distance -=free_dist;
				move_bucket->unlock();
				return;
			}
			move_bucket->unlock();
		}
		++move_bucket;
	}
	(*free_bucket)->_key = NULL;
	val = 0;
	*free_distance = 0;
}


bool Hopscotch::contains(int key){
	//unsigned int h = ((*key)&(MAX_SEGMENTS-1));
	unsigned int h= MurmurHash2A((const int)key, 4);

	Bucket* start_bucket = &segments_arys[h];
	unsigned int try_counter = 0;
	unsigned int timestamp;
	do{
		//cout << "Fast Path : try counter" << try_counter << endl;
		timestamp = start_bucket->_timestamp;
		unsigned int hop_info = start_bucket->_hop_info;
		Bucket* check_bucket = start_bucket;
        unsigned int temp;

		for( int i = 0 ; i < HOP_RANGE ; i++){

            temp=hop_info;
            temp=temp>>i;

			if(temp&1){
				if(key == (check_bucket->_key)){
					//cout << "Found!!" << endl;
					return true;
				}
			}
			++check_bucket;
		}
		++try_counter;
	}while(timestamp != start_bucket->_timestamp && try_counter < MAX_TRIES);
	//cout << "Slow path " << endl;
	if(timestamp != start_bucket->_timestamp){
		Bucket* check_bucket = start_bucket;
		for(int i=0; i<HOP_RANGE ; i++){
			if(key == (check_bucket->_key))
				return true;
			++check_bucket;
		}
	}
    
	//cout << "Not found!!" << endl;
	return false;
}



bool Hopscotch::add(int key,const char *data){
  int val = 1;
  //unsigned int h=((*key)&(MAX_SEGMENTS-1));
  unsigned int h= MurmurHash2A((const int)key, 4);
  //unsigned int h= key % size;
  Bucket* start_bucket=&segments_arys[h];
  start_bucket->lock();
  //sleep(1);
  if(contains(key)){
    start_bucket->unlock();
    return false;
  }

  Bucket* free_bucket=start_bucket;
  int free_distance=0;
  for(;free_distance<ADD_RANGE;++free_distance){
    if(!free_bucket->_key)
      break;
    ++free_bucket;
  }

  if(free_distance < ADD_RANGE){
    do
    {
      if(free_distance < HOP_RANGE){
        start_bucket->_hop_info |= (1<<free_distance);
        free_bucket->_address=(char *)data;
        free_bucket->_key=key;
        start_bucket->unlock();
        return true;
      }
      find_closer_bucket(&free_bucket,&free_distance,val);
    }while(0 != val);
  }
  start_bucket->unlock();
  cout<<"warning: Need resize!"<<endl;
  //resize();
  //return Hopscotch::add(key,data);
  //cout<<"Called Resize"<<endl;
  return false;
}

int Hopscotch::getsize(){
    return size;
}


void Hopscotch::resize(){
    int oldsize= size;
    for(int i=0;i<oldsize;i++){
        segments_arys[i].lock();
    }
    if(size!=oldsize) return;//有程序在 resize（）：
    Bucket* old_segments_arys=segments_arys;
    size=size*2;
    segments_arys= (Bucket*)malloc(sizeof(Bucket) * (size*2));
    for(int i=0;i<oldsize;i++){
        if(!old_segments_arys[i]._key){
            add(old_segments_arys[i]._key,old_segments_arys[i]._address);
        }
    }
    for(int i=0;i<oldsize;i++){
        segments_arys[i].unlock();
    }

    
}

int Hopscotch::find(int key){
    //unsigned int h = ((*key)&(MAX_SEGMENTS-1));
    unsigned int h= MurmurHash2A((const int)key, 4);
    
    if(h >MAX_SEGMENTS)
    {
        std::cout<<" ERROR";
    }
    Bucket* start_bucket = &segments_arys[h];
    unsigned int try_counter = 0;
    unsigned int timestamp;
    do{
        //cout << "Fast Path : try counter" << try_counter << endl;
        timestamp = start_bucket->_timestamp;
        unsigned int hop_info = start_bucket->_hop_info;
        
        Bucket* check_bucket = start_bucket;
        unsigned int temp;
        
        for( int i = 0 ; i < HOP_RANGE ; i++){
            temp=hop_info;
            temp=temp>>i;
            
            if(temp&1){
                if(key == (check_bucket->_key)){
                    //cout << "Found!!" << endl;
                    return h;
                }
            }
            
            ++check_bucket;
            h++;
        }
        
        ++try_counter;
    }while(timestamp != start_bucket->_timestamp && try_counter < MAX_TRIES);
    //cout << "Slow path " << endl;
    if(timestamp != start_bucket->_timestamp){
        Bucket* check_bucket = start_bucket;
        for(int i=0; i<HOP_RANGE ; i++){
            if(key == (check_bucket->_key))
                return true;
            ++check_bucket;
        }
    }
    
    //cout << "Not found!!" << endl;
    return -1;
    
}

char* Hopscotch::returnchar(int id){
    return segments_arys[id]._address;
}

