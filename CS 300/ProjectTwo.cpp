//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Samuel Harrison Parnell
// Course      : CS 300
// Description : ABCU Advising Assistance Program
//
// Purpose:
// This program allows academic advisors to load course data from a file,
// display a sorted list of courses, and retrieve detailed information
// about individual courses including their prerequisites.
//
// The program uses a vector data structure to store course objects and
// provides a menu-driven interface for user interaction.
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

//============================================================================
// Structure: Course
// Purpose  : Represents a single course in the Computer Science program.
//            Stores the course number, title, and any prerequisite courses.
//============================================================================
struct Course {
    // Unique course identifier (e.g., CS300)
    string courseNumber;
    // Full course title
    string courseTitle;
    // List of prerequisite course numbers
    vector<string> prerequisites;
};

//============================================================================
// Function Prototypes
//============================================================================
void displayMenu();
void loadCourses(const string& filename, vector<Course>& courses);
void printAllCourses(vector<Course>& courses);
void printCourseInfo(const vector<Course>& courses, const string& courseNumber);

//============================================================================
// Function: main
// Purpose : Controls program execution and handles user interaction.
//============================================================================
int main() {


    // Stores all loaded course data
    vector<Course> courses;
    // User menu selection
    int choice = 0;
    // Name of input file
    string filename;
    // Course lookup input
    string courseNumber;

    // Main program loop continues until user selects Exit (9)
    while (choice != 9) {

        // Show menu options
        displayMenu();
        if (!(cin >> choice)) {
            // clear error flags
            cin.clear();
            // discard bad input
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        cin.ignore(1000, '\n');

        switch (choice) {

        case 1:
            // Load course data from file
            cout << "Enter the course data file name: ";
            cin >> filename;
            loadCourses(filename, courses);
            break;

        case 2:
            // Print all courses in sorted order
            if (courses.empty()) {
                cout << "No courses loaded. Please load a file first." << endl;
            }
            else {
                printAllCourses(courses);
            }
            break;

        case 3:
            // Print specific course information
            if (courses.empty()) {
                cout << "No courses loaded. Please load a file first." << endl;
            }
            else {
                cout << "Enter course number: ";
                cin >> courseNumber;
                printCourseInfo(courses, courseNumber);
            }
            break;

        case 9:
            // Exit program
            cout << "Exiting program. Goodbye!" << endl;
            break;

        default:
            // Handles invalid menu selections
            cout << "Invalid option. Please select 1, 2, 3, or 9." << endl;
        }
    }

    return 0;
}

//============================================================================
// Function: displayMenu
// Purpose : Displays available user options for the advising system.
//============================================================================
void displayMenu() {

    cout << "\n--- ABCU CS Advising Assistance Menu ---" << endl;
    cout << "1. Load course data from file" << endl;
    cout << "2. Print all courses (alphanumeric order)" << endl;
    cout << "3. Print course information" << endl;
    cout << "9. Exit" << endl;
    cout << "Enter your choice: ";
}

//============================================================================
// Function: loadCourses
// Purpose : Reads course data from a CSV file and stores it in a vector.
// Input   : filename - Name of the file to read
//           courses  - Reference to vector storing course data
// Notes   : Existing course data is cleared before loading new data.
//============================================================================
void loadCourses(const string& filename, vector<Course>& courses) {

    ifstream file(filename);

    // Validate file opening
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }

    // Clear any previously loaded data to prevent duplication
    courses.clear();

    string line;

    // Process file line-by-line
    while (getline(file, line)) {

        // Allows parsing of comma-separated values
        stringstream ss(line);
        string token;

        // Create new course object for each line
        Course course;

        // Extract course number (first field)
        getline(ss, course.courseNumber, ',');

        // Extract course title (second field)
        getline(ss, course.courseTitle, ',');

        // Extract remaining fields as prerequisites (if any exist)
        while (getline(ss, token, ',')) {
            if (!token.empty()) {
                course.prerequisites.push_back(token);
            }
        }

        // Add populated course object to vector
        courses.push_back(course);
    }

    file.close();
    cout << "Course data successfully loaded." << endl;
}

//============================================================================
// Function: printAllCourses
// Purpose : Sorts and prints all courses in alphanumeric order.
// Notes   : Sorting is performed by course number using a lambda expression.
//============================================================================
void printAllCourses(vector<Course>& courses) {

    // Sort courses by course number (ascending order)
    sort(courses.begin(), courses.end(),
        [](const Course& a, const Course& b) {
            return a.courseNumber < b.courseNumber;
        });

    cout << "\nCourse List:" << endl;

    // Display sorted course list
    for (const Course& course : courses) {
        cout << course.courseNumber << " - " << course.courseTitle << endl;
    }
}

//============================================================================
// Function: printCourseInfo
// Purpose : Displays detailed information for a specific course.
// Input   : courses       - Vector containing all course data
//           courseNumber  - Course to search for
// Notes   : Performs a linear search through the vector.
//============================================================================
void printCourseInfo(const vector<Course>& courses, const string& courseNumber) {

    // Tracks whether course is located
    bool found = false;

    // Search through course list
    for (const Course& course : courses) {

        if (course.courseNumber == courseNumber) {

            found = true;

            cout << "\nCourse: " << course.courseNumber
                << " - " << course.courseTitle << endl;

            // Display prerequisite information
            if (course.prerequisites.empty()) {
                cout << "Prerequisites: None" << endl;
            }
            else {
                cout << "Prerequisites: ";
                for (const string& prereq : course.prerequisites) {
                    cout << prereq << " ";
                }
                cout << endl;
            }

            break;  // Stop searching once found
        }
    }

    // Inform user if course does not exist
    if (!found) {
        cout << "Course not found." << endl;
    }
}