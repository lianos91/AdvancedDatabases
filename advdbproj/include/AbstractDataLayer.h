#ifndef ABSTRACTDATALAYER_H_INCLUDED
#define ABSTRACTDATALAYER_H_INCLUDED

#include <string>

class AbstractDataLayer{
 public:

        AbstractDataLayer(){};
        virtual ~AbstractDataLayer(){};

        virtual void connect()=0;
        virtual bool insertItem(int,std::string,int)=0; //itemID,description,initial value
        virtual bool updateItem(int,std::string,int)=0; //itemID, bidderName, bidValue
        virtual void updateItem(int)=0; //update: newbid = 0.9bid
        virtual std::string * selectItem(int)=0; //itemID
        virtual int getValue(int)=0; //itemID

    protected:
        std::string dbname;
};

#endif // DATALAYER_H_INCLUDED
