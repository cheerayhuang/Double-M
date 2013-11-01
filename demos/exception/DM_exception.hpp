#ifndef _DM_EXCEPTION_HPP_
#define _DM_EXCEPTION_HPP_

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <cstdlib>
#include <cstdio>

#include <execinfo.h>
#include <cxxabi.h>

#define DM_THROW(args...)                                \
    do {                                                 \
        char tmp_str[256];                               \
        snprintf(tmp_str, 256, ##args);                  \
        DM_exception e(tmp_str);                         \
        e.init(__FILE__, __PRETTY_FUNCTION__, __LINE__); \
        throw e;                                         \
    }while(false);                                     


class DM_exception : public std::exception {
    
public:
     
    DM_exception(const std::string &msg="") throw();
    virtual ~DM_exception() throw();

public: 

    void init(const std::string& file, 
              const std::string& func,
              int line);
    const char* what() const throw();
    const std::string& get_call_stack() const;  
    const std::string& get_file() const; 
    const std::string& get_func() const;
    int get_line() const;
    const std::string& get_err_msg() const;

protected:

    std::string msg_; 
    std::string file_;
    std::string func_;
    mutable std::string what_;
    int line_;

private:
    
    enum {MAX_ERR_MSG_SIZE_=256, MAX_CALL_STACK_SIZE_ = 50};

    void* call_stack_[MAX_CALL_STACK_SIZE_];
    size_t call_stack_size_;
    mutable std::string call_stack_str_;
};

DM_exception::DM_exception(const std::string &msg) throw() 
    : msg_(msg), 
      file_("<unknown file>"), 
      func_("<unknown func>"),
      what_(""), 
      line_(-1),
      call_stack_size_(0),
      call_stack_str_("") {

}

DM_exception::~DM_exception() throw() {
}

void DM_exception::init(const std::string& file, 
                        const std::string& func,
                        int line) {   

    file_ = file; 
    func_ = func;
    line_ = line;

    call_stack_size_ = backtrace(call_stack_, MAX_CALL_STACK_SIZE_);
}

const std::string& DM_exception::get_file() const {
    return file_;
}

const std::string& DM_exception::get_func() const {
    return func_;
}

int DM_exception::get_line() const {
    return line_;
}

const char* DM_exception::what() const throw() {

    std::ostringstream tmp_stream;  

    tmp_stream << "[DM_EXCEPTION] ";
    if (!file_.empty()) {
        tmp_stream << file_ << ":"; 
    }

    if (line_ > 0) {
        tmp_stream << line_ << ": ";
    }
 
    if (!func_.empty()) {
        tmp_stream << "IN FUNCTION \"" << func_ << "\": "; 
    }

    if (!msg_.empty()) {
        tmp_stream << msg_ << '\n'; 
    }

    tmp_stream << "[CALL STACK]" << '\n'; 
    tmp_stream << get_call_stack() << '\n'; 

    what_ = tmp_stream.str();

    return what_.c_str();
}

const std::string& DM_exception::get_call_stack() const {

    std::ostringstream tmp_stream; 
    char **results = NULL;

    call_stack_str_ = "<unknown call stack>";

    if (call_stack_size_ > 0) {
        
        results = backtrace_symbols(call_stack_, call_stack_size_);
        if (results == NULL) {
            return call_stack_str_;
        }

        for (int i = 0; i < call_stack_size_; ++i) {
           
            std::string tmp_str(results[i]);
            size_t begin = tmp_str.find('(');
            size_t end = tmp_str.find('+');

            if (begin != std::string::npos && end != std::string::npos) {

                int status = 0; 
                char *s = NULL;
               
                ++begin;
                s = abi::__cxa_demangle(tmp_str.substr(begin, end-begin).c_str(),
                                        NULL, NULL, &status);
                if (status == 0) {
                    tmp_str.replace(begin, end-begin, s);
                }

                std::free(s);
            }
            tmp_stream << "  " << tmp_str << '\n'; 
        }
        call_stack_str_ = tmp_stream.str();
    }
    return call_stack_str_;
}

#endif
