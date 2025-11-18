#include <iostream>
#include <string>
#include "WeatherData.h"

using namespace std;

class Assignment2App
{
private:
    WeatherDataCollection weatherData;
    bool dataLoaded;

public:
    Assignment2App() : dataLoaded(false) {}

    void run()
    {
        int choice;
        do
        {
            displayMenu();
            cout << "Enter your choice: ";
            cin >> choice;
            processChoice(choice);
        }

        while (choice != 6);
    }

private:

    void displayMenu()
    {
        cout << "\n=== Assignment 2 Weather Data Analysis ===" << endl;
        cout << "1. Load Weather Data Files" << endl;
        cout << "2. Display All Data" << endl;
        cout << "3. Calculate Pearson Correlation Coefficients" << endl;
        cout << "4. Generate Monthly Statistics Report" << endl;
        cout << "5. Display Data Structure Information" << endl;
        cout << "6. Exit" << endl;
        cout << "==========================================" << endl;
    }
};




int main()
{
    cout << "Hello world!" << endl;
    return 0;
}
