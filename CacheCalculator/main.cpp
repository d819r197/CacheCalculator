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
  std::vector<string[3]> transactions;

  float getTime() {
    t = clock();
    float seconds = (((float)t)/CLOCKS_PER_SEC);

    return(seconds*100);
  }

struct Proccessor {
  //Proccessor Vars
  int lines;
  int valueIndex;
  std::string filePath;
  std::string** importTable = new std::string*[4];

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
      return(number_of_lines);
    }
    else {
      std::cout << "Error opening file";
    }
    return (0);
  }

  //Updates Proccessor State
  bool updateState(int row, char newState) {
    char validStates[5] = {'e', 'i', 's', 'o', 'm'};
    for(int lcv = 0; lcv < 5; lcv++) {
      //Valid State Change
      if(newState == validStates[lcv]) {
        importTable[row][4] = newState;
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
    string currentTag[3];
    //lines = countLines(filePath);
    //importTable = startImport(filePath);
    // std::string** tTable = new std::string*[4];
    //
    // for(int rows = 0; rows < lines; rows++) {
    //   tTable[rows] = new std::string[lines];
    //
    // }

    //Initilize tTable First State
    //tTable[0][3] = "i";
  }
};

//Adds Data if timestamp < currentTime
void startImport(Proccessor pArr[4]) {
  //Declare Variables
  std::string word = "";
  std::string line = "";
  std::ifstream fileArr[3];
  int rows = 0;

  for(int lcv = 0; lcv < 4; lcv++) {
    if(rows < pArr[lcv].countLines(pArr[lcv].filePath)) {
    fileArr[lcv].open(pArr[lcv].filePath, std::ifstream::in);

    //Open File
    if (fileArr[lcv].is_open()) {

        //Break File into Lines
        if(getline(fileArr[lcv], line)) {
          int cols = 0;
          //Count Number of Lines
          pArr[lcv].importTable[rows] = new std::string[pArr[lcv].countLines(pArr[lcv].filePath)];

          //Remove Endline char
          line[line.size()] = ' ';

          //Make Stream Obj of String
          std::stringstream ssLine(line);

          //Break Lines into Words
          while(getline(ssLine, word, ' ')) {
            std::cout<<word <<" (" <<rows <<',' <<cols <<") | ";

            //Assign Word to importTable
            pArr[lcv].importTable[rows][cols] = word;

            //Initilize State
            pArr[lcv].importTable[rows][3] = "i";

            cols++;
          }

          //Check if Record is ready to add
          while(getTime() < stof(pArr[lcv].importTable[rows][0])) {
             std::this_thread::sleep_for (std::chrono::nanoseconds(50));
          }
          std::cout<<"Processor: " <<lcv <<" | Time: " <<getTime() <<" | adding tag: " <<pArr[lcv].importTable[rows][2] <<std::endl;

          // std::cout<<"\n------------------------------------------------\n";
        }
    }
    else {
        std::cout << "Error opening file";
    }
  }
}
  rows++;

  //   //Open File
  //   if (fileArr[lcv].is_open()) {
  //       int rows = 0;
  //
  //       //Break File into Lines
  //       while(getline(fileArr[lcv], line)) {
  //         int cols = 0;
  //         //Count Number of Lines
  //         pArr[lcv].importTable[rows] = new std::string[pArr[lcv].countLines(pArr[lcv].filePath)];
  //
  //         //Remove Endline char
  //         line[line.size()] = ' ';
  //
  //         //Make Stream Obj of String
  //         std::stringstream ssLine(line);
  //
  //         //Break Lines into Words
  //         while(getline(ssLine, word, ' ')) {
  //           // std::cout<<word <<" (" <<rows <<',' <<cols <<") | ";
  //
  //           //Assign Word to importTable
  //           pArr[lcv].importTable[rows][cols] = word;
  //
  //           //Initilize State
  //           pArr[lcv].importTable[rows][3] = "i";
  //
  //           cols++;
  //         }
  //
  //         //Check if Record is ready to add
  //         while(getTime() < stof(pArr[lcv].importTable[rows][0])) {
  //            std::this_thread::sleep_for (std::chrono::nanoseconds(50));
  //         }
  //         std::cout<<"Processor: " <<lcv <<" | Time: " <<getTime() <<" | adding tag: " <<pArr[lcv].importTable[rows][2] <<std::endl;
  //
  //         // std::cout<<"\n------------------------------------------------\n";
  //         rows++;
  //       }
  //   }
  //   else {
  //       std::cout << "Error opening file";
  //   }
  //     fileArr[lcv].close();
  // }
  for(int lcv = 0; lcv < 4; lcv++) {
    fileArr[lcv].close();
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
  if(exclusive) {
    //Update Active Proccessor to Exclusive State
    processors[activeProcessor].updateState(processors[activeProcessor].valueIndex, 'e');
    return(true);
  }
  return(false);
}

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
int main(int argc, char* argv[])
{
  //Create Proccessor Objs
  //Initilizes number of Lines, startImport, and the filePath
  Proccessor pArr[4] = {Proccessor("p0.tr"), Proccessor("p1.tr"), Proccessor("p2.tr"), Proccessor("p3.tr")};

  //Start Clock
  t = clock();

  //Start Processing Transactions
  startImport(pArr);



  //Move Cache Transaction from importTable to tTable
  // for(int lcv = 0; lcv < 4; lcv++) {
  //   std::cout <<"Times: " <<pArr[lcv].importTable[pArr[lcv].valueIndex][0] <<std::endl;
  //   //Proccesor Job Triggered
  //   if(std::stof(pArr[lcv].importTable[pArr[lcv].valueIndex][0]) > t) {
  //     std::cout<<"adding job, current index: " <<pArr[lcv].valueIndex <<std::endl;
  //     pArr[lcv].tTable[pArr[lcv].valueIndex] = pArr[lcv].importTable[pArr[lcv].valueIndex];
  //     pArr[lcv].valueIndex++;
  //   }
  // }

  return 0;
}
