#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <array>
#include <thread>
#include <chrono>


struct Proccessor {
  //Proccessor Vars
  int lines;
  int valueIndex;
  std::string** importTable;
  std::string** tTable;
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
      return(number_of_lines);
    }
    else {
      std::cout << "Error opening file";
    }
    return (0);
  }

  //Returns Ptr to 2D array of Size 3xcountLines(filePath)
  std::string** tagTable(std::string filePath) {
    //Declare Variables
    std::string** importTable = new std::string*[4];
    std::string word = "";
    std::string line = "";
    std::ifstream file;
      file.open(filePath, std::ifstream::in);

    //Open File
    if (file.is_open()) {
        int rows = 0;

        //Break File into Lines
        while(getline(file, line)) {
          int cols = 0;
          //Count Number of Lines
          importTable[rows] = new std::string[countLines(filePath)];

          //Remove Endline char
          line[line.size()] = ' ';

          //Make Stream Obj of String
          std::stringstream ssLine(line);

          //Break Lines into Words
          while(getline(ssLine, word, ' ')) {
            std::cout<<word <<" (" <<rows <<',' <<cols <<") | ";

            //Slow Down Proccess to Avoid Error: Incorrect checksum for freed object
            std::this_thread::sleep_for (std::chrono::nanoseconds(250));

            //Assign Word to importTable
            importTable[rows][cols] = word;
              cols++;

            //Initilize State
            importTable[rows][cols] = "i";
          }
          std::cout<<"\n------------------------------------------------\n";
          rows++;
        }
      return (importTable);
    }
    else {
        std::cout << "Error opening file";
    }
      file.close();
      return(nullptr);
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
    lines = countLines(filePath);
    importTable = tagTable(filePath);
    std::string** tTable = new std::string*[4];

    for(int rows = 0; rows < lines; rows++) {
      tTable[rows] = new std::string[lines];

      //Slow Down Proccess to Avoid Error: Incorrect checksum for freed object
      std::this_thread::sleep_for (std::chrono::nanoseconds(250));
    }

    //Initilize tTable First State
    //tTable[0][3] = "i";
  }
};

//busRead <physical memory address>
bool busRd(Proccessor* processors, int activeProcessor, std::string value) {
  bool exclusive = true;

  // for(int lcv = 0; lcv <= 3; lcv++) {
  //   //Other Cache has Data
  //   if(processors[activeProcessor].tagTable == processors[lcv] && activeProcessor != lcv) {
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
  //Global Timer
  clock_t t;

  //Start Clock
  t = clock();

  //Create Proccessor Objs
  //Initilizes number of Lines, tagTable, and the filePath
  Proccessor pArr[4] = {Proccessor("p0.tr"), Proccessor("p1.tr"), Proccessor("p2.tr"), Proccessor("p3.tr")};
    std::cout<<"SIZE: " <<sizeof(pArr) <<'\n';

  //Move Cache Transaction from importTable to tTable
  for(int lcv = 0; lcv < 4; lcv++) {
    std::cout <<"Times: " <<pArr[lcv].importTable[pArr[lcv].valueIndex][0] <<std::endl;
    //Proccesor Job Triggered
    // if(std::stof(pArr[lcv].importTable[pArr[lcv].valueIndex][0]) > t) {
    //   std::cout<<"adding job, current index: " <<pArr[lcv].valueIndex <<std::endl;
    //   pArr[lcv].tTable[pArr[lcv].valueIndex] = pArr[lcv].importTable[pArr[lcv].valueIndex];
    //   pArr[lcv].valueIndex++;
    // }
  }

  return 0;
}
