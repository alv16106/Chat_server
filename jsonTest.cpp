#include <nlohmann/json.hpp>
#include <stdlib.h>
#include<iostream> 
#include<string> // for string class 
using namespace std; 

// for convenience
using json = nlohmann::json;

int main(int argc, char const *argv[])
{
  json j;
  // add a number that is stored as double (note the implicit conversion of j to an object)
  j["pi"] = 3.141;

  // add a Boolean that is stored as bool
  j["happy"] = true;
  j["name"] = "Re";
  j["answer"]["everything"] = 0;
  json sub_j = j["answer"];
  int count = j["answer"].count("everything");
  string nombre = j["name"];
  if (!j.count("ye") && j.count("happy")) {
    printf("Si");
  }else
  {
    printf("si");
  }
  
  printf("El: %d", count);

  return 0;
}
