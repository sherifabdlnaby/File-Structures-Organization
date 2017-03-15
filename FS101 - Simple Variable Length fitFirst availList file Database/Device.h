//
// Created by Sherif Abdel-Naby on 3/11/2017.
//
#ifndef ASSI_DEVICE_H
#define ASSI_DEVICE_H

#include <fstream>
#include <iostream>
#include <list>

using namespace std;

//Global Variables.
class Device;
extern fstream data;
extern int availList;
extern int lastByte;
extern const int cellHeaderSize;

//File related Functions.
void updateAvailList(int newOffset);
bool initializeFile();
bool createFile();

//Devices related Functions.
Device readDevice();
void writeDevice(const Device &obj);
bool deleteDevice(char *ID);
bool updateDevice(char *ID, Device &obj);
void printAllDevice();
pair<Device, bool> searchDevice(char *ID);

class Device {
public:
    char *ID;
    char *name;
    char *brand;
    float price;
    bool isDeleted = 0; //a flag to indicate if this record is deleted from the file.
    Device(bool isDeleted = 0); //a record is not deleted by default.
    Device(char *ID, char *name, char *brand, float price);
    Device(const Device &obj);
    int getRecordSize() const; //Return record size in bytes + delimiters.
    Device &operator=(const Device &obj);
    friend std::ostream &operator<<(std::ostream &os, const Device &device1);
    friend std::istream &operator>>(std::istream &os, Device &device1);
};

#endif //ASSI_DEVICE_H
