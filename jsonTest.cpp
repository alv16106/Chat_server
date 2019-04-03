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
  vector<json> k;
  // add a number that is stored as double (note the implicit conversion of j to an object)
  j["pi"] = 3.141;

  // add a Boolean that is stored as bool
  j["happy"] = true;
  j["name"] = "Re";
  j["answer"]["everything"] = 0;
  k.push_back(j);
  auto j3 = json::parse("{ \"happy\": [\"1\",\"2\"], \"pi\": 3.141 }");
  k.push_back(j3);
  json prueba;
  prueba["Code"] = 2;
  prueba["Jsons"] = k;
  vector<int *> juan;
  int a = 2;
  int b = 5;
  juan.push_back(&a);
  juan.push_back(&b);
  a = a+5;
  cout << *juan[0] << endl;
  json sub_j = j["answer"];
  int count = j["answer"].count("everything");
  string nombre = j["name"];
  vector<string> g1 = j3["happy"];
  if (!j.count("ye") && j.count("happy")) {
    printf("Si");
  }else
  {
    printf("si");
  }
  
  printf("El: %d", count);

  return 0;
}
