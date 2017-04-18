//
// Created by Sherif on 4/16/2017.
//

#include "printer.h"
#include "iomanip"
#include <iostream>
#include <sstream>
using namespace std;

vector<pri>  primaryKeys;
vector<sec1> sec1Keys;
vector<sec2> sec2Keys;
vector<inv>  inverted;
fstream data;
fstream primary;
fstream secModel;
fstream secPrice;
fstream invertedList;

printer::printer() {
    ID      = new char[30];
    model   = new char[30];
    description = new char[30];
    price = 0;
    isDeleted = '*';
}

printer::printer(char *IDx, char *modelx, char *desc, float price) {
    ID      = new char[30];
    model   = new char[30];
    description = new char[30];
    strcpy(ID,IDx);
    strcpy(model,modelx);
    strcpy(description,desc);
    printer::price = price;
    isDeleted = '-';
}

printer::printer(const printer &printer1) {
    ID      = new char[30];
    model   = new char[30];
    description = new char[30];
    strcpy(ID,printer1.ID);
    strcpy(model,printer1.model);
    strcpy(description,printer1.description);
    price = printer1.price;
    isDeleted = printer1.isDeleted;
}

std::ostream &operator<<(std::ostream &os, const printer &device1) {
    if (device1.isDeleted == '*') //If it is deleted don't print It.
        return os;
    os << "ID: "      << setw(10) <<  device1.ID
       << " | Model: " << setw(10)<< device1.model
       << " | Description: "<< setw(15) << device1.description
       << " | Price: "<< setw(8) << device1.price
       << " $\n";
    return os;
}

std::istream &operator>>(std::istream &os, printer &device1) {
    cout << "Device ID: ";
    os >> device1.ID;
    os.ignore();
    cout << "Device Model: ";
    os.getline(device1.model, 29);
    cout << "Description: ";
    os.getline(device1.description, 29);
    cout << "Device Price: ";
    os >> device1.price;
    device1.isDeleted = 0;
    return os;
}

printer::~printer() {
    delete[] ID;
    delete[] model;
    delete[] description;
}

bool cmpID(pri &i, pri &j) {
    return i.ID < j.ID;
}

bool cmpModel(sec1 &i, sec1 &j) {
    return i.model < j.model;
}

bool cmpFloat(sec2 &i, sec2 &j) {
    return i.price < j.price;
}

int recursiveBinarySearch(vector<pri> arr, string target, int start, int end) {
    if(start > end)
        return -1;
    int mid = ( start+end ) / 2;
    if (arr[mid].ID == target)
        return mid;
    else if( arr[mid].ID > target )
        return recursiveBinarySearch(arr,target,start,mid-1);
    else
        return recursiveBinarySearch(arr,target,mid+1,end);
}

int recursiveBinarySearch(vector<sec1> arr, string target, int start, int end) {
    if(start > end)
        return -1;
    int mid = ( start+end ) / 2;
    if (arr[mid].model == target)
        return mid;
    else if( arr[mid].model > target )
        return recursiveBinarySearch(arr,target,start,mid-1);
    else
        return recursiveBinarySearch(arr,target,mid+1,end);
}

int recursiveBinarySearch(vector<sec2> arr, float target, int start, int end) {
    if(start > end)
        return -1;
    int mid = ( start+end ) / 2;
    if (arr[mid].price == target)
        return mid;
    else if( arr[mid].price > target )
        return recursiveBinarySearch(arr,target,start,mid-1);
    else
        return recursiveBinarySearch(arr,target,mid+1,end);
}

