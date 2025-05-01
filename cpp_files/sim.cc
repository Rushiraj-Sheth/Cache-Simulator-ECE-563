#include "sim.h"

/*  "argc" holds the number of command-line arguments.
    "argv[]" holds the arguments themselves.

    Example:
    ./sim 32 8192 4 262144 8 3 10 gcc_trace.txt
    argc = 9
    argv[0] = "./sim"
    argv[1] = "32"
    argv[2] = "8192"
    ... and so on
*/
int main (int argc, char *argv[]) {
   FILE *fp;			// File pointer.
   char *trace_file;		// This variable holds the trace file name.
   cache_params_t params;	// Look at the sim.h header file for the definition of struct cache_params_t.
   char rw;			// This variable holds the request's type (read or write) obtained from the trace.
   uint32_t addr;		// This variable holds the request's address obtained from the trace.
				// The header file <inttypes.h> above defines signed and unsigned integers of various sizes in a machine-agnostic way.  "uint32_t" is an unsigned integer of 32 bits.

   // Exit with an error if the number of command-line arguments is incorrect.
   if (argc != 9) {
      printf("Error: Expected 8 command-line arguments but was provided %d.\n", (argc - 1));
      exit(EXIT_FAILURE);
   }
    
   // "atoi()" (included by <stdlib.h>) converts a string (char *) to an integer (int).
   params.BLOCKSIZE = (uint32_t) atoi(argv[1]);
   params.L1_SIZE   = (uint32_t) atoi(argv[2]);
   params.L1_ASSOC  = (uint32_t) atoi(argv[3]);
   params.L2_SIZE   = (uint32_t) atoi(argv[4]);
   params.L2_ASSOC  = (uint32_t) atoi(argv[5]);
   params.PREF_N    = (uint32_t) atoi(argv[6]);
   params.PREF_M    = (uint32_t) atoi(argv[7]);
   trace_file       = argv[8];

   // Open the trace file for reading.
   fp = fopen(trace_file, "r");
   if (fp == (FILE *) NULL) {
      // Exit with an error if file open failed.
      printf("Error: Unable to open file %s\n", trace_file);
      exit(EXIT_FAILURE);
   }
    
   // Print simulator configuration.
   printf("===== Simulator configuration =====\n");
   printf("BLOCKSIZE:  %u\n", params.BLOCKSIZE);
   printf("L1_SIZE:    %u\n", params.L1_SIZE);
   printf("L1_ASSOC:   %u\n", params.L1_ASSOC);
   printf("L2_SIZE:    %u\n", params.L2_SIZE);
   printf("L2_ASSOC:   %u\n", params.L2_ASSOC);
   printf("PREF_N:     %u\n", params.PREF_N);
   printf("PREF_M:     %u\n", params.PREF_M);
   printf("trace_file: %s\n", trace_file);
   printf("\n");

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ L1 cache ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   L1_numOfSets = (params.L1_SIZE)/(params.L1_ASSOC * params.BLOCKSIZE);
   L1_numOfIndexBits = log2(L1_numOfSets);
   L1_blockOffestBits = log2(params.BLOCKSIZE); //can also be calculated only once for both caches.
/*
   //extracting tag bits
   int L1_tag = addr>>(L1_numOfIndexBits + L1_blockOffestBits);

   //extracting index bits
   int L1_tagWithIndex = addr>>L1_blockOffestBits;
   //index mask
   int L1_indexMask = (1<<L1_numOfIndexBits)-1;
   int L1_index = L1_tagWithIndex & L1_indexMask;
*/
   //instantiating L1_Cache
   L1_cacheStruct L1_cache(L1_numOfSets, params.L1_ASSOC,params.PREF_N,params.PREF_M);

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ L2 cache ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
   if(params.L2_SIZE>0){
      cacheID=2;
      //parameters for L2 Cache
      L2_numOfSets = (params.L2_SIZE)/(params.L2_ASSOC * params.BLOCKSIZE);
      L2_numOfIndexBits = log2(L2_numOfSets);
      L2_blockOffestBits = log2(params.BLOCKSIZE);
      /*
      //extracting tag bits
      int L2_tag = addr>>(L2_numOfIndexBits + L2_blockOffestBits);

      //extracting index bits
      int L2_tagWithIndex = addr>>L2_blockOffestBits;
      //index mask
      int L2_indexMask = (1<<L2_numOfIndexBits)-1;
      int L2_index = L2_tagWithIndex & L2_indexMask; */

      //instantiating L2_cache
      L2_cacheStruct L2_cache(L2_numOfSets, params.L2_ASSOC);
   }
/*xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx*/

   // Read requests from the trace file and echo them back.
   while (fscanf(fp, "%c %x\n", &rw, &addr) == 2) 
   {	// Stay in the loop if fscanf() successfully parsed two tokens as specified.
      if (rw == 'r'){}
        // printf("r %x\n", addr);
      else if (rw == 'w'){}
        // printf("w %x\n", addr);
      else {
         printf("Error: Unknown request type %c.\n", rw);
	      exit(EXIT_FAILURE);
      }

      ///////////////////////////////////////////////////////
      // Issue the request to the L1 cache instance here.
      ///////////////////////////////////////////////////////

      if(rw=='r'){
         //read request to L1
         //extract the setno. and tag from address
         
         //extracting tag bits
         int curr_tag = addr>>(L1_numOfIndexBits + L1_blockOffestBits);
         //extracting index bits
         int L1_tagWithIndex = addr>>L1_blockOffestBits;
         //index mask
         int L1_indexMask = (1<<L1_numOfIndexBits)-1;
         int L1_set_no = L1_tagWithIndex & L1_indexMask;

         L1_read(L1_set_no, params.L1_ASSOC, curr_tag, L1_numOfSets, ( (cacheID==2)?(params.L2_ASSOC):0), params.PREF_N, params.PREF_M  );

      }   

      if(rw=='w'){
         //write request to L1
         //extracting the req. bits

         //extracting tag bits
         int curr_tag = addr>>(L1_numOfIndexBits + L1_blockOffestBits);
         //extracting index bits
         int L1_tagWithIndex = addr>>L1_blockOffestBits;
         //index mask
         int L1_indexMask = (1<<L1_numOfIndexBits)-1;
         int L1_set_no = L1_tagWithIndex & L1_indexMask;
         L1_write(L1_set_no,params.L1_ASSOC,curr_tag,L1_numOfSets,((cacheID==2)?(params.L2_ASSOC):0),params.PREF_N, params.PREF_M );

      }

   }

   print_L1(L1_numOfSets,params.L1_ASSOC);
   

   print_L2(L2_numOfSets,params.L2_ASSOC);

   print_my_stream_buffer(params.PREF_N,params.PREF_M);

   print_params();

    return(0);
}

