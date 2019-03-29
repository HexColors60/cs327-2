#include "monsterdef.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

#include "dungeon.h"
#include "heap.h"
#include "move.h"
#include "npc.h"
#include "pc.h"
#include "character.h"
#include "utils.h"
#include "path.h"
#include "event.h"
#include "io.h"

#define DEBUG_PARSE 0

using namespace std;

void parse_monsters(std::string filename){
    std::string home = getenv("HOME");
    std::string path = "/.rlg327/";
    std::ifstream infile(home + path + filename);
    int num_parsed_monsters = 0;
    std::string line;
    std::getline(infile, line);
    if(line.compare("RLG327 MONSTER DESCRIPTION 1") != 0){
      fprintf(stderr, "File must begin with \"RLG327 MONSTER DESCRIPTION 1\"");
      exit(-1);
    }
    while(std::getline(infile, line)){
      if(line.compare("BEGIN MONSTER") == 0){
        num_parsed_monsters++;
      }
    }
    cout << "Monsters found: " << num_parsed_monsters << endl;

    monsterdef defs[num_parsed_monsters];

    infile.clear();
    infile.seekg(0, ios::beg);
    while(std::getline(infile, line)){
      if(line.compare("BEGIN MONSTER") == 0){
        std::getline(infile, line);
        break;
      }
    }
    int i, x;
    for(i = 0; i < num_parsed_monsters; i++){
      for(x = 0; x < 9; x++){
        defs[i].valid[x] = 0;
      }
      for(; line.compare("END") != 0; std::getline(infile, line)){
        std::string firstWord = line.substr(0, line.find(" "));
        if(firstWord.compare("NAME") == 0){
          defs[i].name = line.substr(line.find_first_of(" \t")+1);
          defs[i].valid[NAME]++;
        } else if (firstWord.compare("SYMB") == 0){
          defs[i].symb = line.substr(line.find_first_of(" \t")+1).c_str()[0];
          defs[i].valid[SYMB]++;
        } else if (firstWord.compare("COLOR") == 0){
          for(x = 0; x < NUM_COLORS; x++){
            defs[i].color[x] = 0;
          }
          if(line.find("BLACK") != std::string::npos){
            defs[i].color[BLACK] = 1;
          }
          if(line.find("RED") != std::string::npos){
            defs[i].color[RED] = 1;
          }
          if(line.find("GREEN") != std::string::npos){
            defs[i].color[GREEN] = 1;
          }
          if(line.find("YELLOW") != std::string::npos){
            defs[i].color[YELLOW] = 1;
          }
          if(line.find("BLUE") != std::string::npos){
            defs[i].color[BLUE] = 1;
          }
          if(line.find("MAGENTA") != std::string::npos){
            defs[i].color[MAGENTA] = 1;
          }
          if(line.find("CYAN") != std::string::npos){
            defs[i].color[CYAN] = 1;
          }
          if(line.find("WHITE") != std::string::npos){
            defs[i].color[WHITE] = 1;
          }
          defs[i].valid[COLOR]++;
        } else if (firstWord.compare("DESC") == 0){
          std::getline(infile, line);
          while(line.compare(".") != 0){
            defs[i].desc += line;
            defs[i].desc += "\n";
            std::getline(infile, line);
          }
          defs[i].valid[DESC]++;
        } else if (firstWord.compare("SPEED") == 0){
          std::string dice = line.substr(line.find_first_of(" \t")+1);
          defs[i].speed_base = std::stoi(dice.substr(0,dice.find("+")));
          dice = dice.substr(dice.find_first_of("+")+1);
          defs[i].speed_dice = std::stoi(dice.substr(0,dice.find("d")));
          dice = dice.substr(dice.find_first_of("d")+1);
          defs[i].speed_sides = std::stoi(dice);
          defs[i].valid[SPEED]++;
        } else if (firstWord.compare("DAM") == 0){
          std::string dice = line.substr(line.find_first_of(" \t")+1);
          defs[i].dam_base = std::stoi(dice.substr(0,dice.find("+")));
          dice = dice.substr(dice.find_first_of("+")+1);
          defs[i].dam_dice = std::stoi(dice.substr(0,dice.find("d")));
          dice = dice.substr(dice.find_first_of("d")+1);
          defs[i].dam_sides = std::stoi(dice);
          defs[i].valid[DAM]++;
        } else if (firstWord.compare("HP") == 0){
          std::string dice = line.substr(line.find_first_of(" \t")+1);
          defs[i].hp_base = std::stoi(dice.substr(0,dice.find("+")));
          dice = dice.substr(dice.find_first_of("+")+1);
          defs[i].hp_dice = std::stoi(dice.substr(0,dice.find("d")));
          dice = dice.substr(dice.find_first_of("d")+1);
          defs[i].hp_sides = std::stoi(dice);
          defs[i].valid[HP]++;
        } else if (firstWord.compare("ABIL") == 0){
          for(x = 0; x < NUM_ABILITIES; x++){
            defs[i].abil[x] = 0;
          }
          if(line.find("SMART") != std::string::npos){
            defs[i].abil[SMART] = 1;
          }
          if(line.find("TELE") != std::string::npos){
            defs[i].abil[TELE] = 1;
          }
          if(line.find("TUNNEL") != std::string::npos){
            defs[i].abil[TUNNEL] = 1;
          }
          if(line.find("ERRATIC") != std::string::npos){
            defs[i].abil[ERRATIC] = 1;
          }
          if(line.find("PASS") != std::string::npos){
            defs[i].abil[PASS] = 1;
          }
          if(line.find("PICKUP") != std::string::npos){
            defs[i].abil[PICKUP] = 1;
          }
          if(line.find("UNIQ") != std::string::npos){
            defs[i].abil[UNIQ] = 1;
          }
          if(line.find("BOSS") != std::string::npos){
            defs[i].abil[BOSS] = 1;
          }
          if(line.find("DESTROY") != std::string::npos){
            defs[i].abil[DESTROY] = 1;
          }
          defs[i].valid[ABIL]++;
        } else if (firstWord.compare("RRTY") == 0){
          defs[i].rrty = std::stoi(line.substr(line.find_first_of(" \t")+1));
          defs[i].valid[RRTY]++;
        }
      }

      defs[i].isValid = 1;
      for(x = 0; x < NUM_FIELDS; x++){
        if(defs[i].valid[x] != 1){
          defs[i].isValid = 0;
          break;
        }
      }

      if(!DEBUG_PARSE && defs[i].isValid){
        cout << "Name: " << defs[i].name << endl;
        cout << "Description: " << endl << defs[i].desc;
        cout << "Colors: ";
        if(defs[i].color[0]){
          cout << "BLACK ";
        }
        if(defs[i].color[1]){
          cout << "RED ";
        }
        if(defs[i].color[2]){
          cout << "GREEN ";
        }
        if(defs[i].color[3]){
          cout << "YELLOW ";
        }
        if(defs[i].color[4]){
          cout << "BLUE ";
        }
        if(defs[i].color[5]){
          cout << "MAGENTA ";
        }
        if(defs[i].color[6]){
          cout << "CYAN ";
        }
        if(defs[i].color[7]){
          cout << "WHITE ";
        }
        cout << endl;
        cout << "Speed: " << defs[i].speed_base << "+" << defs[i].speed_dice << "d" << defs[i].speed_sides << endl;
        cout << "Abilities: ";
        if(defs[i].abil[SMART]){
          cout << "SMART ";
        }
        if(defs[i].abil[TELE]){
          cout << "TELE ";
        }
        if(defs[i].abil[TUNNEL]){
          cout << "TUNNEL ";
        }
        if(defs[i].abil[ERRATIC]){
          cout << "ERRATIC ";
        }
        if(defs[i].abil[PASS]){
          cout << "PASS ";
        }
        if(defs[i].abil[PICKUP]){
          cout << "PICKUP ";
        }
        if(defs[i].abil[UNIQ]){
          cout << "UNIQ ";
        }
        if(defs[i].abil[BOSS]){
          cout << "BOSS ";
        }
        if(defs[i].abil[DESTROY]){
          cout << "DESTROY ";
        }
        cout << endl;
        cout << "HP: " << defs[i].hp_base << "+" << defs[i].hp_dice << "d" << defs[i].hp_sides << endl;
        cout << "Damage: " << defs[i].dam_base << "+" << defs[i].dam_dice << "d" << defs[i].dam_sides << endl;
        cout << "Symbol: " << defs[i].symb << endl;
        cout << "Rarity: " << defs[i].rrty << endl;
        cout << "**Finished parsing monster " << i << "**" << endl << endl;
      }

      while(std::getline(infile, line)){
        if(line.compare("BEGIN MONSTER") == 0){
          std::getline(infile, line);
          break;
        }
      }
    }
    infile.close();
}