void addPrinter(printer &printer1, fstream& data){
    data.seekg(0,ios::end);
    int  offset, size = printer1.size() + 3;
    char flag = '-';
    offset = data.tellg();
    if(offset == -1) offset = 0;
    char* buffer = new char[size - sizeof(float)];
    strcpy(buffer, printer1.ID);            strcat(buffer,"#");
    strcat(buffer, printer1.model);         strcat(buffer,"#");
    strcat(buffer, printer1.description);   strcat(buffer,"#");
    data.write((const char*) &size, sizeof(int));
    data.write(&flag,sizeof(char));
    data.write(buffer, size - sizeof(float));
    data.write((const char*)&printer1.price, sizeof(float));
    ///Addint to Primary
    primaryKeys.push_back({printer1.ID, offset});
    sort(primaryKeys.begin(),primaryKeys.end(), cmpID);

    ///Adding to SEC1
    int idx1 = recursiveBinarySearch(sec1Keys,printer1.model,0,sec1Keys.size()-1);
    if(idx1 == -1)
    {
        inverted.push_back({printer1.ID, -1});
        sec1Keys.push_back({printer1.model, (int)inverted.size()-1});
        sort(sec1Keys.begin(), sec1Keys.end(), cmpModel);
    }
    else
    {
        inverted.push_back({printer1.ID, -1});
        int itr = sec1Keys[idx1].offset;
        while(inverted[itr].next != -1)
            itr = inverted[itr].next;
        inverted[itr].next = inverted.size()-1;
    }

    ///Adding to SEC2
    int idx2 = recursiveBinarySearch(sec2Keys,printer1.price,0,sec2Keys.size()-1);
    if(idx2 == -1)
    {
        inverted.push_back({printer1.ID, -1});
        sec2Keys.push_back({printer1.price, (int)inverted.size()-1});
        sort(sec2Keys.begin(), sec2Keys.end(), cmpFloat);
    }
    else
    {
        inverted.push_back({printer1.ID, -1});
        int itr = sec2Keys[idx2].offset;
        while(inverted[itr].next != -1)
            itr = inverted[itr].next;
        inverted[itr].next = inverted.size()-1;
    }

    delete[] buffer;
    return;
}

printer readPrinter(int offset) {
    printer ret;
    data.seekg(offset,ios::beg);
    int size;
    data.read((char*)&size, sizeof(int) );
    data.read(&ret.isDeleted , sizeof(char));
    data.getline(ret.ID, 29, '#');
    data.getline(ret.model, 29, '#');
    data.getline(ret.description, 29, '#');
    data.read((char *) &ret.price, sizeof(float));
    data.seekg(offset+size+sizeof(int)+sizeof(char)); //(Sanity Check) Seek to the end(offset + headers(int and char) + size of the records.
    return ret;
}

bool deletePrinter(string ID) {
    int itr = recursiveBinarySearch(primaryKeys,ID,0,primaryKeys.size()-1);
    if(itr == -1)
        return false;
    else
    {
        char isDeleted = '*';
        int offset = primaryKeys[itr].offset;
        data.seekg(offset+sizeof(int),ios::beg); //seek to offset and escape size cell.
        data.write(&isDeleted, sizeof(char));
        primaryKeys.erase(primaryKeys.begin()+itr); //no need to re-sort.
        return true;
    }
}

