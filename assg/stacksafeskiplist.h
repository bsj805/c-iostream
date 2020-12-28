#include <iostream>
#include <sstream>
#include <mutex>
#include <condition_variable>

#define BILLION  1000000000L
#define NPAIRS  3

using namespace std;
using std::mutex;
using std::condition_variable; 
template<class K,class V,int MAXLEVEL>
class skiplist_node
{
public:
    skiplist_node()
    {
        for ( int i=1; i<=MAXLEVEL; i++ ) {
            forwards[i] = NULL;
        }
	cnt = 0;
	invalid=0;
    }
 
    skiplist_node(K searchKey)
    {
        for ( int i=1; i<=MAXLEVEL; i++ ) {
            forwards[i] = NULL;
        }
	key[0] = searchKey;
	cnt = 1;
	invalid=0;
    }
 
    skiplist_node(K searchKey,V val)
    {
        for ( int i=1; i<=MAXLEVEL; i++ ) {
            forwards[i] = NULL;
        }
		
	key[0] = searchKey;
	value[0] = val;
	cnt = 1;
	invalid=0;
    }
 
    virtual ~skiplist_node()
    {
    }

    void insert(K k, V v)
    {
	for(int i=0;i<cnt;i++){
	        if( key[i] < k) 
		    continue;

		// shift to right
		for(int j=cnt-1;j>=i;j--){
	            key[j+1] = key[j] ;
	            value[j+1] = value[j] ;
		}
		// insert to the right position
	        key[i] = k;
	        value[i] = v;
		cnt++;
		return;
	}
	key[cnt] = k;
	value[cnt] = v;
	cnt++;
	return;
    }
 
    int cnt;
	mutex nodelock;
	int invalid;
    // change KV to array of structure later
    K key[NPAIRS];   // 4*44   --> 176
    V value[NPAIRS];   // 4*44   --> 176
    skiplist_node<K,V,MAXLEVEL>* forwards[MAXLEVEL+1];   // 8*17 = 156 bytes --> 128 + 28 bytes 
    // total 352 + 128 + 28 + 4 -->  512 bytes --> 8 cachelines
};
 
///////////////////////////////////////////////////////////////////////////////
 
template<class K, class V, int MAXLEVEL = 16>
class skiplist
{
public:
    typedef K KeyType;
    typedef V ValueType;
    typedef skiplist_node<K,V,MAXLEVEL> NodeType;
 
    skiplist(K minKey,K maxKey):m_pHeader(NULL),m_pTail(NULL),
                                max_curr_level(1),max_level(MAXLEVEL),
                                m_minKey(minKey),m_maxKey(maxKey)
    {
        m_pHeader = new NodeType(m_minKey);
        m_pTail = new NodeType(m_maxKey);
        for ( int i=1; i<=MAXLEVEL; i++ ) {
            m_pHeader->forwards[i] = m_pTail;
        }
		checklow=m_minKey;
		reallow=m_minKey;
    }
 
