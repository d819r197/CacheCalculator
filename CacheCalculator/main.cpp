//Packages
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <array>
#include <thread>
#include <chrono>
#include <vector>
#include <stdlib.h>
#include <algorithm>

//Globar Variables
  //Global Timer
  clock_t t;

  //Index and Tag Byte Size
  const int indexSize = 9;
  const int tagSize = 18;

  //number of lines
  const int numOfIndexs = 2*2*2*2*2*2*2*2*2; //2^9 = 512

  //Array of Dirty Write Back Lines
  std::string dWbArr[numOfIndexs][2];

  //Number of Cache Lines Executed
  int cacheLines;

  //Returns program time skewed by factor of skew
  int getTime() {
    //Increase to get faster proccessing
    //Warning: Increases chance of out of order Cache Line
    int skew = 4000;

    t = clock();
    float seconds = (((float)t)/CLOCKS_PER_SEC);
    return(seconds * skew);
  }

  struct Proccessor {
  //Proccessor Vars
  int lines;
  int cacheLineIndex;
  std::vector<std::string*> importTable;
  std::string cacheTable[numOfIndexs][2];
  std::string filePath;

  //Output Tracking Variables
  int cacheTransfers[3];
  int coherenceInvalidations[4];
  int dirtyWBs;
  int linesInState[5];

  //Proccessor Functions
  //Returns Number of Lines in a Given File
  int countLines(std::string filePath) {
    int number_of_lines = 0;
    std::string line;
    std::ifstream file;
      file.open(filePath, std::ifstream::in);

    if (file.is_open()) {
      while (std::getline(file, line)) {
        ++number_of_lines;
      }
      file.close();
      return(number_of_lines);
    }
    else {
      std::cout << "Error opening file";
    }
    file.close();
    return (0);
  }

  //Imports Data to Proccessor's Table
  void import() {
    //Declare Variables
    std::string word = "";
    std::string line = "";
    std::ifstream file;
      file.open(filePath, std::ifstream::in);

    //Open File
    if (file.is_open()) {
        //Break File into Lines
        while(getline(file, line)) {
          std::string* importRow = new (std::nothrow) std::string[7];
          int cols = 0;

          //Remove Endline char
          line[line.size()] = ' ';

          //Make Stream Obj of String
          std::stringstream ssLine(line);

          //Break Lines into Words
          while(getline(ssLine, word, ' ')) {
            // std::cout<<word <<" | ";

            //Assign Word to importTable
            importRow[cols] = word;
              cols++;
          }
          // std::cout<<std::endl;

          //Assign Index
          importRow[5] = importRow[2][7];

          //Assign mem
          importRow[6] = importRow[2];

          //Assign tag
          importRow[2] = importRow[2].substr(2, 5);

          //Assign Cache Lines Processor Num
          importRow[3] = filePath;

          //initlize tag with states attached
          importRow[4] = "i";


          //Show Row Output
          //std::cout<<"| Row: " <<importRow[0] <<" | " <<importRow[1] <<" | " <<importRow[2] <<" | " <<importRow[3] <<" | \n";

          //Add Row to importTable
          importTable.push_back(importRow);
        }
      file.close();
    }
    else {
        std::cout << "Error opening file";
    }
      file.close();
  }

  //Output Copy of importTable
  void printImportTable() {
    for(int lcv = 0; lcv < importTable.size(); lcv++) {
      std::string* currentArr = importTable.at(lcv);
      for(int vals = 0; vals < 3; vals++) {
        std::cout <<currentArr[vals] <<" , ";
      }
      std::cout <<"\n----------------------------\n";
    }
  }

  //Collects processor states after program runs
  void collectStates() {
    std::string validStates[5] = {"m", "o", "e", "s", "i"};
    for(int cacheIndex = 0; cacheIndex < numOfIndexs; cacheIndex++) {
      // for(int tagIndex = 0; tagIndex < numOfIndexs; tagIndex++) {
        for(int stateIndex = 0; stateIndex < 5; stateIndex++) {
          if(cacheTable[cacheIndex][1] != "") {
            // std::cout<<"Looking at state: " <<cacheTable[cacheIndex][tagIndex][1] <<std::endl;
          }
          if(cacheTable[cacheIndex][1] == validStates[stateIndex]) {
            // std::cout<<"Increasing State\n";
            linesInState[stateIndex]++;
            break;
          }
        }
      // }
    }
  }

  //Return Position of Tag in Cache's Index Array
  bool findTag(std::string t, int i) {
    // std::cout<<"Looking for tag...";
      for(int indexCounter = 0; indexCounter < numOfIndexs; indexCounter++ ){
        // std::cout<<"New: Comparing " <<cacheTable[indexCounter][0] <<" to " <<t <<std::endl;

        if(cacheTable[indexCounter][0] == t) {
          // std::cout<<"New: Found tag at " <<pos <<std::endl;
          return(true);
        }
        else{
          // std::cout<<"New: done checking checking tag at "<<pos <<"\n";
        }
      }
    return(false);
  }

  //Updates Proccessor State
  bool updateState(std::string tag, int index, int pNum, std::string newState) {
    std::string validStates[5] = {"m", "o", "e", "s", "i"};
    //Valid State Change
    if(newState == validStates[0] || newState == validStates[1] || newState == validStates[2] || newState == validStates[3] || newState == validStates[4] ) {
      // std::cout<<"Updating tag: " <<tag <<" at index: " <<index <<" with state: " <<newState <<"....";

      //Tag Found
      if(findTag(tag, index)) {
        // std::cout<<"Tag Found at: [" <<index <<"][" <<pos <<"]\n";
        cacheTable[index][1] = newState;
        return(true);
      }
      //Tag not found
      else {
        cacheTable[index][0] = tag;
        cacheTable[index][1] = newState;
        return(true);
      }
      // else if(!findTag("", index)) {
      //   //Find empty tag to write to
      //   pos = findTag("", index);
      //   if(pos != -1) {
      //     // std::cout<<"Info: Tag Not Found, Writing Tag #: " <<cacheLineIndex <<" to Tag Pos: " <<pos <<std::endl;
      //     cacheTable[index][pos][0] = tag;
      //     cacheTable[index][pos][1] = newState;
      //       cacheLineIndex++;
      //   }
        // else {
        //   std::cout<<"Error: Cache ran out of lines to write to\n";
        // }
      // }
      // else {
      //   std::cout<<"Error: tag at invalid position\n";
      // }
    }
    else {
      std::cout<<"Error: State: " <<newState <<" for Cache Line is invalid.\n";
    }

  return(false);
  }

  //Proccessor Constructor
  Proccessor(std::string path) {
    filePath = path;
    cacheLineIndex = 0;
    lines = countLines(filePath);

    //initlize cache table
    for(int lcv = 0; lcv<numOfIndexs; lcv++) {
      cacheTable[lcv][0] = "0xTag";
      cacheTable[lcv][1] = "i";
    }

    import();
  }
};

