#include <cstring>
#include <iostream>
#include <iomanip>
#include "Device.h"

using namespace std;

//Global Variables
fstream data;
int availList;
int lastByte;
const int cellHeaderSize = sizeof(int) + sizeof(availList); //Cell Header = Int(for size) + size for availList;

Device::Device() {
    ID = new char[20];
    name = new char[30];
    brand = new char[30];
    price = 0;
}

Device::Device(bool isDeleted) {
    ID = new char[0];
    name = new char[0];
    brand = new char[0];
    price = 0;
    this->isDeleted = isDeleted;
}

Device::Device(const Device &obj) {
    ID = new char[20];
    name = new char[30];
    brand = new char[30];
    strcpy(ID, obj.ID);
    strcpy(name, obj.name);
    strcpy(brand, obj.brand);
    price = obj.price;
    isDeleted = obj.isDeleted;
}

Device &Device::operator=(const Device &obj) {
    strcpy(ID, obj.ID);
    strcpy(name, obj.name);
    strcpy(brand, obj.brand);
    price = obj.price;
    isDeleted = obj.isDeleted;
    return *this;
}

std::ostream &operator<<(std::ostream &os, const Device &device1) {
    if (device1.isDeleted) //If it is deleted don't print It.
        return os;
    os << "ID: "      << setw(6) <<  device1.ID
       << " | Name: " << setw(11)<< device1.name
       << " | Brand: "<< setw(8) << device1.brand
       << " | Price: "<< setw(8) << device1.price
       << " $\n";
    return os;
}

std::istream &operator>>(std::istream &os, Device &device1) {
    cout << "Device ID: ";
    os >> device1.ID;
    os.ignore();
    cout << "Device Name: ";
    os.getline(device1.name, 29);
    cout << "Device Brand: ";
    os.getline(device1.brand, 29);
    cout << "Device Price: ";
    os >> device1.price;
    device1.isDeleted = 0;
    return os;
}

bool initializeFile() {
    data.open("data.dat", ios::out | ios::in | ios::binary); //Open file.
    if (data.is_open()) {
        data.read((char *) &availList, sizeof(int)); //Read header into availList.
        data.seekg(0, ios::end);
        lastByte = data.tellg();//Set lastByte.
        return true;
    }
    return false; //File couldn't be opened or doesn't exist...
}

bool createFile() {
    int initialAvailListHeader = -1; //Initial empty availList value;
    data.open("data.dat", fstream::out | fstream::trunc); //Create New File.(Truncate if it exist)
    data.write((const char *) &initialAvailListHeader, sizeof(initialAvailListHeader)); //Add -1 in File's Header
    data.close();//close file.
    return initializeFile(); //initialize files and global variables.
}

int compactFile() {
    vector<Device> allDevices = loadAllDevices(); //load all all not-deleted records.
    data.close(); //Close existing file
    createFile(); //Create new files with new availList, etc.
    for (int i = 0; i < allDevices.size(); ++i) {
        writeDevice(allDevices[i]);
    }
    return 0;
}


int checkAvail(int size) {
    int offset = availList; //Header
    int recordSize;
    int lastOffset;
    int nextOffset;
    while (offset > -1) //CHECK (-1 = availList is empty)
    {
        data.seekg(offset, ios::beg);
        data.read((char *) &recordSize, sizeof(int));
        data.read((char *) &nextOffset, sizeof(int));
        if (recordSize >= size) {
            if(offset == availList)     //If it is the availList header.
                updateAvailList(nextOffset); //set a New AvailList Header
            else //else Update lastOffset with the NextOffset (to remove current offset from the LinkedList)
            {
                data.seekp(lastOffset+ sizeof(int),ios::beg);
                data.write((const char*)&nextOffset, sizeof(int)); //Write the nextOffset to LastOffset;
            }
            return offset;
        } else
        {
            lastOffset = offset; //Save last-offset
            offset = nextOffset; //Iterate
        }
    }
    return -1;
}

void updateAvailList(int newOffset) {
    availList = newOffset;      //Update global Variable.
    int origin = data.tellg();  //Save your Current position
    data.seekg(0, ios::beg);    //Seek to the File's Header
    data.write((const char *) &newOffset, sizeof(int)); //Update file's Header.
    data.seekg(origin, ios::beg); //Seek Back to origin (Current Position).
}

int Device::getRecordSize() const {
    return strlen(ID) + strlen(name) + strlen(brand) + 3 + sizeof(float);
}

