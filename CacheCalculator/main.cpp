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

  //Number of Transactions Executed
  int trans;

  //Vector Containing Transaction from Processors
  std::vector<std::string*> transactions;

  //Returns program time skewed by factor of skew
  int getTime() {
    //Increase to get faster proccessing
    //Warning: Increases chance of out of order transactions
    int skew = 1000;

    t = clock();
    float seconds = (((float)t)/CLOCKS_PER_SEC);
    return(seconds * skew);
  }

  struct Proccessor {
  //Proccessor Vars
  int lines;
  int valueIndex;
  char state;
  bool is_finished;
  std::vector<std::string*> tTable;
  std::vector<std::string*> transTable;
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

  //Returns Ptr to 2D array of Size 3xcountLines(filePath)
  void tagTable() {
    //Declare Variables
    std::string word = "";
    std::string line = "";
    std::ifstream file;
      file.open(filePath, std::ifstream::in);

    //Open File
    if (file.is_open()) {
        //Break File into Lines
        while(getline(file, line)) {
          std::string* importRow = new (std::nothrow) std::string[8];
          int cols = 0;

          //Remove Endline char
          line[line.size()] = ' ';

          //Make Stream Obj of String
          std::stringstream ssLine(line);

          //Break Lines into Words
          while(getline(ssLine, word, ' ')) {
            // std::cout<<word <<" (" <<cols <<") | ";

            //Assign Word to importTable
            importRow[cols] = word;
              cols++;
          }

          //Assign Transaction Processor Num
          importRow[3] = filePath;


          //initlize tag with states attached
          importRow[4] = "i";
          importRow[5] = "i";
          importRow[6] = "i";
          importRow[7] = "i";


          //Show Row Output
          //std::cout<<"| Row: " <<importRow[0] <<" | " <<importRow[1] <<" | " <<importRow[2] <<" | " <<importRow[3] <<" | \n";

          //Add Row to tagTable
          tTable.push_back(importRow);
        }
      file.close();
    }
    else {
        std::cout << "Error opening file";
    }
      file.close();
  }

  //Output Copy of tTable
  void printTTable() {
    for(int lcv = 0; lcv < tTable.size(); lcv++) {
      std::string* currentArr = tTable.at(lcv);
      for(int vals = 0; vals < 3; vals++) {
        std::cout <<currentArr[vals] <<" , ";
      }
      std::cout <<"\n----------------------------\n";
    }
  }

  //Return Position of Tag in Proccessor's Tag Array
  int findTag(std::string t) {
    int pos = 0;
    for(int lcv = 0; lcv < transTable.size(); lcv++) {
      std::cout<<"Comparing " <<transTable.at(lcv)[2].substr(0, 8) <<" to " <<t <<std::endl;
      if(transTable.at(lcv)[2].substr(0, 7) == t) {
        std::cout<<"Found";
        return(pos);
      }
      else{
        pos++;
      }
    }
    return(-1);
  }

  //Updates Proccessor State
  bool updateState(std::string tag, int pNum, std::string newState) {
    std::string validStates[5] = {"m", "o", "e", "s", "i"};

    for(int lcv = 0; lcv < 5; lcv++) {
      //Valid State Change
      if(newState == validStates[lcv]) {
        int pos = findTag(tag);
        if(pos != -1) {
          transTable.at(pos)[2+pNum] = newState;

          //Increment State for Processor
          linesInState[lcv]++;
          std::cout<<"State: " <<validStates[lcv] <<'=' <<linesInState[lcv] <<" incremented.\n";
          return(true);
        }
        else {
          // std::cout<<"Tag Not Found!\n";
          // std::cout<<"Tag " <<tag <<"processor: " <<pNum <<" State: " <<newState <<". \n";
        }
      }
    }
  return(false);
  }

  //Proccessor Constructor
  Proccessor(std::string path) {
    filePath = path;
    valueIndex = 0;
    is_finished = false;
    lines = countLines(filePath);
    tagTable();
  }
};