//Create Proccessor Objs
Proccessor pArr[4] = {Proccessor("p0.tr"), Proccessor("p1.tr"), Proccessor("p2.tr"), Proccessor("p3.tr")};

//Binary to Decimal
const int convertBin(int c) {
  //Hex to Binary
    int decInt = 0;
    int val = 0;
    int temp = c;
    int b = 1;

    std::cout<<"Info: Converting " <<c <<"to binary...";
    while (c > 0)
    {
        val = temp % 10;
        decInt = decInt + val * b;
        b *= 2;
        temp /= 10;
    }
std::cout<<"value is now: " <<decInt <<std::endl;
  return(decInt);
}

//Hex to Binary
const std::string convertHex(std::string c, char convertTo) {
  //Hex to Binary
  if(convertTo == 'b') {
    std::string binString = "";
    // std::cout<<"Info: Converting " <<c <<" to binary...\n";
    for(int lcv = 0; lcv < c.size(); lcv++) {
      switch(toupper(c[lcv])) {
          case '0': binString += "0000"; break;
          case '1': binString += "0001"; break;
          case '2': binString += "0010"; break;
          case '3': binString += "0011"; break;
          case '4': binString += "0100"; break;
          case '5': binString += "0101"; break;
          case '6': binString += "0110"; break;
          case '7': binString += "0111"; break;
          case '8': binString += "1000"; break;
          case '9': binString += "1001"; break;
          case 'A': binString += "1010"; break;
          case 'B': binString += "1011"; break;
          case 'C': binString += "1100"; break;
          case 'D': binString += "1101"; break;
          case 'E': binString += "1110"; break;
          case 'F': binString += "1111"; break;
      }
      // std::cout<<"Info: Digit #" <<lcv <<": " <<c[lcv] <<" added. binString: " <<binString <<". ";
    }
    // std::cout<<"Info: Converting done. Binary Value: " <<binString <<std::endl;

    return(binString);
  }
  //Hex tp Decimal
  else if(convertTo == 'd' && c.size() == 1) {

        switch(toupper(c[0])) {
            case '0': return(std::to_string(0));
            case '1': return(std::to_string(1));
            case '2': return(std::to_string(2));
            case '3': return(std::to_string(3));
            case '4': return(std::to_string(4));
            case '5': return(std::to_string(5));
            case '6': return(std::to_string(6));
            case '7': return(std::to_string(7));
            case '8': return(std::to_string(8));
            case '9': return(std::to_string(9));
            case 'A': return(std::to_string(10));
            case 'B': return(std::to_string(11));
            case 'C': return(std::to_string(12));
            case 'D': return(std::to_string(13));
            case 'E': return(std::to_string(14));
            case 'F': return(std::to_string(15));
        }
  }
  else {
    std::cout <<"Could not convert hex value to: " <<convertTo <<std::endl;
  }
}