Device readDevice() {
    Device device;
    int size,flag;
    int offset = data.tellg(); //Current Pos.
    data.read((char *) &size, sizeof(int));
    data.read((char *) &flag, sizeof(int));
    if (flag > -2) { //If flag > -2 == file is deleted!.
        data.seekg(size, ios::cur); //Seek to the end of this deleted record.
        return Device(1); //returns empty device with flag = deleted;
    } else {
        data.getline(device.ID, 19, '#');
        data.getline(device.name, 29, '#');
        data.getline(device.brand, 29, '#');
        data.read((char *) &device.price, sizeof(float));
        data.seekg(offset + size + cellHeaderSize, ios::beg); //Seek to the end of the record. (In case there was an internal fragmentation)
        return device;
    }
}

void writeDevice(const Device &obj) {
    int flag = -2; //-2 = not Deleted obj.
    int recordSize = obj.getRecordSize();
    int offset = checkAvail(recordSize); //Get a big enough empty space from availList (returns -1 if list is empty)
    if (offset != -1) { //There is a big enough deleted Record in availList for the new one.
        data.seekg(offset + sizeof(int), ios::beg); //Skip sizeCell in recordHeader(We don't change oldRecord size)
        data.write((const char *) &flag, sizeof(int));
        data << obj.ID << '#' << obj.name << '#' << obj.brand << "#";
        data.write((const char *) &obj.price, sizeof(float));
        return;
    } else { //Append at the end of the file.
        data.seekg(0, ios::end);
        data.write((const char *) &recordSize, sizeof(int));
        data.write((const char *) &flag, sizeof(int));
        data << obj.ID << '#' << obj.name << '#' << obj.brand << "#";
        data.write((const char *) &obj.price, sizeof(float));
        lastByte = data.tellg(); //Update lastByte global-var.
        return;
    }
}

bool deleteDevice(char *ID) {
    int flag;
    data.seekg(sizeof(availList), ios::beg);
    while (data.tellg() != lastByte) {
        int offset = data.tellg();
        Device holder = readDevice();
        if (!(strcmp(holder.ID, ID))) {
            flag = availList; //add current availList header to current record.
            data.seekg(offset + sizeof(int), ios::beg); //Seek to flag byte. (Offset + sizeByte)
            data.write((const char *) &flag, sizeof(int));
            updateAvailList(offset); //Update AvailList with the new deleted record.
            return true;
        }
    }
    return false; //Device with ID not found.
}

bool updateDevice(char *ID, Device &obj) {
    int size, flag = -2; //-2 = not Deleted Flag
    data.seekg(sizeof(availList), ios::beg);
    while (data.tellg() != lastByte) { //Search for Device.
        int offset = data.tellg();
        Device holder = readDevice();
        if (!(strcmp(holder.ID, ID))) {
            data.seekg(offset, ios::beg); //Seek to offset of the Record
            data.read((char *) &size, sizeof(int));
            data.seekg(offset+ sizeof(size), ios::beg); //Seek to flagCell directly before writing
                                                        //^FIXES MacOS/XcodeCompiler misbehaviour #TODO
                                                        // working as expected on Windows(minGW), only MacOs/XcodeCompiler (Mixing read and write without seeking seeks the pointers to eof)
            //data.tellg();                             //<-FIXES MacOS/XcodeCompiler misbehaviour too (Surprisingly?)
            if (size >= obj.getRecordSize()) { //New record can fit into old record.
                data.write((const char *) &flag, sizeof(int));
                data << obj.ID << '#' << obj.name << '#' << obj.brand << "#";
                data.write((const char *) &obj.price, sizeof(float));
                return true;
            } else { //New Record is bigger, mark it Deleted.
                data.write((const char *) &availList, sizeof(int)); //Add last AvailList record to the the deleted file.
                updateAvailList(offset); // Update AvailList Header.
                writeDevice(obj); //Write it at the end or in an availList record.
                return true;
            }
        }
    }
    return false;
}

pair<Device, bool> searchDevice(char *ID) {
    data.seekg(sizeof(availList), ios::beg); //Read from the beginning of the file(and Skip availList Header).
    while (data.tellg() != lastByte) {
        Device holder = readDevice();
        if (!(strcmp(ID, holder.ID)))
            return make_pair(holder, true); //return found device + ture flag.
    }
    return make_pair(Device(1), false);     //return empty device with isDeleted = 1, and false flag.
}

vector<Device> loadAllDevices(){
        data.seekg(sizeof(availList), ios::beg); //Read from the beginning
        vector<Device> allDevices;
        Device holder;
        while (data.tellg() != lastByte) {
            holder = readDevice();
            if(!holder.isDeleted)
            {
                allDevices.push_back(holder); //Add non-deleted devices.
            }
        }
    return allDevices;
};

void printAllDevice() {
    data.seekg(sizeof(availList), ios::beg); //Read from the beginning of the file(and Skip availList Header)
    while (data.tellg() != lastByte) {
        cout << readDevice();
    }
    return;
}