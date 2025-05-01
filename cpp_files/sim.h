#ifndef SIM_CACHE_H
#define SIM_CACHE_H
#endif
#include <bits/stdc++.h>

using namespace std;


typedef 
struct {
   uint32_t BLOCKSIZE;
   uint32_t L1_SIZE;
   uint32_t L1_ASSOC;
   uint32_t L2_SIZE;
   uint32_t L2_ASSOC;
   uint32_t PREF_N;
   uint32_t PREF_M;
} cache_params_t;

// Put additional data structures here as per your requirement.
int L1_numOfIndexBits;
int L1_blockOffestBits;
int L1_numOfSets; 

int L2_numOfSets;
int L2_numOfIndexBits; 
int L2_blockOffestBits;

long int L1_reads=0,L1_writes=0,L2_reads=0,L2_writes=0,L1_writeback=0,Mem_traffic=0;
long int L1_read_miss=0,L1_write_miss=0,L2_read_miss=0,L2_write_miss=0,L2_writeback_to_MEM=0;
double L1_miss_rate=0.0, L2_miss_rate=0.0;
int L1_prefetches=0, L2_prefetches=0;

void print_L1(int set, int assoc);
void print_L2(int set, int assoc);
void print_params();
void print_my_stream_buffer(int N, int M);

void L1_read(int set,int assoc, int tag,int total_L1_sets,int L2_assoc, int N, int M);
void L1_write(int curr_set, int assoc,int tag,int total_L1_sets,int L2_assoc, int N, int M);
void L1_LRU_update(int curr_set, int curr_way,int total_assoc);
int L1_eviction(int curr_set,int total_assoc,int L2_assoc, int N,int M);

void L2_read(int L1_set,int L2_assoc ,int L1_tag,int N, int M);
void L2_write(int L1_set, int L1_tag,int L2_assoc,int N, int M);
void L2_LRU_update(int curr_set,int curr_way,int L2_assoc);
int L2_eviction(int curr_set,int total_assoc);


int prefetcher(int tag, int set_no,int N, int M,int cache_stat);
void prefetcher_LRU_update(int col_no, int N);
int cacheID=1;



class cacheSet{
   public:
   //to add below prop in constructor
   bool valid;
   int dirty;
   int tag;
   int lru;
  
  cacheSet(){
   valid=0;
   dirty=-1;//-1 reperesents dont care --- here only
   tag=-1;
   //lru=-1;
  }
   
};

//below class is used to create L1 and L2 cache
vector <vector<cacheSet>> L1_vector;
vector <vector<cacheSet>> L2_vector;

//tracking stream buffer
vector<vector<int32_t>> stream_buffer;
vector<int> stream_buf_valid;
vector<int> stream_buf_lru;

class L1_cacheStruct:public cacheSet{
   public:
   
   int assoc,total_sets;

   L1_cacheStruct(int no_of_set, int associativity, int N, int M):cacheSet(){
      assoc=associativity;
      total_sets=no_of_set;
      L1_vector.resize(total_sets,vector<cacheSet>(assoc));
      //set random values for LRU in range 0 to no_of_set-1
      int temp_lru=0;
      for(int i=0;i<no_of_set;i++){
         temp_lru=0;
         for(int j=0;j<assoc;j++){
            L1_vector[i][j].lru=temp_lru;temp_lru++;
         }
      }

      stream_buffer.resize(M,vector<int32_t>(N));
      stream_buf_valid.resize(N,0);
      stream_buf_lru.resize(N);
      //initialise LRU
      for(int i=0;i<N;i++){
         stream_buf_lru[i]=i;
      }
   }

};

class L2_cacheStruct:public cacheSet{
   public:
   
   int assoc,total_sets;

   L2_cacheStruct(int no_of_set, int associativity):cacheSet(){
      assoc=associativity;
      total_sets=no_of_set;
      L2_vector.resize(total_sets,vector<cacheSet>(assoc));
      
      int temp_lru=0;
      for(int i=0;i<no_of_set;i++){
         temp_lru=0;
         for(int j=0;j<assoc;j++){
            L2_vector[i][j].lru=temp_lru;temp_lru++;
         }
      }
   }

};



//now 