void printOutput() {
  std::string states[] = {"m", "o", "e", "s", "i"};

  std::cout<<"----------------------------------------------------------------------------------\n";
  std::cout<<"|                                PROGRAM OUTPUT                                  |\n";
  std::cout<<"----------------------------------------------------------------------------------\n";
  //Output 1: Cache Transfers
  std::cout << "The total number of cache-to-cache transfers for each processor pair \n";
  std::cout <<"P0 Cache Transfers: <p0-p1> = " << pArr[0].cacheTransfers[0] <<", <p0-p2> = "<< pArr[0].cacheTransfers[1] <<", <p0-p3> = " << pArr[0].cacheTransfers[2] <<std::endl;
  std::cout <<"P1 Cache Transfers: <p1-p0> = " << pArr[1].cacheTransfers[0] <<", <p1-p2> = "<< pArr[1].cacheTransfers[1] <<", <p1-p3> = " << pArr[1].cacheTransfers[2] <<std::endl;
  std::cout <<"P2 Cache Transfers: <p2-p0> = " << pArr[2].cacheTransfers[0] <<", <p2-p1> = "<< pArr[2].cacheTransfers[1] <<", <p2-p3> = " << pArr[2].cacheTransfers[2] <<std::endl;
  std::cout <<"P3 Cache Transfers: <p3-p0> = " << pArr[3].cacheTransfers[0] <<", <p3-p1> = "<< pArr[3].cacheTransfers[1] <<", <p3-p2> = " << pArr[3].cacheTransfers[2] <<std::endl;
  std::cout<<"----------------------------------------------------------------------------------\n";
  //Output 2: Coherence Invalidations
  std::cout << "The total number of invalidations due to coherence \n";
  for(int p = 0; p < 4; p++) {
    std::cout <<"P" <<p <<" Invalidation from: ";
    for(int lcv = 0; lcv < 4; lcv++) {
      std::cout <<states[lcv] <<" = " <<pArr[p].coherenceInvalidations[lcv] <<" ";
    }
    std::cout<<std::endl;
  }
  std::cout<<"----------------------------------------------------------------------------------\n";
  //Output 3a: Number of Dirty Write Backs
  std::cout << "The number of dirty writebacks from each processor.\n";
  for(int p = 0; p < 4; p++) {
    std::cout <<"P" <<p <<": " <<states[p] <<" = " <<pArr[p].dirtyWBs <<" ";
  }
  std::cout<<"\n----------------------------------------------------------------------------------\n";
  //Output 3b: List of Dirty Write Backs
  std::cout <<"Dirty WBs in P0: \n";
      for(int tagIndex = 0; tagIndex < numOfIndexs; tagIndex++) {
      if(dWbArr[tagIndex][0] != "") {
        std::cout<<"Dirty Write Back's Tag at [" <<tagIndex <<"] is: " <<dWbArr[tagIndex][0] <<std::endl;
      }
    }
  std::cout<<"\n----------------------------------------------------------------------------------\n";
  //Output 4: Number of Lines in the Proccessor's States
  std::cout << "The number of lines in each state at the end the simulation for each processor. \n";
  for(int p = 0; p < 4; p++) {
    std::cout <<"P" <<p <<" Invalidation from: ";
    for(int lcv = 0; lcv < 5; lcv++) {
      std::cout <<states[lcv] <<" = " <<pArr[p].linesInState[lcv] <<" ";
    }
    std::cout<<std::endl;
  }
  std::cout<<"----------------------------------------------------------------------------------\n";
  std::cout<<"|                            PROGRAM OUTPUT COMPLETE                             |\n";
  std::cout<<"----------------------------------------------------------------------------------\n";
}