bool updatePrinter(printer &newPrinter){
    int oldSize;
    int newSize    = newPrinter.size() + 3;
    int oldOffset  = searchID(newPrinter.ID);

    if(oldOffset == -1) ///If Record(ID) doesn't exist return
        return false;

    printer oldPrinter = readPrinter(oldOffset); ///Old Record.

    data.seekg(oldOffset,ios::beg);
    data.read((char*)&oldSize, sizeof(int)); data.tellg();//<-Fix MacOS bug :/
    if(newSize > oldSize) ///If new is bigger, Write at the end of the file(Seek and write new size)
    {
        deletePrinter(newPrinter.ID);      //Delete Record.
        data.seekg(0,ios::end); //Seek to write at the end.
        int newOffset = data.tellg(); //<-New Offset to Update primary Indexs.
        data.write((const char*) &newSize, sizeof(int));
        primaryKeys.push_back({newPrinter.ID, newOffset });
        sort(primaryKeys.begin(),primaryKeys.end(), cmpID);
    } ///Continue Writing....(If oldSize is bigger (overwrite without seeking to end) )
    char flag = '-';
    char* buffer = new char[newSize - sizeof(float)];
    strcpy(buffer, newPrinter.ID);            strcat(buffer,"#");
    strcat(buffer, newPrinter.model);         strcat(buffer,"#");
    strcat(buffer, newPrinter.description);   strcat(buffer,"#");
    data.write(&flag,sizeof(char));
    data.write(buffer, newSize - sizeof(float));
    data.write((const char*)&newPrinter.price, sizeof(float));
    delete[] buffer;
    ///UPDATE PRIMARY INDEX


    ///UPDATE SECONDARY INDEXS
    if(oldPrinter.model != newPrinter.model)
    {
        ///ADD THE NEW INDEX
        int idx1 = recursiveBinarySearch(sec1Keys,newPrinter.model,0,sec1Keys.size()-1);
        if(idx1 == -1)
        {
            inverted.push_back({newPrinter.ID, -1});
            sec1Keys.push_back({newPrinter.model, (int)inverted.size()-1});
            sort(sec1Keys.begin(), sec1Keys.end(), cmpModel);
        }
        else
        {
            inverted.push_back({newPrinter.ID, -1});
            int itr = sec1Keys[idx1].offset;
            while(inverted[itr].next != -1)
                itr = inverted[itr].next;
            inverted[itr].next = inverted.size()-1;
        }
        ///REMOVE THE OLD INDEX
        int idx2 = recursiveBinarySearch(sec1Keys,oldPrinter.model,0,sec1Keys.size()-1);
        int itr = sec1Keys[idx2].offset; //<-offset in inverted list.
        if(inverted[itr].next == -1) //<-If only one exist (Remove it completely from secondary Index list)
        {
            sec1Keys.erase(sec1Keys.begin() + idx2);
        }
        else if(inverted[itr].ID == oldPrinter.ID)
        {
            sec1Keys[idx2].offset = inverted[itr].next;
        }
        else
        {
            int previous = 0;
            while(itr != -1)
            {
                if(inverted[itr].ID == oldPrinter.ID)
                {
                    inverted[previous].next = inverted[itr].next;
                }
                previous = itr;
                itr = inverted[itr].next;
            }
        }
    }
    if(oldPrinter.price != newPrinter.price)
    {

        ///ADD THE NEW INDEX
        int idx1 = recursiveBinarySearch(sec2Keys,newPrinter.price,0,sec2Keys.size()-1);
        if(idx1 == -1)
        {
            inverted.push_back({newPrinter.ID, -1});
            sec2Keys.push_back({newPrinter.price, (int)inverted.size()-1});
            sort(sec2Keys.begin(), sec2Keys.end(), cmpFloat);
        }
        else
        {
            inverted.push_back({newPrinter.ID, -1});
            int itr = sec2Keys[idx1].offset;
            while(inverted[itr].next != -1)
                itr = inverted[itr].next;
            inverted[itr].next = inverted.size()-1;
        }
        ///REMOVE THE OLD INDEX
        int idx2 = recursiveBinarySearch(sec2Keys,oldPrinter.price,0,sec2Keys.size()-1);
        int itr = sec2Keys[idx2].offset; //<-offset in inverted list.
        if(inverted[itr].next == -1) //<-If only one exist (Remove it completely from secondary Index list)
        {
            sec2Keys.erase(sec2Keys.begin() + idx2);
        }
        else if(inverted[itr].ID == oldPrinter.ID)
        {
            sec2Keys[idx2].offset = inverted[itr].next;
        }
        else
        {
            int previous;
            while(itr != -1)
            {
                if(inverted[itr].ID == oldPrinter.ID)
                {
                    inverted[previous].next = inverted[itr].next;
                }
                previous = itr;
                itr = inverted[itr].next;
            }
        }
    }
    return true;
}