    virtual ~skiplist()
    {
        NodeType* currNode = m_pHeader->forwards[1];
        while ( currNode != m_pTail ) {
            NodeType* tempNode = currNode;
            currNode = currNode->forwards[1];
            delete tempNode;
        }
        delete m_pHeader;
        delete m_pTail;
    }
	void newinsort(int* arr,int len)
	{
		for(int i=1;i<len;i++)
		{
			int temp=arr[i];
			int j;
			for(j=i;j>0 && arr[j-1] > temp ; j--)
			{
				arr[j]=arr[j-1];
			}
			arr[j]=temp;
		}
	}
	void lockinsert(K searchKey,V newValue)
    {
		//long checklow;
		//conditionvar cv
		//mutex skiplistmutex
		//m_maxKey;m_minKey
		// long reallow
        skiplist_node<K,V,MAXLEVEL>* update[MAXLEVEL];
        NodeType* currNode = m_pHeader;
		//std::unique_lock<std::mutex> lk(skiplistmutex);
		skiplistmutex.lock();
		if(checklow != m_minKey)//일단 하나의 쓰레드가 시작했는데,
		{//실제로 들어가는 노드의 key[0]값보다 작아야하니까. 기다리는건 reallow
			//if(searchKey>= checklow)
			
				//cv.wait(lk,[&]{return checklow==m_minKey || searchKey<reallow;});//만약 최종 searchKey보다 낮으면 wait풀어도되긴하는데..
				//실제로 searchKey가 더 낮으면 그냥 insert하자.
			while(reallow != m_minKey && searchKey >= reallow)//spin이 제일 빨라
			{//cpu는 waste하더라도 이게제일빠르니까
				usleep(1);
			}
		

		}
		else//아무것도없으면checklow==m_minKey
		{
			checklow =searchKey;
		}
		skiplistmutex.unlock();
		//mutex loopmutex
        for(int level=max_curr_level; level >=1; level--) {
	//		if(searchKey>= reallow) //인경우에 여기 들어오는 건 reallow==minimum

			if(searchKey>= checklow)
			{
				loopmutex.lock();
			}
            while ( currNode->forwards[level]->key[0] <= searchKey ) {
                currNode = currNode->forwards[level];
            }
			if(level==max_curr_level && (checklow > searchKey || checklow ==m_minKey)) checklow= searchKey;
			
            update[level] = currNode;
			loopmutex.unlock();
        }

		reallow=currNode->key[0]; //이것보다 낮은애는 insert출발해도돼
		
        //currNode = currNode->forwards[1];

	if( currNode->cnt < NPAIRS){
	    //  insert
	    currNode->insert(searchKey, newValue);
	}
	else { // split
	    int newlevel = randomLevel();
	    if ( newlevel > max_curr_level ) {
		for ( int level = max_curr_level+1; level <= newlevel; level++ ) {
			update[level] = m_pHeader;
		}
		max_curr_level = newlevel;
	    }

       	    //currNode = new NodeType(searchKey,newValue);
	    NodeType* newNode = new NodeType();
	    int mid=currNode->cnt/2; 
	    for (int i=mid; i<currNode->cnt; i++){
	        newNode->insert(currNode->key[i], currNode->value[i]);
	    }
	    currNode->cnt = mid;
	    if(newNode->key[0] < searchKey){
	        newNode->insert(searchKey, newValue);
	    }
	    else{
	        currNode->insert(searchKey, newValue);
	    }

	    for ( int lv=1; lv<=max_curr_level; lv++ ) {
		newNode->forwards[lv] = update[lv]->forwards[lv];
		update[lv]->forwards[lv] = newNode; // make previous node point to new node
	    }
	}
	reallow=m_minKey;
	checklow=m_minKey;
    }
	void newinsert(K searchKey,V newValue)//while (true)로하기 mutex nodelock int invalid
    {
		while(true){
        skiplist_node<K,V,MAXLEVEL>* update[MAXLEVEL];
        NodeType* currNode = m_pHeader;
		
        for(int level=max_curr_level; level >=1; level--) {
            while ( currNode->forwards[level]->key[0] <= searchKey ) {
                currNode = currNode->forwards[level];
            }
            update[level] = currNode;
        }

        //currNode = currNode->forwards[1];

		currNode->nodelock.lock();
	if( currNode->cnt < NPAIRS){
	    //  insert
		if(currNode->invalid==1) 
		{
			currNode->nodelock.unlock();
			continue;
		}
	    currNode->insert(searchKey, newValue);
		currNode->nodelock.unlock();
	}
	else { // split
		if(currNode->invalid==1)
		{
			currNode->nodelock.unlock();
			continue;
		}
		currNode->invalid=1;
		currNode->nodelock.unlock();
	    int newlevel = randomLevel();
	    if ( newlevel > max_curr_level ) {
		for ( int level = max_curr_level+1; level <= newlevel; level++ ) {
			update[level] = m_pHeader;
		}
		max_curr_level = newlevel;
	    }

       	    //currNode = new NodeType(searchKey,newValue);
	    NodeType* newNode = new NodeType();
		newNode->invalid=1;
	    int mid=currNode->cnt/2; 
	    for (int i=mid; i<currNode->cnt; i++){
	        newNode->insert(currNode->key[i], currNode->value[i]);
	    }
	    currNode->cnt = mid;
	    if(newNode->key[0] < searchKey){
	        newNode->insert(searchKey, newValue);
	    }
	    else{
	        currNode->insert(searchKey, newValue);
	    }

	    for ( int lv=1; lv<=max_curr_level; lv++ ) {
		newNode->forwards[lv] = update[lv]->forwards[lv];
		update[lv]->forwards[lv] = newNode; // make previous node point to new node
	    }
		currNode->invalid=0;
		newNode->invalid=0;
			}
		break;//정상종료된 애들은 그냥 나오면되구
		}
    }
	void stackinsert(K searchKey,V newValue)
    {
        skiplist_node<K,V,MAXLEVEL>* update[MAXLEVEL];
        NodeType* currNode = m_pHeader;
		
		skiplistmutex.lock();
        for(int level=max_curr_level; level >=1; level--) {
            while ( currNode->forwards[level]->key[0] <= searchKey ) {
                currNode = currNode->forwards[level];
            }
            update[level] = currNode;
        }

        //currNode = currNode->forwards[1];
		//(currNode->nodelock).lock();
	if( currNode->cnt < NPAIRS){
	    //  insert
	    //currNode->insert(searchKey, newValue);
		if(currNode->cnt==0)
		{
			currNode->key[currNode->cnt]=searchKey;
			
			(currNode->cnt)++;
			skiplistmutex.unlock();
			//(currNode->nodelock).unlock();
			currNode->value[currNode->cnt]=newValue;

		}
		else if(currNode->key[0] > searchKey) //searchKey가 key[0]되야
		{
			currNode->key[currNode->cnt]=currNode->key[0];
			(currNode->cnt)++;
			currNode->key[0]=searchKey;
			//(currNode->nodelock).unlock();
			skiplistmutex.unlock();

			currNode->value[currNode->cnt]=currNode->value[0];
			currNode->value[0]=newValue;

		}
		else
		{
			currNode->key[currNode->cnt]=searchKey;

			(currNode->cnt)++;
			//(currNode->nodelock).unlock();
			skiplistmutex.unlock();
			currNode->value[currNode->cnt]=newValue;
		}
	}
	else { // split
		newinsort(currNode->key,currNode->cnt);
		newinsort(currNode->value,currNode->cnt);
	    int newlevel = randomLevel();
	    if ( newlevel > max_curr_level ) {
		for ( int level = max_curr_level+1; level <= newlevel; level++ ) {
			update[level] = m_pHeader;
		}

		max_curr_level = newlevel;
	    }

       	    //currNode = new NodeType(searchKey,newValue);
	    NodeType* newNode = new NodeType();
	    int mid=currNode->cnt/2; 
	    for (int i=mid; i<currNode->cnt; i++){
	        newNode->insert(currNode->key[i], currNode->value[i]);
	    }
	    currNode->cnt = mid;
	    if(newNode->key[0] < searchKey){
	        newNode->insert(searchKey, newValue);
	    }
	    else{
	        currNode->insert(searchKey, newValue);
	    }

	    for ( int lv=1; lv<=max_curr_level; lv++ ) {
		newNode->forwards[lv] = update[lv]->forwards[lv];
		update[lv]->forwards[lv] = newNode; // make previous node point to new node
	    }
		skiplistmutex.unlock();
	}
    }
	void insert(K searchKey,V newValue)
    {
        skiplist_node<K,V,MAXLEVEL>* update[MAXLEVEL];
        NodeType* currNode = m_pHeader;
		
        for(int level=max_curr_level; level >=1; level--) {
            while ( currNode->forwards[level]->key[0] <= searchKey ) {
                currNode = currNode->forwards[level];
            }
            update[level] = currNode;
        }

        //currNode = currNode->forwards[1];

	if( currNode->cnt < NPAIRS){
	    //  insert
	    currNode->insert(searchKey, newValue);
	}
	else { // split
	    int newlevel = randomLevel();
	    if ( newlevel > max_curr_level ) {
		for ( int level = max_curr_level+1; level <= newlevel; level++ ) {
			update[level] = m_pHeader;
		}
		max_curr_level = newlevel;
	    }

       	    //currNode = new NodeType(searchKey,newValue);
	    NodeType* newNode = new NodeType();
	    int mid=currNode->cnt/2; 
	    for (int i=mid; i<currNode->cnt; i++){
	        newNode->insert(currNode->key[i], currNode->value[i]);
	    }
	    currNode->cnt = mid;
	    if(newNode->key[0] < searchKey){
	        newNode->insert(searchKey, newValue);
	    }
	    else{
	        currNode->insert(searchKey, newValue);
	    }

	    for ( int lv=1; lv<=max_curr_level; lv++ ) {
		newNode->forwards[lv] = update[lv]->forwards[lv];
		update[lv]->forwards[lv] = newNode; // make previous node point to new node
	    }
	}
    }

