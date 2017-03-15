#include <iostream>
#include "Device.h"
using namespace std;

int main() {

    if (!(initializeFile())) //If you can't Open and Initialize the file.
        if (!(createFile())) { //Create new one and Initialize it, If you can't Stop the Program.
            cout << "data.dat is not found and can't be created. Please check your operating system permissions." << endl;
            return 0;
        }
    //////////ValueHolderVars//////////
    int n = 0;
    char IDHolder[20];
    Device placeHolder;
    ///////////////////////////////////
    while(n != 6)
    {
        cout << "-----------------------------------\n"
             << "Welcome, To GSMArena DBMS o8o8o...\n"
             << "1) Add New Device\n"
             << "2) Update Device By ID\n"
             << "3) Delete Device By ID\n"
             << "4) Search for Device By ID\n"
             << "5) Print All Devices\n"
             << "6) Compact File\n"
             << "7) Exit\n\n"
             << "->> Please Enter an Option..." << endl;
        cin >> n;
        if(n == 1)
        {
            cout << "Enter Device Info: " << endl;
            cin >> placeHolder;
            writeDevice(placeHolder);
            cout << "Successful!" << endl;
        }
        else if(n == 2)
        {
            cout << "Enter ID: " << endl;
            cin >> IDHolder;
            cout << "Enter New Device Info: ";
            cin >> placeHolder;
            if( updateDevice(IDHolder,placeHolder) )
                cout << "Successful Updated!" << endl;
            else
                cout << "Device Not Found!" << endl;
        }
        else if(n == 3)
        {
            cout << "Enter ID: " << endl;
            cin >> IDHolder;
            if ( deleteDevice(IDHolder) )
                cout << "Successful Deleted!" << endl;
            else
                cout << "Device Not Found!" << endl;
        }
        else if(n == 4)
        {
            cout << "Enter ID: " << endl;
            cin >> IDHolder;
            pair<Device,int> search = searchDevice(IDHolder);
            if( search.second )
                cout << search.first << endl;
            else
                cout << "Device Not Found!" << endl;
        }
        else if(n == 5)
        {
            printAllDevice();
        }
        else if (n == 6)
        {
            //compactDeviceFile(); //TODO
        }
        else if (n == 7)
        {
            break;
        }
        else
        {
            cout << "Invalid option!." << endl;
            continue;
        }
    }
    return 0;
}





