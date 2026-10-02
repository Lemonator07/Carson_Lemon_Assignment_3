/*
claude.cpp
C++ program that organizes emails by priority.
Inputs: Text File
Outputs: Text from program
Sources: Claude / claude.ai
Author: Carson Lemon
Creation Date: 10/1/2026
    */
/*
To run code do 
1. g++ claude.cpp -o claude
2. ./claude < testfile.txt 
*/

/* ============================================================
   email_priority.cpp

   A priority-queue based Email Inbox system for a busy CEO.
   Implemented with a hand-written vector based MaxHeap
   (no std::priority_queue or <algorithm> heap functions used).

   Commands supported (read one per line from a test file):
     EMAIL <sender category>,<subject line>,<date MM-DD-YYYY>
     NEXT
     READ
     COUNT

   Priority order (highest to lowest):
     Boss > Subordinate > Peer > ImportantPerson > OtherPerson
   Ties (same category) are broken by date - newest first.

   Compile:  g++ -std=c++11 -o email_priority email_priority.cpp
   Run:      ./email_priority testfile.txt
             (or)  ./email_priority < testfile.txt
   ============================================================ */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

/* ---------------- Email record ---------------- */

struct Email {
    std::string sender;    // category string, e.g. "Boss"
    std::string subject;
    std::string date;      // original MM-DD-YYYY string
    int  priority;         // numeric priority, higher = more important
    long dateValue;        // YYYYMMDD form, higher = newer
};

/* ---------------- MaxHeap (vector based) ---------------- */

class MaxHeap {
public:
    void insert(const Email &e) {
        data_.push_back(e);
        heapifyUp(static_cast<int>(data_.size()) - 1);
    }

    // Removes and returns the max element. Returns false if empty.
    bool extractMax(Email &out) {
        if (data_.empty()) return false;

        out = data_[0];
        data_[0] = data_.back();
        data_.pop_back();
        if (!data_.empty()) {
            heapifyDown(0);
        }
        return true;
    }

    // Peek at the max element without removing it. Returns false if empty.
    bool peekMax(Email &out) const {
        if (data_.empty()) return false;
        out = data_[0];
        return true;
    }

    int count() const {
        return static_cast<int>(data_.size());
    }

private:
    std::vector<Email> data_;

    // Returns true if a should be HIGHER priority than b
    // (i.e. a should sit above/before b in the max-heap).
    static bool isHigherPriority(const Email &a, const Email &b) {
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }
        // same category -> newest date wins
        return a.dateValue > b.dateValue;
    }

    // Move the element at index up until heap property restored
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (isHigherPriority(data_[index], data_[parent])) {
                std::swap(data_[index], data_[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    // Move the element at index down until heap property restored
    void heapifyDown(int index) {
        int size = static_cast<int>(data_.size());
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && isHigherPriority(data_[left], data_[largest])) {
                largest = left;
            }
            if (right < size && isHigherPriority(data_[right], data_[largest])) {
                largest = right;
            }
            if (largest != index) {
                std::swap(data_[index], data_[largest]);
                index = largest;
            } else {
                break;
            }
        }
    }
};

/* ---------------- Helper parsing functions ---------------- */

int priorityForCategory(const std::string &category) {
    if (category == "Boss")            return 5;
    if (category == "Subordinate")     return 4;
    if (category == "Peer")            return 3;
    if (category == "ImportantPerson") return 2;
    if (category == "OtherPerson")     return 1;
    return 0; // unknown category, lowest priority
}

long dateToValue(const std::string &date) {
    int mm = 0, dd = 0, yyyy = 0;
    char d1 = 0, d2 = 0;
    std::istringstream iss(date);
    iss >> mm >> d1 >> dd >> d2 >> yyyy;
    return static_cast<long>(yyyy) * 10000L +
           static_cast<long>(mm) * 100L +
           static_cast<long>(dd);
}

// Trim leading/trailing whitespace of a string
std::string trim(const std::string &s) {
    const char *ws = " \t\r\n\f\v";
    std::string::size_type start = s.find_first_not_of(ws);
    if (start == std::string::npos) return "";
    std::string::size_type end = s.find_last_not_of(ws);
    return s.substr(start, end - start + 1);
}

// Parse "<category>,<subject>,<date>" (the text AFTER "EMAIL ")
// into an Email. Fields are comma-delimited; the subject may contain
// spaces (and even commas, since the LAST comma splits off the date).
bool parseEmailLine(const std::string &rest, Email &e) {
    std::string::size_type firstComma = rest.find(',');
    if (firstComma == std::string::npos) return false;

    std::string::size_type lastComma = rest.rfind(',');
    if (lastComma == firstComma) return false; // need two commas

    std::string category = trim(rest.substr(0, firstComma));
    std::string subject  = trim(rest.substr(firstComma + 1, lastComma - firstComma - 1));
    std::string date     = trim(rest.substr(lastComma + 1));

    e.sender    = category;
    e.subject   = subject;
    e.date      = date;
    e.priority  = priorityForCategory(e.sender);
    e.dateValue = dateToValue(e.date);

    return true;
}

/* ---------------- Main program ---------------- */

int main(int argc, char *argv[]) {
    std::ifstream file;
    std::istream *in = &std::cin;

    if (argc > 1) {
        file.open(argv[1]);
        if (!file) {
            std::cout << "Could not open file: " << argv[1] << "\n";
            return 1;
        }
        in = &file;
    }

    MaxHeap heap;
    std::string line;

    while (std::getline(*in, line)) {
        line = trim(line);

        if (line.empty()) {
            continue; // skip blank lines
        }

        if (line.compare(0, 6, "EMAIL ") == 0) {
            Email e;
            if (parseEmailLine(line.substr(6), e)) {
                heap.insert(e);
            }
        }
        else if (line == "NEXT") {
            Email top;
            if (heap.peekMax(top)) {
                std::cout << "Next email:\n"
                          << "Sender: "  << top.sender  << "\n"
                          << "Subject: " << top.subject << "\n"
                          << "Date: "    << top.date    << "\n";
            } else {
                std::cout << "No emails to read.\n";
            }
        }
        else if (line == "READ") {
            Email discarded;
            // Remove the top email (if any) without displaying it.
            heap.extractMax(discarded);
        }
        else if (line == "COUNT") {
            std::cout << "There are " << heap.count() << " emails to read.\n";
        }
        // Unknown commands are silently ignored
    }

    return 0;
}
