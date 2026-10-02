/*
copilot.cpp
C++ program that organizes emails by priority.
Inputs: Text File
Outputs: Text from program
Sources: Copilot / copilot.com
Author: Carson Lemon
Creation Date: 10/1/2026
    */
/*
To run code do 
1. g++ copilot.cpp -o copilot
2. ./copilot < testfile.txt 
*/
#include <iostream>  
#include <vector>     
#include <string>     
#include <sstream>    

using namespace std;  // Lets you write string/vector/cout without the std:: prefix

// Return numeric priority for each sender category
int categoryPriority(const string& cat) {   // Takes the sender category as input
    if (cat == "Boss") return 5;            // Boss emails get the highest priority
    if (cat == "Subordinate") return 4;     // Subordinate emails come next
    if (cat == "Peer") return 3;            // Peer emails are in the middle
    if (cat == "ImportantPerson") return 2; // Important person emails are lower
    return 1;                               // Any other sender gets the lowest priority
}                                          

// Convert MM-DD-YYYY into YYYYMMDD integer
int convertDate(const string& date) {       // Takes a date string 
    int mm, dd, yyyy;                       // Variables to hold month, day, and year
    char dash;                              // Variable to absorb the '-' characters

    stringstream ss(date);                  // Wrap the date string so we can read from it
    ss >> mm >> dash >> dd >> dash >> yyyy; // Read month, dash, day, dash, year in order

    return yyyy * 10000 + mm * 100 + dd;    // Combine into one number, e.g. 20240315
}                                         

// Email structure
struct Email {                              // Groups all the data for one email
    string sender;                          // Sender category 
    string subject;                         // Subject line of the email
    string dateStr;                         // Original date text, kept for printing
    int priority;                           // Numeric priority from the sender category
    int dateValue;                          // Date as a YYYYMMDD number for comparing
};                                         

// Max Heap class
class MaxHeap {                             // Heap where the largest email is on top
private:                                    // Members below are only usable inside the class
    vector<Email> heap;                     // Vector that stores the heap's emails

    // Compare emails by priority first, then date
    bool compareEmails(const Email& a, const Email& b) {  // True if a should rank above b
        if (a.priority != b.priority)       // If the two priorities are different
            return a.priority > b.priority; // the higher priority ranks first

        return a.dateValue > b.dateValue;   // If priorities tie, compare by date value
    }                                       

    // Restore heap upward
    void heapifyUp(int index) {             // Moves a new item up to its correct spot
        while (index > 0) {                 // Keep going until we reach the root
            int parent = (index - 1) / 2;   // Calculate the parent's index

            if (compareEmails(heap[index], heap[parent])) {  // If child outranks parent
                swap(heap[index], heap[parent]);             // swap them
                index = parent;             // Move up to the parent's position
            } else {                        // Otherwise the child is in the right place
                break;                      // Stop moving up
            }                     
        }                                   
    }                                       

    // Restore heap downward
    void heapifyDown(int index) {           // Moves an item down to its correct spot
        int size = heap.size();             // Store how many emails are in the heap

        while (true) {                      // Loop until we break out
            int left = 2 * index + 1;       // Index of the left child
            int right = 2 * index + 2;      // Index of the right child
            int largest = index;            // Assume the current item is the largest

            if (left < size &&              // If the left child exists
                compareEmails(heap[left], heap[largest])) {  // and outranks the current largest
                largest = left;             // Left child becomes the largest
            }                               

            if (right < size &&             // If the right child exists
                compareEmails(heap[right], heap[largest])) { // and outrnks the current largest
                largest = right;            // Right child becomes the largest
            }                              

            if (largest != index) {         // If a child outranks the current item
                swap(heap[index], heap[largest]);  // swap them
                index = largest;            // Continue from the child's position
            } else {                        // Otherwise the item is in the right place
                break;                      // Stop moving down
            }                              
        }                                  
    }                                     

public:                                     // Members below can be used from outside the class
    // Insert email
    void insertEmail(const Email& e) {      // Adds a new email to the heap
        heap.push_back(e);                  // Place the email at the end of the vector
        heapifyUp(heap.size() - 1);         // Move it up to its correct position
    }                                      

    // Get highest-priority email
    Email* getMax() {                       // Returns a pointer to the top email
        if (heap.empty())                   // If there are no emails
            return nullptr;                 // return a null pointer

        return &heap[0];                    // Otherwise return the root (highest priority)
    }                                   

    // Delete highest-priority email
    void deleteMax() {                      // Removes the top email from the heap
        if (heap.empty())                   // If the heap is empty
            return;                         // there is nothng to delete

        heap[0] = heap.back();              // Overwrite the root with the last email
        heap.pop_back();                    // Remove the last slot (now a duplicate)

        if (!heap.empty()) {                // If emails remain
            heapifyDown(0);                 // push the new root down to its place
        }                            
    }                                  

    // Return current size
    int size() const {                      // Reports how many emails are stored
        return heap.size();                 // Return the vector's size
    }                                     
};                                      

int main() {                                // Program entry point
    MaxHeap inbox;                          // Create the heap that acts as the inbox
    string line;                            // Holds each line of input

    while (getline(cin, line)) {            // Read input one line at a time until it ends

        // EMAIL command
        if (line.substr(0, 5) == "EMAIL") { // If the line starts with "EMAIL"

            string data = line.substr(6);   // Take everything after "EMAIL "

            string sender, subject, date;   // Variables for the three fields
            stringstream ss(data);          // Wrap the data so we can split it by commas

            getline(ss, sender, ',');       // Read the sender up to the first comma
            getline(ss, subject, ',');      // Read the subject up to the next comma
            getline(ss, date);              // Read the rest of the line as the date

            Email e;                        // Create a new Email object
            e.sender = sender;              // Store the sender
            e.subject = subject;            // Store the subject
            e.dateStr = date;               // Store the original date text
            e.priority = categoryPriority(sender);  // Look up the numeric priority
            e.dateValue = convertDate(date);        // Convert the date to a number

            inbox.insertEmail(e);           // Add the email to the heap
        }                                 

        // COUNT command
        else if (line.substr(0, 5) == "COUNT") {  // If the line starts with "COUNT"
            cout << "There are "            // Print the start of the message
                 << inbox.size()            // Print how many emails are in the inbox
                 << " emails to read.\n";   // Finish the message and add a newline
        }                                

        // NEXT command
        else if (line.substr(0, 4) == "NEXT") {  // If the line starts with "NEXT"

            Email* e = inbox.getMax();      // Get the highest-priority email (or null)

            if (!e) {                       // If the inbox is empty
                cout << "No emails to read.\n";  // tell the user
            }                            
            else {                          // Otherwise there is an email to show
                cout << "Next email:\n";    // Print a header
                cout << "Sender: " << e->sender << '\n';    // Print the sender
                cout << "Subject: " << e->subject << '\n';  // Print the subject
                cout << "Date: " << e->dateStr << '\n';     // Print the date
            }                               
        }                                
        // READ command
        else if (line.substr(0, 4) == "READ") {  // If the line starts with "READ"
            inbox.deleteMax();              // Remove the top email 
        }                                   
    }                                    

    return 0;                              
}                                          