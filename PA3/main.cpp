#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

struct ImageRowThreadData{
    int length, row;  // Image dimensions and current row index
    vector<int> headPos, dataPos;  // head positions and data positions
    vector<string>* ASCIIDrawing2Dimage; // Reference to the final image container
    vector<pair<char, vector<pair<int, int>>>> symbols; // Symbols and their ranges
    pthread_mutex_t *bsem; // Mutex for thread creation synchronization
    pthread_mutex_t *bsemTwo; // Mutex for ordering the row printing
    pthread_cond_t *condition; // Condition variable for thread synchronization
    int * counter;  // Counter to keep track of which row should print next
};

// Function executed by each thread to decode a specific row
void* decodingRow(void* arg){
    struct ImageRowThreadData* data = (struct ImageRowThreadData*)arg;
    int length = data->length;
    int row = data->row;

    pthread_mutex_unlock(data->bsem); //unlock the mutex from main

    string imageLine(length, ' '); // Initialize the row as empty spaces

    // Ensure that the row index is within the bounds
    if(row >= data->headPos.size()){
        pthread_exit(nullptr); //exit if the row index is out of bounds
    } 

    // Calculate the begin and end index for this row
    int beginIndex = data->headPos[row];
    int endIndex;
    
    if(row + 1 < data->headPos.size()){
        endIndex = data->headPos[row + 1]; // Next row start index
    }
    else{
        endIndex = data->dataPos.size(); // End index is the size of dataPos for last row
    } 

    // Places characters in the image line based on dataPos, checking each range for each symbol.
    for(const auto& symbolData : data->symbols){       // Iterates through symbols and their ranges, placing them into the image row
        char symbol = symbolData.first;                 // Current symbol to be placed
        for(int j = beginIndex; j < endIndex; j++){  // Loop over dataPos for the current row
            int x = data->dataPos[j];                   // Get the x position for this data point
            for(const auto& range : symbolData.second){ // Loop over the symbol's ranges
                if(x >= range.first && x <= range.second){ // If the current position is within the symbol's range, place the symbol in the image line
                    imageLine[x] = symbol;
                }
            }
        }
    }

    // CRITICAL SECTION: Synchronization to ensure rows are printed in order
    pthread_mutex_lock(data->bsemTwo);

    // Ensure threads print in the correct order using condition variable
    while(*data->counter != row){ // Wait until the current row is processed ( Wait until it's this row's turn to print)
        pthread_cond_wait(data->condition, data->bsemTwo); 
    }
    pthread_mutex_unlock(data->bsemTwo);

    // Print the decoded row - each thread prints its own row in the correct order
    // threads must print their decoded rows
    cout << imageLine << endl; 

    // CRITICAL SECTION: Update the counter to indicate the next row can print
    pthread_mutex_lock(data->bsemTwo); // Lock the mutex to ensure thread-safe access to shared resources
    (*data->counter)++; // Increment the counter to keep track of completed rows (to move to the next row)
    pthread_cond_broadcast(data->condition); // Notify other threads that this row is done (Notifies all waiting threads that counter changed)
    pthread_mutex_unlock(data->bsemTwo); // Unlock the mutex after processing the row
    return nullptr;
}

int main() {

    int length, width;  // Image dimensions: length (length of row) and width (number of rows)
    string imageLine;

    // Reads in the image dimensions
    getline(cin, imageLine);
    stringstream ss(imageLine);
    ss >> length >> width;

     // Reads in the symbol definitions (character and ranges)
    vector<pair<char, vector<pair<int, int>>>> symbols;
    getline(cin, imageLine);
    stringstream ssSecond(imageLine);
    string token;

    while(getline(ssSecond, token, ',')){ // Split the symbol data by commas
        stringstream ssThird(token);
        char symbol;
        ssThird >> symbol; // Extract the symbol
        vector<pair<int, int>> ranges;
        int begin, end;
        while(ssThird >> begin >> end) // Extract the ranges for this symbol
            ranges.emplace_back(begin, end);  // Add the ranges for the symbol
        symbols.emplace_back(symbol, ranges); // Stores the symbol and its ranges
    }

    // Reads in the headPos (Reads in head positions of the image)
    vector<int> headPos, dataPos;
    getline(cin, imageLine);
    stringstream ssThird(imageLine);
    int numbers;
    while(ssThird >> numbers){
        headPos.push_back(numbers); // Stores the head positions (y-axis)
    }

     // Reads in the dataPos (Reads in data positions of the image)
    getline(cin, imageLine);
    stringstream ssForth(imageLine);
    while(ssForth >> numbers){
        dataPos.push_back(numbers); // Store data positions (x-axis)
    }

    vector<string> ASCIIDrawing2Dimage(width); // Initializes the image storage

    // Create and initialize the threads to process each row
    pthread_t theThreads[width];
    ImageRowThreadData myThread;
    
    pthread_mutex_t bsem; //initialize first semaphore
    pthread_mutex_init(&bsem, nullptr);
    pthread_mutex_t bsemTwo; //initialize second semaphore
    pthread_mutex_init(&bsemTwo, nullptr);
    pthread_cond_t condition = PTHREAD_COND_INITIALIZER; //initialize first conditional variable
    static int counter = 0;  // Tracks the current row to print

    myThread.length = length;
    myThread.headPos = headPos;
    myThread.dataPos = dataPos;
    myThread.ASCIIDrawing2Dimage = &ASCIIDrawing2Dimage;
    myThread.symbols = symbols;
    myThread.bsem = &bsem; 
    myThread.bsemTwo = &bsemTwo;
    myThread.condition = &condition;
    myThread.counter = &counter;

    for(int j = 0; j < width; j++){
        // Lock mutex before creating each thread to ensure threads process in order
        pthread_mutex_lock(&bsem);

        // Set up the data for the current row's thread
        myThread.row = j;

        if(pthread_create(&theThreads[j], nullptr, decodingRow, &myThread)){ // Create thread for each row
            fprintf(stderr, "Error creating thread\n");
            return 1; // Exit if thread creation fails
        }
    }

    // Wait for all threads to finish and join the threads
    for(int j = 0; j < width; j++){
        pthread_join(theThreads[j], nullptr);  // Wait for each thread to finish
    }

    return 0;
}
