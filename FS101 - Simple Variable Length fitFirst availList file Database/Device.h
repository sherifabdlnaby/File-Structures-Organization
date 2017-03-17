//
// Created by Sherif Abdel-Naby on 3/11/2017.
//
#ifndef ASSI_DEVICE_H
#define ASSI_DEVICE_H

#include <fstream>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

//Global Variables.
class  Device;
extern fstream data;
extern int availList;
extern int lastByte;
extern const int cellHeaderSize;

//File related Functions.
void updateAvailList(int newOffset); //update both the Global Variable and FileHeader
bool initializeFile();
bool createFile();
int  compactFile();

//Devices related Functions.
Device readDevice();
void   writeDevice(const Device &obj);      //Write Devices into file.
bool   deleteDevice(char *ID);              //Delete Device by ID.
bool   updateDevice(char *ID, Device &obj); //Update Device by ID.
void   printAllDevice();                    //Prints all non-Deleted Devices.
vector<Device> loadAllDevices();            //load All non-Deleted Devices into a vector.
pair<Device, bool> searchDevice(char *ID);  //Search for Device by-ID and return a pair of<TheDevice,True/False flag>

//Class Declaration
class Device {
public:
    char *ID;
    char *name;
    char *brand;
    float price;
    bool isDeleted = 0; //a flag to indicate if this record is deleted from the file.
    Device(); //a record is not deleted by default.
    Device(bool isDeleted); //a record is not deleted by default.
    Device(const Device &obj); //copyConstructor
    Device &operator=(const Device &obj);
    int getRecordSize() const; //Return record size in bytes + delimiters.
    friend std::ostream &operator<<(std::ostream &os, const Device &device1); //cout <<
    friend std::istream &operator>>(std::istream &os, Device &device1); //cin <<
};

#endif //ASSI_DEVICE_H
