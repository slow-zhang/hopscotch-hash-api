//Your index class
class KeyComparator;
#define KeyType int
#include "hopscotch.h"
//#include "hopscotch.cpp"

class  HopscotchHashing{
private:

    Hopscotch H;
    int cusor=-1;
	// KeyComparator不需要大家实现，但是key的比较需要由compare_来完成
	KeyComparator const compare_();

public:
    
    
    HopscotchHashing(){
        Hopscotch H;
    }
	// Insert or replace the tuple address with key into the index.
	// Returns true iff Insert or replace successfully.
    bool Insert(const char* address, const KeyType &key){
    // assume key not equal to 0;
        return H.add(key,address);
    }

	// delete
	// Returns true iff find the equal key in the index and delete it successfully.
    bool Delete(const KeyType &key){
        return H.remove(key);
    }
	
	// Returns true iff an entry that compares equal to key is in the index.
    bool Contains(const KeyType &key)  {
        return H.contains(key);
    }

	/**
	 * This method moves to the first tuple equal to given key.  To
	 * iterate through all entries with the key (if non-unique index)
	 * or all entries that follow the entry, use nextValueAtKey().
	 *
	 * This method can be used <b>only for perfect matching</b> in
	 * which the whole search key matches with at least one entry in
	 * this index.  For example,
	 * (a,b,c)=(1,3,2),(1,3,3),(2,1,2),(2,1,3)....
	 *
	 * This method works for "WHERE a=2 AND b=1 AND c>=2", but does
	 * not work for "WHERE a=2 AND b=1 AND c>=1". For partial index
	 * search, use Seek.
	 *
	 * @see searchKey the value to be searched.
	 */
    bool moveToKey(const KeyType &searchKey){
        int tmp=H.find((int)searchKey);
        if(tmp!=-1){
            cusor=tmp;
            return true;
        }
        return false;
    }

	/**
	 * sets the tuple to point the entry found by moveToKey().  calls
	 * this repeatedly to get all entries with the search key (for
	 * non-unique index).
	 */
    char* nextValueAtKey(){
            return H.returnchar(cusor);
    }

    size_t getSize(){
        return H.getsize();
    }

	// Only for hash index to reSize and rehash
    void ensureCapacity(uint32_t capacity){
        //H.resize();
    }

    void prr(){
        H.pr();
    }
};

// KeyComparator provides a means to compare keys, which are internal keys
// concatenated with values.
class KeyComparator {
	public:
	// Compare a and b. Return a negative value if a is less than b, 0 if they
	// are equal, and a positive value if a is greater than b
	virtual int operator()(const KeyType &a,
						const KeyType &b) const = 0;
};
