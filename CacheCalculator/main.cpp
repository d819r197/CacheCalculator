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
#include <list>

//Globar Variables
  //Global Timer
  clock_t t;
  std::list<std::string*> transactions;
  std::ifstream fileArr[4];

  float getTime() {
    t = clock();
    float seconds = (((float)t)/CLOCKS_PER_SEC);
    return(seconds * 1000);
  }

  // struct Proccessor {
  //   //Proccessor Vars
  //   int lines;
  //   std::string* currentTag = nullptr;
  //   std::string filePath;
  //   std::string data;
  //   //std::string** importTable = new std::string*[4];
  //   //int valueIndex;
  //
  //   //Proccessor Functions
  //   //Returns Number of Lines in a Given File
  //   int countLines(std::string filePath) {
  //     int number_of_lines = 0;
  //     std::string line;
  //     std::ifstream file;
  //       file.open(filePath, std::ifstream::in);
  //
  //     if (file.is_open()) {
  //       while (std::getline(file, line)) {
  //         ++number_of_lines;
  //       }
  //       return(number_of_lines);
  //     }
  //     else {
  //       std::cout << "Error opening file";
  //     }
  //     return (0);
  //   }
  //
  //   //Updates Proccessor State
  //   // bool updateState(int row, char newState) {
  //   //   char validStates[5] = {'e', 'i', 's', 'o', 'm'};
  //   //   for(int lcv = 0; lcv < 5; lcv++) {
  //   //     //Valid State Change
  //   //     if(newState == validStates[lcv]) {
  //   //       importTable[row][4] = newState;
  //   //       return(true);
  //   //     }
  //   //     else {
  //   //       std::cout<<"Can't update processor state with passed input.\n";
  //   //     }
  //   //   }
  //   //   return(false);
  //   // }
  //
  //   void collectRaw() {
  //     std::ifstream file(filePath);
  //     // constexpr size_t bufferSize = 1024 * 1024;
  //     constexpr size_t bufferSize = 1024 * 1024;
  //     std::unique_ptr<char[]> buffer(new char[bufferSize]);
  //     while (file)
  //     {
  //         file.read(buffer.get(), bufferSize);
  //         // process data in buffer
  //         // std::cout<<buffer <<"\n----------------------------------\n";
  //         data=buffer.get();
  //     }
  //     checkRaw();
  //   }
  //
  //   void checkRaw() {
  //     std::string time = data.substr(0, data.find(" "));
  //     std::string wr = data.substr(3, data.find(" "));
  //     std::string value = data.substr(5, data.find("\n"));
  //       currentTag[0] = time;
  //       currentTag[1] = wr;
  //       currentTag[2] = value;
  //
  //       std::cout<<"Time: " <<time <<" Read/Write: " <<wr <<" Data: " <<value <<std::endl;
  //   }
  //
  //   //Proccessor Constructor
  //   Proccessor(std::string path) {
  //     filePath = path;
  //     std::string arr[4] = {"0", "", "", "i"};
  //       std::string *currentTag = arr;
  //   }
  // };

  struct Proccessor {
  //Proccessor Vars
  int lines;
  int valueIndex;
  std::string** importTable = new (std::nothrow) std::string*[4];
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
          importTable[rows] = new (std::nothrow) std::string[countLines(filePath)];

          //Remove Endline char
          line[line.size()] = ' ';

          //Make Stream Obj of String
          std::stringstream ssLine(line);

          //Break Lines into Words
          while(getline(ssLine, word, ' ')) {
            std::cout<<word <<" (" <<rows <<',' <<cols <<") | ";

            //Slow Down Proccess to Avoid Error: Incorrect checksum for freed object
            std::this_thread::sleep_for (std::chrono::nanoseconds(600));

            //Assign Word to importTable
            importTable[rows][cols] = word;
              cols++;

            //Initilize State
            // importTable[rows][cols] = "i";
          }
          std::cout<<"\n------------------------------------------------\n";
          rows++;
        }
      std::cout<<"\n------------------------------------------------DONE------------------------------------------------\n";
      file.close();
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


  }
};

  //Create Proccessor Objs
  //Initilizes first entry and filePath
  Proccessor pArr[4] = {Proccessor("p0.tr"), Proccessor("p1.tr"), Proccessor("p2.tr"), Proccessor("p3.tr")};

//Adds Data if timestamp < currentTime
// void startImport() {
//   std::cout<<"Starting Import \n";
//   //Declare Variables
//   std::string word = "";
//   std::string line = "";
//
//   int tagIndex = 0;
//   if(fileArr[0].eof() && fileArr[1].eof() && fileArr[2].eof() && fileArr[3].eof()) {
//     //No Files with Data Left
//     std::cout << "Files have finished executing\n";
//   }
//   else {
//     for(int lcv = 0; lcv < 4; lcv++) {
//       std::cout<<"Starting: " <<lcv <<'\n';
//       //Check File is_open
//       if (fileArr[lcv].is_open()) {
//         std::cout<<"Triggering P" <<lcv <<'\n';
//
//         //Check if Record is ready to add
//         if(stoi(pArr[1].currentTag[0]) < getTime()) {
//           std::cout<<"Adding Line\n";
//
//           //Break File into Lines
//           if(getline(fileArr[lcv], line)) {
//
//             //Remove Endline char
//             line[line.size()] = ' ';
//
//             //Make Stream Obj of String
//             std::stringstream ssLine(line);
//
//             //Break Lines into Words
//             while(getline(ssLine, word, ' ')) {
//
//               std::cout<<"TI: " <<tagIndex <<"lcv: " <<lcv <<"\n";
//
//               //Assign Word to importTable
//               pArr[lcv].currentTag[tagIndex] = word;
//
//               tagIndex++;
//             }
//
//             std::cout<<"Processor: " <<lcv <<" adding: " <<pArr[lcv].currentTag[2]  <<" \n";
//             tagIndex = 0;
//             }
//           }
//
//         }
//       else {
//           std::cout << "Error opening file";
//       }
//     }
//     startImport();
//   }
// }

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

  //Open all the files
  for(int lcv = 0; lcv < 4; lcv++) {
    //pArr[lcv].collectRaw();
  }

  //Start Processing Transactions
  //startImport();
  // startImport();

  //Close all the files
  // for(int lcv = 0; lcv < 4; lcv++) {
  //   fileArr[lcv].close();
  // }

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