//busReadX <physical memory address>
//Update everything to invalid state
void busUpgr(std::string tag, int index) {
    //Set tag at index to invalid for each processor
    pArr[0].updateState(tag, index, 0, "i");
    pArr[1].updateState(tag, index, 1, "i");
    pArr[2].updateState(tag, index, 2, "i");
    pArr[3].updateState(tag, index, 3, "i");
}

//Read Cache Lines
bool read(int pNum,int index, std::string tag) {
    bool isInvalid[4] = {true, true, true, true};

    for(int lcv = 0; lcv < 4; lcv++) {
      //Other Cache has Data
      if(pNum != lcv && pArr[lcv].findTag(tag, index)) {

        std::string currTag = pArr[lcv].cacheTable[index][0];
        std::string currState = pArr[lcv].cacheTable[index][1];

        //Other Cache in Modified State
        if(currState == "m") {
          pArr[pNum].updateState(tag, index, pNum, "s");
          pArr[pNum].updateState(tag, index, lcv, "o");
          return(true);
        }
        //Other Cache in Owner State
        else if(currState == "o") {
          if(lcv == 0){
            pArr[pNum].updateState(tag, index, pNum, "s");
            dWbArr[index][0] = tag;
              pArr[pNum].dirtyWBs++;
            pArr[pNum].cacheTransfers[0]++;
            return(true);
          }
          else if(lcv == 1){
            pArr[pNum].updateState(tag, index, pNum, "s");
            dWbArr[index][0] = tag;
              pArr[pNum].dirtyWBs++;
            pArr[pNum].cacheTransfers[1]++;
            // std::cout <<"Info: incrementing cache transfer at " <<pNum <<" and " <<lcv <<std::endl;

            return(true);
          }
          else if(lcv == 2){
            pArr[pNum].updateState(tag, index, pNum, "s");
            dWbArr[index][0] = tag;
              pArr[pNum].dirtyWBs++;
            pArr[pNum].cacheTransfers[2]++;
            // std::cout <<"Info: incrementing cache transfer at " <<pNum <<" and " <<lcv <<std::endl;

            return(true);
          }
        }
        //Other Cache in Sharred State
        else if(currState == "e"){
          if(lcv == 0){
            pArr[pNum].cacheTransfers[0]++;
            pArr[pNum].updateState(tag, index, pNum, "s");
            pArr[pNum].updateState(tag, index, 0, "s");
            return(true);
          }
          else if(lcv == 1){
            // std::cout <<"Info: incrementing cache transfer at " <<pNum <<" and " <<lcv <<std::endl;
            pArr[pNum].cacheTransfers[1]++;
            pArr[pNum].updateState(tag, index, pNum, "s");
            pArr[pNum].updateState(tag, index, 1, "s");
            return(true);
          }
          else if(lcv == 2){
            // std::cout <<"Info: incrementing cache transfer at " <<pNum <<" and " <<lcv <<std::endl;
            pArr[pNum].cacheTransfers[2]++;
            pArr[pNum].updateState(tag, index, pNum, "s");
            pArr[pNum].updateState(tag, index, 2, "s");
            return(true);
          }
        }

        //Cache has invalid states
        else if(currState != "i") {
          isInvalid[lcv] = false;
        }

      }
      if(isInvalid[0] && isInvalid[1] && isInvalid[2] && isInvalid[3]) {
        // Only Copy of Data
          //Update Active Proccessor to Exclusive State
          pArr[pNum].updateState(tag, index, pNum, "e");
          return(true);
      }
    }
  return(false);
}

//Write Cache Lines
bool write(int pNum, int index, std::string tag) {
    //Call BusUpgr to Invalidate States
    busUpgr(tag, index);

    //Change State to Modified
    pArr[pNum].updateState(tag, index, pNum, "m");
    pArr[pNum].coherenceInvalidations[0]++;
    dWbArr[index][0] = tag;
      pArr[pNum].dirtyWBs++;
}

