#include <cstdio>
#include <string>
#include "findfile.h"

int main(){

    FBN file_search("File","/");                //Initializing Find By Name object

    if(file_search.start_search()){             //Execution search function that return search state
        printf("%s\n",file_search.path.c_str());
    } else {
        printf("Failed\n");
    }

    return 0;
}