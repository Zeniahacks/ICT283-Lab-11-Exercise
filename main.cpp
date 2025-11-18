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

    // Switch choices following the above display menu as provided
    void processChoice(int choice)
    {

    }


    void loadData()
    {
        string filename;
        cout << "Enter the data source file name: ";
        cin >> filename;

        weatherData.loadFromFiles(filename);
        dataLoaded = true;
        cout << "File name found. Data loading completed." << endl;
    }

    void displayData()
    {
        if (!dataLoaded)
        {
            cout << "Please load the data first (Option 1)." << endl;
            return;
        }

        weatherData.displayAllData();
    }

    void calculateCorrelations()
    {
        if (!dataLoaded)
        {
            cout << "Please load the data first (Option 1)." << endl;
            return;
        }

        int month;
        cout << "Enter month (1-12): ";
        cin >> month;

        if (month < 1 || month > 12)
        {
            cout << "You have entered an invalid month. Please try again." << endl;
            return;
        }

        cout << "\nSample Pearson Correlation Coefficient for Month" << endl;
        cout << "S_T" << weatherData.calculateSPCC(month, "S_T") << endl;
        cout << "S_R" << weatherData.calculateSPCC(month, "S_R") << endl;
        cout << "T_R" << weatherData.calculateSPCC(month, "T_R") << endl;
    }
};




int main()
{
    cout << "ICT283 Lab 11 Exercise" << endl;
    cout << "======================" << endl;

    Assignment2App app;
    app.run();

    return 0;
}
