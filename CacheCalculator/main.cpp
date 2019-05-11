//Tomorrow
//Import 100 Elements at a time to list.
//Pop_front elements that are ready
//If list size gt 100 wait

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <array>
#include <thread>
#include <chrono>
#include <vector>

//Globar Variables
  //Global Timer
  clock_t t;
  std::vector<std::string*> transactions;
  std::ifstream fileArr[4];

  int getTime() {
    //Increase to get faster proccessing
    //Warning: Increases chance of out of order transactions
    int skew = 1000;

    t = clock();
    float seconds = (((float)t)/CLOCKS_PER_SEC);
    return(seconds * 1000);
  }

  struct Proccessor {
  //Proccessor Vars
  int lines;
  int valueIndex;
  bool is_finished;
  std::vector<std::string*> tTable;
  std::string filePath;

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
          std::string* importRow = new (std::nothrow) std::string[4];
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
          //Initilize State
          importRow[3] = "i";

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

  void printTTable() {
    for(int lcv = 0; lcv < lines; lcv++) {
      std::string* currentArr = tTable.at(lcv);
      for(int vals = 0; vals < 4; vals++) {
        std::cout <<currentArr[vals] <<" , ";
      }
      std::cout <<"\n----------------------------\n";
    }
  }

  //Updates Proccessor State
  bool updateState(int rowNum, char newState) {
    char validStates[5] = {'e', 'i', 's', 'o', 'm'};
    std::string* row = tTable.at(rowNum);

    for(int lcv = 0; lcv < 5; lcv++) {
      //Valid State Change
      if(newState == validStates[lcv]) {
        row[3] = newState;
        return(true);
      }
      else {
        std::cout<<"Can't update processor state with passed input.\n";
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
void startImport() {
  std::cout<<"Starting Import \n";
  while(1) {
      if(!(pArr[0].is_finished && pArr[1].is_finished && pArr[2].is_finished && pArr[3].is_finished)) {
        for(int lcv = 0; lcv < 4; lcv++) {
          //Check if Record is ready to add
          if(pArr[lcv].valueIndex < pArr[lcv].lines && !pArr[lcv].is_finished) {
            std::string* currArr = pArr[lcv].tTable.at(pArr[lcv].valueIndex);
            if(std::stoi(currArr[0]) < getTime()) {
              std::cout<<"----------------------------------Adding Line---------------------------------\n";
              std::cout <<"tTIme: " <<currArr[0] <<" and Compared Time " <<getTime() <<std::endl;
              std::cout<<"Starting at index: " <<pArr[lcv].valueIndex <<" and with P"<<lcv <<'\n';
              std::cout<<"----------------------------------Line Added----------------------------------\n\n";
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
      break;
    }
  }
}

//busRead <physical memory address>
bool busRd(Proccessor* processors, int activeProcessor, std::string value) {
  bool exclusive = true;

  // for(int lcv = 0; lcv <= 3; lcv++) {
  //   //Other Cache has Data
  //   if(processors[activeProcessor].startImport == processors[lcv] && activeProcessor != lcv) {
  //     exclusive = false;
  //   }
  // }
  //Only Copy of Data
  // if(exclusive) {
  //   //Update Active Proccessor to Exclusive State
  //   processors[activeProcessor].updateState(processors[activeProcessor].valueIndex, 'e');
  //   return(true);
  // }
  // return(false);
}

//Cache Functions
//busReadX <physical memory address>
bool busRdX(Proccessor* processors, int activeProcessor, std::string value) {
  return(1);
}

//ProccessorRead <physical memory address>
bool prRd(Proccessor* processors, int activeProcessor, std::string value) {
  return(1);
}

//ProccessorWrite <physical memory address>
bool prWr(Proccessor* processors, int activeProcessor, std::string value) {
  return(1);
}

//busUpgrade <physical memory address>
bool busUpgr(Proccessor* processors, int activeProcessor, std::string value) {
  return(1);
}

//Flush
bool flush(Proccessor* processors, int activeProcessor, std::string value) {
  return(1);
}

//Start the program
int main(int argc, char* argv[]) {
  //Start Clock
  t = clock();

  //Test Print To Insure Initilization is Correct
  for(int lcv = 0; lcv < 4; lcv++) {
    //pArr[lcv].printTTable();
  }

  //Start Processing Transactions
  startImport();

  //Final Run Time
  double finalTime = getTime();
    std::cout <<"Time = " <<finalTime <<std::endl;

  return 0;
}