void print_L1(int set,int assoc)
{
   //sort in MRU -> LRU

   for(auto& j:L1_vector){
      sort(j.begin(), j.end(), [](const cacheSet& a, const cacheSet& b) {
         return a.lru < b.lru; // Sort in ascending order
      });
   }

   int newline_flg=0;
   cout<<"===== L1 contents ====="<<endl;
   for(int i=0;i<set;i++){
      int flag=0; newline_flg=0;
      for(auto j: L1_vector[i]){
         
         if(j.valid!=1){
            newline_flg++;
            //cout<<"             ";
            continue;
         }
         if(flag==0)
         {
            cout<<dec<<"set  "<<i<<":  ";
            flag=1;
         }
         cout<<hex<<j.tag<<" ";
         if(j.dirty){cout<<"D  ";}
         else{
            cout<<"  ";
         }
      }
      if(newline_flg==assoc)
      {}
      else{   
         cout<<endl;     
      }
   }
}

void print_L2(int set,int assoc)
{
   //sort in MRU -> LRU
   if(cacheID==2){
      cout<<endl;
      int newline_flg=0;

      for(auto& j:L2_vector){
         sort(j.begin(), j.end(), [](const cacheSet& a, const cacheSet& b) {
            return a.lru < b.lru; // Sort in ascending order
         });
      }

      cout<<"===== L2 contents ====="<<endl;
      for(int i=0;i<set;i++){
         int flag=0;newline_flg=0;
         for(auto j: L2_vector[i]){
            
            if(j.valid!=1){
               newline_flg++;
               continue;
            }
            if(flag==0)
            {
               cout<<dec<<"set  "<<i<<":  ";
               flag=1;
            }
            cout<<hex<<j.tag<<" ";
            if(j.dirty){cout<<"D  ";}
            else{
               cout<<"  ";
            }

         }
         if(newline_flg==assoc)
         {}
         else{
            cout<<endl;
         }         
      }
   }
}

