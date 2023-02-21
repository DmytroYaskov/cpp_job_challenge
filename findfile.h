#ifndef FINDFILE_H_INCLUDED
#define FINDFILE_H_INCLUDED

#include <string>
#include <stack>
#include <mutex>

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
    const int max_threads = 8;              // Max number of working threads in one time
    int tip = 0;                            // Thread in process
    void search_thread(std::string cur_dir);// Thread function
    std::mutex dir_stack;                   // Mutex for preventig data leakage
    std::mutex data;                        //
    
};

#endif /* FINDFILE_H_INCLUDED */