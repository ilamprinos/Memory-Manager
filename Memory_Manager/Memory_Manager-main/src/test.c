//  #include <stdlib.h>

#include "allocator.h"


int main(int argc, char** argv){

    // int temp[8*4096];
    char* buf = "CALL---\n";
//     // write(STDOUT_FILENO,buf,15);



    // int* p = (int*)malloc(sizeof(int));
//     // *p = 24;
    int * k[20000];

    // temp = *p;
    int* t;
    int* p=NULL;
    for(int i =0;i<20000;i++){
        // int* p = (int*)malloc(sizeof(int));
        // write(STDOUT_FILENO,buf,8);
        t = (int*)calloc(1,sizeof(int));
        if(t == NULL){
            write(STDOUT_FILENO,buf,30);     
            break;
        }
        for(int j =0;j<1;j++){
            t[j]=j;
        }           
        
        // *t=4;
        k[i]=t;
        // free(t);
        // fflush(stdout);
        // free(t);
    }


    // int* klapa = (int*)calloc(sizeof(int)*1000,1);

    for(int i =0;i<20000;i++){
        if(((unsigned long)k[i])%16!=0){
            write(STDOUT_FILENO,"EEE\n",4);

        }
        free(k[i]);
    }
    // free(k[0]);


    int *kre = (int*)malloc(2045);
    free(kre);
    // free(kre);
    // free(klapa);

//       for(int i =0;i<200;i++){

//         k[i]= malloc(sizeof(int));
//         // for(int j =0;j<511;j++){
//         //     k[i][j]=j;
//         // }           
//         *k[i] =i;
//     }

// //  for(int i =50;i<100;i++){
// //      free(k[i]);
// //  }

//   for(int i =0;i<200;i++){
//       free(k[i]);
//   }


// for(int j=0;j<1000;j++){

 for(int i =0;i<1000;i++){
     k[i]= malloc(10*sizeof(int));

 }
  for(int i =0;i<1000;i++){
      free(k[i]);
  }

// }





// //     // free(k[]);
// //  for(int i =0;i<20;i++){
// //      free(k[i]);
// //  }

// // 
//     // t = malloc(sizeof(int)*511);
//     // for(int j =0;j<511;j++){
//     //         t[j]=j*10;
//     // }           

//     // free(t);

    // int* p = (int*)malloc(sizeof(int)*511);
    // // for(int j =0;j<511;j++){
    // //         p[j]=j;
    // // }

    // free(p);     


    


    return 0;
}