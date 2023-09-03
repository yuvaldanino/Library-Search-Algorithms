#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <chrono>

using namespace std;



class ElapsedTime {
    public:
        void start() {
            start_time = std::chrono::high_resolution_clock::now();
        }
        void stop() {
            stop_time = std::chrono::high_resolution_clock::now();
        }
        double getElapsedTime() {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time);
            return duration.count();
        }
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        std::chrono::time_point<std::chrono::high_resolution_clock> stop_time;
};


struct Data {
    int ISBN;
    string language;
    int type;
};

// map to convert type to int value 
map<string, int> typeMapping = {
    {"new", 0},
    {"used", 1},
    {"digital", 2}
};

// method that gets file as input and returns Data vector 
vector<Data> readFile(string fileName) {
    vector<Data> data;
    string line;
    ifstream file(fileName);

    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream ss(line);
            Data d;
            string field;
            getline(ss, field, ',');
            d.ISBN = stoi(field); 
            getline(ss, d.language, ',');
            getline(ss, field);
            d.type = typeMapping[field];// converts type to number value 
            data.push_back(d);
        }
        file.close();
    } else {
        cout << "Error: cannot open file "<< fileName << endl;
        exit(1);
    }
    return data;
}


bool compareBooks(const Data& book1, const Data& book2){

    if(book1.ISBN != book2.ISBN){
        return book1.ISBN < book2.ISBN;
    }
    if(book1.type != book2.type){
        if(book1.type == 0) return true; // b1 is new 
        if(book2.type == 0) return false; // b2 is new 
        if(book1.type == 1) return true; // b1 is used
        if(book2.type == 1) return false; // b2 is used
        if(book1.type == 2) return true; // b1 is digital
        if(book2.type == 2) return false; // b2 is digital
    }
    return book1.language < book2.language;

}

int linearMatchCounter(vector<Data>& booksToFind, vector<Data>& books){

        int requestedBookNum = 0;
        int size = booksToFind.size();
        int bookCounter = 0;
        
        while(requestedBookNum < size){
            for(auto& book: books){ // loops of needed books 
                if(book.ISBN == booksToFind[requestedBookNum].ISBN){ // same isbn num
                    if(book.language == booksToFind[requestedBookNum].language){
                        if(book.type == booksToFind[requestedBookNum].type){
                            bookCounter++;
                        }
                    }
                }
            }
            requestedBookNum++;
        }


    return bookCounter;
}

// int binaryMatchCounter(vector<Data>& booksToFind, vector<Data>& books) {
//     int count = 0;
//     for (const auto& book : booksToFind) {
//         int left = 0, right = books.size() - 1;
//         while (left <= right) {
//             int mid = (left + right) / 2;
//             if (books[mid].ISBN == book.ISBN && books[mid].language == book.language && books[mid].type == book.type) {
//                 count++;
//                 break;
//             } else if (compareBooks(book, books[mid])) {
//                 right = mid - 1;
//             } else {
//                 left = mid + 1;
//             }
//         }
//     }
//     return count;
// }

bool binarySearch(Data bookToFind, vector<Data>& books) {
    int left = 0;
    int right = books.size() - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (books[mid].ISBN == bookToFind.ISBN && books[mid].language == bookToFind.language && books[mid].type == bookToFind.type) {
            return true;
        } else if (compareBooks(bookToFind, books[mid])) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return false;
}



bool recursiveMatchCounter(Data bookToFind, vector<Data>& books, int left, int right) {
    
    if (left > right) {
        return false;
    }
    int mid = (left + right) / 2;
    if (books[mid].ISBN == bookToFind.ISBN && books[mid].language == bookToFind.language && books[mid].type == bookToFind.type) {
        return true;
    } else if ( compareBooks(bookToFind,books[mid])) {
        return recursiveMatchCounter(bookToFind, books, left, mid - 1);
    }
    else {
        return recursiveMatchCounter(bookToFind, books, mid + 1, right);
    }


}





int main(int argc, char* argv[]) {
    
    if(argc != 4){
        std::cerr << "Usage: .SearchNewBooks <newBooks.dat> <request.dat> <result_file.dat>" << std::endl;
        return 1;
    }

    // reads files and puts them into vector<data> objects 
    vector<Data> newBooksData = readFile(argv[1]); // newbooks file 
    vector<Data> requestedData = readFile(argv[2]);// requested books file 
    //timer
    ElapsedTime timer; 
    
    //user input 
    string userInput;
    while (true) {
        cout << "Choice of search method ([l]inear, [b]inary, [r]ecursiveBinary)?";
        cin >> userInput;
        if (userInput == "l" || userInput == "b" || userInput == "r") {
            break;
        } else {
            cout << "Incorrect choice" << endl;
        }
    }

    // if l, linear
    if(userInput == "l"){ 
        timer.start();
        int sameBooks = linearMatchCounter(requestedData, newBooksData);
        timer.stop();
        std::cout << "CPU time: " << timer.getElapsedTime() << " microseconds" << std::endl;
        cout << "There were: " << sameBooks << " books that matched" << endl;
        

    }


    // if b, binary
    if(userInput == "b"){


        int sameBooks = 0;
        timer.start();
        sort(newBooksData.begin(), newBooksData.end(), compareBooks);
        sort(requestedData.begin(), requestedData.end(), compareBooks);
        for(const auto& book : requestedData){
            if(binarySearch(book,newBooksData) == true){
                sameBooks++;
            }
        }
        timer.stop();

        std::cout << "CPU time: " << timer.getElapsedTime() << " microseconds" << std::endl;
        cout << "There were: " << sameBooks << " books that matched" << endl;
    }


    // if r, recursive 
    if(userInput == "r"){

        
        int sameBooks = 0;
        timer.start();
        sort(newBooksData.begin(), newBooksData.end(), compareBooks);
        sort(requestedData.begin(), requestedData.end(), compareBooks);
        for(const auto& book : requestedData){
            if(recursiveMatchCounter(book,newBooksData,0, newBooksData.size() - 1) == true){
                sameBooks++;
            }
        }
        timer.stop();

        std::cout << "CPU time: " << timer.getElapsedTime() << " microseconds" << std::endl;
        cout << "There were: " << sameBooks << " books that matched" << endl;

    }



    return 0;
}
