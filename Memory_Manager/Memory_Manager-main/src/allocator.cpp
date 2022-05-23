

#include "allocator.h"
#include <new>

// QUESTION, IF A THREAD DOES NOT FREE ITS OBJECTS, AND THE PAGEBLOCK APPEARS TO HAVE LESS FREED THAN ALLOCATED, WE DECLARE AGAIN THE BLOCK ORPHANED

// HACK adding 15 to our size, because we have one byte as a header, so if the pageblock_size = 128, we want our first address to be aligned with 16 so, we add 15
// and the first available address is page_start+143 + 1( 1 byte for the header ) = 144 (aligned with 16)
 const static unsigned long pageblock_size= NEAREST_POWER_OF_2_NUMBER(sizeof(pageblock)+sizeof(t_stack)+sizeof(t_stack_node))+15;



static thread_local int flag=0;
static unsigned int global_thread_id = 1;
static char calloc_memory[8192];
static thread_local char struct_memory[8192];
static thread_local int dlsym_calloc_flag= 0;
static thread_local int dlsym_calloc_size=0;

struct thread_data_struct{

unsigned int thread_id;

heap thread_heap[CLASS_NUMBER];

    thread_data_struct(){
        thread_id= atmc_fetch_and_add(&global_thread_id,1);
    }

    ~thread_data_struct(){
        //   char* buf = "Destroying struct\n";
        // write(STDOUT_FILENO,buf,18);
        // dlsym_calloc_flag=1;
        for(int i =0;i<CLASS_NUMBER;i++){

                //  original(nullptr);
                pageblock* temp=(pageblock*)popFront((t_deque*)&thread_heap[i]);
                while (temp)//temp!=&thread_heap[i])
                {
                    if(temp->num_alloc_objects!=temp->num_free_objects){
                        // write(STDOUT_FILENO,"PROVLIMA\n",9);
                        temp->is_orphan=1;




                    }else{
                        //GLOBAL CACHE OR WHATEVER
                        original(temp);
                    }
                    temp=(pageblock*)popFront((t_deque*)&thread_heap[i]);
                }
                

        }
        
    }
};


static thread_local struct thread_data_struct thread_data;

	__attribute__((constructor)) static void initializer(void) {
        thread_data.thread_id=0;
    }


void declare_pageblock_orphan( pageblock* ptr){
        t_stack* temp_stack =(t_stack*)CLEAN_FIRST_16_BYTES_OF_POINTER((unsigned long)&(ptr->remote_stack));

    do
    {
       //check remote free list

    } while (compare_and_swap_ptr(&(ptr->remote_stack.head),ptr->remote_stack.head,temp_stack));
    

}


int retrieve_class_id(size_t size){


    if(size<MINIMUM_OBJECT_SIZE){
        return 0;
    }

    // double size_log = ((double)size)/MINIMUM_OBJECT_SIZE;
    // int pos =((int)ceil(size_log))-1;

    int pos =((size+MINIMUM_OBJECT_SIZE-1)/MINIMUM_OBJECT_SIZE)-1;
    //WE ADD 1 BYTE TO IDENTIFY LARGE, OBJECTS, SO WE WANT TO ADD FREE SPACE
    if(size==base[pos])
        pos =pos+1;

    return pos;
}

void* get_memory(int class_id){
    void** block=nullptr;
    // write(STDOUT_FILENO,"get_memory\n",11);
    if(posix_memalign((void **)&(block), alignment[class_id],alignment[class_id])){
        // write(STDOUT_FILENO,"error\n",6);
        exit;
    }
    if(block==nullptr){
        write(STDOUT_FILENO,"error\n",6);
    }
 
    return(block);
}

