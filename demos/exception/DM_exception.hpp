#include <iostream>
#include <exception>
#include <string>

class DM_exception : public std::exception {
    
public:
     
    DM_exception(const std::string &msg = "") throw();
    virtual ~DM_exception() throw();

public: 

    void init(const std::string& file, 
              const std::string& func,
              int line
             );
    const char* what() const throw();
    const std::string& get_backtrace();  

protected:

    std::string _msg; 
    std::string _file;
    std::string _what;
    int _line;
};

