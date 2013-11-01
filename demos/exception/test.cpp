#include "DM_exception.hpp"

using namespace std; 

int main()
{

   try {
    DM_THROW("this is a %s exception %d.", "big", 1);
   } 
   catch(DM_exception e) {
      cout << e.what(); 
   }
    try {
    DM_THROW("this is a %s exception %d.", "small", 2);
   } 
   catch(DM_exception e) {
      cout << e.what(); 
   }  
   
   return 0; 
}

