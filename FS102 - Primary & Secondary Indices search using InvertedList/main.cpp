#include <iostream>
#include "printer.h"

int main() {
    load();
    int choice;
    while(true)
    {
        cout << "-----------------------------------\n"
             << "Welcome, To GSMArena DBMS o8o8o...\n"
             << "1) Print All Devices\n"
             << "2) Add New Device\n"
             << "3) Update Device By ID\n"
             << "4) Delete Device By ID\n"
             << "5) Search for Device By ID\n"
             << "6) Search for Device By Model\n"
             << "7) Search for Device By Price\n"
             << "8) Search for Device By Model & Price\n"
             << "9) Visualize File Fragmentation\n"
             << "10) Compact File\n"
             << "0) Exit\n\n"
             << "->> Please Enter an Option..." << endl;
        cin >> choice;
        printer tmp; string ID,model; float price;
        if(choice == 1)
        {
            printAll();
        }
        else if(choice == 2)
        {
            cin >> tmp;
            addPrinter(tmp, data);
            cout << "Added!" << endl;
        }
        else if(choice == 3)
        {
            cout << "--> Please enter the mode ID -> followed by new info.." << endl;
            cin >> tmp;
            if (updatePrinter(tmp))
                cout << "Updated!" << endl;
            else
                cout << "Device with this ID is not found in database" << endl;
        }
        else if(choice == 4)
        {
            cout << "--> Please enter the printer ID" << endl;
            cin >> ID;
            if(deletePrinter(ID))
                cout << "Deleted!" << endl;
            else
                cout << "Device with this ID is not found in database" << endl;
        }
        else if(choice == 5)
        {
            cout << "--> Please enter the ID." << endl;
            cin >> ID;
            cout << "---------------------------" << endl;
            searchByID(ID);
            cout << "---------------------------" << endl;
        }
        else if(choice == 6)
        {
            cout << "--> Please enter the model." << endl;
            cin >> model;
            cout << "---------------------------" << endl;
            searchByModel(model);
            cout << "---------------------------" << endl;
        }
        else if(choice == 7)
        {
            cout << "--> Please enter the price." << endl;
            cin >> price;
            cout << "---------------------------" << endl;
            searchByPrice(price);
            cout << "---------------------------" << endl;
        }
        else if(choice == 8)
        {
            cout << "--> Please enter the model." << endl;
            cin >> model;
            cout << "--> Please enter the price." << endl;
            cin >> price;
            cout << "---------------------------" << endl;
            searchByModelPrice(model, price);
            cout << "---------------------------" << endl;
        }
        else if(choice == 9)
        {
            visualize();
        }
        else if(choice == 10)
        {
            compactFile();
            cout << "Defragmented!" << endl;
        }
        else if(choice == 0)
        {
            break;
        }
        else
        {
            cout << "--> Please enter a valid choice!" << endl;
        }
    }
    update();
    return 0;
}