    void everynodeinsert(K searchKey,V newValue)//모든 노드 lock하면서
    {
        skiplist_node<K,V,MAXLEVEL>* update[MAXLEVEL];
        NodeType* currNode = m_pHeader;
		
        for(int level=max_curr_level; level >=1; level--) {
            while ( currNode->forwards[level]->key[0] <= searchKey ) {
                currNode = currNode->forwards[level];
            }
            update[level] = currNode;
        }

        //currNode = currNode->forwards[1];

	if( currNode->cnt < NPAIRS){
	    //  insert
	    currNode->insert(searchKey, newValue);
	}
	else { // split
	    int newlevel = randomLevel();
	    if ( newlevel > max_curr_level ) {
		for ( int level = max_curr_level+1; level <= newlevel; level++ ) {
			update[level] = m_pHeader;
		}
		max_curr_level = newlevel;
	    }

       	    //currNode = new NodeType(searchKey,newValue);
	    NodeType* newNode = new NodeType();
	    int mid=currNode->cnt/2; 
	    for (int i=mid; i<currNode->cnt; i++){
	        newNode->insert(currNode->key[i], currNode->value[i]);
	    }
	    currNode->cnt = mid;
	    if(newNode->key[0] < searchKey){
	        newNode->insert(searchKey, newValue);
	    }
	    else{
	        currNode->insert(searchKey, newValue);
	    }

	    for ( int lv=1; lv<=max_curr_level; lv++ ) {
		newNode->forwards[lv] = update[lv]->forwards[lv];
		update[lv]->forwards[lv] = newNode; // make previous node point to new node
	    }
	}
    }
 