//Create Proccessor Objs
Proccessor pArr[4] = {Proccessor("p0.tr"), Proccessor("p1.tr"), Proccessor("p2.tr"), Proccessor("p3.tr")};

//Adds Data if timestamp < currentTime
bool startImport() {
      if(!(pArr[0].is_finished && pArr[1].is_finished && pArr[2].is_finished && pArr[3].is_finished)) {
        for(int lcv = 0; lcv < 4; lcv++) {
          //Check if Record is ready to add
          if(pArr[lcv].valueIndex < pArr[lcv].lines && !pArr[lcv].is_finished) {
            std::string* currArr = pArr[lcv].tTable.at(pArr[lcv].valueIndex);
            if(std::stoi(currArr[0]) < getTime()) {
              // std::cout<<"----------------------------------Adding Line---------------------------------\n";
              // std::cout <<"tTIme: " <<currArr[0] <<" and Compared Time " <<getTime() <<std::endl;
              // std::cout<<"Starting at index: " <<pArr[lcv].valueIndex <<" and with P"<<lcv <<'\n';
              // std::cout<<"----------------------------------Line Added----------------------------------\n\n";
              transactions.push_back(currArr);
              pArr[lcv].valueIndex++;
            }
          }
          else {
            pArr[lcv].is_finished = true;
          }
        }
    }
    else {
      std::cout << "Import has finshed! \n";
      return(true);
    }
    return(false);
}

