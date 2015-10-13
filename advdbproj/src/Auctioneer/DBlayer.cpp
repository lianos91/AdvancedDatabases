#include "DBlayer.h"
#include "AbstractDataLayer.h"

#include <string>
#include <iostream>       // std::cout
#include <vector>         //
#include <fstream>
#include <pqxx/pqxx>
#include <unistd.h>
#include <mutex>


using namespace std;
using namespace pqxx;


DBlayer::DBlayer(string _dbname) : AbstractDataLayer() {
    dbname = _dbname;
}

DBlayer::~DBlayer()
{
    C->disconnect();
    delete(C);
}

void DBlayer::connect(){
  try{
        C = new connection("dbname="+dbname+" user=postgres password=postgres\
        hostaddr=127.0.0.1 port=5432");
        if (C->is_open()) {
            cout << "Opened database successfully: " << C->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            exit(-1);
        }
       //C.disconnect();
    }catch (const std::exception &e){
        cerr << e.what() << std::endl;
        exit(-1);
    }
    string sql = "TRUNCATE TABLE items;";
    work W(*C);
    W.exec( sql );
    W.commit();
}

bool DBlayer::insertItem(int itemid, string description, int price){
    /*execute INSERT query*/
    string sql = "INSERT INTO items (itemid,description,curr_bid,curr_bidder) "\
            "VALUES ("+to_string(itemid)+",'"+to_string(description) +"',"+to_string(price)+",'no_holder');";
    work W(*C);
    W.exec( sql );
    W.commit();

    return true;
}

bool DBlayer::updateItem(int itemID,string bidder, int newbid){

        //UPDATE query
        /* Create a transactional object. */

try{
        /** WORK ERROR**/
        work W(*C);

        /* Create  SQL UPDATE statement */
        string sql = "UPDATE ITEMS SET curr_bid="+to_string(newbid)+", curr_bidder='"+bidder+
             +"' WHERE itemID="+to_string(itemID)+";";
        /* Execute SQL query */
        W.exec(sql);

        W.commit();
 }
catch (const std::exception &e){
    cout << e.what() << endl;
    return false;
}
    return true;
}

void DBlayer::updateItem(int itemID){

    //UPDATE query
    string sql = "UPDATE ITEMS SET curr_bid=curr_bid*0.9 WHERE itemID="+to_string(itemID)+";";
    work W(*C);
    W.exec(sql);
    W.commit();

    return;
}

string* DBlayer::selectItem(int itemID){
    string* res = new string[4];

    string sql = "SELECT * FROM ITEMS WHERE itemID="+to_string(itemID)+";";
    nontransaction N(*C);
    result R( N.exec( sql ));

    result::const_iterator c = R.begin();

    if (c==R.end()){ //no items found
        return res;
    }
    res[0] = c[0].as<string>();
    res[1] = c[1].as<string>();
    res[2] = c[2].as<string>();
    res[3] = c[3].as<string>();

    return res;
}

int DBlayer::getValue(int itemID){
    int res = -1;

    string sql = "SELECT * FROM ITEMS WHERE itemID="+to_string(itemID)+";";
    nontransaction N(*C);
    result R( N.exec( sql ));

    result::const_iterator c = R.begin();

    if (c==R.end()){ //no items found
        return res;
    }

    res = c[2].as<int>();

    return res;

}
