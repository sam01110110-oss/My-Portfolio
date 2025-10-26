// Program to analyze text records and generate report
// Samuel Harrison Parnell
// October 19, 2025

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <cctype>
#include <iomanip>

// Simple class that loads items, counts them, and shows results
class ItemTracker {
private:
    std::map<std::string, int> items; // Stores item name → count

    // Make words lowercase and remove punctuation
    std::string cleanWord(const std::string& word) {
        std::string result;
        for (char c : word) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                result += std::tolower(static_cast<unsigned char>(c));
            }
        }
        return result;
    }

public:
    // Load items from a file and count how many times each appears
    bool load(const std::string& filename) {
        std::ifstream inFile(filename);
        if (!inFile) return false;

        std::string word;
        while (inFile >> word) {
            std::string cleaned = cleanWord(word);
            if (!cleaned.empty()) {
                items[cleaned]++;
            }
        }
        inFile.close();
        return true;
    }

    // Save the item counts to a backup file
    bool saveBackup(const std::string& filename = "frequency.dat") {
        std::ofstream outFile(filename);
        if (!outFile) return false;

        for (auto& pair : items) {
            outFile << pair.first << " " << pair.second << "\n";
        }
        outFile.close();
        return true;
    }

    // Get how many times an item appears
    int getCount(const std::string& name) const {
        std::string cleaned;
        for (char c : name) {
            if (std::isalnum(static_cast<unsigned char>(c))) {
                cleaned += std::tolower(static_cast<unsigned char>(c));
            }
        }

        auto it = items.find(cleaned);
        if (it == items.end()) return 0;
        return it->second;
    }

    // Print all items with their counts
    void printAll() const {
        if (items.empty()) {
            std::cout << "No items loaded.\n";
            return;
        }

        std::cout << "\nItem              Count\n";
        std::cout << "------------------------\n";
        for (auto& pair : items) {
            std::cout << std::left << std::setw(18) << pair.first << pair.second << "\n";
        }
        std::cout << std::endl;
    }

    // Print a histogram using stars (*)
    void printHistogram() const {
        if (items.empty()) {
            std::cout << "No items loaded.\n";
            return;
        }

        std::cout << "\nItem Frequency Histogram:\n";
        for (auto& pair : items) {
            std::cout << std::left << std::setw(15) << pair.first << " ";
            for (int i = 0; i < pair.second; ++i) std::cout << "*";
            std::cout << " (" << pair.second << ")\n";
        }
        std::cout << std::endl;
    }
};

// Display menu and get a valid choice
int menu() {
    int choice;
    while (true) {
        std::cout << "---------------------------\n";
        std::cout << "1. Find an item frequency\n";
        std::cout << "2. Print all item counts\n";
        std::cout << "3. Show histogram\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter choice (1-4): ";

        if (std::cin >> choice && choice >= 1 && choice <= 4) {
            std::cin.ignore(10000, '\n'); // clear leftover input
            return choice;
        }
        std::cout << "Invalid input. Try again.\n";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

int main() {
    std::cout << "Corner Grocer - Item Tracker\n";
    std::cout << "Loading items from file...\n";

    ItemTracker tracker;
    const std::string inputFile = "CS210_Project_Three_Input_File.txt";

    if (!tracker.load(inputFile)) {
        std::cerr << "Error: could not open " << inputFile << "\n";
        return 1;
    }

    // Save backup file
    if (tracker.saveBackup()) {
        std::cout << "Backup saved to frequency.dat\n";
    }
    else {
        std::cout << "Warning: could not save backup file.\n";
    }

    // Menu loop
    while (true) {
        int choice = menu();

        if (choice == 1) {
            std::cout << "Enter item name: ";
            std::string name;
            std::getline(std::cin, name);
            int count = tracker.getCount(name);
            std::cout << "Frequency of \"" << name << "\": " << count << "\n\n";
        }
        else if (choice == 2) {
            tracker.printAll();
        }
        else if (choice == 3) {
            tracker.printHistogram();
        }
        else {
            std::cout << "Goodbye!\n";
            break;
        }
    }

    return 0;
}