    void erase(K searchKey)
    {
	    /*
        skiplist_node<K,V,MAXLEVEL>* update[MAXLEVEL];
        NodeType* currNode = m_pHeader;
        for(int level=max_curr_level; level >=1; level--) {
            while ( currNode->forwards[level]->key < searchKey ) {
                currNode = currNode->forwards[level];
            }
            update[level] = currNode;
        }
        currNode = currNode->forwards[1];
        if ( currNode->key == searchKey ) {
            for ( int lv = 1; lv <= max_curr_level; lv++ ) {
                if ( update[lv]->forwards[lv] != currNode ) {
                    break;
                }
                update[lv]->forwards[lv] = currNode->forwards[lv];
            }
            delete currNode;
            // update the max level
            while ( max_curr_level > 1 && m_pHeader->forwards[max_curr_level] == NULL ) {
                max_curr_level--;
            }
        }
	*/

    }
	int partition(int *arr,int left, int right)
	{
		int lindex;
		int rindex;
		int pivot;
		int temp;
		pivot=left;
		lindex=left+1;
		rindex=right;
		while(lindex<=rindex)
		{
			while(lindex<rindex && arr[lindex]<=arr[pivot])
			{
				lindex++;
			}
			while(lindex<=rindex && arr[rindex]>=arr[pivot])
			{
				rindex--;
			}
			if(lindex <= rindex)
			{
				temp=arr[lindex];
				arr[lindex]=arr[rindex];
				arr[rindex]=temp;
			}
		}
		temp=arr[pivot];
		arr[pivot]=arr[rindex];//pivot과교환
		arr[rindex]=temp;
		return rindex;

	}
	void quicksort(int *arr, int left, int right)
	{
		int mid;
		if(left<right)
		{
			mid=partition(arr,left,right);
			quicksort(arr,left,mid-1);
			quicksort(arr,mid+1,right);
		}

	}
	V binfind(K searchKey)
    {
        NodeType* currNode = m_pHeader;
        for(int level=max_curr_level; level >=1; level--) {
            while ( currNode->forwards[level]->key[0] <= searchKey ) {
                currNode = currNode->forwards[level];
            }
        }
        // currNode = currNode->forwards[1];
	long i,j,left,right,mid,midnum;
	//target=searchkey
	left=0;
	//if(currNode->key[0]==searchKey)return currNode->value[0]; //numpair 1일때
	right=currNode->cnt-1;
	mid=(left+right)/2;
	while(left<=right)
	{
		mid=(left+right)/2;
		midnum=currNode->key[mid];
		if(searchKey==midnum)
		{
			return currNode->value[mid];
		}
		else if(searchKey<midnum)
		{
			right=mid-1;
		}
		else
		{
			left=mid+1;
		}
	}/*
	for(int i=0;i<currNode->cnt;i++){
            if ( currNode->key[i] == searchKey ) {
                return currNode->value[i];
            }
	}*/

        //return NULL;
        return -1;
    }
    //const NodeType* find(K searchKey)
    V find(K searchKey)
    {
        NodeType* currNode = m_pHeader;
        for(int level=max_curr_level; level >=1; level--) {
            while ( currNode->forwards[level]->key[0] <= searchKey ) {
                currNode = currNode->forwards[level];
            }
        }
        // currNode = currNode->forwards[1];
	
	for(int i=0;i<currNode->cnt;i++){
            if ( currNode->key[i] == searchKey ) {
                return currNode->value[i];
            }
	}

        //return NULL;  
	return -1;
    }
 
