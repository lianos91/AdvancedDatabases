#include <fstream>
#include <string>
#include <time.h>
#include "logger.h"

#include <iostream>

using namespace std;

logger::logger(string logfilename)
{
    logfile.open("log_"+ logfilename +".out", fstream::out);
    logfile << "========================================================\n" ;
}

logger::~logger()
{
    logfile.close();
}

string logger::logTime(void) {

    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
    std::string str(buffer);

    return "[" + str + "] ";
}

void logger::log(string message){
    cout << "[] "+message << endl;
    logfile << logTime()+message+"\n";
    return;
}
void logger::close() {

    return;
}
