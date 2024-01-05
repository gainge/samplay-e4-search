//
//  main.cpp
//  samplay-e4-search
//
//  Created by Gibson Ainge on 1/5/24.
//

#include <iostream>
#include <algorithm>

using namespace std;

// Max of 10 hours @ 4833.9 rolls/second
uint32_t MAX_SEARCH_LENGTH = 4833.9 * 60 * 60 * 10;
uint32_t CANDIDATE_1 = 0xEA0EEDC2;
uint32_t CANDIDATE_2 = 0xD082D644;

// Implementation of LCG by Savestate
void rng_adv(uint32_t *seed) {
    *seed = ((*seed * 214013) + 2531011);
}

bool charInRange(char c, char low, char high) {
    return c >= low && c <= high;
}

bool isValidHex(string str) {
    for (int i = 0; i < str.length(); i++) {
        char c = str[i];
        
        if (isspace(c)) continue;
        
        if (!charInRange(c, '0', '9') &&
            !charInRange(c, 'A', 'F') &&
            !charInRange(c, 'a', 'f') &&
            c != 'x' && c != 'X' &&
            c != '-' && c != '+')
        {
            return false;
        }
    }
    
    return true;
}

bool isQuit(string str) {
    return str[0] == 'x' || str[0] == 'X';
}

bool isCandidateSeed(uint32_t seed) {
    return seed == CANDIDATE_1 || seed == CANDIDATE_2;
}

string removeSpaces(string str) {
    int i = 0;
    int j = 0;
    
    while (j < str.length()) {
        if (str[j] != ' ') {
            // Copy over char
            str[i] = str[j];
            i++;
        }
        j++;
    }
    
    // Null terminate string
    str.erase(i);
    
    return str;
}

string getUserHex(string prompt) {
    string input;
    
    while (true) {
        cout << prompt;
        getline(cin, input);
        
        // Remove spaces
        input = removeSpaces(input);
        
        // Validate input
        if (input.length() == 0 || !isValidHex(input)) {
            printf("\n\n!---- Please Input a Valid Hex Number (x to quit) ----!\n\n");
        } else {
            break;
        }
    }
    
    return input;
}

// Return int so we can go negative
// This is capped by the max roll count anyway
int findDistanceToClosestTarget(uint32_t startSeed) {
    if (isCandidateSeed(startSeed)) {
        return 0;
    }
    
    uint32_t seedFromStart = startSeed;
    int rollCount = 0;
    
    do {
        rng_adv(&seedFromStart);
        rollCount++;
    } while (rollCount < MAX_SEARCH_LENGTH && !isCandidateSeed(seedFromStart));
    
    return rollCount >= MAX_SEARCH_LENGTH ? -1 : rollCount;
}

int main(int argc, const char * argv[]) {
    cout << "===========================" << endl;
    cout << "SSBM Event 4 Seed Diff Calc" << endl;
    cout << "===========================" << endl;
    
    string userInput;
    
    while (true) {
        cout << endl;
        cout << "Please supply seeds (x to quit)" << endl;
        userInput = getUserHex("Please input current seed: ");
        
        // Check quit flag
        if (isQuit(userInput)) {
            break;
        }
        
        // Convert user input as hex
        uint32_t currentSeed = stoul(userInput, 0, 16);
        
        int distance = findDistanceToClosestTarget(currentSeed);
        
        if (distance == -1) {
            cout << "Manip time exceeds 10 hours!" << endl;
            cout << endl;
        } else {
            // Print out roll count and manip time
            cout << "Seed found in: [" << distance << "] iterations!" << endl;
            cout << "CSS Manip Time: ";
            float seconds = ((float)distance / 4833.9);
            if (seconds >= 60.0) {
                uint32_t minutes = seconds / 60;
                cout << minutes << " minutes, ";
            }
            cout << ((int)seconds % 60) << " seconds" << endl;
        }
    }
    
    return 0;
}