void print_params(){
   L1_miss_rate = ((double)L1_read_miss+L1_write_miss)/(L1_reads+L1_writes);
   if(cacheID==2)L2_miss_rate = (double)L2_read_miss/L2_reads;
   cout<<endl;
   cout<<"===== Measurements ====="<<endl;
   cout<<dec<<"a. L1 reads:                   "<<L1_reads<<endl;
   cout<<dec<<"b. L1 read misses:             "<<L1_read_miss<<endl;
   cout<<dec<<"c. L1 writes:                  "<<L1_writes<<endl;
   cout<<dec<<"d. L1 write misses:            "<<L1_write_miss<<endl;
   cout<<dec<<"e. L1 miss rate:               "<<fixed<<setprecision(4)<<L1_miss_rate<<endl;
   cout<<dec<<"f. L1 writebacks:              "<<L1_writeback<<endl;
   cout<<dec<<"g. L1 prefetches:              "<<L1_prefetches<<endl;
   cout<<dec<<"h. L2 reads (demand):          "<<L2_reads<<endl;
   cout<<dec<<"i. L2 read misses (demand):    "<<L2_read_miss<<endl;
   cout<<dec<<"j. L2 reads (prefetch):        "<<0<<endl;
   cout<<dec<<"k. L2 read misses (prefetch):  "<<0<<endl;
   cout<<dec<<"l. L2 writes:                  "<<L2_writes<<endl;
   cout<<dec<<"m. L2 write misses:            "<<L2_write_miss<<endl;
   cout<<dec<<"n. L2 miss rate:               "<<fixed<<setprecision(4)<<L2_miss_rate<<endl;
   cout<<dec<<"o. L2 writebacks:              "<<L2_writeback_to_MEM<<endl;
   cout<<dec<<"p. L2 prefetches:              "<<L2_prefetches<<endl;
   cout<<dec<<"q. memory traffic:             "<<Mem_traffic;

   /*for(uint16_t i=0;i<stream_buf_lru.size();i++){
      cout<<"col no:"<<i<<"  lru no:"<<stream_buf_lru[i];
   }
   cout<<endl;
   for(int i=0;i<stream_buffer.size();i++){
      for(int j=0;j<stream_buffer[0].size();j++){
         cout<<hex<<stream_buffer[i][j]<<" ";
      }
   }
   cout<<endl;
   for(auto i:stream_buf_valid){
      cout<<i<<" ";
   }
   cout<<endl;*/

}

void print_my_stream_buffer(int N, int M){
   if(N>0 && M>0){
      cout<<endl;
      cout<<"===== Stream Buffer(s) contents ====="<<endl;
      vector<vector<int32_t>>sorted_buffer;
      sorted_buffer.resize(M,vector<int32_t>(N));

      vector<int> col_as_per_lru(N,0);
      vector<int> validbit_as_per_lru(N,0);

      for(int counter=0;counter<N;counter++){
         for(int i=0;i<N;i++){
            if(stream_buf_lru[i]==counter){
               col_as_per_lru[counter]=i;
               validbit_as_per_lru[counter]=stream_buf_valid[i];
            }
         }
      }

      for(int lru=0;lru<N;lru++){
         int j=col_as_per_lru[lru];
         for(int i=0;i<M;i++){
            sorted_buffer[i][lru]=stream_buffer[i][j];
         }
      }
      int flg=0;
      for(int j=0;j<N;j++){
         flg=0;
         for(int i=0;i<M;i++){
            if(validbit_as_per_lru[j]==0){
               flg++;
               continue;
            }
            cout<<sorted_buffer[i][j]<<"  ";
         }
         if(flg==M)
         {
            //do nothing
         }
         else{         
            cout<<endl;
         }
      }
   }   

}


