#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

// Struct to store the data for each row that needs to be decoded
struct ImageRowThreadData{
    int length, row;  // Image dimensions and current row index
    vector<int> headPos, dataPos;  // head positions and data positions
    vector<string>* ASCIIDrawing2Dimage; // Reference to the final image container
    vector<pair<char, vector<pair<int, int>>>> symbols; // Symbols and their ranges
};

// Function executed by each thread to decode a specific row
void* decodingRow(void* arg){
    struct ImageRowThreadData* data = (struct ImageRowThreadData*)arg;
    int length = data->length;
    int row = data->row;
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

     // Stores the decoded row in the final image
    (*data->ASCIIDrawing2Dimage)[row] = imageLine;
    
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
    vector<ImageRowThreadData> myThread(width); // Vector to hold data for each row's thread

    for(int j = 0; j < width; j++){
        // Set up the data for the current row's thread
        myThread[j].row = j;
        myThread[j].length = length;
        myThread[j].headPos = headPos;
        myThread[j].dataPos = dataPos;
        myThread[j].ASCIIDrawing2Dimage = &ASCIIDrawing2Dimage;
        myThread[j].symbols = symbols;

        if(pthread_create(&theThreads[j], nullptr, decodingRow, &myThread[j])){ // Create thread for each row
            fprintf(stderr, "Error creating thread\n");
            return 1; // Exit if thread creation fails
        }
    }

    // Wait for all threads to finish and join the threads
    for(int j = 0; j < width; j++){
        pthread_join(theThreads[j], nullptr);  // Wait for each thread to finish
    }

    // Print the decoded 2D image with printable ASCII characters
     for(const auto& row : ASCIIDrawing2Dimage){
         cout << row << endl; // Outputs each row of the image
     }

    return 0;
}