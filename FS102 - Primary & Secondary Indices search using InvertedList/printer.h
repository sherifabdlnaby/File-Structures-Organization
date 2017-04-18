//
// Created by Sherif on 4/16/2017.
//

#ifndef FS102_PRINTER_H
#define FS102_PRINTER_H


#include <cstring>
#include <ostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

struct pri{string ID; int offset;};
struct sec1{string model; int offset;};
struct sec2{float price; int offset;};
struct inv{string ID;  int next;};
bool cmpID(pri &i, pri &j);
bool cmpModel(sec1 &i, sec1 &j);
bool cmpFloat(sec2 &i, sec2 &j);
int recursiveBinarySearch(vector<pri>  arr, string target, int start, int end);
int recursiveBinarySearch(vector<sec1> arr, string target, int start, int end);
int recursiveBinarySearch(vector<sec2> arr, float target , int start, int end);


extern fstream data;
extern fstream primary;
extern fstream secModel;
extern fstream secPrice;
extern fstream invertedList;
extern vector<pri>  primaryKeys;
extern vector<sec1> sec1Keys;
extern vector<sec2> sec2Keys;
extern vector<inv>  inverted;

class printer {
public:
    char* ID;
    char* model;
    char* description;
    float price;
    char isDeleted;
    virtual ~printer();
    printer();
    printer(char* IDx, char* modelx, char* desc, float price);
    printer(const printer &printer1);
    int size(){return strlen(ID)+strlen(model)+strlen(description)+sizeof(price);}
    friend std::ostream &operator<<(std::ostream &os, const printer &printer1);
    friend std::istream &operator>>(std::istream &is, printer &printer1);
};


void addPrinter(printer &printer1, fstream& data);
printer readPrinter(int offset);
bool deletePrinter(string ID);
bool updatePrinter(printer &newPrinter);


int searchID(string ID);
void searchByID(string ID);
void searchByModel(string model);
void searchByPrice(float price);
void searchByModelPrice(string model, float price);
void printAll();
void visualize();
void compactFile();

void load();
void loadData();
void loadPri();
void loadSec1();
void loadSec2();
void loadInv();

void update();
void updatePri();
void updateSec1();
void updateSec2();
void updateInv();



#endif //FS102_PRINTER_H