void L1_read(int set,int assoc ,int tag, int total_L1_sets,int L2_assoc, int N, int M)
{  L1_reads++;

   int counter=0;//to count empty way in a particular set
   int way_no=-1,hit=0;
   for(int j=0;j<assoc;j++){
      if(L1_vector[set][j].valid==0)
      {
         //if valid=0; this way is empty
         counter++;
         way_no=j; //storing the current way for future ref to cache any block
      }
      else
      {
         //if valid=1; then check the tag for "hit"
         if(tag==L1_vector[set][j].tag){
            //we have a hit.
            //no more seraching req. for L1 cache
            //just update the LRU
            L1_LRU_update(set,j,assoc);
            hit=1;
            if(cacheID==1 && N>0 && M>0){
               int prefetch_req=prefetcher(tag,set,N,M,1);
               if(prefetch_req==(M+1))prefetch_req--;
               
               Mem_traffic+=(prefetch_req);
               L1_prefetches=L1_prefetches+(prefetch_req);
            }
            
            break;
         }
      }
   }

   //if we READ MISS and the cache has empty way
   if(!hit){
      if(counter){
         if(cacheID==2){
            L1_read_miss++;
            //read request to L2
            L2_read(set,L2_assoc,tag,N,M);
         }
         else{
            if(N>0 && M>0){
               int prefetch_req=prefetcher(tag,set,N,M,0);
               if(prefetch_req==M){
                  //had a miss in prefetcher.
                  //L1 read miss
                  L1_read_miss++;
                  //cout<<"read miss caught"<<endl;
                  Mem_traffic+=prefetch_req;
                  Mem_traffic++;//L1 fetches from memory
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
               else if(prefetch_req==(M+1)){
                  prefetch_req--;//had a hit in prefetcher. no L1 miss.
                  Mem_traffic+=prefetch_req;
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
               else{
                  Mem_traffic+=prefetch_req;
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches

               }
            }
            else{
               L1_read_miss++;//cout<<"read miss caught"<<endl;
               Mem_traffic++;
            }
         }
         //then cache the req block
         L1_vector[set][way_no].valid=1;
         L1_vector[set][way_no].tag=tag;
         L1_vector[set][way_no].dirty=0; //clean block
         //update the LRU counter
         L1_LRU_update(set,way_no,assoc);
      }
      else{
         //the set is full.
         //call the function to perform eviction
         int L1_evicted_way=L1_eviction(set,assoc,L2_assoc,N,M);
         //cout<<"L1_evicted_way:--->"<<L1_evicted_way<<endl;
         if(cacheID==2){
            L1_read_miss++;
            //read request to L2 for new block to be cached
            L2_read(set,L2_assoc,tag,N,M);
         }
         else{
            //read req to prefetcher
            if(N>0 && M>0){
            int prefetch_req=prefetcher(tag,set,N,M,0);
            if(prefetch_req==M){
               //had a miss in prefetcher.
               //L1 read miss
               L1_read_miss++;
             //  cout<<"read miss caught"<<endl;
               Mem_traffic+=prefetch_req;
               Mem_traffic++;//L1 fetches from memory
               L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
            }
            else if(prefetch_req==(M+1)){
               prefetch_req--;//had a hit in prefetcher. no L1 miss.
               Mem_traffic+=prefetch_req;
               L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
            }
            else{
               Mem_traffic+=prefetch_req;
               L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
            }
            }
            else{
               L1_read_miss++;//cout<<"read miss caught"<<endl;
               Mem_traffic++;
            }
         }
         //install the block.
         L1_vector[set][L1_evicted_way].dirty=0;
         L1_vector[set][L1_evicted_way].valid=1;
         L1_vector[set][L1_evicted_way].tag=tag;
         //update the lru
         L1_LRU_update(set,L1_evicted_way,assoc);

      }
   }

}

void L1_write(int curr_set, int assoc,int tag,int total_L1_sets,int L2_assoc, int N,int M){
   L1_writes++;
   int count=0,way_no=-1;
   int hit=0;
   for(int j=0;j<assoc;j++){
      if(L1_vector[curr_set][j].valid==0){
         //this cell is empty
         count++;//keeps count of empty cells
         way_no=j;//keeps track of empty way number
      }
      else{
         //check for hit
         if(L1_vector[curr_set][j].tag==tag){
            //we have a HIT. CPU will write, so set dirty bit=1
            L1_vector[curr_set][j].dirty=1;
            //set hit flag
            hit=1;
            if(cacheID==1 && (N>0 && M>0) ){
               int prefetch_req=prefetcher(tag,curr_set,N,M,1);
               if(prefetch_req==(M+1))prefetch_req--;

               Mem_traffic+=(prefetch_req);
               L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
            }
            //update the LRU
            L1_LRU_update(curr_set,j,assoc);
            break;
         }
      }
   }

   //if there is A Write MISS
   if(!hit){
      //if count tracked any empty cell
      if(count){
         if(cacheID==2){
            L1_write_miss++;
            //read request to L2
            L2_read(curr_set,L2_assoc,tag,N,M);
         }
         else{
            if(N>0 && M>0){
               // read req to prefetcher
               int prefetch_req=prefetcher(tag,curr_set,N,M,0);
               if(prefetch_req==M){
                  //had a miss in prefetcher.
                  //L1 write miss
                  L1_write_miss++;
                  Mem_traffic+=prefetch_req;
                  Mem_traffic++;//L1 fetches from memory
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
               else if(prefetch_req==(M+1)){
                  prefetch_req--;//had a hit in prefetcher. no L1 miss.
                  Mem_traffic+=prefetch_req;
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
               else{
                  Mem_traffic+=prefetch_req; //rest of the hits in prefetcher
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
            }
            else{
               L1_write_miss++;
               Mem_traffic++;
            }  
         }
         //cache block in L1
         L1_vector[curr_set][way_no].dirty=1;//CPU will perform write
         L1_vector[curr_set][way_no].valid=1;
         L1_vector[curr_set][way_no].tag=tag;
         //now update its LRU counter
         L1_LRU_update(curr_set,way_no,assoc);

      }
      else{
         //the cache set is full and we need to evict the LRU block
         int L1_evicted_way=L1_eviction(curr_set,assoc,L2_assoc,N,M);
         //cout<<"L1_evicted_way:--->"<<L1_evicted_way<<endl;
         if(cacheID==2){
            L1_write_miss++;
            //read request to L2 for new block to be cached
            L2_read(curr_set,L2_assoc,tag,N,M);
         }
         else{
            if(N>0 && M>0){
               //read req to prefetcher
               int prefetch_req=prefetcher(tag,curr_set,N,M,0);
               if(prefetch_req==M){
                  //had a miss in prefetcher.
                  //L1 write miss
                  L1_write_miss++;
                  Mem_traffic+=prefetch_req;
                  Mem_traffic++;//L1 fetches from memory
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
               else if(prefetch_req==(M+1)){
                  prefetch_req--;//had a hit in prefetcher. no L1 miss.
                  Mem_traffic+=prefetch_req;
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
               else{
                  Mem_traffic+=prefetch_req; //rest of the hits in prefetcher
                  L1_prefetches=L1_prefetches+prefetch_req;//total l1 prefetches
               }
            }
            else{
               L1_write_miss++;
               Mem_traffic++;
            }
         }
         //cache block in L1
         L1_vector[curr_set][L1_evicted_way].dirty=1;
         L1_vector[curr_set][L1_evicted_way].tag=tag;
         L1_vector[curr_set][L1_evicted_way].valid=1;
         //update the LRU
         L1_LRU_update(curr_set,L1_evicted_way,assoc);

      }
   }


}

void L2_read(int L1_set,int L2_assoc ,int L1_tag,int N,int M){

   L2_reads++;
   //construct addr again
   int temp_tag=L1_tag<<(L1_numOfIndexBits+L1_blockOffestBits);
   int temp_set=L1_set<<(L1_blockOffestBits);
   int L2_addr=(temp_tag|temp_set);

   //extract L2_tag
   int L2_tag = L2_addr>>(L2_numOfIndexBits+L2_blockOffestBits);
   //extracting L2_index bits
   int L2_tagWithIndex = L2_addr>>L2_blockOffestBits;
   //index mask
   int L2_indexMask = (1<<L2_numOfIndexBits)-1;
   int L2_set_no = L2_tagWithIndex & L2_indexMask;

   int count=0, way_no=-1,hit=0;

   for(int j=0;j<L2_assoc;j++){
      if(L2_vector[L2_set_no][j].valid==0){
         //empty cell
         count++;
         way_no=j;
      }
      else{
         //check if tag matches
         if(L2_vector[L2_set_no][j].tag==L2_tag){
            //we have a HIT
            hit=1;
            //if requested, block is sent back to L1
            if(cacheID==2 && (N>0 && M>0) ){
               int prefetch_req=prefetcher(L2_tag,L2_set_no,N,M,1);
               if(prefetch_req==(M+1))prefetch_req--;
               
               Mem_traffic+=(prefetch_req);
               L2_prefetches=L2_prefetches+(prefetch_req);
            }
            //update LRU
            L2_LRU_update(L2_set_no,j,L2_assoc);
            break;
         }
      }
   }

   if(!hit){
      //check if cache set has empty space
      if(count){
         if(N>0 && M>0){
            // read req to prefetcher
            int prefetch_req=prefetcher(L2_tag,L2_set_no,N,M,0);
            if(prefetch_req==M){
               //had a miss in prefetcher.
               //L2 reAD miss
               L2_read_miss++;
               Mem_traffic+=prefetch_req;
               Mem_traffic++;//L2 fetches from memory
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else if(prefetch_req==(M+1)){
               prefetch_req--;//had a hit in prefetcher. no L2 miss.
               Mem_traffic+=prefetch_req;
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else{

               Mem_traffic+=prefetch_req; //rest of the hits in prefetcher
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
         }
         else{
            L2_read_miss++;
            Mem_traffic++;
         }

         L2_vector[L2_set_no][way_no].dirty=0;
         L2_vector[L2_set_no][way_no].valid=1;
         L2_vector[L2_set_no][way_no].tag=L2_tag;
         //update LRU
         L2_LRU_update(L2_set_no,way_no,L2_assoc);
      }
      else{
         //the cache set is full. so eviction of LRU block happens
         //call evict function
         int evicted_way=L2_eviction(L2_set_no,L2_assoc);
         if(N>0 && M>0){
            //read request to PREFETCHER
            int prefetch_req=prefetcher(L2_tag,L2_set_no,N,M,0);
            if(prefetch_req==M){
               //had a miss in prefetcher.
               //L2 READ miss
               L2_read_miss++;
               Mem_traffic+=prefetch_req;
               Mem_traffic++;//L2 fetches from memory
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else if(prefetch_req==(M+1)){
               prefetch_req--;//had a hit in prefetcher. no L2 miss.
               Mem_traffic+=prefetch_req;
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else{
               Mem_traffic+=prefetch_req; //rest of the hits in prefetcher
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
         }
         else{
            L2_read_miss++;
            Mem_traffic++;
         }

         //set new parameters
         L2_vector[L2_set_no][evicted_way].valid=1;
         L2_vector[L2_set_no][evicted_way].dirty=0;
         L2_vector[L2_set_no][evicted_way].tag=L2_tag;
         //update LRU
         L2_LRU_update(L2_set_no,evicted_way,L2_assoc);
      }
   }
}

void L2_write(int L1_set, int L1_tag,int L2_assoc,int N,int M){
   L2_writes++;
   //construct addr again
   int temp_tag=L1_tag<<(L1_numOfIndexBits+L1_blockOffestBits);
   int temp_set=L1_set<<(L1_blockOffestBits);
   int L2_addr=(temp_tag|temp_set);

   //extract L2_tag
   int L2_tag = L2_addr>>(L2_numOfIndexBits+L2_blockOffestBits);
   //extracting L2_index bits
   int L2_tagWithIndex = L2_addr>>L2_blockOffestBits;
   //index mask
   int L2_indexMask = (1<<L2_numOfIndexBits)-1;
   int L2_set_no = L2_tagWithIndex & L2_indexMask;

   int count=0, way_no=-1,hit=0;

   for(int j=0;j<L2_assoc;j++){
      if(L2_vector[L2_set_no][j].valid==0){
         count++;
         way_no=j;
      }
      else{
         //check for HIT
         if(L2_vector[L2_set_no][j].tag==L2_tag){
            //ITS A Write HIT
            hit=1;
            L2_vector[L2_set_no][j].dirty=1;
            if(cacheID==2 && (N>0 && M>0) ){
               int prefetch_req=prefetcher(L2_tag,L2_set_no,N,M,1);
               if(prefetch_req==(M+1))prefetch_req--;
               
               Mem_traffic+=(prefetch_req);
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            L2_LRU_update(L2_set_no,j,L2_assoc);
            break;
         }
      }
   }
//its a WRITE MISS
   if(!hit){
      if(count){
         //cache set has an empty cell
         if(N>0 && M>0){
            int prefetch_req=prefetcher(L2_tag,L2_set_no,N,M,0);
            if(prefetch_req==M){
               //had a miss in prefetcher.
               //L2 write miss
               L2_write_miss++;
               Mem_traffic+=prefetch_req;
               Mem_traffic++;//L2 fetches from memory
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else if(prefetch_req==(M+1)){
               prefetch_req--;//had a hit in prefetcher. no L2 miss.
               Mem_traffic+=prefetch_req;
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else{
               Mem_traffic+=prefetch_req; //rest of the hits in prefetcher
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
         }
         else{
            L2_write_miss++;
            Mem_traffic++;
         }
         
         L2_vector[L2_set_no][way_no].valid=1;
         L2_vector[L2_set_no][way_no].dirty=1;
         L2_vector[L2_set_no][way_no].tag=L2_tag;
         //update the LRU counter
         L2_LRU_update(L2_set_no,way_no,L2_assoc);
      }
      else{
         //cache set is full.
         //eviction is performed
         int evicted_way=L2_eviction(L2_set_no,L2_assoc);
         if(N>0 && M>0){
            int prefetch_req=prefetcher(L2_tag,L2_set_no,N,M,0);
            if(prefetch_req==M){
               //had a miss in prefetcher.
               //L2 write miss
               L2_write_miss++;
               Mem_traffic+=prefetch_req;
               Mem_traffic++;//L2 fetches from memory
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else if(prefetch_req==(M+1)){
               prefetch_req--;//had a hit in prefetcher. no L2 miss.
               Mem_traffic+=prefetch_req;
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
            else{
               Mem_traffic+=prefetch_req; //rest of the hits in prefetcher
               L2_prefetches=L2_prefetches+(prefetch_req);//L2 prefetch from memory
            }
         }
         else{
            L2_write_miss++;
            Mem_traffic++;
         }

         L2_vector[L2_set_no][evicted_way].valid=1;
         L2_vector[L2_set_no][evicted_way].dirty=1;
         L2_vector[L2_set_no][evicted_way].tag=L2_tag;
         //update the LRU counter
         L2_LRU_update(L2_set_no,evicted_way,L2_assoc);
      }

   }

}


void L1_LRU_update(int curr_set, int curr_way,int total_assoc)
{
    //current block under transaction with highest priority

   int curr_lru=L1_vector[curr_set][curr_way].lru;
   //cout<<"curr_lru====== "<<curr_lru<<endl;
   
   for(int j=0;j<total_assoc;j++){
      if(L1_vector[curr_set][j].lru<curr_lru){
         L1_vector[curr_set][j].lru=(L1_vector[curr_set][j].lru+1);
      }
   }

   L1_vector[curr_set][curr_way].lru=0;   

}

void L2_LRU_update(int curr_set,int curr_way,int L2_assoc){
   int curr_lru=L2_vector[curr_set][curr_way].lru;
   for(int j=0;j<L2_assoc;j++)
   {
      if(L2_vector[curr_set][j].lru<curr_lru){
         L2_vector[curr_set][j].lru=(L2_vector[curr_set][j].lru)+1;
      }
   }
   L2_vector[curr_set][curr_way].lru=0;
}

int L1_eviction(int curr_set,int total_assoc,int L2_assoc,int N,int M)
{  
   int curr_way=-1;

   for(int j=0;j<total_assoc;j++){

      if(L1_vector[curr_set][j].lru==total_assoc-1){
         if(L1_vector[curr_set][j].dirty==1){
            if(L2_assoc>0){
               //write request to L2 for current cached block
               
               L2_write(curr_set,L1_vector[curr_set][j].tag,L2_assoc,N,M);
            }
            else{
               //writeback to memory
               Mem_traffic++;
            }
            L1_writeback++;
         }

         //reset the current way's parameters
         L1_vector[curr_set][j].dirty=0;
         L1_vector[curr_set][j].tag=-1;
         L1_vector[curr_set][j].valid=0;
         curr_way=j;
         break;

      }
      
   }


   return curr_way;
}


int L2_eviction(int curr_set,int L2_assoc){
   int curr_way=-1;

   for(int j=0;j<L2_assoc;j++){
      if(L2_vector[curr_set][j].lru==L2_assoc-1){
         //check for dirty bit
         if(L2_vector[curr_set][j].dirty==1){
            //write back to memory
            Mem_traffic++;
            L2_writeback_to_MEM++;
         }
         L2_vector[curr_set][j].dirty=0;
         L2_vector[curr_set][j].valid=0;
         L2_vector[curr_set][j].tag=-1;
         curr_way=j;
         break;
      }
   }
   
   return curr_way;

}

int prefetcher(int tag, int set_no,int N, int M, int cache_stat){
   
   int numOfIndexbits, blockOffestBits, total_sets;
   int memreq=0;

   if(cacheID==1){
      numOfIndexbits=L1_numOfIndexBits;
      blockOffestBits=L1_blockOffestBits;
      total_sets=pow(2,numOfIndexbits);
   }
   else{
      numOfIndexbits=L2_numOfIndexBits;
      blockOffestBits=L2_blockOffestBits;
      total_sets=pow(2,numOfIndexbits);
   }

   //tag for storing new addr
   tag=tag<<(numOfIndexbits+blockOffestBits);
   //ref addr for comparision
   int ref_set=set_no<<blockOffestBits;
   int ref_addr=tag | ref_set;
   ref_addr=ref_addr>>(blockOffestBits);
   int new_addr=ref_addr;

   int count=0;
   int buffer_no=-1;

   for(int j=0;j<N;j++){
      if(stream_buf_valid[j]==0){
         count++;
         buffer_no=j;
      }
   }

   if(count==N){
      //entire buffer is empty
      if(cache_stat==1)
      {
         return 0;
      }
      //fill the LRU col
      stream_buf_valid[N-1]=1;
      for(int i=0;i<M;i++){
         //set_no=(set_no+1)%total_sets;
         new_addr=new_addr+1;
         stream_buffer[i][N-1]=new_addr;
         memreq++;
      }
      //Update LRU
      prefetcher_LRU_update(N-1,N);
      
      return memreq; //return false
   }
   else{
      //buffer is not empty
      int mru_col=-1, mru=N+1, mru_row=-1;

      for(int j=0;j<N;j++){
         if(stream_buf_valid[j]==1)
         {
            for(int i=0;i<M;i++){
               if(ref_addr==stream_buffer[i][j]){
                  //select MRU buffer

                  if(stream_buf_lru[j]<=mru){
                     mru=stream_buf_lru[j];
                     mru_col=j;
                     mru_row=i;
                     break;
                  }
               }
            }
         }
      }

      if(mru_col!=-1){
         //we had a HIT
         //update the stream buffer
         for(int i=0;i<M;i++){
            //set_no=(set_no+1)%total_sets;
            new_addr=new_addr+1;
            stream_buffer[i][mru_col]=new_addr;
         }

         //update LRU
         prefetcher_LRU_update(mru_col,N);
         memreq = mru_row+1;
         if(memreq==M){
            return (memreq+1); //M+1 is not possible. Its a flag to differentiate btwn M fetches after a hit and 
            //M fetches after a miss... M+1 signifies M fetches after a HIT.
         }
         return memreq; //return true

      }
      else if((buffer_no!=-1) &&  (mru_col==-1) && (cache_stat==0)){
         //prefetch in empty buffer
         stream_buf_valid[buffer_no]=1;

         for(int i=0;i<M;i++){
            //set_no=(set_no+1)%total_sets;
            new_addr=new_addr+1;
            stream_buffer[i][buffer_no]=new_addr;
         }

         //update LRU
         prefetcher_LRU_update(buffer_no,N);
         memreq=M;
         return memreq; // return false
      }

      //reached here --> entire buffer is full and we have a MISS.
      //evict LRU buffer, then update LRU
      else if( (buffer_no==-1) && (mru_col==-1) && (cache_stat==0) ){
         int lru_col=-1;
         for(int i=0;i<N;i++){
            if(stream_buf_lru[i]==N-1){
               lru_col=i;
               break;
            }
         }

         for(int i=0;i<M;i++){
            //set_no=(set_no+1)%total_sets;
            new_addr=new_addr+1;
            stream_buffer[i][lru_col]=new_addr;
         }

         //update LRU
         prefetcher_LRU_update(lru_col,N);
         return M;
      }

      
   }

   return 0;
}

void prefetcher_LRU_update(int col_no, int N){
   int curr_lru=stream_buf_lru[col_no];
   for(int i=0;i<N;i++){
      if(stream_buf_lru[i]<curr_lru){
         stream_buf_lru[i]++;
      }
   }
   stream_buf_lru[col_no]=0;
}
