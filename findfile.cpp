#include <cstdio>
#include <dirent.h>
#include <cstring>
#include <string>
#include <stack>
#include <thread>
#include <mutex>
#include <chrono>
#include <cerrno>

class FBN {

    public:
    FBN(std::string Name);    
    FBN(std::string Name, std::string Path);
    int start_search();
    std::string path = "NULL";
    ~FBN();

    private:
    std::string name = "NULL";              // Wanted file name
    std::string start_path = "/";           // Default start path
    bool answer = 0;                        // Search state
    std::stack<std::string> dirs;           // Stack that contains directoriesfo search
    const int max_threads = 8;              // Max number of working threads at one time
    int tip = 0;                            // Thread in process
    void search_thread(std::string cur_dir);// Thread function
    std::mutex dir_stack;                   // Mutex for preventig data leakage
    std::mutex data;                        //
    
};

FBN::FBN(std::string Name = "file"){

    name = Name;

}

FBN::FBN(std::string Name = "file", std::string Path = "path"){

    name = Name;
    dirs.push(Path);

}

int FBN::start_search(){

    while(!answer){                         // Main thread that monitors seach state
                                            // and start new searching thread when 
        if(dirs.size() > 0){                // there are available threads and dirs

            if(tip < max_threads){

                dir_stack.lock();                                           

                std::thread(&FBN::search_thread, this, dirs.top()).detach();
                dirs.pop();

                dir_stack.unlock();


                data.lock();

                tip++;

                data.unlock();

            }

        } else {                            // Pending report from last thread when dirs stack is empty

            if(tip == 0){
                return 0;
            }

        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); //Optional delay

    }

    return 1;

}

void FBN::search_thread(std::string cur_dir){

    bool f_files = 0,f_dirs = 0;            // Flags for founded files and directories
    std::stack<std::string> dir_dirs;       // Stack of founded dirs
    std::stack<std::string> dir_files;      // Stack of founded files
    std::stack<std::string> buff_s;         // Stack of founded and processed dirs for transfering their to main stack
    std::string result;                     // Intermediary string for diverse operations

    DIR *dr = opendir(cur_dir.c_str());
    struct dirent *dir_ent;

    if(dr == NULL){                         // Checking directory for errors

        //printf("\n\t[%3i] Error when open dir \"%s\"\n\n",errno,cur_dir.c_str());
        data.lock();
        tip--;
        data.unlock();
        closedir(dr);
        return;

    }

    while ( ( dir_ent = readdir(dr) ) != NULL ){

        switch(dir_ent->d_type){
            case /*DT_FIFO */  1:           // Recording everything that is not dir to file's stack
            case /*DT_CHR  */  2:
            case /*DT_BLK  */  6:
            case /*DT_LNK  */ 10:
            case /*DT_SOCK */ 12:
            case /*DT_REG  */  8:{

                dir_files.push(dir_ent->d_name);
                f_files = 1;

                break;
            }
            
            case /*DT_DIR  */  4:{          // Recording dirs
                                            // Rejection current directory (.) and previos directory (..)
                if( strcmp(dir_ent->d_name,".") != 0 && strcmp(dir_ent->d_name,"..") != 0 ){ 

                    dir_dirs.push(dir_ent->d_name);
                    f_dirs = 1;

                }

                break;
            }

            default:break;
        }

    }

    if(f_files){                            // If found files in folder check their for wanted file name
    
        while(dir_files.size() > 0){
        
                                            
            char *pch;                      // Split file name for name and type,
            result = dir_files.top();       // provides dotfile support
            pch = strtok((char*)result.c_str(),".");
    
            if(!strcmp(pch,name.c_str())){
                                            // record finded file path
                path = cur_dir + "/" + dir_files.top();
                answer = 1;
                break;

            }

            dir_files.pop();
    
        }

    }

    if(!answer && f_dirs){                  // Process directory's path and record it to main stack

        while(dir_dirs.size() > 0){

            if(!strcmp(cur_dir.c_str(),"/")){
                result = cur_dir + dir_dirs.top();
            } else {
                result = cur_dir + "/" + dir_dirs.top();
            }
            
            dir_dirs.pop();
            buff_s.push(result);

        }
        
        dir_stack.lock();
        while(buff_s.size() > 0){

            dirs.push(buff_s.top());
            buff_s.pop();

        }
        dir_stack.unlock();

    }

    data.lock();
    tip--;
    data.unlock();
    closedir(dr);
    return;

}


FBN::~FBN(){

}