int searchID(string ID) {
    int idx = recursiveBinarySearch(primaryKeys, ID, 0, primaryKeys.size()-1);
    if( idx == -1)
        return -1;
    else
        return primaryKeys[idx].offset;
}

void searchByID(string ID) {
    int idx = recursiveBinarySearch(primaryKeys, ID, 0, primaryKeys.size()-1);
    if( idx == -1)
    {
        cout << "NOT FOUND" << endl;
        return;
    }
    else
    {
        cout << readPrinter(primaryKeys[idx].offset);
        return;
    }
}

void searchByModel(string model) {
    int idx = recursiveBinarySearch(sec1Keys, model, 0, sec1Keys.size()-1);
    if( idx == -1) ///NOT FOUND
        return;
    else
    {
        int itr = sec1Keys[idx].offset;
        while(itr != -1)
        {
            int offset = searchID(inverted[itr].ID);
            if(offset != -1) //If ID exist.
                cout << readPrinter(offset);
            itr = inverted[itr].next;
        }
        return;
    }
}

void searchByPrice(float price) {
    int idx = recursiveBinarySearch(sec2Keys, price, 0, sec2Keys.size()-1);
    if( idx == -1)  ///NOT FOUND
        return;
    else
    {
        int itr = sec2Keys[idx].offset;
        while(itr != -1)
        {
            int offset = searchID(inverted[itr].ID);
            if(offset != -1) //If exist
                cout << readPrinter(offset);
            itr = inverted[itr].next; //Iterate
        }
        return;
    }
}

void searchByModelPrice(string model, float price) {
    int idx = recursiveBinarySearch(sec1Keys, model, 0, sec1Keys.size()-1);
    if( idx == -1)
        return;
    else
    {
        int itr = sec1Keys[idx].offset;
        while(itr != -1)
        {

            int offset = searchID(inverted[itr].ID);
            if(offset != -1){ //If ID Exists...
                printer tmp = readPrinter(offset);
                if(tmp.price == price)
                    cout << tmp;
            }
            itr = inverted[itr].next;
        }
        return;
    }
}

void printAll() {
    for (int i = 0; i < primaryKeys.size(); ++i) {
        cout << readPrinter(primaryKeys[i].offset);
    }
    cout << endl;
}

void load(){
    loadData();
    loadPri();
    loadSec1();
    loadSec2();
    loadInv();
}

void loadData() {
    data.open("data.dat", ios::out | ios::in | ios::binary);
    if(!data.is_open())
    {
        data.open("data.dat", ios::out);
        data.close();
        data.open("data.dat", ios::out | ios::in | ios::binary);
    }
}

void loadPri() {
    primary.open("primary.dat", ios::in | ios::binary);
    primary.seekg(0,ios::end);
    int lastByte = primary.tellg();
    primary.seekg(0,ios::beg);
    while(primary.tellg() != lastByte)
    {
        char* ID = new char[30];
        int offset;
        primary.read(ID,30);
        primary.read((char*)&offset, sizeof(int));
        primaryKeys.push_back({ID,offset});
        delete[] ID;
    }
    primary.close();
}

void loadSec1() {
    secModel.open("sec1.dat", ios::in | ios::binary);
    secModel.seekg(0,ios::end);
    int lastByte = secModel.tellg();
    secModel.seekg(0,ios::beg);
    while(secModel.tellg() != lastByte)
    {
        char* model = new char[30];
        int offset;
        secModel.read(model,30);
        secModel.read((char*)&offset, 4);
        sec1Keys.push_back({model,offset});
        delete[] model;
    }
    secModel.close();
}