void* page_block_allocation(pageblock* page_b,int class_id){
    // char *ptr = (char *)page_b;
    unsigned char *object = nullptr;
    unsigned long page_start = (unsigned long)page_b;


    if(page_b->free_list_offset != 1){
        object = (unsigned char *) (page_b->free_list_offset);
        page_b->free_list_offset = *((unsigned long *) object);
        page_b->num_alloc_objects++;
        // page_b->num_free_objects--;

        // set the object to non free, OBJECT CLASS SIZE RANGE IS 1-128, we add an extra 128 (on free()) (which fits in a byte 255) and we know its a freed object
        (object-1)[0]=(object-1)[0]-BASE_SIZE; 
        return((object));
    }
    //check for allocation page limits.
    // unsigned long boundary = (page_start + page_b->mem_size);

    if(alignment[page_b->class_id]>(page_b->unallocated_offset+base[page_b->class_id])){

        page_b->num_alloc_objects++;
        
        object = (( unsigned char *) (page_start + page_b->unallocated_offset));
        page_b->unallocated_offset += base[page_b->class_id];

        //HERE WE ADD 1 TO THE CLASS ID, TO DISTINGUISH THE FIRST CLASS FROM LARGE OBJECTS, LARGE OBJECTS ARE CLASS 0, SMALL OBJECTS ARE CLASS 1-128
        object[0]=((unsigned char)(class_id+1));
      
        
        return (void*)(object+1);
    }
    return(nullptr);
}

pageblock* init_page_block(int class_id){
    pageblock* page_b = (pageblock *) get_memory(class_id);

    // int temp = (int)sizeof(pageblock);
   
    if(page_b == nullptr)
        return nullptr;
    // printf("pageblock start %p\n",page_b);
    page_b->class_id = class_id;
    // page_b->class_size = base[class_id];
    page_b->free_list_offset = 1;
    // page_b->mem_size = alignment[class_id];
    page_b->num_free_objects = 0;
    page_b->unallocated_offset = pageblock_size; //sizeof(pageblock); //pageblock_size;
    page_b->num_alloc_objects = 0;
    page_b->next = nullptr;
    page_b->prev = nullptr;
    page_b->thread_id = thread_data.thread_id;
    // page_b->is_orphan=0;
    // page_b->remote_stack = (t_stack*)(((unsigned long)page_b + sizeof(pageblock)));
    page_b->remote_stack.head = (t_stack_node*)((unsigned long)page_b +sizeof(pageblock)+sizeof(t_stack));
    page_b->remote_stack.head->next = NULL;
 
    return(page_b);
    // na ginei alligned me poll/sia tou 16.
}




void adopt_pageblock(pageblock* pg_block,void* ptr){



    if(compare_and_swap64((volatile unsigned long long *)(&pg_block->thread_id),0,thread_data.thread_id)){


        pg_block->next = nullptr;
        pg_block->prev = nullptr;

        pushFront((t_deque*)&(thread_data.thread_heap[pg_block->class_id]),(t_deque_node*)pg_block);
        // pg_block->thread_id = thread_data.thread_id;
        free(ptr);


        
    }else{
        remote_free(pg_block,ptr);
    }
  
}




void* large_object_allocation(size_t size){

    // unsigned char *block =(unsigned char*) mmap(nullptr, size+sizeof(unsigned long)+1, PROT_READ | PROT_WRITE , MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    unsigned char *block =(unsigned char*) mmap(nullptr, size+(sizeof(unsigned long)*2), PROT_READ | PROT_WRITE , MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if(block ==nullptr){
    
        write(STDOUT_FILENO,"MMAPBLOCK\n",10);
        return nullptr;
    
    }
    
    //PUT HEADERS INDICATING THE BLOCK IS LARGE AND ITS SIZE
    *((unsigned long*) block)= size;
    // block[sizeof(unsigned long)]=0;
    block[(2*sizeof(unsigned long))-1]=0;

    // return (void*)(((unsigned char*)block)+sizeof(unsigned long)+1);
    return (void*)(((unsigned char*)block)+(2*sizeof(unsigned long)));

}