void printOutput() {
  std::string states[] = {"m", "o", "e", "s", "i"};

  std::cout<<"----------------------------------------------------------------------------------\n";
  //Output 1: Cache Transfers
  std::cout << "The total number of cache-to-cache transfers for each processor pair \n";
  for(int p = 0; p < 4; p++) {
    std::cout <<"P" <<p <<" Cache Transfers: ";
    for(int lcv = 0; lcv < 3; lcv++) {
      std::cout <<states[lcv] <<" = " <<pArr[p].cacheTransfers[lcv] <<" ";
    }
    std::cout<<std::endl;
  }
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
  //Output 3: Dirty Write Backs
  std::cout << "The number of dirty writebacks from each processor.\n";
  for(int p = 0; p < 4; p++) {
    std::cout <<"P" <<p <<": " <<states[p] <<" = " <<pArr[p].dirtyWBs <<" ";
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
}

//busReadX <physical memory address>
//Update everything to invalid state
bool busUpgr(std::string tag) {
  for(int lcv = 0; lcv < 4; lcv++) {
    int tagPos = pArr[lcv].findTag(tag);
    if(tagPos != -1) {
      pArr[lcv].updateState(tag, lcv, "i");
    }
  }
  return false;
}

//Read Transactions
bool read(int pNum, std::string pAddress) {
  bool exclusive = true;

  //Parse physical address
  //0x Plus 5Bits + index
  std::string tag = pAddress.substr(0, 8);

  //2 Bits
  std::string offset = pAddress.substr(9, 10);
    //std::cout <<"Tag: " <<tag <<" | index: " <<index <<" | offset: " <<offset <<std::endl;

  //BusRd
  for(int lcv = 0; lcv < 4; lcv++) {
    //Other Cache has Data
    int tagPos = pArr[lcv].findTag(tag);
    if(tagPos != -1 && pNum != lcv) {
      std::string* currTrans = pArr[lcv].transTable.at(tagPos);
      //Other Cache in Modified State
      if(currTrans[lcv] == "m") {
        pArr[pNum].updateState(tag, pNum, "s");
        pArr[pNum].updateState(tag, lcv, "o");
      }
      //Other Cache in Sharred State
      else {
        pArr[pNum].updateState(tag, pNum, "s");
        pArr[pNum].updateState(tag, lcv, "s");
      }
      exclusive = false;
    }
  }
  // Only Copy of Data
  if(exclusive) {
    //Update Active Proccessor to Exclusive State
    pArr[pNum].updateState(tag, pNum, "e");
    return(true);
  }
  return(false);
}

//Write Transactions
bool write(int pNum, std::string pAddress) {
  //Parse physical address
  //0x Plus 5Bits + index
  std::string tag = pAddress.substr(0, 8);

  //2 Bits
  std::string offset = pAddress.substr(9, 10);
    //std::cout <<"Tag: " <<tag <<" | index: " <<index <<" | offset: " <<offset <<std::endl;

  //Change to Modified State
  for(int lcv = 0; lcv < 4; lcv++) {
    //Call BusUpgr to Invalidate States
    busUpgr(tag);

    //Change State to Modified
    pArr[pNum].updateState(tag, pNum, "m");
  }


}

//Pulls from Transaction Vector to Start Prcessing Transactions
bool pushTransaction() {
  if(!transactions.empty()) {
    //Get First Transaction of Vector
    //Arr = timestamp | read/write | memory address | filePath | P0 State | P1 State | P2 State | P3 State
    std::string* tag = transactions.front();

      //Push Finished Transaction to transTable
      if(tag[3] == "p0.tr") {
        //Read Action
        if(tag[1] == "0") {
          read(0, tag[2]);
        }
        //Write Action
        else if(tag[1] == "1") {
          write(0, tag[2]);
        }
        else {
          std::cout <<"Error: transaction doesn't have a read or write attribute\n";
        }
        pArr[0].transTable.push_back(tag);
      }
      else if(tag[3] == "p1.tr") {
        //Read Action
        if(tag[1] == "0") {
          read(0, tag[2]);
        }
        //Write Action
        else if(tag[1] == "1") {
          write(0, tag[2]);
        }
        else {
          std::cout <<"Error: transaction doesn't have a read or write attribute\n";
        }
        pArr[1].transTable.push_back(tag);
      }
      else if(tag[3] == "p2.tr") {
        //Read Action
        if(tag[1] == "0") {
          read(0, tag[2]);
        }
        //Write Action
        else if(tag[1] == "1") {
          write(0, tag[2]);
        }
        else {
          std::cout <<"Error: transaction doesn't have a read or write attribute\n";
        }
        pArr[2].transTable.push_back(tag);
      }
      else if(tag[3] == "p3.tr") {
        //Read Action
        if(tag[1] == "0") {
          read(0, tag[2]);
        }
        //Write Action
        else if(tag[1] == "1") {
          write(0, tag[2]);
        }
        else {
          std::cout <<"Error: transaction doesn't have a read or write attribute\n";
        }
        pArr[3].transTable.push_back(tag);
      }
      else {
        // std::cout << "Can not add transaction to trans table, because no proccessor: " <<pArr[0].filePath <<pArr[1].filePath <<pArr[2].filePath <<pArr[3].filePath <<"was found to write to.\n";
      }

    //Update Transactions by Removing First Element
    transactions.erase(transactions.begin());

    //Update Number of Transactions Proccessed
    trans++;
    return false;
  }
  return true;
}

//Start the program
int main(int argc, char* argv[]) {
  //Becomes True after action Completed
  bool i_finished = false;
  bool p_finished = false;


  //Test Print To Insure Initilization is Correct
  // for(int lcv = 0; lcv < 4; lcv++) {
  //   //pArr[lcv].printTTable();
  // }

  //Start Importing Transactions
  while(!i_finished) {
    if(getTime() % 1000 == 0) {
      std::cout<<">| Time Elapsed: " <<getTime()/1000 <<" | Trasactions Processed: " <<trans <<std::endl;
      std::cout <<std::endl << std::string( 20, '-' );
    }
    i_finished =  startImport();
  }

  //Start Clock
  t = clock();

  //Start Proccessing Transactions
  while(!p_finished) {
    if(getTime() % 1000 == 0) {
      std::cout<<">| Time Elapsed: " <<getTime()/1000 <<" | Trasactions Processed: " <<trans <<std::endl;
      std::cout <<std::endl << std::string( 20, '-' );
    }
    p_finished = pushTransaction();
  }

  //Print Results of Program
  printOutput();

  //Final Run Time
  double finalTime = getTime();
    std::cout <<"Time = " <<finalTime/1000 <<" Seconds.\n";

  return 0;
}