//Pulls from Cache Line Vector to Start Prcessing Cache Lines
bool addCacheLine() {
  for(int p = 0; p<4;p++) {
    if(!pArr[p].importTable.empty()) {
      //Get First Cache Line of each Vector
      //Arr = timestamp | read/write | tag | filePath | P0 State | index | offset
      std::string* cline = pArr[p].importTable.front();
      std::string fullMem = convertHex(cline[6], 'b');
      std::string binaryIndex = fullMem.substr(tagSize, tagSize+indexSize);
      std::string t = fullMem.substr(0, tagSize);
      int i = std::stoi(convertHex(cline[5], 'd'));
      // int tempInt;
      // std::istringstream iss (binaryIndex);
      //   iss >> tempInt;
      // int i;
      //   if(iss.good()) {
      //     i = convertBin(tempInt);
      //   }

      // std::cout <<"Full String: " <<fullMem << " with size: " <<fullMem.size() <<" Tag Arr Size: " <<t.size() << " and " <<t << " and Index arr " <<binaryIndex.size() << " and " <<binaryIndex  << " = " <<i <<std::endl;

      if(std::stoi(cline[0]) <= getTime() && i < numOfIndexs) {
          //Process Cache Line
          if(cline[3] == "p0.tr") {

            // //Read Action
            if(cline[1] == "0") {
              read(0, i, t);
            }
            //Write Action
            else if(cline[1] == "1") {
              write(0, i, t);
            }
            else {
              std::cout <<"Error: Cache Line doesn't have a read or write attribute\n";
            }

          }
          // //Process Cache Line
          else if(cline[3] == "p1.tr") {
            //Read Action
            if(cline[1] == "0") {
              read(1, i, t);
            }
            //Write Action
            else if(cline[1] == "1") {
              write(1, i, t);
            }
            else {
              std::cout <<"Error: Cache Line doesn't have a read or write attribute\n";
            }
          }
          //Process Cache Line
          else if(cline[3] == "p2.tr") {
            //Read Action
            if(cline[1] == "0") {
              read(2, i, t);
            }
            //Write Action
            else if(cline[1] == "1") {
              write(2, i, t);
            }
            else {
              std::cout <<"Error: Cache Line doesn't have a read or write attribute\n";
            }
          }
          else if(cline[3] == "p3.tr") {
            //Read Action
            if(cline[1] == "0") {
              read(3, i, t);
            }
            //Write Action
            else if(cline[1] == "1") {
              write(3, i, t);
            }
            else {
              std::cout <<"Error: Cache Line doesn't have a read or write attribute\n";
            }
          }
          else {
            // std::cout << "Can not add Cache Line to cacheLinestable, because no proccessor: " <<pArr[0].filePath <<pArr[1].filePath <<pArr[2].filePath <<pArr[3].filePath <<"was found to write to.\n";
          }
          //Update Cache Lines by Removing First Element
          pArr[p].importTable.erase(pArr[p].importTable.begin());

          //Update Number of Cache Lines Proccessed
          cacheLines++;
          return false;
        }
    }
  }
  if(pArr[0].importTable.empty() && pArr[1].importTable.empty() && pArr[2].importTable.empty() && pArr[3].importTable.empty()) {
    std::cout<<"Cache Lines Finished\n";
    return true;
  }
}

//Start the program
int main(int argc, char* argv[]) {
  //Becomes True after action Completed
  bool i_finished = false;
  bool p_finished = false;

  //Test Print To Insure Initilization is Correct
  // for(int lcv = 0; lcv < 4; lcv++) {
  //   //pArr[lcv].printimport();
  // }

  //Start Clock
  t = clock();

  //Start Proccessing Cache Lines
  while(!p_finished) {
    if(getTime() % 1000 == 0) {
      std::cout<<std::string( 20, '-' ) <<">| Time Elapsed: " <<getTime()/1000 <<" | Cache Lines Processed: " <<cacheLines<<std::endl;
      std::cout <<std::endl;
    }
    p_finished = addCacheLine();
  }

  //Collect States before printing output
  for(int lcv = 0; lcv < 4; lcv++) {
    pArr[lcv].collectStates();
  }

  //Print Results of Program
  printOutput();

  //Final Run Time and line size
  double finalTime = getTime();
    std::cout <<"Time = " <<finalTime/1000 <<" Seconds.\n";

    // std::cout <<"Cache Lines: \n";
    // for(int p = 0; p < 4; p++) {
    //   // for(int cacheIndex = 0; cacheIndex < indexSize; cacheIndex++) {
    //     for(int tagIndex = 0; tagIndex < numOfIndexs; tagIndex++) {
    //       if(pArr[p].cacheTable[tagIndex][1] != "") {
    //         std::cout<<"Processor: "<<p <<" Line at [" <<tagIndex <<"] is: " <<pArr[p].cacheTable[tagIndex][0] <<std::endl;
    //       }
    //     }
    //   // }
    // }

    // std::cout <<"Lines in P0: " <<pArr[0].cacheLineIndex <<std::endl;
    // std::cout <<"Lines in P1: " <<pArr[1].cacheLineIndex <<std::endl;
    // std::cout <<"Lines in P2: " <<pArr[2].cacheLineIndex <<std::endl;
    // std::cout <<"Lines in P3: " <<pArr[3].cacheLineIndex <<std::endl;

  return 0;
}
