// $Id: main.cpp,v 1.7 2015-02-03 15:39:46-08 - - $

/*
  This Program was edited by
  Partner: Brandon Gomez (brlgomez@ucsc.edu)
  Partner: Antony Robbins (androbbi@ucsc.edu)
*/

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_pair = xpair<const string,string>;
using str_str_map = listmap<string,string>;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

/*
  http://stackoverflow.com/questions/1798112/
  removing-leading-and-trailing-spaces-from-a-string
*/
string trim(const std::string& str,
                 const std::string& whitespace = " \t"){
  const auto strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return ""; // no content
  const auto strEnd = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;
  return str.substr(strBegin, strRange);
}

string reduce(const std::string& str,
                   const std::string& fill = " ",
                   const std::string& whitespace = " \t"){
  // trim first
  auto result = trim(str, whitespace);
  // replace sub ranges
  auto beginSpace = result.find_first_of(whitespace);
  while (beginSpace != std::string::npos)
    {
      const auto endSpace = result.find_first_not_of
        (whitespace, beginSpace);
      const auto range = endSpace - beginSpace;
      result.replace(beginSpace, range, fill);
      const auto newStart = beginSpace + fill.length();
      beginSpace = result.find_first_of(whitespace, newStart);
    }
  return result;
}

void key_value (istream &file, str_str_map &test, int &count){
  for(;;){
    string line;
    
    getline(file, line);
    if(file.eof()) break;
    if(line.size() == 0){
      //cout << "this line is empty" << endl;
      continue;
    }
    // trim leading trailing and extra spaces
    cout << "-: " << count << ": " << line << endl;
    count++;
    line = trim(line);
    line = reduce(line);
     
    // do nothing on hash
    if(line.at(0) == '#') continue;

    // =, print all in lexico
    if(line.compare("=") == 0){
      for(auto it = test.begin(); it != test.end(); ++it){
        cout << it->first << " = " << it->second << endl;
      }
      //cout << "=" << endl;
      continue;
    }
     
    std::size_t found = line.find("=");
    // KEY, print key and associated value, 
    // else print not found
    if(found == string::npos){
      auto found_it = test.find(line);
      if(found_it != test.end()){
        cout << found_it->first << " = " << found_it->second << endl;
      }
      else{
        cout << line << ": key not found" << endl;
      }
      //cout << line << endl;
      continue;
    }
    // split at the site of a "="
    list<string> splitted = split(line, "=");
    // trim leading and trailing spaces
    splitted.front() = trim(splitted.front());
    splitted.back() = trim(splitted.back());
    if(splitted.front().compare(splitted.back()) == 0){
       
      // = VALUE, print lexico that has value
      if(found == 0){
        for(auto it = test.begin(); it != test.end(); ++it){
          if(it->second == splitted.back()){
            cout << it->first << " = " << it->second << endl;
          }
        }
        //cout <<"= "<<splitted.back() << endl;
        continue;
      }
       
      // KEY =, delete key and associated value
      else{
        auto found_it = test.find(splitted.front());
        if(found_it != test.end()){
          test.erase(found_it);
        }
        else{
          cout << splitted.front() 
               <<": key not found cannot delete" << endl;
        }
        //cout << splitted.front() << " =" << endl;
        continue;
      }
    }
       
    // KEY = VALUE 
    else{
      // insert into list
      str_str_pair pair({splitted.front(), splitted.back()});
      cout << splitted.front() << " = " << splitted.back() << endl;
      auto found_it = test.find(splitted.front());
      if(found_it != test.end()){ 
        found_it->second = splitted.back();
      }
      else{
        test.insert(pair);
      }
      continue;
    }
  }
}

int main (int argc, char** argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   str_str_map test{};
   int line_count = 1;
   if(argc > 1){
     for(int argi = 1; argi < argc; ++argi){
       string file_name = argv[argi];
       ifstream file(file_name.c_str());
       key_value(file, test, line_count);
       file.close();
     }
   }
   if(argc == 1){
     key_value(cin, test, line_count);
   }
   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}