    bool empty() const
    {
        return ( m_pHeader->forwards[1] == m_pTail );
    }
	std::string printlevel()
	{
		int i=0;
		int count=0;
		std::stringstream sstr;
		NodeType* currNode = m_pHeader; //->forwards[1];
        int j=0;
		for (j=max_curr_level;j>=1;j--)
		{
			sstr <<"level: "<<j;
		
			while ( currNode != m_pTail ) 
			{
		    sstr << "(" ;
			for( i=0;i<currNode->cnt;i++)
			{
				sstr << currNode->key[i] <<",";
			}
			sstr << ")" ;
			currNode = currNode->forwards[j];
			}
			currNode=m_pHeader;
			sstr <<endl;
		}
		return sstr.str();

	}
    std::string printList()
    {
	int i=0;
        std::stringstream sstr;
        NodeType* currNode = m_pHeader; //->forwards[1];
        while ( currNode != m_pTail ) {
	    sstr << "(" ;
		newinsort(currNode->key,currNode->cnt);
		newinsort(currNode->value,currNode->cnt);
	    for(int i=0;i<currNode->cnt;i++){
		sstr << currNode->key[i] << "," ;
	    }
	    sstr << ")";
            currNode = currNode->forwards[1];
	    i++;
	    if(i>200) break;
        }
        return sstr.str();
    }
 
    const int max_level;
 
protected:
    double uniformRandom()
    {
        return rand() / double(RAND_MAX);
    }
 
    int randomLevel() {
        int level = 1;
        double p = 0.5;
        while ( uniformRandom() < p && level < MAXLEVEL ) {
            level++;
        }
        return level;
    }
    K m_minKey;
    K m_maxKey;
	long checklow,reallow;
    int max_curr_level;
	mutex skiplistmutex;
	mutex loopmutex;
	condition_variable cv;
    skiplist_node<K,V,MAXLEVEL>* m_pHeader;
    skiplist_node<K,V,MAXLEVEL>* m_pTail;
};
 
///////////////////////////////////////////////////////////////////////////////
 
