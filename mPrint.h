#pragma once

#define mPrint(s) (printf("%s:%d:%s\n",__FILE__,__LINE__,s))
//#define mPrint(s) (printf("%s:%s:%d:%s\n",__DATE__,__FILE__,__LINE__,s))

//#include <iostream>
//using std::cout;
//using std::endl;
//using std::to_string;

//#define mPrint(s) (cout << __FILE__ << ":" << __LINE__ << ":" << s << endl)

// define mPrint to also print the date
//#define mPrint(s) (cout << __FILE__ << ":" << __LINE__ << ":" << __DATE__ << ":" << __TIME__ << ": " << s << endl)
//#define mPrint(s) (cout << __FILE__ << ":" << __LINE__ << ": " << s << endl)
//#define mPrint(s) (cout << __DATE__ << " " << __TIME__ << " | " << __FILE__ << ":" << __LINE__ << ": " << s << endl)
