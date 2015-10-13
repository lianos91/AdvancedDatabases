#ifndef DBLAYER_H
#define DBLAYER_H

#include "AbstractDataLayer.h"

#include <pqxx/pqxx>
#include <mutex>

class DBlayer : public AbstractDataLayer {

    public:
        DBlayer(std::string);
        virtual ~DBlayer();
        void connect();
        bool insertItem(int,std::string,int);
        bool updateItem(int,std::string,int);
        void updateItem(int); //update: newbid = 0.9bid
        std::string* selectItem(int);
        int getValue(int);
    protected:

    private:
        pqxx::connection* C;
        std::string dbname;
};

#endif // DBLAYER_H