char* get_large_object_real_address(void* ptr){
    // return (char*)((((char*)ptr)-1) -sizeof(unsigned long));
    return (char*)(((char*)ptr) -2*sizeof(unsigned long));

}
size_t get_large_object_size(void* real_address){

    return *((unsigned long*)real_address);
}

void large_object_deallocation(void* ptr){
    char* real_addr= get_large_object_real_address(ptr);
    size_t size =  get_large_object_size(real_addr);
    munmap(real_addr,size);
    return;
}
int is_large_object(void *ptr){
    int temp= !((unsigned int)*((((unsigned char*)ptr)-1)));
    return temp;
}


pageblock* get_page_block_start(void*ptr){


    unsigned int mask_position = ((unsigned int)(*(((unsigned char*)ptr)-1))-1);
    if(mask_position<0| mask_position>127){
        exit;
    }
    unsigned int mask=LOG2(alignment[mask_position]);
    unsigned long page_block_start = (unsigned long)((((unsigned long)ptr)>>mask)<<mask);
   
    return (pageblock*)page_block_start;
}


int is_double_free(void* ptr){
    return (((unsigned int)*((((unsigned char*)ptr)-1)))-1> BASE_SIZE);
}
void set_freed_object_header(void* ptr){
    unsigned int temp =((unsigned int)(*((((unsigned char*)ptr)-1))));
    *(((unsigned char*)ptr)-1) =(unsigned char)(temp +BASE_SIZE);
}

void* malloc(size_t size){
    // char* buf = "Always failing malloc\n";
    // write(STDOUT_FILENO,buf,22);

    heap* thr_heap;
    pageblock* pg_block;
    int class_id;
    void *allocated= nullptr;

    if(!size ||size<0) return nullptr;


    if(__glibc_likely(size <MAXIMUM_OBJECT_SIZE)){
        class_id= retrieve_class_id(size);
        thr_heap = &(thread_data.thread_heap[class_id]);

        pg_block = (pageblock*)thr_heap->head;

        // check page_block not nullptr
        for(pg_block; pg_block; pg_block = pg_block->next){
            allocated = page_block_allocation(pg_block,class_id);
            if(allocated!=NULL){
                return(allocated);
            }
        }

        pageblock* node;
        node = init_page_block(class_id);
        
        if(node==nullptr) {
            
            return nullptr;}

        pushFront((t_deque* )thr_heap,(t_deque_node*)node);
        // void* temp =page_block_allocation(node,class_id);
        
        // return temp;
        return page_block_allocation(node,class_id);
    }else{
  
        return large_object_allocation(size);

    }
  
   

    return nullptr;
}


void *realloc(void *ptr, size_t sz){

    size_t ptr_size;
    void* object=NULL;
            // write(STDOUT_FILENO,"ralloc\n",7);

    if( ptr ==NULL){
        return malloc(sz);
    }
    
    if(sz==0 && ptr!=NULL){
        free(ptr);
    }

    if(is_large_object(ptr)){
        ptr_size= get_large_object_size(\
        (void*) get_large_object_real_address(ptr));
    }else{
        //SMALL object
       ptr_size= base[get_page_block_start(ptr)->class_id];
    }

    if(ptr_size>sz) return ptr;

    object = malloc(sz);

    if(object!=NULL){
        memcpy(object,ptr,ptr_size);
        free(ptr);
    }
    return object;

}


void *calloc(size_t nmemb, size_t sz){

    
    // if(__glibc_unlikely(original_free==NULL)){
 
    //HACK FOR DLSYM CALLING CALLOC
    if(dlsym_calloc_flag==0){
        dlsym_calloc_flag++;
        return calloc_memory;
    }


    //HACK FOR STRUCT CONSTRUCTOR CALLING CALLOC
    if(flag==0 ){
        flag=1;
        return struct_memory;
    }

    // return memset(large_object_allocation(nmemb*sz),0,nmemb*sz);


    size_t total_allocation_size = nmemb*sz;

    if(nmemb==0 || sz==0)return NULL;

    if (nmemb != 0 && (total_allocation_size / nmemb != sz)) return NULL;

    void *ptr=malloc(total_allocation_size);

    if(ptr!=NULL){
        for(int i =0;i<total_allocation_size;i++){
            ((unsigned char*)ptr)[i]=0;
        }
        return ptr;
    }

    return NULL;
}