void loadSec2() {
    secPrice.open("sec2.dat", ios::in | ios::binary);
    secPrice.seekg(0,ios::end);
    int lastByte = secPrice.tellg();
    secPrice.seekg(0,ios::beg);
    while(secPrice.tellg() != lastByte)
    {
        float price;
        int offset;
        secPrice.read((char*)&price , sizeof(float));
        secPrice.read((char*)&offset, sizeof(int)  );
        sec2Keys.push_back({price,offset});
    }
    secPrice.close();
}

void loadInv() {
    invertedList.open("inv.dat", ios::in | ios::binary);
    invertedList.seekg(0,ios::end);
    int lastByte = invertedList.tellg();
    invertedList.seekg(0,ios::beg);
    while(invertedList.tellg() != lastByte)
    {
        char* ID = new char[30];
        int offset;
        invertedList.read(ID,30);
        invertedList.read((char*)&offset, sizeof(int));
        inverted.push_back({ID,offset});
        delete[] ID;
    }
    invertedList.close();
}

void update(){
    updatePri();
    updateSec1();
    updateSec2();
    updateInv();
}

void updatePri() {
    primary.open("primary.dat", ios::out | ios::binary);
    for (int i = 0; i < primaryKeys.size(); ++i) {
        primary.write(primaryKeys[i].ID.c_str(), 30);
        primary.write((const char*) &primaryKeys[i].offset, sizeof(int));
    }
    primary.close();
}

void updateSec1() {
    secModel.open("sec1.dat", ios::out | ios::binary);
    for (int i = 0; i < sec1Keys.size(); ++i) {
        secModel.write(sec1Keys[i].model.c_str(), 30);
        secModel.write((const char*) &sec1Keys[i].offset, sizeof(int));
    }
    secModel.close();
}

void updateSec2() {
    secPrice.open("sec2.dat", ios::out | ios::binary);
    for (int i = 0; i < sec2Keys.size(); ++i) {
        secPrice.write((const char*) &sec2Keys[i].price , sizeof(float));
        secPrice.write((const char*) &sec2Keys[i].offset, sizeof(int)  );
    }
    secPrice.close();
}

void updateInv() {
    invertedList.open("inv.dat", ios::out | ios::binary);
    for (int i = 0; i < inverted.size(); ++i) {
        invertedList.write(inverted[i].ID.c_str(), 30);
        invertedList.write((const char*) &inverted[i].next, sizeof(int));
    }
    invertedList.close();
}

void visualize() {
    data.seekg(0,ios::end);
    int lastByte = data.tellg();
    data.seekg(0,ios::beg);
    int  size;    char flag;
    while(data.tellg() != lastByte) {
        data.read((char *) &size, sizeof(int));
        data.read(&flag, sizeof(char));
        data.seekg(size, ios::cur); //(Sanity Check) Seek to the end(offset + headers(int and char) + size of the records.
        if (flag == '-')
            cout << "-";
        else
            cout << "*";
    }
    cout << endl;
}

void compactFile() {
    ///DELETE OLD DATA OFF RAM AND RESET.
    primaryKeys.clear();
    sec1Keys.clear();
    sec2Keys.clear();
    inverted.clear();

    ///LOAD RECORDS FROM DATA
    data.seekg(0,ios::end);
    int lastByte = data.tellg();
    data.seekg(0,ios::beg);
    fstream newFile("newData.dat", ios::out | ios::binary); //creating a tmp file.
    while(data.tellg() != lastByte)
    {
        printer tmp = readPrinter(data.tellg());
        if(tmp.isDeleted == '-') {
            addPrinter(tmp, newFile);
        }
    }
    data.close();   newFile.close();
    remove("data.dat"); //delete old file.
    rename("newData.dat","data.dat");//rename the new file.
    remove("newdata.dat"); //delete the new redundant file.
    data.open("data.dat", ios::out | ios::in | ios::binary); //reopen the file.

    ///SAVE NEW DATA TO INDEXS FILES
    updatePri();
    updateSec1();
    updateSec2();
    updateInv();
}
