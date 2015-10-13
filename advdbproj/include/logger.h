#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

class logger
{
    public:
        logger(std::string);
        virtual ~logger();

        void close();
        void log(std::string);

    protected:

    private:

        std::ofstream logfile;
        std::string logTime(void);
};

#endif // LOGGER_H