void remote_free(pageblock* page, void* ptr){

    unsigned long long id;
    t_stack_node* temp_node;
    do
    {
        id =page->thread_id;


        if(id==0){

            adopt_pageblock(page,ptr);
            break;
        }

        temp_node = (t_stack_node*) (CLEAN_FIRST_16_BYTES_OF_POINTER((unsigned long)ptr));//| ((id)<<48));
        temp_node->next = page->remote_stack.head;
        temp_node =(t_stack_node*) (((unsigned long)temp_node)| ((id)<<48));


    } while (!compare_and_swap_ptr(&(page->remote_stack.head), page->remote_stack.head,temp_node));
    

}


void free(void* ptr){
    // char* buf = "Always failing free\n";
    // write(STDOUT_FILENO,buf,20);

    if(!ptr)return;

    //  if(__glibc_unlikely(ptr == calloc_memory)){
    //     return ;
    // }
 
    //HACK
    if(__glibc_unlikely(ptr == calloc_memory)){
        return;
    }
    //HACK
    if(__glibc_unlikely(ptr == struct_memory)){
        return;
    }



    if(is_large_object(ptr)){
        // write(STDOUT_FILENO,"LARGE OBJECT\n",13);
      
        return large_object_deallocation(ptr);
    }
      
    pageblock* page_block_info = get_page_block_start(ptr);//(pageblock*)page_block_start;

     if(is_double_free(ptr)){
         write(STDOUT_FILENO,"Double free\n",12);
         return;
    }else{
        set_freed_object_header(ptr);
    }

    //REMOTE DEALLOCATION
    if(page_block_info->thread_id!=thread_data.thread_id){

        // write(STDOUT_FILENO,"REMOTE DEALLOCATION\n",20);

        //CHECK if the block is orphaned, and fetch it
        int temp;
        if(temp=atmc_fetch_and_add(&(page_block_info->thread_id),0)){
            
            //FETCHED THE BLOCK SUCCESFULLY
           if(compare_and_swap64((volatile unsigned long long *)&(page_block_info->thread_id),temp,thread_data.thread_id)){

           }
        }else{//BLOCK IS NOT ORPHAN, PUT THE OBJECT IN THE REMOTE FREE LIST


            atomic_push(&(page_block_info->remote_stack),(t_stack_node*)ptr);

        }

    }



    if (page_block_info->free_list_offset==1){
        page_block_info->free_list_offset= ((unsigned long)(ptr));//-((unsigned long)page_block_info));
        *(unsigned long *)(ptr)=1;
    }else{
       

        *((unsigned long *)(ptr)) =page_block_info->free_list_offset;
        page_block_info->free_list_offset = (unsigned long)(ptr);
    }

        page_block_info->num_free_objects++;
        // page_block_info->num_alloc_objects--;

    return; //(*original_free)(ptr);
}




void *operator new(std::size_t count)
{
    return malloc(count);
}

void *operator new[](std::size_t count)
{
    return malloc(count);
}

void *operator new(std::size_t count, const std::nothrow_t &tag)
{
    return malloc(count);
}

void *operator new[](std::size_t count, const std::nothrow_t &tag)
{
    return malloc(count);
}

void operator delete(void *ptr)
{
    free(ptr);
}

void operator delete[](void *ptr)
{
    free(ptr);
}

void operator delete(void *ptr, const std::nothrow_t &tag)
{
    free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t &tag)
{
    free(ptr);
}

void operator delete(void *ptr, std::size_t sz)
{
    free(ptr);
}

void operator delete[](void *ptr, std::size_t sz)
{
    free(ptr);
}
