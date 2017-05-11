#include <iostream>
#include <fstream>
#include <cstring>
#include <set>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;



int start_s;
int stop_s;
int timeX;
int total = 0;
#define timeStart()  start_s=clock();
#define timeEnd(s)   stop_s=clock(); timeX = (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000; total+= timeX; cout << s << " -> Time: " << timeX << " MS" << endl;

using namespace std;
fstream data;
int N = 997; int bucketSize = 2;

struct student{
    int ID;
    char name[51];
    student(int IDx, char* namex = NULL)
    {
        ID = IDx;
        if(ID != -1)
            strcpy(name,namex);
    }

    friend ostream &operator<<(ostream &os, const student &student1) {
        os << "ID: " << student1.ID << " name: " << student1.name;
        return os;
    }
};

void initializeFile(int N)
{
    student x(-1);
    for (int i = 0; i < N+1; ++i) {
        for (int j = 0; j < bucketSize; ++j) {
            data.write((const char*)&x, sizeof(x));
        }
    }
}

int foldShiftHash(int ID)
{
    int sum = 0;
    while(ID)
    {
        sum += ID % 1000; //GET THREE DIGITS
        ID /= 1000;
    }
    return sum%N;
}

int nDigits(int id) {
    int n = 0;
    while(id)
    {
        n++;
        id/=10;
    }
    return  n;
}

int midSquareHash(int ID)
{
    double ten = 10;
    ID = ID%10000; //Select a portion of the ID
    ID *= ID;      //Square
    int n = nDigits(ID);
    int maxLocation = nDigits(N);
    int leftPivot  = (n - maxLocation) / 2;
    int rightPivot = (n - maxLocation) % 2 ? leftPivot+1 : leftPivot;
    ID = ID % (int)pow(ten,n-rightPivot);
    ID /= pow(ten,leftPivot);
    return ID;
}

int newLocation(int location, int i, int step)
{
    return ( location + (i*step) ) % N;
}



bool insertInBucket(int offset, student x)
{
    int key;
    for (int i = 0; i < bucketSize; ++i) {
        data.seekg(offset,ios::beg);
        data.read((char*) &key, sizeof(key));
        if(key == -1 || key == -2)
        {
            data.seekg(offset, ios::beg); data.tellg();
            data.write((const char*)&x, sizeof(x));
            return true;
        }
        else
            offset += sizeof(student);
    }
    return false;
}

bool insert(student x)
{
    int i = 0;
    int location = foldShiftHash(x.ID);
    int step = midSquareHash(x.ID);
    int offset = location * sizeof(student) * bucketSize;
    int newOffset = offset;
    do
    {
        if(insertInBucket(newOffset,x))
            return true;
        else
            newOffset = newLocation(location, ++i, step)*sizeof(student) * bucketSize;
    }while(newOffset != offset);
    return false;
}

bool eraseFromBucket(int offset, int ID){
    int key;
    for (int i = 0; i < bucketSize; ++i) {
        data.seekg(offset,ios::beg);
        data.read((char*) &key, sizeof(key));
        if(key == ID){
            data.tellg(); //MacOS BUG FIX;
            int deletedID = -2;
            data.seekg(offset, ios::beg);
            data.write((const char*)&deletedID, sizeof(deletedID));
            return true;
        }
        if(key == -1)
            return false;
        else
            offset += sizeof(student);
    }
    return false;
}


bool erase(int ID)
{
    int i = 0;
    int location = foldShiftHash(ID);
    int step = midSquareHash(ID);
    int offset =  location * sizeof(student) * bucketSize;
    //Start Searching further
    int newOffset = offset;
    do
    {
        if(eraseFromBucket(newOffset,ID))
            return true;
        else
            newOffset = newLocation(newOffset, ++i, step) * sizeof(student) * bucketSize;
    }while(newOffset != offset);
    return false;
}

bool searchInBucket(int offset, int ID)
{
    data.clear();
    int key;
    for (int i = 0; i < bucketSize; ++i) {
        data.seekg(offset,ios::beg);
        data.read((char*) &key, sizeof(key));
        if(key == ID)
            return true;
        if(key == -1)
            return false;
        else
            offset += sizeof(student);
    }
    return false;
}

bool search(int ID)
{
    int i = 0;
    int location = foldShiftHash(ID);
    int step = midSquareHash(ID);
    int offset = location * sizeof(student) * bucketSize;
    //Start Searching further
    int newOffset = offset;
    do
    {
        if(searchInBucket(newOffset,ID))
            return true;
        else {
            newOffset = newLocation(location, ++i, step) * sizeof(student) * bucketSize;
        }
    }while(newOffset != offset);
    return false;
}




int main() {
    //srand(time(NULL));
    data.open("data.txt", ios::out | ios::in | ios::binary);
    if(!data.is_open())
    {
        data.close();
        data.open("data.txt", ios::out);
        initializeFile(N);
        data.close();
        data.open("data.txt", ios::out | ios::in | ios::binary);
    }


    ///Generate RANDOM IDs
    set<int> randomIDs;
    while(randomIDs.size() < 500)
    {
        int ID = 10000+rand()%1000000;
        randomIDs.insert(ID);
    }


    ///INSERT RANDOM IDS
    char s[51] = "SHERIF";
    set<int>::iterator it = randomIDs.begin();
    int i = 0;
    for (; it!=randomIDs.end(); it++) { ;
        if( insert(student(*it,s)) )
            i++;
    }
    cout << "INSERTED:  " << i << " Item. out of : " << randomIDs.size() << " Item." << endl;

    ///PRINT ALL INSERTED
    timeStart();
    fstream allPrint("out.txt", ios::out);
    data.clear();
    data.seekg(0, ios::beg);
    student holder(-1);
    while(!data.eof()) {
        data.read((char *) &holder, sizeof(student));
        if(holder.ID > -1)
            allPrint << holder << endl;
    }
    timeEnd("Print Before Delete")

    ///Search for ALL INSERTED
    timeStart();
    i = 0;
    for (it = randomIDs.begin(); it!=randomIDs.end(); it++) {
        if(search(*it))
            i++;
    }
    timeEnd("Searching for All")
    cout << "FOUND:     " << i << " Item. out of : " << randomIDs.size() << " Item" << endl;



    ///DELETE 50 Record
    timeStart();
    i = 0;
    for (it = randomIDs.begin(); it!=randomIDs.end(); it++) {
        if(erase(*it))
            i++;
        if(i >= 50)
            break;
    }
    timeEnd("Erased 50 Record")


    ///PRINT After Deletion
    timeStart();
    data.clear();
    fstream allAftDelete("outAftrDelete.txt", ios::out);
    data.seekg(0, ios::beg);
    while(!data.eof()) {
        data.read((char *) &holder, sizeof(student));
        if(holder.ID > -1)
            allAftDelete << holder << endl;
    }
    data.clear();
    timeEnd("Print After Delete");


    cout << "Total Time: " << total << endl;
    return 0;
}