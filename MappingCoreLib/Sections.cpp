#include "Sections.h"
#include <unordered_map>
#include <algorithm>
#include <set>

std::unordered_map<Sc::Ai::ScriptId, std::string> aiScriptNames = {
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter3, "Terran 3 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter5, "Terran 5 - Terran Main Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te5H, "Terran 5 - Terran Harvest Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter6, "Terran 6 - Air Attack Zerg" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te6b, "Terran 6 - Ground Attack Zerg" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te6c, "Terran 6 - Zerg Support Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter7, "Terran 7 - Bottom Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te7s, "Terran 7 - Right Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te7m, "Terran 7 - Middle Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter8, "Terran 8 - Confederate Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Tr9L, "Terran 9 - Light Attack" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Tr9H, "Terran 9 - Heavy Attack" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te10, "Terran 10 - Confederate Towns" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T11z, "Terran 11 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T11a, "Terran 11 - Lower Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T11b, "Terran 11 - Upper Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T12N, "Terran 12 - Nuke Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T12P, "Terran 12 - Phoenix Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T12T, "Terran 12 - Tank Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TED1, "Terran 1 - Electronic Distribution" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TED2, "Terran 2 - Electronic Distribution" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TED3, "Terran 3 - Electronic Distribution" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW1, "Terran 1 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW2, "Terran 2 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW3, "Terran 3 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW4, "Terran 4 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW5, "Terran 5 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer1, "Zerg 1 - Terran Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer2, "Zerg 2 - Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer3, "Zerg 3 - Terran Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer4, "Zerg 4 - Right Terran Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ze4S, "Zerg 4 - Lower Terran Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer6, "Zerg 6 - Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zr7a, "Zerg 7 - Air Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zr7g, "Zerg 7 - Ground Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zr7s, "Zerg 7 - Support Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer8, "Zerg 8 - Scout Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ze8T, "Zerg 8 - Templar Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer9, "Zerg 9 - Teal Protoss" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9ly, "Zerg 9 - Left Yellow Protoss" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9ry, "Zerg 9 - Right Yellow Protoss" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9lo, "Zerg 9 - Left Orange Protoss" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9ro, "Zerg 9 - Right Orange Protoss" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10a, "Zerg 10 - Left Teal (Attack)," ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10b, "Zerg 10 - Right Teal (Support)," ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10c, "Zerg 10 - Left Yellow (Support)," ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10d, "Zerg 10 - Right Yellow (Attack)," ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10e, "Zerg 10 - Red Protoss" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro1, "Protoss 1 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro2, "Protoss 2 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr3R, "Protoss 3 - Air Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr3G, "Protoss 3 - Ground Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro4, "Protoss 4 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr5I, "Protoss 5 - Zerg Town Island" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr5B, "Protoss 5 - Zerg Town Base" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro7, "Protoss 7 - Left Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr7B, "Protoss 7 - Right Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr7S, "Protoss 7 - Shrine Protoss" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro8, "Protoss 8 - Left Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr8B, "Protoss 8 - Right Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr8D, "Protoss 8 - Protoss Defenders" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro9, "Protoss 9 - Ground Zerg" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr9W, "Protoss 9 - Air Zerg" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr9Y, "Protoss 9 - Spell Zerg" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr10, "Protoss 10 - Mini-Towns" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::P10C, "Protoss 10 - Mini-Town Master" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::P10o, "Protoss 10 - Overmind Defenders" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1A, "Brood Wars Protoss 1 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1B, "Brood Wars Protoss 1 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1C, "Brood Wars Protoss 1 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1D, "Brood Wars Protoss 1 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1E, "Brood Wars Protoss 1 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1F, "Brood Wars Protoss 1 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2A, "Brood Wars Protoss 2 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2B, "Brood Wars Protoss 2 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2C, "Brood Wars Protoss 2 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2D, "Brood Wars Protoss 2 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2E, "Brood Wars Protoss 2 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2F, "Brood Wars Protoss 2 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3A, "Brood Wars Protoss 3 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3B, "Brood Wars Protoss 3 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3C, "Brood Wars Protoss 3 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3D, "Brood Wars Protoss 3 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3E, "Brood Wars Protoss 3 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3F, "Brood Wars Protoss 3 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4A, "Brood Wars Protoss 4 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4B, "Brood Wars Protoss 4 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4C, "Brood Wars Protoss 4 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4D, "Brood Wars Protoss 4 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4E, "Brood Wars Protoss 4 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4F, "Brood Wars Protoss 4 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5A, "Brood Wars Protoss 5 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5B, "Brood Wars Protoss 5 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5C, "Brood Wars Protoss 5 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5D, "Brood Wars Protoss 5 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5E, "Brood Wars Protoss 5 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5F, "Brood Wars Protoss 5 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6A, "Brood Wars Protoss 6 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6B, "Brood Wars Protoss 6 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6C, "Brood Wars Protoss 6 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6D, "Brood Wars Protoss 6 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6E, "Brood Wars Protoss 6 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6F, "Brood Wars Protoss 6 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7A, "Brood Wars Protoss 7 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7B, "Brood Wars Protoss 7 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7C, "Brood Wars Protoss 7 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7D, "Brood Wars Protoss 7 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7E, "Brood Wars Protoss 7 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7F, "Brood Wars Protoss 7 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8A, "Brood Wars Protoss 8 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8B, "Brood Wars Protoss 8 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8C, "Brood Wars Protoss 8 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8D, "Brood Wars Protoss 8 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8E, "Brood Wars Protoss 8 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8F, "Brood Wars Protoss 8 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1A, "Brood Wars Terran 1 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1B, "Brood Wars Terran 1 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1C, "Brood Wars Terran 1 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1D, "Brood Wars Terran 1 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1E, "Brood Wars Terran 1 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1F, "Brood Wars Terran 1 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2A, "Brood Wars Terran 2 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2B, "Brood Wars Terran 2 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2C, "Brood Wars Terran 2 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2D, "Brood Wars Terran 2 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2E, "Brood Wars Terran 2 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2F, "Brood Wars Terran 2 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3A, "Brood Wars Terran 3 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3B, "Brood Wars Terran 3 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3C, "Brood Wars Terran 3 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3D, "Brood Wars Terran 3 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3E, "Brood Wars Terran 3 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3F, "Brood Wars Terran 3 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4A, "Brood Wars Terran 4 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4B, "Brood Wars Terran 4 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4C, "Brood Wars Terran 4 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4D, "Brood Wars Terran 4 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4E, "Brood Wars Terran 4 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4F, "Brood Wars Terran 4 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5A, "Brood Wars Terran 5 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5B, "Brood Wars Terran 5 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5C, "Brood Wars Terran 5 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5D, "Brood Wars Terran 5 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5E, "Brood Wars Terran 5 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5F, "Brood Wars Terran 5 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6A, "Brood Wars Terran 6 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6B, "Brood Wars Terran 6 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6C, "Brood Wars Terran 6 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6D, "Brood Wars Terran 6 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6E, "Brood Wars Terran 6 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6F, "Brood Wars Terran 6 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7A, "Brood Wars Terran 7 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7B, "Brood Wars Terran 7 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7C, "Brood Wars Terran 7 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7D, "Brood Wars Terran 7 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7E, "Brood Wars Terran 7 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7F, "Brood Wars Terran 7 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8A, "Brood Wars Terran 8 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8B, "Brood Wars Terran 8 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8C, "Brood Wars Terran 8 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8D, "Brood Wars Terran 8 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8E, "Brood Wars Terran 8 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8F, "Brood Wars Terran 8 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1A, "Brood Wars Zerg 1 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1B, "Brood Wars Zerg 1 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1C, "Brood Wars Zerg 1 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1D, "Brood Wars Zerg 1 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1E, "Brood Wars Zerg 1 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1F, "Brood Wars Zerg 1 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2A, "Brood Wars Zerg 2 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2B, "Brood Wars Zerg 2 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2C, "Brood Wars Zerg 2 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2D, "Brood Wars Zerg 2 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2E, "Brood Wars Zerg 2 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2F, "Brood Wars Zerg 2 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3A, "Brood Wars Zerg 3 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3B, "Brood Wars Zerg 3 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3C, "Brood Wars Zerg 3 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3D, "Brood Wars Zerg 3 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3E, "Brood Wars Zerg 3 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3F, "Brood Wars Zerg 3 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4A, "Brood Wars Zerg 4 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4B, "Brood Wars Zerg 4 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4C, "Brood Wars Zerg 4 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4D, "Brood Wars Zerg 4 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4E, "Brood Wars Zerg 4 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4F, "Brood Wars Zerg 4 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5A, "Brood Wars Zerg 5 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5B, "Brood Wars Zerg 5 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5C, "Brood Wars Zerg 5 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5D, "Brood Wars Zerg 5 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5E, "Brood Wars Zerg 5 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5F, "Brood Wars Zerg 5 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6A, "Brood Wars Zerg 6 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6B, "Brood Wars Zerg 6 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6C, "Brood Wars Zerg 6 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6D, "Brood Wars Zerg 6 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6E, "Brood Wars Zerg 6 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6F, "Brood Wars Zerg 6 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7A, "Brood Wars Zerg 7 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7B, "Brood Wars Zerg 7 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7C, "Brood Wars Zerg 7 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7D, "Brood Wars Zerg 7 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7E, "Brood Wars Zerg 7 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7F, "Brood Wars Zerg 7 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8A, "Brood Wars Zerg 8 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8B, "Brood Wars Zerg 8 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8C, "Brood Wars Zerg 8 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8D, "Brood Wars Zerg 8 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8E, "Brood Wars Zerg 8 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8F, "Brood Wars Zerg 8 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9A, "Brood Wars Zerg 9 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9B, "Brood Wars Zerg 9 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9C, "Brood Wars Zerg 9 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9D, "Brood Wars Zerg 9 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9E, "Brood Wars Zerg 9 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9F, "Brood Wars Zerg 9 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0A, "Brood Wars Zerg 10 - Town A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0B, "Brood Wars Zerg 10 - Town B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0C, "Brood Wars Zerg 10 - Town C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0D, "Brood Wars Zerg 10 - Town D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0E, "Brood Wars Zerg 10 - Town E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0F, "Brood Wars Zerg 10 - Town F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMCu, "Terran Custom Level" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMCu, "Zerg Custom Level" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMCu, "Protoss Custom Level" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMCx, "Terran Expansion Custom Level" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMCx, "Zerg Expansion Custom Level" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMCx, "Protoss Expansion Custom Level" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TLOf, "Terran Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMED, "Terran Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::THIf, "Terran Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSUP, "Terran Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TARE, "Terran Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZLOf, "Zerg Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMED, "Zerg Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZHIf, "Zerg Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZSUP, "Zerg Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZARE, "Zerg Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PLOf, "Protoss Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMED, "Protoss Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PHIf, "Protoss Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PSUP, "Protoss Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PARE, "Protoss Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TLOx, "Expansion Terran Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMEx, "Expansion Terran Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::THIx, "Expansion Terran Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSUx, "Expansion Terran Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TARx, "Expansion Terran Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZLOx, "Expansion Zerg Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMEx, "Expansion Zerg Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZHIx, "Expansion Zerg Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZSUx, "Expansion Zerg Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZARx, "Expansion Zerg Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PLOx, "Expansion Protoss Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMEx, "Expansion Protoss Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PHIx, "Expansion Protoss Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PSUx, "Expansion Protoss Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PARx, "Expansion Protoss Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Suic, "Send All Units on Strategic Suicide Missions" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::SuiR, "Send All Units on Random Suicide Missions" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Rscu, "Switch Computer Player to Rescue Passive" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi0, "Turn ON Shared Vision for Player 1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi1, "Turn ON Shared Vision for Player 2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi2, "Turn ON Shared Vision for Player 3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi3, "Turn ON Shared Vision for Player 4" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi4, "Turn ON Shared Vision for Player 5" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi5, "Turn ON Shared Vision for Player 6" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi6, "Turn ON Shared Vision for Player 7" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi7, "Turn ON Shared Vision for Player 8" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi0, "Turn OFF Shared Vision for Player 1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi1, "Turn OFF Shared Vision for Player 2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi2, "Turn OFF Shared Vision for Player 3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi3, "Turn OFF Shared Vision for Player 4" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi4, "Turn OFF Shared Vision for Player 5" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi5, "Turn OFF Shared Vision for Player 6" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi6, "Turn OFF Shared Vision for Player 7" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi7, "Turn OFF Shared Vision for Player 8" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MvTe, "Move Dark Templars to Region" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ClrC, "Clear Previous Combat Data" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Enmy, "Set Player to Enemy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ally, "Set Player to Ally" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::VluA, "Value This Area Higher" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::EnBk, "Enter Closest Bunker" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::StTg, "Set Generic Command Target" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::StPt, "Make These Units Patrol" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::EnTr, "Enter Transport" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ExTr, "Exit Transport" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::NuHe, "AI Nuke Here" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::HaHe, "AI Harass Here" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::JYDg, "Set Unit Order To: Junk Yard Dog" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::DWHe, "AI Nuke Here" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ReHe, "AI Nuke Here" )
};

std::unordered_map<Sc::Ai::ScriptId, std::string> aiScriptIdStr = {
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter3, "Ter3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter5, "Ter5" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te5H, "Te5H" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter6, "Ter6" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te6b, "Te6b" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te6c, "Te6c" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter7, "Ter7" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te7s, "Te7s" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te7m, "Te7m" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ter8, "Ter8" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Tr9L, "Tr9L" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Tr9H, "Tr9H" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Te10, "Te10" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T11z, "T11z" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T11a, "T11a" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T11b, "T11b" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T12N, "T12N" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T12P, "T12P" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::T12T, "T12T" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TED1, "TED1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TED2, "TED2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TED3, "TED3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW1, "TSW1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW2, "TSW2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW3, "TSW3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW4, "TSW4" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSW5, "TSW5" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer1, "Zer1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer2, "Zer2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer3, "Zer3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer4, "Zer4" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ze4S, "Ze4S" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer6, "Zer6" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zr7a, "Zr7a" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zr7g, "Zr7g" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zr7s, "Zr7s" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer8, "Zer8" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ze8T, "Ze8T" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Zer9, "Zer9" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9ly, "Z9ly" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9ry, "Z9ry" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9lo, "Z9lo" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z9ro, "Z9ro" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10a, "Z10a" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10b, "Z10b" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10c, "Z10c" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10d, "Z10d" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Z10e, "Z10e" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro1, "Pro1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro2, "Pro2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr3R, "Pr3R" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr3G, "Pr3G" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro4, "Pro4" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr5I, "Pr5I" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr5B, "Pr5B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro7, "Pro7" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr7B, "Pr7B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr7S, "Pr7S" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro8, "Pro8" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr8B, "Pr8B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr8D, "Pr8D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pro9, "Pro9" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr9W, "Pr9W" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr9Y, "Pr9Y" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Pr10, "Pr10" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::P10C, "P10C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::P10o, "P10o" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1A, "PB1A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1B, "PB1B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1C, "PB1C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1D, "PB1D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1E, "PB1E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB1F, "PB1F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2A, "PB2A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2B, "PB2B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2C, "PB2C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2D, "PB2D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2E, "PB2E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB2F, "PB2F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3A, "PB3A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3B, "PB3B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3C, "PB3C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3D, "PB3D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3E, "PB3E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB3F, "PB3F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4A, "PB4A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4B, "PB4B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4C, "PB4C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4D, "PB4D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4E, "PB4E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB4F, "PB4F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5A, "PB5A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5B, "PB5B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5C, "PB5C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5D, "PB5D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5E, "PB5E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB5F, "PB5F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6A, "PB6A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6B, "PB6B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6C, "PB6C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6D, "PB6D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6E, "PB6E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB6F, "PB6F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7A, "PB7A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7B, "PB7B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7C, "PB7C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7D, "PB7D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7E, "PB7E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB7F, "PB7F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8A, "PB8A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8B, "PB8B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8C, "PB8C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8D, "PB8D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8E, "PB8E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PB8F, "PB8F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1A, "TB1A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1B, "TB1B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1C, "TB1C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1D, "TB1D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1E, "TB1E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB1F, "TB1F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2A, "TB2A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2B, "TB2B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2C, "TB2C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2D, "TB2D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2E, "TB2E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB2F, "TB2F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3A, "TB3A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3B, "TB3B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3C, "TB3C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3D, "TB3D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3E, "TB3E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB3F, "TB3F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4A, "TB4A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4B, "TB4B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4C, "TB4C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4D, "TB4D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4E, "TB4E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB4F, "TB4F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5A, "TB5A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5B, "TB5B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5C, "TB5C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5D, "TB5D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5E, "TB5E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB5F, "TB5F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6A, "TB6A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6B, "TB6B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6C, "TB6C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6D, "TB6D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6E, "TB6E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB6F, "TB6F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7A, "TB7A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7B, "TB7B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7C, "TB7C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7D, "TB7D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7E, "TB7E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB7F, "TB7F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8A, "TB8A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8B, "TB8B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8C, "TB8C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8D, "TB8D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8E, "TB8E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TB8F, "TB8F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1A, "ZB1A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1B, "ZB1B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1C, "ZB1C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1D, "ZB1D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1E, "ZB1E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB1F, "ZB1F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2A, "ZB2A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2B, "ZB2B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2C, "ZB2C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2D, "ZB2D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2E, "ZB2E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB2F, "ZB2F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3A, "ZB3A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3B, "ZB3B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3C, "ZB3C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3D, "ZB3D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3E, "ZB3E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB3F, "ZB3F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4A, "ZB4A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4B, "ZB4B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4C, "ZB4C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4D, "ZB4D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4E, "ZB4E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB4F, "ZB4F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5A, "ZB5A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5B, "ZB5B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5C, "ZB5C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5D, "ZB5D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5E, "ZB5E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB5F, "ZB5F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6A, "ZB6A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6B, "ZB6B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6C, "ZB6C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6D, "ZB6D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6E, "ZB6E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB6F, "ZB6F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7A, "ZB7A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7B, "ZB7B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7C, "ZB7C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7D, "ZB7D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7E, "ZB7E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB7F, "ZB7F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8A, "ZB8A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8B, "ZB8B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8C, "ZB8C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8D, "ZB8D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8E, "ZB8E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB8F, "ZB8F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9A, "ZB9A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9B, "ZB9B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9C, "ZB9C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9D, "ZB9D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9E, "ZB9E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB9F, "ZB9F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0A, "ZB0A" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0B, "ZB0B" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0C, "ZB0C" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0D, "ZB0D" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0E, "ZB0E" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZB0F, "ZB0F" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMCu, "TMCu" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMCu, "ZMCu" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMCu, "PMCu" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMCx, "TMCx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMCx, "ZMCx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMCx, "PMCx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TLOf, "TLOf" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMED, "TMED" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::THIf, "THIf" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSUP, "TSUP" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TARE, "TARE" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZLOf, "ZLOf" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMED, "ZMED" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZHIf, "ZHIf" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZSUP, "ZSUP" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZARE, "ZARE" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PLOf, "PLOf" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMED, "PMED" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PHIf, "PHIf" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PSUP, "PSUP" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PARE, "PARE" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TLOx, "TLOx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TMEx, "TMEx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::THIx, "THIx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TSUx, "TSUx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::TARx, "TARx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZLOx, "ZLOx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZMEx, "ZMEx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZHIx, "ZHIx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZSUx, "ZSUx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ZARx, "ZARx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PLOx, "PLOx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PMEx, "PMEx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PHIx, "PHIx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PSUx, "PSUx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PARx, "PARx" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Suic, "Suic" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::SuiR, "SuiR" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Rscu, "Rscu" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi0, "+Vi0" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi1, "+Vi1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi2, "+Vi2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi3, "+Vi3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi4, "+Vi4" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi5, "+Vi5" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi6, "+Vi6" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::PlusVi7, "+Vi7" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi0, "-Vi0" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi1, "-Vi1" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi2, "-Vi2" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi3, "-Vi3" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi4, "-Vi4" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi5, "-Vi5" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi6, "-Vi6" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MinusVi7, "-Vi7" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::MvTe, "MvTe" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ClrC, "ClrC" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Enmy, "Enmy" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::Ally, "Ally" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::VluA, "VluA" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::EnBk, "EnBk" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::StTg, "StTg" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::StPt, "StPt" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::EnTr, "EnTr" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ExTr, "ExTr" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::NuHe, "NuHe" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::HaHe, "HaHe" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::JYDg, "JYDg" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::DWHe, "DWHe" ),
    std::pair<Sc::Ai::ScriptId, std::string>( Sc::Ai::ScriptId::ReHe, "ReHe" )
};

const std::vector<std::string> upgradeNames = {
    "Terran Infantry Armor",
    "Terran Vehicle Plating",
    "Terran Ship Plating",
    "Zerg Carapace",
    "Zerg Flyer Carapace",
    "Protoss Armor",
    "Protoss Plating",
    "Terran Infantry Weapons",
    "Terran Vehicle Weapons",
    "Terran Ship Weapons",
    "Zerg Melee Attacks",
    "Zerg Missile Attacks",
    "Zerg Flyer Attacks",
    "Protoss Ground Weapons",
    "Protoss Air Weapons",
    "Protoss Plasma Shields",
    "U-238 Shells",
    "Ion Thrusters",
    "Burst Lasers (Unused)",
    "Titan Reactor (SV +50)",
    "Ocular Implants",
    "Moebius Reactor (Ghost +50)",
    "Apollo Reactor (Wraith +50)",
    "Colossus Reactor (BC +50)",
    "Ventral Sacs",
    "Antennae",
    "Pneumatized Carapace",
    "Metabolic Boost",
    "Adrenal Glands",
    "Muscular Augments",
    "Grooved Spines",
    "Gamete Meiosis (Queen +50)",
    "Metasynaptic Node (Defiler +50)",
    "Singularity Charge",
    "Leg Enhancements",
    "Scarab Damage",
    "Reaver Capacity",
    "Gravitic Drive",
    "Sensor Array",
    "Gravitic Boosters",
    "Khaydarin Amulet (HT +50)",
    "Apial Sensors",
    "Gravitic Thrusters",
    "Carrier Capacity",
    "Khaydarin Core (Arbiter +50)",
    "Unused Upgrade (45)",
    "Unused Upgrade (46)",
    "Argus Jewel (Corsair +50)",
    "Unused Upgrade (48)",
    "Argus Talisman (DA +50)",
    "Unused Upgrade (50)",
    "Caduceus Reactor (Medic +50)",
    "Chitinous Plating",
    "Anabolic Synthesis",
    "Charon Booster",
    "Unused Upgrade (55)",
    "Unused Upgrade (56)",
    "Unused Upgrade (57)",
    "Unused Upgrade (58)",
    "Unused Upgrade (59)",
    "Special Upgrade (60)"
};

const std::vector<std::string> techNames = {
    "Stim Packs",
    "Lockdown",
    "EMP Shockwave",
    "Spider Mines",
    "Scanner Sweep",
    "Tank Siege Mode",
    "Defensive Matrix",
    "Irradiate",
    "Yamato Gun",
    "Cloaking Field",
    "Personnel Cloaking",
    "Burrowing",
    "Infestation",
    "Spawn Broodlings",
    "Dark Swarm",
    "Plague",
    "Consume",
    "Ensnare",
    "Parasite",
    "Psionic Storm",
    "Hallucination",
    "Recall",
    "Stasis Field",
    "Archon Warp",
    "Restoration",
    "Disruption Web",
    "Unused Tech (26)",
    "Mind Control",
    "Dark Archon Meld",
    "Feedback",
    "Optical Flare",
    "Maelstrom",
    "Lurker Aspect",
    "Unused Tech (33)",
    "Healing",
    "Unused Tech (35)",
    "Unused Tech (36)",
    "Unused Tech (37)",
    "Unused Tech (38)",
    "Unused Tech (39)",
    "Unused Tech (40)",
    "Unused Tech (41)",
    "Unused Tech (42)",
    "Unused Tech (43)",
};

Chk::Condition::Argument noCndArg = { Chk::Condition::ArgType::NoType, Chk::Condition::ArgField::NoField };
Chk::Condition::Argument amountCndArg = { Chk::Condition::ArgType::Amount, Chk::Condition::ArgField::Amount };
Chk::Condition::Argument numericComparisonCndArg = { Chk::Condition::ArgType::NumericComparison, Chk::Condition::ArgField::Comparison };
Chk::Condition::Argument switchStateCndArg = { Chk::Condition::ArgType::SwitchState, Chk::Condition::ArgField::Comparison };
Chk::Condition::Argument resourceCndArg = { Chk::Condition::ArgType::ResourceType, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument scoreCndArg = { Chk::Condition::ArgType::ScoreType, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument switchCndArg = { Chk::Condition::ArgType::Switch, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument playerCndArg = { Chk::Condition::ArgType::Player, Chk::Condition::ArgField::Player };
Chk::Condition::Argument unitCndArg = { Chk::Condition::ArgType::Unit, Chk::Condition::ArgField::UnitType };
Chk::Condition::Argument locationCndArg = { Chk::Condition::ArgType::Location, Chk::Condition::ArgField::LocationId };
Chk::Condition::Argument comparisonCndArg = { Chk::Condition::ArgType::Comparison, Chk::Condition::ArgField::Comparison };
Chk::Condition::Argument conditionTypeCndArg = { Chk::Condition::ArgType::ConditionType, Chk::Condition::ArgField::ConditionType };
Chk::Condition::Argument typeIndexCndArg = { Chk::Condition::ArgType::TypeIndex, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument flagsCndArg = { Chk::Condition::ArgType::Flags, Chk::Condition::ArgField::Flags };
Chk::Condition::Argument maskFlagCndArg = { Chk::Condition::ArgType::MaskFlag, Chk::Condition::ArgField::MaskFlag };
Chk::Condition::Argument memoryOffsetCndArg = { Chk::Condition::ArgType::MemoryOffset, Chk::Condition::ArgField::Player };

Chk::Condition::Argument Chk::Condition::classicArguments[NumConditionTypes][MaxArguments] = {
    /**  0 = No Condition         */ {},
    /**  1 = Countdown Timer ---- */ { numericComparisonCndArg, amountCndArg },
    /**  2 = Command              */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /**  3 = Bring -------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg, locationCndArg},
    /**  4 = Accumulate           */ { playerCndArg, numericComparisonCndArg, amountCndArg, resourceCndArg },
    /**  5 = Kill --------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /**  6 = Command The Most     */ { unitCndArg },
    /**  7 = Command The Most At  */ { unitCndArg, locationCndArg },
    /**  8 = Most Kills           */ { unitCndArg },
    /**  9 = Highest Score ------ */ { scoreCndArg },
    /** 10 = Most Resources       */ { resourceCndArg },
    /** 11 = Switch ------------- */ { switchCndArg, switchStateCndArg },
    /** 12 = Elapsed Time         */ { numericComparisonCndArg, amountCndArg },
    /** 13 = Is Briefing -------- */ {},
    /** 14 = Opponents            */ { playerCndArg, numericComparisonCndArg, amountCndArg },
    /** 15 = Deaths ------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /** 16 = Command The Least    */ { unitCndArg },
    /** 17 = Command The Least At */ { unitCndArg, locationCndArg },
    /** 18 = Least Kills          */ { unitCndArg },
    /** 19 = Lowest Score ------- */ { scoreCndArg },
    /** 20 = Least Resources      */ { resourceCndArg },
    /** 21 = Score -------------- */ { playerCndArg, scoreCndArg, numericComparisonCndArg, amountCndArg },
    /** 22 = Always               */ {},
    /** 23 = Never -------------- */ {}
};

Chk::Condition::Argument Chk::Condition::textArguments[NumConditionTypes][MaxArguments] = {
    /**  0 = No Condition         */ {},
    /**  1 = Countdown Timer ---- */ { numericComparisonCndArg, amountCndArg },
    /**  2 = Command              */ { playerCndArg, unitCndArg, numericComparisonCndArg, amountCndArg },
    /**  3 = Bring -------------- */ { playerCndArg, unitCndArg, locationCndArg, numericComparisonCndArg, amountCndArg },
    /**  4 = Accumulate           */ { playerCndArg, numericComparisonCndArg, amountCndArg, resourceCndArg },
    /**  5 = Kill --------------- */ { playerCndArg, unitCndArg, numericComparisonCndArg, amountCndArg },
    /**  6 = Command The Most     */ { unitCndArg },
    /**  7 = Command The Most At  */ { unitCndArg, locationCndArg },
    /**  8 = Most Kills           */ { unitCndArg },
    /**  9 = Highest Score ------ */ { scoreCndArg },
    /** 10 = Most Resources       */ { resourceCndArg },
    /** 11 = Switch ------------- */ { switchCndArg, switchStateCndArg },
    /** 12 = Elapsed Time         */ { numericComparisonCndArg, amountCndArg },
    /** 13 = Is Briefing -------- */ {},
    /** 14 = Opponents            */ { playerCndArg, numericComparisonCndArg, amountCndArg },
    /** 15 = Deaths ------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /** 16 = Command The Least    */ { unitCndArg },
    /** 17 = Command The Least At */ { unitCndArg, locationCndArg },
    /** 18 = Least Kills          */ { unitCndArg },
    /** 19 = Lowest Score ------- */ { scoreCndArg },
    /** 20 = Least Resources      */ { resourceCndArg },
    /** 21 = Score -------------- */ { playerCndArg, scoreCndArg, numericComparisonCndArg, amountCndArg },
    /** 22 = Always               */ {},
    /** 23 = Never -------------- */ {}
};

u8 Chk::Condition::defaultFlags[NumConditionTypes] = {
    /**  0 = No Condition         */ 0,
    /**  1 = Countdown Timer ---- */ 0,
    /**  2 = Command              */ (u8)Flags::UnitTypeUsed,
    /**  3 = Bring -------------- */ (u8)Flags::UnitTypeUsed,
    /**  4 = Accumulate           */ 0,
    /**  5 = Kill --------------- */ (u8)Flags::UnitTypeUsed,
    /**  6 = Command The Most     */ (u8)Flags::UnitTypeUsed,
    /**  7 = Command The Most At  */ (u8)Flags::UnitTypeUsed,
    /**  8 = Most Kills           */ (u8)Flags::UnitTypeUsed,
    /**  9 = Highest Score ------ */ 0,
    /** 10 = Most Resources       */ 0,
    /** 11 = Switch ------------- */ 0,
    /** 12 = Elapsed Time         */ 0,
    /** 13 = Is Briefing -------- */ 0,
    /** 14 = Opponents            */ 0,
    /** 15 = Deaths ------------- */ (u8)Flags::UnitTypeUsed,
    /** 16 = Command The Least    */ (u8)Flags::UnitTypeUsed,
    /** 17 = Command The Least At */ (u8)Flags::UnitTypeUsed,
    /** 18 = Least Kills          */ (u8)Flags::UnitTypeUsed,
    /** 19 = Lowest Score ------- */ 0,
    /** 20 = Least Resources      */ 0,
    /** 21 = Score -------------- */ 0,
    /** 22 = Always               */ 0,
    /** 23 = Never -------------- */ 0
};

std::unordered_map<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition> Chk::Condition::virtualConditions = {
    std::pair<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition>(
        Chk::Condition::VirtualType::Custom, { Chk::Condition::VirtualType::Indeterminate, {
            locationCndArg, playerCndArg, amountCndArg, unitCndArg, comparisonCndArg, conditionTypeCndArg, typeIndexCndArg, flagsCndArg, maskFlagCndArg } } ),
    std::pair<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition>(
        Chk::Condition::VirtualType::Memory, { Chk::Condition::VirtualType::Deaths, { memoryOffsetCndArg, numericComparisonCndArg, amountCndArg } } )
};

Chk::Action::Argument noArg = { Chk::Action::ArgType::NoType, Chk::Action::ArgField::NoField };
Chk::Action::Argument amountArg = { Chk::Action::ArgType::Amount, Chk::Action::ArgField::Number };
Chk::Action::Argument percentArg = { Chk::Action::ArgType::Percent, Chk::Action::ArgField::Number };
Chk::Action::Argument durationArg = { Chk::Action::ArgType::Duration, Chk::Action::ArgField::Time };
Chk::Action::Argument numUnitsArg = { Chk::Action::ArgType::NumUnits, Chk::Action::ArgField::Type2 };
Chk::Action::Argument numericModArg = { Chk::Action::ArgType::NumericMod, Chk::Action::ArgField::Type2 };
Chk::Action::Argument switchModArg = { Chk::Action::ArgType::SwitchMod, Chk::Action::ArgField::Type2 };
Chk::Action::Argument stateModArg = { Chk::Action::ArgType::StateMod, Chk::Action::ArgField::Type2 };
Chk::Action::Argument allyStateArg = { Chk::Action::ArgType::AllyState, Chk::Action::ArgField::Type };
Chk::Action::Argument scoreArg = { Chk::Action::ArgType::ScoreType, Chk::Action::ArgField::Type };
Chk::Action::Argument resourceArg = { Chk::Action::ArgType::ResourceType, Chk::Action::ArgField::Type };
Chk::Action::Argument scriptArg = { Chk::Action::ArgType::Script, Chk::Action::ArgField::Number };
Chk::Action::Argument textFlagsArg = { Chk::Action::ArgType::TextFlags, Chk::Action::ArgField::Flags };
Chk::Action::Argument orderArg = { Chk::Action::ArgType::Order, Chk::Action::ArgField::Type2 };
Chk::Action::Argument unitArg = { Chk::Action::ArgType::Unit, Chk::Action::ArgField::Type };
Chk::Action::Argument locationArg = { Chk::Action::ArgType::Location, Chk::Action::ArgField::LocationId }; // Only location/dest in Move Location/source in Order and Move Unit
Chk::Action::Argument playerArg = { Chk::Action::ArgType::Player, Chk::Action::ArgField::Group };
Chk::Action::Argument destPlayerArg = { Chk::Action::ArgType::Player, Chk::Action::ArgField::Number };
Chk::Action::Argument secondaryLocationArg = { Chk::Action::ArgType::Location, Chk::Action::ArgField::Number }; // Source in Move Location/dest in Order and Move Unit
Chk::Action::Argument cuwpArg = { Chk::Action::ArgType::CUWP, Chk::Action::ArgField::Number };
Chk::Action::Argument switchArg = { Chk::Action::ArgType::Switch, Chk::Action::ArgField::Number };
Chk::Action::Argument stringArg = { Chk::Action::ArgType::String, Chk::Action::ArgField::StringId };
Chk::Action::Argument soundArg = { Chk::Action::ArgType::Sound, Chk::Action::ArgField::SoundStringId };
Chk::Action::Argument memoryOffsetArg = { Chk::Action::ArgType::MemoryOffset, Chk::Action::ArgField::Group };
Chk::Action::Argument numberArg = { Chk::Action::ArgType::Number, Chk::Action::ArgField::Number };
Chk::Action::Argument typeIndexArg = { Chk::Action::ArgType::TypeIndex, Chk::Action::ArgField::Type };
Chk::Action::Argument actionTypeArg = { Chk::Action::ArgType::ActionType, Chk::Action::ArgField::ActionType };
Chk::Action::Argument secondaryTypeIndexArg = { Chk::Action::ArgType::SecondaryTypeIndex, Chk::Action::ArgField::Type2 };
Chk::Action::Argument flagsArg = { Chk::Action::ArgType::Flags, Chk::Action::ArgField::Flags };
Chk::Action::Argument paddingArg = { Chk::Action::ArgType::Padding, Chk::Action::ArgField::Padding };
Chk::Action::Argument maskFlagArg = { Chk::Action::ArgType::MaskFlag, Chk::Action::ArgField::MaskFlag };

Chk::Action::Argument briefingSlotArg = { Chk::Action::ArgType::Amount, Chk::Action::ArgField::Group };

Chk::Action::Argument Chk::Action::classicArguments[NumActionTypes][MaxArguments] = {
    /**  0 = No Action                              */ {},
    /**  1 = Victory ------------------------------ */ {},
    /**  2 = Defeat                                 */ {},
    /**  3 = Preserve Trigger --------------------- */ {},
    /**  4 = Wait                                   */ { durationArg },
    /**  5 = Pause Game --------------------------- */ {},
    /**  6 = Unpause Game                           */ {},
    /**  7 = Transmission ------------------------- */ { unitArg, locationArg, soundArg, numericModArg, amountArg, stringArg },
    /**  8 = Play Sound                             */ { soundArg },
    /**  9 = Display Text Message ----------------- */ { stringArg },
    /** 10 = Center View                            */ { locationArg },
    /** 11 = Create Unit with Properties ---------- */ { numUnitsArg, unitArg, locationArg, playerArg, cuwpArg },
    /** 12 = Set Mission Objectives                 */ { stringArg },
    /** 13 = Set Switch --------------------------- */ { switchModArg, switchArg },
    /** 14 = Set Countdown Timer                    */ { numericModArg, durationArg },
    /** 15 = Run AI Script ------------------------ */ { scriptArg },
    /** 16 = Run AI Script At Location              */ { scriptArg, locationArg },
    /** 17 = Leader Board (Control) --------------- */ { unitArg, stringArg },
    /** 18 = Leader Board (Control At Location) --- */ { unitArg, locationArg, stringArg },
    /** 19 = Leader Board (Resources)               */ { resourceArg, stringArg },
    /** 20 = Leader Board (Kills) ----------------- */ { unitArg, stringArg },
    /** 21 = Leader Board (Points)                  */ { scoreArg, stringArg },
    /** 22 = Kill Unit ---------------------------- */ { unitArg, playerArg },
    /** 23 = Kill Unit At Location                  */ { numUnitsArg, unitArg, playerArg, locationArg },
    /** 24 = Remove Unit -------------------------- */ { unitArg, playerArg },
    /** 25 = Remove Unit At Location                */ { numUnitsArg, unitArg, playerArg, locationArg },
    /** 26 = Set Resources ------------------------ */ { playerArg, numericModArg, amountArg, resourceArg },
    /** 27 = Set Score                              */ { playerArg, numericModArg, amountArg, scoreArg },
    /** 28 = Minimap Ping ------------------------- */ { locationArg },
    /** 29 = Talking Portrait                       */ { unitArg, durationArg },
    /** 30 = Mute Unit Speech --------------------- */ {},
    /** 31 = Unmute Unit Speech                     */ {},
    /** 32 = Leaderboard Computer Players --------- */ { stateModArg },
    /** 33 = Leaderboard Goal (Control)             */ { amountArg, unitArg, stringArg },
    /** 34 = Leaderboard Goal (Control At Location) */ { amountArg, unitArg, locationArg, stringArg },
    /** 35 = Leaderboard Goal (Resources)           */ { amountArg, resourceArg, stringArg },
    /** 36 = Leaderboard Goal (Kills) ------------- */ { amountArg, unitArg, stringArg },
    /** 37 = Leaderboard Goal (Points)              */ { amountArg, scoreArg, stringArg },
    /** 38 = Move Location ------------------------ */ { secondaryLocationArg, unitArg, playerArg, locationArg }, // srcLocation, unit, player, destLocation
    /** 39 = Move Unit                              */ { numUnitsArg, unitArg, playerArg, locationArg, secondaryLocationArg }, // numUnits, unit, player, srcLocation, destLocation
    /** 40 = Leaderboard (Greed) ------------------ */ { amountArg },
    /** 41 = Set Next Scenario                      */ { stringArg },
    /** 42 = Set Doodad State --------------------- */ { stateModArg, unitArg, playerArg, locationArg },
    /** 43 = Set Invincibility                      */ { stateModArg, unitArg, playerArg, locationArg },
    /** 44 = Create Unit -------------------------- */ { numUnitsArg, unitArg, locationArg, playerArg },
    /** 45 = Set Deaths                             */ { playerArg, numericModArg, amountArg, unitArg },
    /** 46 = Order -------------------------------- */ { unitArg, playerArg, locationArg, orderArg, secondaryLocationArg }, // unit, player, srcLocation, order, destLocation
    /** 47 = Comment                                */ { stringArg },
    /** 48 = Give Units to Player ----------------- */ { numUnitsArg, unitArg, playerArg, locationArg, destPlayerArg },
    /** 49 = Modify Unit Hit Points                 */ { numUnitsArg, unitArg, playerArg, locationArg, percentArg },
    /** 50 = Modify Unit Energy ------------------- */ { numUnitsArg, unitArg, playerArg, locationArg, percentArg },
    /** 51 = Modify Unit Shield points              */ { numUnitsArg, unitArg, playerArg, locationArg, percentArg },
    /** 52 = Modify Unit Resource Amount ---------- */ { numUnitsArg, playerArg, locationArg, amountArg },
    /** 53 = Modify Unit Hanger Count               */ { amountArg, numUnitsArg, unitArg, locationArg, playerArg },
    /** 54 = Pause Timer -------------------------- */ {},
    /** 55 = Unpause Timer                          */ {},
    /** 56 = Draw --------------------------------- */ {},
    /** 57 = Set Alliance Status                    */ { playerArg, allyStateArg },
    /** 58 = Disable Debug Mode ------------------- */ {},
    /** 59 = Enable Debug Mode                      */ {}
};

Chk::Action::Argument Chk::Action::textArguments[NumActionTypes][MaxArguments] = {
    /**  0 = No Action                              */ {},
    /**  1 = Victory ------------------------------ */ {},
    /**  2 = Defeat                                 */ {},
    /**  3 = Preserve Trigger --------------------- */ {},
    /**  4 = Wait                                   */ { durationArg },
    /**  5 = Pause Game --------------------------- */ {},
    /**  6 = Unpause Game                           */ {},
    /**  7 = Transmission ------------------------- */ { textFlagsArg, stringArg, unitArg, locationArg, numericModArg, amountArg, soundArg, durationArg },
    /**  8 = Play Sound                             */ { soundArg, durationArg },
    /**  9 = Display Text Message ----------------- */ { textFlagsArg, stringArg },
    /** 10 = Center View                            */ { locationArg },
    /** 11 = Create Unit with Properties ---------- */ { playerArg, unitArg, numUnitsArg, locationArg, cuwpArg },
    /** 12 = Set Mission Objectives                 */ { stringArg },
    /** 13 = Set Switch --------------------------- */ { switchArg, switchModArg },
    /** 14 = Set Countdown Timer                    */ { numericModArg, durationArg },
    /** 15 = Run AI Script ------------------------ */ { scriptArg },
    /** 16 = Run AI Script At Location              */ { scriptArg, locationArg },
    /** 17 = Leader Board (Control) --------------- */ { stringArg, unitArg },
    /** 18 = Leader Board (Control At Location) --- */ { stringArg, unitArg, locationArg },
    /** 19 = Leader Board (Resources)               */ { stringArg, resourceArg },
    /** 20 = Leader Board (Kills) ----------------- */ { stringArg, unitArg },
    /** 21 = Leader Board (Points)                  */ { stringArg, scoreArg },
    /** 22 = Kill Unit ---------------------------- */ { playerArg, unitArg },
    /** 23 = Kill Unit At Location                  */ { playerArg, unitArg, numUnitsArg, locationArg },
    /** 24 = Remove Unit -------------------------- */ { playerArg, unitArg },
    /** 25 = Remove Unit At Location                */ { playerArg, unitArg, numUnitsArg, locationArg },
    /** 26 = Set Resources ------------------------ */ { playerArg, numericModArg, amountArg, resourceArg },
    /** 27 = Set Score                              */ { playerArg, numericModArg, amountArg, scoreArg },
    /** 28 = Minimap Ping ------------------------- */ { locationArg },
    /** 29 = Talking Portrait                       */ { unitArg, durationArg },
    /** 30 = Mute Unit Speech --------------------- */ {},
    /** 31 = Unmute Unit Speech                     */ {},
    /** 32 = Leaderboard Computer Players --------- */ { stateModArg },
    /** 33 = Leaderboard Goal (Control)             */ { stringArg, unitArg, amountArg },
    /** 34 = Leaderboard Goal (Control At Location) */ { stringArg, unitArg, amountArg, locationArg },
    /** 35 = Leaderboard Goal (Resources)           */ { stringArg, amountArg, resourceArg },
    /** 36 = Leaderboard Goal (Kills) ------------- */ { stringArg, unitArg, amountArg },
    /** 37 = Leaderboard Goal (Points)              */ { stringArg, scoreArg, amountArg },
    /** 38 = Move Location ------------------------ */ { playerArg, unitArg, locationArg, secondaryLocationArg }, // srcLocation, unit, player, destLocation
    /** 39 = Move Unit                              */ { playerArg, unitArg, numUnitsArg, locationArg, secondaryLocationArg }, // numUnits, unit, player, srcLocation, destLocation
    /** 40 = Leaderboard (Greed) ------------------ */ { amountArg },
    /** 41 = Set Next Scenario                      */ { stringArg },
    /** 42 = Set Doodad State --------------------- */ { playerArg, unitArg, locationArg, stateModArg },
    /** 43 = Set Invincibility                      */ { playerArg, unitArg, locationArg, stateModArg },
    /** 44 = Create Unit -------------------------- */ { playerArg, unitArg, numUnitsArg, locationArg },
    /** 45 = Set Deaths                             */ { playerArg, unitArg, numericModArg, amountArg },
    /** 46 = Order -------------------------------- */ { playerArg, unitArg, locationArg, secondaryLocationArg, orderArg }, // unit, player, srcLocation, order, destLocation
    /** 47 = Comment                                */ { stringArg },
    /** 48 = Give Units to Player ----------------- */ { playerArg, destPlayerArg, unitArg, numUnitsArg, locationArg },
    /** 49 = Modify Unit Hit Points                 */ { playerArg, unitArg, percentArg, numUnitsArg, locationArg },
    /** 50 = Modify Unit Energy ------------------- */ { playerArg, unitArg, percentArg, numUnitsArg, locationArg },
    /** 51 = Modify Unit Shield points              */ { playerArg, unitArg, percentArg, numUnitsArg, locationArg },
    /** 52 = Modify Unit Resource Amount ---------- */ { playerArg, amountArg, numUnitsArg, locationArg },
    /** 53 = Modify Unit Hanger Count               */ { playerArg, unitArg, amountArg, numUnitsArg, locationArg },
    /** 54 = Pause Timer -------------------------- */ {},
    /** 55 = Unpause Timer                          */ {},
    /** 56 = Draw --------------------------------- */ {},
    /** 57 = Set Alliance Status                    */ { playerArg, allyStateArg },
    /** 58 = Disable Debug Mode ------------------- */ {},
    /** 59 = Enable Debug Mode                      */ {}
};

u8 Chk::Action::defaultFlags[NumActionTypes] = {
    /**  0 = No Action                              */ 0,
    /**  1 = Victory ------------------------------ */ (u8)Flags::AlwaysDisplay,
    /**  2 = Defeat                                 */ (u8)Flags::AlwaysDisplay,
    /**  3 = Preserve Trigger --------------------- */ (u8)Flags::AlwaysDisplay,
    /**  4 = Wait                                   */ (u8)Flags::AlwaysDisplay,
    /**  5 = Pause Game --------------------------- */ (u8)Flags::AlwaysDisplay,
    /**  6 = Unpause Game                           */ (u8)Flags::AlwaysDisplay,
    /**  7 = Transmission ------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /**  8 = Play Sound                             */ (u8)Flags::AlwaysDisplay,
    /**  9 = Display Text Message ----------------- */ (u8)Flags::AlwaysDisplay,
    /** 10 = Center View                            */ (u8)Flags::AlwaysDisplay,
    /** 11 = Create Unit with Properties ---------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::UnitPropertiesUsed | (u8)Flags::AlwaysDisplay,
    /** 12 = Set Mission Objectives                 */ 0,
    /** 13 = Set Switch --------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 14 = Set Countdown Timer                    */ (u8)Flags::AlwaysDisplay,
    /** 15 = Run AI Script ------------------------ */ (u8)Flags::AlwaysDisplay,
    /** 16 = Run AI Script At Location              */ (u8)Flags::AlwaysDisplay,
    /** 17 = Leader Board (Control) --------------- */ (u8)Flags::UnitTypeUsed,
    /** 18 = Leader Board (Control At Location) --- */ (u8)Flags::UnitTypeUsed,
    /** 19 = Leader Board (Resources)               */ 0,
    /** 20 = Leader Board (Kills) ----------------- */ (u8)Flags::UnitTypeUsed,
    /** 21 = Leader Board (Points)                  */ 0,
    /** 22 = Kill Unit ---------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 23 = Kill Unit At Location                  */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 24 = Remove Unit -------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 25 = Remove Unit At Location                */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 26 = Set Resources ------------------------ */ (u8)Flags::AlwaysDisplay,
    /** 27 = Set Score                              */ (u8)Flags::AlwaysDisplay,
    /** 28 = Minimap Ping ------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 29 = Talking Portrait                       */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 30 = Mute Unit Speech --------------------- */ (u8)Flags::AlwaysDisplay,
    /** 31 = Unmute Unit Speech                     */ (u8)Flags::AlwaysDisplay,
    /** 32 = Leaderboard Computer Players --------- */ (u8)Flags::AlwaysDisplay,
    /** 33 = Leaderboard Goal (Control)             */ (u8)Flags::UnitTypeUsed,
    /** 34 = Leaderboard Goal (Control At Location) */ (u8)Flags::UnitTypeUsed,
    /** 35 = Leaderboard Goal (Resources)           */ 0,
    /** 36 = Leaderboard Goal (Kills) ------------- */ (u8)Flags::UnitTypeUsed,
    /** 37 = Leaderboard Goal (Points)              */ 0,
    /** 38 = Move Location ------------------------ */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 39 = Move Unit                              */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 40 = Leaderboard (Greed) ------------------ */ (u8)Flags::AlwaysDisplay,
    /** 41 = Set Next Scenario                      */ (u8)Flags::AlwaysDisplay,
    /** 42 = Set Doodad State --------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 43 = Set Invincibility                      */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 44 = Create Unit -------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 45 = Set Deaths                             */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 46 = Order -------------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 47 = Comment                                */ 0,
    /** 48 = Give Units to Player ----------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 49 = Modify Unit Hit Points                 */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 50 = Modify Unit Energy ------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 51 = Modify Unit Shield points              */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 52 = Modify Unit Resource Amount ---------- */ (u8)Flags::AlwaysDisplay,
    /** 53 = Modify Unit Hanger Count               */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 54 = Pause Timer -------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 55 = Unpause Timer                          */ (u8)Flags::AlwaysDisplay,
    /** 56 = Draw --------------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 57 = Set Alliance Status                    */ (u8)Flags::AlwaysDisplay,
    /** 58 = Disable Debug Mode ------------------- */ 0,
    /** 59 = Enable Debug Mode                      */ 0
};

Chk::Condition::ArgType Chk::Condition::getClassicArgType(Type conditionType, size_t argNum)
{
    if ( (size_t)conditionType < NumConditionTypes && argNum < MaxArguments )
        return classicArguments[(size_t)conditionType][argNum].type;
    else
        return Chk::Condition::ArgType::NoType;
}

Chk::Condition::ArgType Chk::Condition::getClassicArgType(VirtualType conditionType, size_t argNum)
{
    if ( argNum < MaxArguments )
    {
        if ( (size_t)conditionType < NumConditionTypes )
            return classicArguments[(size_t)conditionType][argNum].type;

        auto virtualCondition = virtualConditions.find(conditionType);
        if ( virtualCondition != virtualConditions.end() )
            return virtualCondition->second.arguments[argNum].type;
    }
    return Chk::Condition::ArgType::NoType;
}

Chk::Action::ArgType Chk::Action::getClassicArgType(Type actionType, size_t argNum)
{
    if ( (size_t)actionType < NumActionTypes && argNum < MaxArguments )
        return classicArguments[(size_t)actionType][argNum].type;
    else
        return Chk::Action::ArgType::NoType;
}

Chk::Action::ArgType Chk::Action::getClassicArgType(VirtualType actionType, size_t argNum)
{
    if ( argNum < MaxArguments )
    {
        if ( (size_t)actionType < NumActionTypes )
            return classicArguments[(size_t)actionType][argNum].type;

        auto virtualAction = virtualActions.find(actionType);
        if ( virtualAction != virtualActions.end() )
            return virtualAction->second.arguments[argNum].type;
    }
    return Chk::Action::ArgType::NoType;
}

bool Chk::Action::stringUsed(size_t stringId)
{
    return (size_t)actionType < NumActionTypes &&
        ((actionUsesStringArg[(size_t)actionType] && this->stringId == stringId) ||
         (actionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId));
}

bool Chk::Action::gameStringUsed(size_t stringId)
{
    return (size_t)actionType < NumActionTypes &&
        ((actionUsesGameStringArg[(size_t)actionType] && this->stringId == stringId) ||
         (actionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId));
}

bool Chk::Action::commentStringUsed(size_t stringId)
{
    return actionType == Type::Comment && this->stringId == stringId;
}

bool Chk::Action::briefingStringUsed(size_t stringId)
{
    return (size_t)actionType < NumBriefingActionTypes &&
        ((briefingActionUsesStringArg[(size_t)actionType] && this->stringId == stringId) ||
         (briefingActionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId));
}

void Chk::Action::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[(size_t)actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( actionUsesSoundArg[(size_t)actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

void Chk::Action::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesGameStringArg[(size_t)actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( actionUsesSoundArg[(size_t)actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

void Chk::Action::markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( actionType == Type::Comment && stringId > 0 && stringId < Chk::MaxStrings )
        stringIdUsed[stringId] = true;
}

void Chk::Action::markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( (size_t)actionType < NumBriefingActionTypes )
    {
        if ( briefingActionUsesStringArg[(size_t)actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( briefingActionUsesSoundArg[(size_t)actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

void Chk::Action::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(stringId);
            if ( found != stringIdRemappings.end() )
                stringId = found->second;
        }

        if ( actionUsesSoundArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(soundStringId);
            if ( found != stringIdRemappings.end() )
                soundStringId = found->second;
        }
    }
}

void Chk::Action::remapBriefingStringIds(std::map<u32, u32> stringIdRemappings)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( briefingActionUsesStringArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(stringId);
            if ( found != stringIdRemappings.end() )
                stringId = found->second;
        }

        if ( briefingActionUsesSoundArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(soundStringId);
            if ( found != stringIdRemappings.end() )
                soundStringId = found->second;
        }
    }
}

void Chk::Action::deleteString(size_t stringId)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[(size_t)actionType] && this->stringId == stringId )
            this->stringId = 0;

        if ( actionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId )
            this->soundStringId = 0;
    }
}

void Chk::Action::deleteBriefingString(size_t stringId)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( briefingActionUsesStringArg[(size_t)actionType] && this->stringId == stringId )
            this->stringId = 0;

        if ( briefingActionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId )
            this->soundStringId = 0;
    }
}

std::unordered_map<Chk::Action::VirtualType, Chk::Action::VirtualAction> Chk::Action::virtualActions = {
    std::pair<Chk::Action::VirtualType, Chk::Action::VirtualAction>(
        Chk::Action::VirtualType::Custom, { Chk::Action::VirtualType::Indeterminate, {
             locationArg, textFlagsArg, soundArg, durationArg, playerArg, numberArg, actionTypeArg, secondaryTypeIndexArg, flagsArg, paddingArg, maskFlagArg } } ),
    std::pair<Chk::Action::VirtualType, Chk::Action::VirtualAction>(
        Chk::Action::VirtualType::SetMemory, { Chk::Action::VirtualType::SetDeaths, { memoryOffsetArg, numericModArg, amountArg } } )
};

bool Chk::Action::actionUsesStringArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */ true , /**  8 = Play Sound                             */ false,
    /**  9 = Display Text Message ----------------- */ true , /** 10 = Center View -------------------------- */ false, /** 11 = Create Unit with Properties ---------- */ false,
    /** 12 = Set Mission Objectives                 */ true , /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ false, /** 17 = Leader Board (Control) --------------- */ true ,
    /** 18 = Leader Board (Control At Location)     */ true , /** 19 = Leader Board (Resources)               */ true , /** 20 = Leader Board (Kills)                   */ true ,
    /** 21 = Leader Board (Points) ---------------- */ true , /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ false,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ false, /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ false, /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ true , /** 34 = Leaderboard Goal (Control At Location) */ true , /** 35 = Leaderboard Goal (Resources) --------- */ true ,
    /** 36 = Leaderboard Goal (Kills)               */ true , /** 37 = Leaderboard Goal (Points)              */ true , /** 38 = Move Location                          */ false,
    /** 39 = Move Unit ---------------------------- */ false, /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ true ,
    /** 42 = Set Doodad State                       */ false, /** 43 = Set Invincibility                      */ false, /** 44 = Create Unit                            */ false,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ false, /** 47 = Comment ------------------------------ */ true ,
    /** 48 = Give Units to Player                   */ false, /** 49 = Modify Unit Hit Points                 */ false, /** 50 = Modify Unit Energy                     */ false,
    /** 51 = Modify Unit Shield points ------------ */ false, /** 52 = Modify Unit Resource Amount ---------- */ false, /** 53 = Modify Unit Hanger Count ------------- */ false,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
};

bool Chk::Action::actionUsesGameStringArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */ true , /**  8 = Play Sound                             */ false,
    /**  9 = Display Text Message ----------------- */ true , /** 10 = Center View -------------------------- */ false, /** 11 = Create Unit with Properties ---------- */ false,
    /** 12 = Set Mission Objectives                 */ true , /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ false, /** 17 = Leader Board (Control) --------------- */ true ,
    /** 18 = Leader Board (Control At Location)     */ true , /** 19 = Leader Board (Resources)               */ true , /** 20 = Leader Board (Kills)                   */ true ,
    /** 21 = Leader Board (Points) ---------------- */ true , /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ false,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ false, /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ false, /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ true , /** 34 = Leaderboard Goal (Control At Location) */ true , /** 35 = Leaderboard Goal (Resources) --------- */ true ,
    /** 36 = Leaderboard Goal (Kills)               */ true , /** 37 = Leaderboard Goal (Points)              */ true , /** 38 = Move Location                          */ false,
    /** 39 = Move Unit ---------------------------- */ false, /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ true ,
    /** 42 = Set Doodad State                       */ false, /** 43 = Set Invincibility                      */ false, /** 44 = Create Unit                            */ false,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ false, /** 47 = Comment ------------------------------ */ false,
    /** 48 = Give Units to Player                   */ false, /** 49 = Modify Unit Hit Points                 */ false, /** 50 = Modify Unit Energy                     */ false,
    /** 51 = Modify Unit Shield points ------------ */ false, /** 52 = Modify Unit Resource Amount ---------- */ false, /** 53 = Modify Unit Hanger Count ------------- */ false,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
};

bool Chk::Action::actionUsesSoundArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */  true, /**  8 = Play Sound                             */ true ,
    /**  9 = Display Text Message ----------------- */ false, /** 10 = Center View -------------------------- */ false, /** 11 = Create Unit with Properties ---------- */ false,
    /** 12 = Set Mission Objectives                 */ false, /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ false, /** 17 = Leader Board (Control) --------------- */ false,
    /** 18 = Leader Board (Control At Location)     */ false, /** 19 = Leader Board (Resources)               */ false, /** 20 = Leader Board (Kills)                   */ false,
    /** 21 = Leader Board (Points) ---------------- */ false, /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ false,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ false, /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ false, /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ false, /** 34 = Leaderboard Goal (Control At Location) */ false, /** 35 = Leaderboard Goal (Resources) --------- */ false,
    /** 36 = Leaderboard Goal (Kills)               */ false, /** 37 = Leaderboard Goal (Points)              */ false, /** 38 = Move Location                          */ false,
    /** 39 = Move Unit ---------------------------- */ false, /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ false,
    /** 42 = Set Doodad State                       */ false, /** 43 = Set Invincibility                      */ false, /** 44 = Create Unit                            */ false,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ false, /** 47 = Comment ------------------------------ */ false,
    /** 48 = Give Units to Player                   */ false, /** 49 = Modify Unit Hit Points                 */ false, /** 50 = Modify Unit Energy                     */ false,
    /** 51 = Modify Unit Shield points ------------ */ false, /** 52 = Modify Unit Resource Amount ---------- */ false, /** 53 = Modify Unit Hanger Count ------------- */ false,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
};

Chk::Action::Argument Chk::Action::classicBriefingArguments[NumBriefingActionTypes][MaxArguments] = {
    /** 0 = No Action                 */ {},
    /** 1 = Wait -------------------- */ { durationArg },
    /** 2 = Play Sound                */ { soundArg }, // Hidden: durationArg (auto-calculate)
    /** 3 = Text Message ------------ */ { durationArg, stringArg },
    /** 4 = Mission Objectives        */ { stringArg },
    /** 5 = Show Portrait ----------- */ { unitArg, briefingSlotArg }, // Flags::UnitTypeUsed is set
    /** 6 = Hide Portrait             */ { briefingSlotArg },
    /** 7 = Display Speaking Portrait */ { briefingSlotArg, durationArg },
    /** 8 = Transmission              */ { briefingSlotArg, soundArg, numericModArg, amountArg, stringArg }, // Hidden: durationArg (auto-calculate)
    /** 9 = Skip Tutorial Enabled --- */ {}
};

Chk::Action::Argument Chk::Action::briefingTextArguments[NumBriefingActionTypes][MaxArguments] = {
    /** 0 = No Action                 */ {},
    /** 1 = Wait -------------------- */ { durationArg },
    /** 2 = Play Sound                */ { soundArg, durationArg },
    /** 3 = Text Message ------------ */ { stringArg, durationArg },
    /** 4 = Mission Objectives        */ { stringArg },
    /** 5 = Show Portrait ----------- */ { unitArg, briefingSlotArg },
    /** 6 = Hide Portrait             */ { briefingSlotArg },
    /** 7 = Display Speaking Portrait */ { briefingSlotArg, durationArg },
    /** 8 = Transmission              */ { stringArg, briefingSlotArg, numericModArg, amountArg, soundArg, durationArg },
    /** 9 = Skip Tutorial Enabled --- */ {}
};

u8 Chk::Action::briefingDefaultFlags[NumBriefingActionTypes] = {
    /** 0 = No Action                 */ 0,
    /** 1 = Wait -------------------- */ 0,
    /** 2 = Play Sound                */ 0,
    /** 3 = Text Message ------------ */ 0,
    /** 4 = Mission Objectives        */ 0,
    /** 5 = Show Portrait ----------- */ (u8)Chk::Action::Flags::UnitTypeUsed,
    /** 6 = Hide Portrait             */ 0,
    /** 7 = Display Speaking Portrait */ 0,
    /** 8 = Transmission              */ 0,
    /** 9 = Skip Tutorial Enabled --- */ 0
};

bool Chk::Action::briefingActionUsesStringArg[NumBriefingActionTypes] = {
    /** 0 = No Action                 */ false, /** 1 = Wait -------------------- */ false, /** 2 = Play Sound                */ false, /** 3 = Text Message ------------ */ true ,
    /** 4 = Mission Objectives        */  true, /** 5 = Show Portrait ----------- */ false, /** 6 = Hide Portrait             */ false, /** 7 = Display Speaking Portrait */ false,
    /** 8 = Transmission              */  true, /** 9 = Skip Tutorial Enabled --- */ false
};

bool Chk::Action::briefingActionUsesSoundArg[NumBriefingActionTypes] = {
    /** 0 = No Action                 */ false, /** 1 = Wait -------------------- */ false, /** 2 = Play Sound                */  true, /** 3 = Text Message ------------ */ false,
    /** 4 = Mission Objectives        */ false, /** 5 = Show Portrait ----------- */ false, /** 6 = Hide Portrait             */ false, /** 7 = Display Speaking Portrait */ false,
    /** 8 = Transmission              */  true, /** 9 = Skip Tutorial Enabled --- */ false
};

Chk::Condition & Chk::Trigger::condition(size_t conditionIndex)
{
    if ( conditionIndex < MaxConditions )
        return conditions[conditionIndex];
    else
        throw std::out_of_range("conditionIndex " + std::to_string(conditionIndex) + " exceeds max " + std::to_string(MaxConditions-1));
}

Chk::Action & Chk::Trigger::action(size_t actionIndex)
{
    if ( actionIndex < MaxActions )
        return actions[actionIndex];
    else
        throw std::out_of_range("actionIndex " + std::to_string(actionIndex) + " exceeds max " + std::to_string(MaxActions-1));
}

void Chk::Condition::ToggleDisabled()
{
    if ( (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled )
        flags &= (u8)Flags::xDisabled;
    else
        flags |= (u8)Flags::Disabled;
}

bool Chk::Condition::isDisabled()
{
    return (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled;
}

void Chk::Action::ToggleDisabled()
{
    if ( (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled )
        flags &= (u8)Flags::xDisabled;
    else
        flags |= (u8)Flags::Disabled;
}

Chk::Trigger::Owned & Chk::Trigger::owned(size_t ownerIndex)
{
    if ( ownerIndex < MaxOwners )
        return owners[ownerIndex];
    else
        throw std::out_of_range("ownerIndex " + std::to_string(ownerIndex) + " exceeds max " + std::to_string(MaxOwners-1));
}

Chk::Trigger& Chk::Trigger::operator=(const Trigger &trigger)
{
    if ( this != &trigger )
        memcpy(this, &trigger, sizeof(Trigger));

    return *this;
}

void Chk::Trigger::deleteAction(size_t actionIndex, bool alignTop)
{
    if ( actionIndex < MaxActions )
    {
        actions[actionIndex].locationId = 0;
        actions[actionIndex].stringId = 0;
        actions[actionIndex].soundStringId = 0;
        actions[actionIndex].time = 0;
        actions[actionIndex].group = 0;
        actions[actionIndex].number = 0;
        actions[actionIndex].type = 0;
        actions[actionIndex].actionType = Chk::Action::Type::NoAction;
        actions[actionIndex].type2 = 0;
        actions[actionIndex].flags = 0;
        actions[actionIndex].padding = 0;
        actions[actionIndex].maskFlag = Chk::Action::MaskFlag::Disabled;

        if ( alignTop )
            alignActionsTop();
    }
}

void Chk::Trigger::deleteCondition(size_t conditionIndex, bool alignTop)
{
    if ( conditionIndex < MaxConditions )
    {
        conditions[conditionIndex].locationId = 0;
        conditions[conditionIndex].player = 0;
        conditions[conditionIndex].amount = 0;
        conditions[conditionIndex].unitType = Sc::Unit::Type::TerranMarine;
        conditions[conditionIndex].comparison = Chk::Condition::Comparison::AtLeast;
        conditions[conditionIndex].conditionType = Chk::Condition::Type::NoCondition;
        conditions[conditionIndex].typeIndex = 0;
        conditions[conditionIndex].flags = 0;
        conditions[conditionIndex].maskFlag = Chk::Condition::MaskFlag::Disabled;
    
        if ( alignTop )
            alignConditionsTop();
    }
}

bool Chk::Trigger::preserveTriggerFlagged()
{   
    return (flags & (u32)Flags::PreserveTrigger) == (u32)Flags::PreserveTrigger;
}

bool Chk::Trigger::disabled()
{
    return (flags & (u32)Flags::Disabled) == (u32)Flags::Disabled;
}

bool Chk::Trigger::ignoreConditionsOnce()
{
    return (flags & (u32)Flags::IgnoreConditionsOnce) == (u32)Flags::IgnoreConditionsOnce;
}

bool Chk::Trigger::ignoreWaitSkipOnce()
{
    return (flags & (u32)Flags::IgnoreWaitSkipOnce) == (u32)Flags::IgnoreWaitSkipOnce;
}

bool Chk::Trigger::ignoreMiscActionsOnce()
{
    return (flags & (u32)Flags::IgnoreMiscActions) == (u32)Flags::IgnoreMiscActions;
}

bool Chk::Trigger::ignoreDefeatDraw()
{
    return (flags & (u32)Flags::IgnoreDefeatDraw) == (u32)Flags::IgnoreDefeatDraw;
}

bool Chk::Trigger::pauseFlagged()
{
    return (flags & (u32)Flags::Paused) == (u32)Flags::Paused;
}

void Chk::Trigger::setPreserveTriggerFlagged(bool preserved)
{
    if ( preserved )
        flags |= (u32)Flags::PreserveTrigger;
    else
        flags &= ~(u32)Flags::PreserveTrigger;
}

void Chk::Trigger::setDisabled(bool disabled)
{
    if ( disabled )
        flags |= (u32)Flags::Disabled;
    else
        flags &= ~(u32)Flags::Disabled;
}

void Chk::Trigger::setIgnoreConditionsOnce(bool ignoreConditionsOnce)
{
    if ( ignoreConditionsOnce )
        flags |= (u32)Flags::IgnoreConditionsOnce;
    else
        flags &= ~(u32)Flags::IgnoreConditionsOnce;
}

void Chk::Trigger::setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce)
{
    if ( ignoreWaitSkipOnce )
        flags |= (u32)Flags::IgnoreWaitSkipOnce;
    else
        flags &= ~(u32)Flags::IgnoreWaitSkipOnce;
}

void Chk::Trigger::setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce)
{
    if ( ignoreMiscActionsOnce )
        flags |= (u32)Flags::IgnoreMiscActions;
    else
        flags &= ~(u32)Flags::IgnoreMiscActions;
}

void Chk::Trigger::setIgnoreDefeatDraw(bool ignoreDefeatDraw)
{
    if ( ignoreDefeatDraw )
        flags |= (u32)Flags::IgnoreDefeatDraw;
    else
        flags &= ~(u32)Flags::IgnoreDefeatDraw;
}

void Chk::Trigger::setPauseFlagged(bool pauseFlagged)
{
    if ( pauseFlagged )
        flags |= (u32)Flags::Paused;
    else
        flags &= ~(u32)Flags::Paused;
}

size_t Chk::Trigger::numUsedConditions()
{
    size_t total = 0;
    for ( size_t i=0; i<Chk::Trigger::MaxConditions; i++ )
    {
        if ( conditions[i].conditionType != Chk::Condition::Type::NoCondition )
            total ++;
    }
    return total;
}

size_t Chk::Trigger::numUsedActions()
{
    size_t total = 0;
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        if ( actions[i].actionType != Chk::Action::Type::NoAction )
            total ++;
    }
    return total;
}

bool Chk::Trigger::stringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].stringUsed(stringId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::gameStringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].gameStringUsed(stringId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::commentStringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].commentStringUsed(stringId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::briefingStringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].briefingStringUsed(stringId) )
            return true;
    }
    return false;
}

void Chk::Trigger::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedStrings(stringIdUsed);
}

void Chk::Trigger::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedGameStrings(stringIdUsed);
}

void Chk::Trigger::markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedCommentStrings(stringIdUsed);
}

void Chk::Trigger::markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedBriefingStrings(stringIdUsed);
}

void Chk::Trigger::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].remapStringIds(stringIdRemappings);
}

void Chk::Trigger::remapBriefingStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].remapBriefingStringIds(stringIdRemappings);
}

void Chk::Trigger::deleteString(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].deleteString(stringId);
}

void Chk::Trigger::deleteBriefingString(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].deleteBriefingString(stringId);
}

void Chk::Trigger::alignConditionsTop()
{
    for ( u8 i=0; i<MaxConditions; i++ )
    {
        if ( conditions[i].conditionType == Chk::Condition::Type::NoCondition )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<MaxConditions; j++ )
            {
                if ( conditions[j].conditionType != Chk::Condition::Type::NoCondition )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<MaxConditions; j++ )
                    conditions[j] = conditions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}

void Chk::Trigger::alignActionsTop()
{
    for ( u8 i=0; i<MaxActions; i++ )
    {
        if ( actions[i].actionType == Chk::Action::Type::NoAction )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<MaxActions; j++ )
            {
                if ( actions[j].actionType != Chk::Action::Type::NoAction )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<MaxActions; j++ )
                    actions[j] = actions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Unit & unit)
{
    out.write(reinterpret_cast<const char*>(&unit), sizeof(Chk::Unit));
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Doodad & doodad)
{
    out.write(reinterpret_cast<const char*>(&doodad), sizeof(Chk::Doodad));
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Sprite & sprite)
{
    out.write(reinterpret_cast<const char*>(&sprite), sizeof(Chk::Sprite));
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Location & location)
{
    out.write(reinterpret_cast<const char*>(&location), sizeof(Chk::Location));
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Condition & condition)
{
    out.write(reinterpret_cast<const char*>(&condition), sizeof(Chk::Condition));
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Action & action)
{
    out.write(reinterpret_cast<const char*>(&action), sizeof(Chk::Action));
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Trigger & trigger)
{
    out.write(reinterpret_cast<const char*>(&trigger), sizeof(Chk::Trigger));
    return out;
}

bool Chk::Sprite::isDrawnAsSprite()
{
    return flags & (u16)SpriteFlags::DrawAsSprite == (u16)SpriteFlags::DrawAsSprite;
}

bool Chk::Cuwp::isCloaked()
{
    return unitStateFlags & (u16)State::Cloak == (u16)State::Cloak;
}

bool Chk::Cuwp::isBurrowed()
{
    return unitStateFlags & (u16)State::Burrow == (u16)State::Burrow;
}

bool Chk::Cuwp::isInTransit()
{
    return unitStateFlags & (u16)State::InTransit == (u16)State::InTransit;
}

bool Chk::Cuwp::isHallucinated()
{
    return unitStateFlags & (u16)State::Hallucinated == (u16)State::Hallucinated;
}

bool Chk::Cuwp::isInvincible()
{
    return unitStateFlags & (u16)State::Invincible == (u16)State::Invincible;
}

void Chk::Cuwp::setCloaked(bool cloaked)
{
    unitStateFlags = cloaked ? unitStateFlags | (u16)State::Cloak : unitStateFlags & (u16)State::xCloak;
}

void Chk::Cuwp::setBurrowed(bool burrowed)
{
    unitStateFlags = burrowed ? unitStateFlags | (u16)State::Burrow : unitStateFlags & (u16)State::xBurrow;
}

void Chk::Cuwp::setInTransit(bool inTransit)
{
    unitStateFlags = inTransit ? unitStateFlags | (u16)State::InTransit : unitStateFlags & (u16)State::xInTransit;
}

void Chk::Cuwp::setHallucinated(bool hallucinated)
{
    unitStateFlags = hallucinated ? unitStateFlags | (u16)State::Hallucinated : unitStateFlags & (u16)State::xHallucinated;
}

void Chk::Cuwp::setInvincible(bool invincible)
{
    unitStateFlags = invincible ? unitStateFlags | (u16)State::Invincible : unitStateFlags & (u16)State::xInvincible;
}

Chk::Location::Location() : left(0), top(0), right(0), bottom(0), stringId(0), elevationFlags(0)
{

}

ChkSection::ChkSection(SectionName sectionName, bool virtualizable, bool isVirtual)
    : sectionIndex(SectionIndex::UNKNOWN), sectionName(sectionName), virtualizable(virtualizable), dataIsVirtual(isVirtual)
{
    auto foundSectionIndex = sectionIndexes.find(sectionName);
    if ( foundSectionIndex != sectionIndexes.end() )
        sectionIndex = foundSectionIndex->second;
}

template <typename StructType>
ChkSection::ChkSection(SectionName sectionName, StructType & data, bool virtualizable, bool isVirtual)
    : sectionIndex(SectionIndex::UNKNOWN), sectionName(sectionName), virtualizable(virtualizable), dataIsVirtual(isVirtual), rawData(buffer::make(data))
{
    auto foundSectionIndex = sectionIndexes.find(sectionName);
    if ( foundSectionIndex != sectionIndexes.end() )
        sectionIndex = foundSectionIndex->second;
}

std::ostream & ChkSection::write(std::ostream &s)
{
    u32 size = getSize();
    s << (u32)sectionName << size;
    return writeData(s, size);
}

u32 ChkSection::getSize()
{
    s64 bufferSize = rawData->size();
    if ( bufferSize > (s64)ChkSection::MaxChkSectionSize )
        throw MaxSectionSizeExceeded(sectionName, std::to_string(bufferSize));
    else
        return (u32)rawData->size();
}

LoadBehavior ChkSection::getLoadBehavior(SectionIndex sectionIndex)
{
    auto nonStandardLoadBehavior = nonStandardLoadBehaviors.find(sectionIndex);
    if ( nonStandardLoadBehavior != nonStandardLoadBehaviors.end() )
        return nonStandardLoadBehavior->second;
    else
        return LoadBehavior::Standard;
}

std::string ChkSection::getNameString(SectionName sectionName)
{
    auto foundSectionNameString = sectionNameStrings.find(sectionName);
    if ( foundSectionNameString != sectionNameStrings.end() )
        return foundSectionNameString->second;
    else
        return sectionNameStrings.find(SectionName::UNKNOWN)->second;
}

ChkSection::MaxSectionSizeExceeded::MaxSectionSizeExceeded(SectionName sectionName, std::string sectionSize) :
    error("The size of ChkSection " + ChkSection::getNameString(sectionName) + " was " + sectionSize + " which exceeded the max size: " + std::to_string(MaxChkSectionSize))
{

}

ChkSection::SectionSerializationSizeMismatch::SectionSerializationSizeMismatch(SectionName sectionName, u32 expectedSectionSize, size_t actualDataSize) :
    error("The expected size of ChkSection " + ChkSection::getNameString(sectionName) + " was " + std::to_string(expectedSectionSize)
        + " which exceeded the actual size: " + std::to_string(actualDataSize))
{

}

ChkSection::SectionValidationException::SectionValidationException(SectionName sectionName, std::string error) :
    error("Validation error in chk section " + ChkSection::getNameString(sectionName) + " : " + error)
{

}



SectionName ChkSection::sectionNames[] = {
    SectionName::TYPE, SectionName::VER, SectionName::IVER, SectionName::IVE2,
    SectionName::VCOD, SectionName::IOWN, SectionName::OWNR, SectionName::ERA,
    SectionName::DIM, SectionName::SIDE, SectionName::MTXM, SectionName::PUNI,
    SectionName::UPGR, SectionName::PTEC, SectionName::UNIT, SectionName::ISOM,
    SectionName::TILE, SectionName::DD2, SectionName::THG2, SectionName::MASK,
    SectionName::STR, SectionName::UPRP, SectionName::UPUS, SectionName::MRGN,
    SectionName::TRIG, SectionName::MBRF, SectionName::SPRP, SectionName::FORC,
    SectionName::WAV, SectionName::UNIS, SectionName::UPGS, SectionName::TECS,
    SectionName::SWNM, SectionName::COLR, SectionName::PUPx, SectionName::PTEx,
    SectionName::UNIx, SectionName::UPGx, SectionName::TECx,

    SectionName::KSTR,

    SectionName::UNKNOWN
};

std::unordered_map<SectionName, std::string> ChkSection::sectionNameStrings = {
    std::pair<SectionName, std::string>(SectionName::TYPE, "TYPE"), std::pair<SectionName, std::string>(SectionName::VER, "VER"),
    std::pair<SectionName, std::string>(SectionName::IVER, "IVER"), std::pair<SectionName, std::string>(SectionName::IVE2, "IVE2"),
    std::pair<SectionName, std::string>(SectionName::VCOD, "VCOD"), std::pair<SectionName, std::string>(SectionName::IOWN, "IOWN"),
    std::pair<SectionName, std::string>(SectionName::OWNR, "OWNR"), std::pair<SectionName, std::string>(SectionName::ERA, "ERA"),
    std::pair<SectionName, std::string>(SectionName::DIM, "DIM"), std::pair<SectionName, std::string>(SectionName::SIDE, "SIDE"),
    std::pair<SectionName, std::string>(SectionName::MTXM, "MTXM"), std::pair<SectionName, std::string>(SectionName::PUNI, "PUNI"),
    std::pair<SectionName, std::string>(SectionName::UPGR, "UPGR"), std::pair<SectionName, std::string>(SectionName::PTEC, "PTEC"),
    std::pair<SectionName, std::string>(SectionName::UNIT, "UNIT"), std::pair<SectionName, std::string>(SectionName::ISOM, "ISOM"),
    std::pair<SectionName, std::string>(SectionName::TILE, "TILE"), std::pair<SectionName, std::string>(SectionName::DD2, "DD2"),
    std::pair<SectionName, std::string>(SectionName::THG2, "THG2"), std::pair<SectionName, std::string>(SectionName::MASK, "MASK"),
    std::pair<SectionName, std::string>(SectionName::STR, "STR"), std::pair<SectionName, std::string>(SectionName::UPRP, "UPRP"),
    std::pair<SectionName, std::string>(SectionName::UPUS, "UPUS"), std::pair<SectionName, std::string>(SectionName::MRGN, "MRGN"),
    std::pair<SectionName, std::string>(SectionName::TRIG, "TRIG"), std::pair<SectionName, std::string>(SectionName::MBRF, "MBRF"),
    std::pair<SectionName, std::string>(SectionName::SPRP, "SPRP"), std::pair<SectionName, std::string>(SectionName::FORC, "FORC"),
    std::pair<SectionName, std::string>(SectionName::WAV, "WAV"), std::pair<SectionName, std::string>(SectionName::UNIS, "UNIS"),
    std::pair<SectionName, std::string>(SectionName::UPGS, "UPGS"), std::pair<SectionName, std::string>(SectionName::TECS, "TECS"),
    std::pair<SectionName, std::string>(SectionName::SWNM, "SWNM"), std::pair<SectionName, std::string>(SectionName::COLR, "COLR"),
    std::pair<SectionName, std::string>(SectionName::PUPx, "PUPx"), std::pair<SectionName, std::string>(SectionName::PTEx, "PTEx"),
    std::pair<SectionName, std::string>(SectionName::UNIx, "UNIx"), std::pair<SectionName, std::string>(SectionName::UPGx, "UPGx"),
    std::pair<SectionName, std::string>(SectionName::TECx, "TECx"),

    std::pair<SectionName, std::string>(SectionName::KSTR, "KSTR"),

    std::pair<SectionName, std::string>(SectionName::UNKNOWN, "UNKNOWN")
};

std::unordered_map<SectionName, SectionIndex> ChkSection::sectionIndexes = {
    std::pair<SectionName, SectionIndex>(SectionName::TYPE, SectionIndex::TYPE),
    std::pair<SectionName, SectionIndex>(SectionName::VER, SectionIndex::VER),
    std::pair<SectionName, SectionIndex>(SectionName::IVER, SectionIndex::IVER),
    std::pair<SectionName, SectionIndex>(SectionName::IVE2, SectionIndex::IVE2),
    std::pair<SectionName, SectionIndex>(SectionName::VCOD, SectionIndex::VCOD),
    std::pair<SectionName, SectionIndex>(SectionName::IOWN, SectionIndex::IOWN),
    std::pair<SectionName, SectionIndex>(SectionName::OWNR, SectionIndex::OWNR),
    std::pair<SectionName, SectionIndex>(SectionName::ERA, SectionIndex::ERA),
    std::pair<SectionName, SectionIndex>(SectionName::DIM, SectionIndex::DIM),
    std::pair<SectionName, SectionIndex>(SectionName::SIDE, SectionIndex::SIDE),
    std::pair<SectionName, SectionIndex>(SectionName::MTXM, SectionIndex::MTXM),
    std::pair<SectionName, SectionIndex>(SectionName::PUNI, SectionIndex::PUNI),
    std::pair<SectionName, SectionIndex>(SectionName::UPGR, SectionIndex::UPGR),
    std::pair<SectionName, SectionIndex>(SectionName::PTEC, SectionIndex::PTEC),
    std::pair<SectionName, SectionIndex>(SectionName::UNIT, SectionIndex::UNIT),
    std::pair<SectionName, SectionIndex>(SectionName::ISOM, SectionIndex::ISOM),
    std::pair<SectionName, SectionIndex>(SectionName::TILE, SectionIndex::TILE),
    std::pair<SectionName, SectionIndex>(SectionName::DD2, SectionIndex::DD2),
    std::pair<SectionName, SectionIndex>(SectionName::THG2, SectionIndex::THG2),
    std::pair<SectionName, SectionIndex>(SectionName::MASK, SectionIndex::MASK),
    std::pair<SectionName, SectionIndex>(SectionName::STR, SectionIndex::STR),
    std::pair<SectionName, SectionIndex>(SectionName::UPRP, SectionIndex::UPRP),
    std::pair<SectionName, SectionIndex>(SectionName::UPUS, SectionIndex::UPUS),
    std::pair<SectionName, SectionIndex>(SectionName::MRGN, SectionIndex::MRGN),
    std::pair<SectionName, SectionIndex>(SectionName::TRIG, SectionIndex::TRIG),
    std::pair<SectionName, SectionIndex>(SectionName::MBRF, SectionIndex::MBRF),
    std::pair<SectionName, SectionIndex>(SectionName::SPRP, SectionIndex::SPRP),
    std::pair<SectionName, SectionIndex>(SectionName::FORC, SectionIndex::FORC),
    std::pair<SectionName, SectionIndex>(SectionName::WAV, SectionIndex::WAV),
    std::pair<SectionName, SectionIndex>(SectionName::UNIS, SectionIndex::UNIS),
    std::pair<SectionName, SectionIndex>(SectionName::UPGS, SectionIndex::UPGS),
    std::pair<SectionName, SectionIndex>(SectionName::TECS, SectionIndex::TECS),
    std::pair<SectionName, SectionIndex>(SectionName::SWNM, SectionIndex::SWNM),
    std::pair<SectionName, SectionIndex>(SectionName::COLR, SectionIndex::COLR),
    std::pair<SectionName, SectionIndex>(SectionName::PUPx, SectionIndex::PUPx),
    std::pair<SectionName, SectionIndex>(SectionName::PTEx, SectionIndex::PTEx),
    std::pair<SectionName, SectionIndex>(SectionName::UNIx, SectionIndex::UNIx),
    std::pair<SectionName, SectionIndex>(SectionName::UPGx, SectionIndex::UPGx),
    std::pair<SectionName, SectionIndex>(SectionName::TECx, SectionIndex::TECx),

    std::pair<SectionName, SectionIndex>(SectionName::KSTR, SectionIndex::KSTR),

    std::pair<SectionName, SectionIndex>(SectionName::UNKNOWN, SectionIndex::UNKNOWN)
};

std::unordered_map<SectionIndex, LoadBehavior> ChkSection::nonStandardLoadBehaviors = {
    // TODO: Populate me
};

std::ostream & operator<< (std::ostream & out, const Chk::IsomEntry & isomEntry)
{
    out.write(reinterpret_cast<const char*>(&isomEntry), sizeof(Chk::IsomEntry));
}

Chk::StringProperties::StringProperties()
    : Chk::StringProperties(0, 0, 0, false, false, false, false, false, 8)
{

}

Chk::StringProperties::StringProperties(u8 red, u8 green, u8 blue, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics, u32 size)
    : red(red), green(green), blue(blue)
{
    u8 flags = (isUsed ? (u8)StrFlags::isUsed : 0) | (hasPriority ? (u8)StrFlags::hasPriority : 0) | (isBold ? (u8)StrFlags::bold : 0) |
        (isUnderlined ? (u8)StrFlags::underlined : 0) | (isItalics ? (u8)StrFlags::italics : 0);

    constexpr u32 maxFontSize = baseFontSize + 7*fontStepSize;
    if ( size < baseFontSize || size > maxFontSize )
        throw InvalidSize(size, baseFontSize, fontStepSize);

    size -= baseFontSize;
    if ( size > 4*fontStepSize )
    {
        flags |= (u8)StrFlags::sizePlusFourSteps;
        size -= 4*fontStepSize;
    }
    if ( size > 2*fontStepSize )
    {
        flags |= (u8)StrFlags::sizePlusTwoSteps;
        size -= 2*fontStepSize;
    }
    if ( size > fontStepSize )
    {
        flags |= (u8)StrFlags::sizePlusOneStep;
        size -= fontStepSize;
    }

    if ( size > 0 )
        throw InvalidSize(size, baseFontSize, fontStepSize);

    this->flags = flags;
}

StrProp::StrProp() : red(0), green(0), blue(0), isUsed(false), hasPriority(false), isBold(false), isUnderlined(false), isItalics(false), size(Chk::baseFontSize)
{

}

StrProp::StrProp(Chk::StringProperties stringProperties) :
    red(stringProperties.red), green(stringProperties.green), blue(stringProperties.blue),
    isUsed(stringProperties.flags & Chk::StrFlags::isUsed == Chk::StrFlags::isUsed), hasPriority(stringProperties.flags & Chk::StrFlags::hasPriority == Chk::StrFlags::hasPriority),
    isBold(stringProperties.flags & Chk::StrFlags::bold == Chk::StrFlags::bold), isUnderlined(stringProperties.flags & Chk::StrFlags::underlined == Chk::StrFlags::underlined),
    isItalics(stringProperties.flags & Chk::StrFlags::italics == Chk::StrFlags::italics),
    size(Chk::baseFontSize +
        (stringProperties.flags & Chk::StrFlags::sizePlusFourSteps == Chk::StrFlags::sizePlusFourSteps) ? 4*Chk::fontStepSize : 0 +
        (stringProperties.flags & Chk::StrFlags::sizePlusTwoSteps == Chk::StrFlags::sizePlusTwoSteps) ? 2*Chk::fontStepSize : 0 +
        (stringProperties.flags & Chk::StrFlags::sizePlusOneStep == Chk::StrFlags::sizePlusOneStep) ? 1*Chk::fontStepSize : 0)
{
    
}

ScStr::ScStr(const std::string &str) : allocation(str)
{

}

ScStr::ScStr(const std::string &str, const StrProp &strProp) : allocation(str), strProp(strProp)
{

}

bool ScStr::empty()
{
    return parentStr == nullptr ? allocation.empty() : parentStr->empty();
}

size_t ScStr::length()
{
    return parentStr == nullptr ? allocation.length() : strlen(str);
}

template <typename StringType>
int ScStr::compare(const StringType &str)
{
    RawString rawStr;
    ConvertStr<StringType, RawString>(str, rawStr);
    return strcmp(this->str, rawStr.c_str());
}
template int ScStr::compare<RawString>(const RawString &str);
template int ScStr::compare<EscString>(const EscString &str);
template int ScStr::compare<ChkdString>(const ChkdString &str);
template int ScStr::compare<SingleLineChkdString>(const SingleLineChkdString &str);

template <typename StringType>
StringType ScStr::toString()
{
    StringType destStr;
    ConvertStr<RawString, StringType>(str, destStr);
    return destStr;
}
template RawString ScStr::toString<RawString>();
template EscString ScStr::toString<EscString>();
template ChkdString ScStr::toString<ChkdString>();
template SingleLineChkdString ScStr::toString<SingleLineChkdString>();

ScStrPtr ScStr::getParentStr()
{
    return parentStr;
}

ScStrPtr ScStr::getChildStr()
{
    return childStr;
}

bool ScStr::adopt(ScStrPtr lhs, ScStrPtr rhs)
{
    if ( rhs != nullptr && lhs != nullptr && rhs->parentStr == nullptr || lhs->parentStr == nullptr )
    {
        size_t lhsLength = lhs->parentStr == nullptr ? lhs->allocation.length() : strlen(lhs->str);
        size_t rhsLength = rhs->parentStr == nullptr ? rhs->allocation.length() : strlen(rhs->str);
        if ( rhsLength > lhsLength ) // The length of rhs is greater
        {
            const char* rhsSubString = &rhs->str[rhsLength-lhsLength];
            if ( strcmp(lhs->str, rhsSubString) == 0 ) // rhs can adopt lhs
                ScStr::adopt(rhs, lhs, rhsLength, lhsLength, rhsSubString);
        }
        else // lhsLength >= rhsLength - the length of lhs is greater or equal to rhs
        {
            const char* lhsSubString = &lhs->str[lhsLength-rhsLength];
            if ( strcmp(rhs->str, lhsSubString) == 0 ) // lhs can adopt rhs
                ScStr::adopt(lhs, rhs, lhsLength, rhsLength, lhsSubString);
        }
    }
    return false;
}

void ScStr::disown()
{
    if ( childStr != nullptr )
    {
        childStr->parentStr = parentStr; // Make child's parent its grandparent (or nullptr if none)
        childStr = nullptr;
    }
    if ( parentStr != nullptr )
    {
        parentStr->childStr = childStr; // Make parent's child its grandchild (or nullptr if none)
        parentStr = nullptr;
    }
}

void ScStr::adopt(ScStrPtr parent, ScStrPtr child, size_t parentLength, size_t childLength, const char* parentSubString)
{
    ScStrPtr nextChild = parent->childStr; // Some descendent of parent may be a more suitable parent for this child
    while ( nextChild != nullptr && nextChild->length() > childLength )
    {
        parent = nextChild;
        nextChild = parent->childStr;
    }
    if ( nextChild != nullptr ) // There are more children smaller than this one that this child should adopt
    {
        child->childStr = parent->childStr; // Make child's child the smaller child
        child->childStr->parentStr = child; // Make parent for that same smaller child this child
        child->parentStr = parent; // Make child's parent the smallest suitable parent selected above
        parent->childStr = child; // Assign the child as child to the parent
    }
    else // nextChild == nullptr, there are no more children smaller than this one
    {
        child->parentStr = parent; // Make child's parent the smallest suitable parent selected above
        parent->childStr = child; // Assign the child as child to the parent
    }
}

ScStrPair::ScStrPair(const std::string &str, Chk::Scope storageScope)
    : gameStr(storageScope == Chk::Scope::Game ? ScStrPtr(new ScStr(str)) : nullptr), editorStr(storageScope == Chk::Scope::Editor ? ScStrPtr(new ScStr(str)) : nullptr)
{

}

ScStrPair::ScStrPair(const std::string &str, Chk::Scope storageScope, StrProp strProp)
    : gameStr(storageScope == Chk::Scope::Game ? ScStrPtr(new ScStr(str)) : nullptr), editorStr(storageScope == Chk::Scope::Editor ? ScStrPtr(new ScStr(str, strProp)) : nullptr)
{

}

ScStrPair::ScStrPair(const std::string &gameStr, const std::string &editorStr)
    : gameStr(ScStrPtr(new ScStr(gameStr))), editorStr(ScStrPtr(new ScStr(editorStr)))
{

}

ScStrPair::ScStrPair(const std::string &gameStr, const std::string &editorStr, StrProp strProp)
    : gameStr(ScStrPtr(new ScStr(gameStr))), editorStr(ScStrPtr(new ScStr(editorStr, strProp)))
{

}

MaximumStringsExceeded::MaximumStringsExceeded(std::string sectionName, size_t numStrings, size_t maxStrings)
    : StringException("Error with " + sectionName + " section: " + std::to_string(numStrings)
        + " strings given, but the " + sectionName + " section can have " + std::to_string(maxStrings) + " strings max.")
{
    
}

MaximumStringsExceeded::MaximumStringsExceeded()
    : StringException("Error placing string, limit of 65536 strings exceeded!")
{

}

InsufficientStringCapacity::InsufficientStringCapacity(std::string sectionName, size_t numStrings, size_t requestedCapacity, bool autoDefragment)
    : StringException("Error changing the " + sectionName + " section string capacity to " + std::to_string(requestedCapacity)
        + (autoDefragment || numStrings > requestedCapacity ? " there are " + std::to_string(numStrings) + " strings used in the map." : " the " + sectionName + " section would need to be defragmented."))
{

}

MaximumCharactersExceeded::MaximumCharactersExceeded(std::string sectionName, size_t numCharacters, size_t characterSpaceSize)
    : StringException("Error serializing " + sectionName + " section: " + std::to_string(numCharacters) + " characters given, which cannot fit in the "
        + std::to_string(characterSpaceSize) + " bytes of pointable character space using selected compression.")
{

}

MaximumOffsetAndCharsExceeded::MaximumOffsetAndCharsExceeded(std::string sectionName, size_t numStrings, size_t numCharacters, size_t sectionSpace)
    : StringException("Error serializing " + sectionName + " section: " + std::to_string(numStrings) + " strings worth of offsets and " + std::to_string(numCharacters)
        + " characters given, which cannot together fit in the " + std::to_string(sectionSpace) + " bytes of section space using selected compression.")
{

}

Section TypeSection::GetDefault()
{
    Chk::TYPE data = { Chk::Type::RAWB };
    return Section((ChkSection*)new (std::nothrow) TypeSection(data));
}

TypeSection::TypeSection(Chk::TYPE & data) : StructSection<Chk::TYPE, true>(SectionName::TYPE, data)
{

}

void TypeSection::setType(Chk::Type type)
{
    data->scenarioType = type;
    setVirtual(false);
}

Section VerSection::GetDefault()
{
    Chk::VER data = { Chk::Version::StarCraft_Hybrid };
    return Section((ChkSection*)new (std::nothrow) VerSection(data));
}

VerSection::VerSection(Chk::VER & data) : StructSection<Chk::VER, false>(SectionName::VER, data)
{

}

bool VerSection::isOriginal()
{
    return data->version < Chk::Version::StarCraft_Hybrid;
}

bool VerSection::isHybrid()
{
    return data->version >= Chk::Version::StarCraft_Hybrid && data->version < Chk::Version::StarCraft_BroodWar;
}

bool VerSection::isExpansion()
{
    return data->version >= Chk::Version::StarCraft_Hybrid;
}

bool VerSection::isHybridOrAbove()
{
    return data->version >= Chk::Version::StarCraft_Hybrid;
}

Chk::Version VerSection::getVersion()
{
    return data->version;
}

void VerSection::setVersion(Chk::Version version)
{
    data->version = version;
}

Section IverSection::GetDefault()
{
    Chk::IVER data = { Chk::IVersion::Current };
    return Section((ChkSection*)new (std::nothrow) IverSection(data));
}

IverSection::IverSection(Chk::IVER & data) : StructSection<Chk::IVER, true>(SectionName::IVER, data)
{

}

Chk::IVersion IverSection::getVersion()
{
    return data->version;
}

void IverSection::setVersion(Chk::IVersion version)
{
    data->version = version;
}

Section Ive2Section::GetDefault()
{
    Chk::IVE2 data = { Chk::I2Version::StarCraft_1_04 };
    return Section((ChkSection*)new (std::nothrow) Ive2Section(data));
}

Ive2Section::Ive2Section(Chk::IVE2 & data) : StructSection<Chk::IVE2, true>(SectionName::IVE2, data)
{

}

Chk::I2Version Ive2Section::getVersion()
{
    return data->version;
}

void Ive2Section::setVersion(Chk::I2Version version)
{
    data->version = version;
}

Section VcodSection::GetDefault()
{
    Chk::VCOD data = {
        {
            0x77CA1934, 0x7168DC99, 0xC3BF600A, 0xA775E7A7, 0xA67D291F, 0xBB3AB0D7, 0xED2431CC, 0x0B134C17, 0xB7A22065, 0x6B18BD91, 0xDD5DC38D, 0x37D57AE2,
            0xD46459F6, 0x0F129A63, 0x462E5C43, 0x2AF874E3, 0x06376A08, 0x3BD6F637, 0x1663940E, 0xEC5C6745, 0xB7F77BD7, 0x9ED4FC1A, 0x8C3FFA73, 0x0FE1C02E,
            0x070974D1, 0xD764E395, 0x74681675, 0xDA4FA799, 0x1F1820D5, 0xBEA0E6E7, 0x1FE3B6A6, 0x70EF0CCA, 0x311AD531, 0x3524B84D, 0x7DC7F8E3, 0xDE581AE1,
            0x432705F4, 0x07DBACBA, 0x0ABE69DC, 0x49EC8FA8, 0x3F1658D7, 0x8AC1DBE5, 0x05C0CF41, 0x721CCA9D, 0xA55FB1A2, 0x9B7023C4, 0x14E10484, 0xDA907B80,
            0x0669DBFA, 0x400FF3A3, 0xD4CEF3BE, 0xD7CBC9E3, 0x3401405A, 0xF81468F2, 0x1AC58E38, 0x4B3DD6FE, 0xFA050553, 0x8E451034, 0xFE6991DD, 0xF0EEE0AF,
            0xDD7E48F3, 0x75DCAD9F, 0xE5AC7A62, 0x67621B31, 0x4D36CD20, 0x742198E0, 0x717909FB, 0x7FCD6736, 0x3CD65F77, 0xC6A6A2A2, 0x6ACEE31A, 0x6CA9CD4E,
            0x3B9DBA86, 0xFD76F4B5, 0xBCF044F8, 0x296EE92E, 0x6B2F2523, 0x4427AB08, 0x99CC127A, 0x75F2DCED, 0x7E383CC5, 0xC51B1CF7, 0x65942DD1, 0xDD48C906,
            0xAC2D32BE, 0x8132C9B5, 0x34D84A66, 0xDF153F35, 0xB6EBEEB2, 0x964DF604, 0x9C944235, 0x61D38A62, 0x6F7BA852, 0xF4FC61DC, 0xFE2D146C, 0x0AA4EA99,
            0x13FED9E8, 0x594448D0, 0xE3F36680, 0x198DD934, 0xFE63D716, 0x3A7E1830, 0xB10F8D9B, 0x8CF5F012, 0xDB58780A, 0x8CB8633E, 0x8EF3AA3A, 0x2E1A8A37,
            0xEFF9315C, 0x7EE36DE3, 0x133EBD9B, 0xB9C044C6, 0x90DA3ABC, 0x74B0ADA4, 0x892757F8, 0x373FE647, 0x5A7942E4, 0xEE8D43DF, 0xE8490AB4, 0x1A88C33C,
            0x766B0188, 0xA3FDC38A, 0x564E7A16, 0xBACB7FA7, 0xEC1C5E02, 0x76C9B9B0, 0x39B1821E, 0xC557C93E, 0x4C382419, 0xB8542F5D, 0x8E575D6F, 0x520AA130,
            0x5E71186D, 0x59C30613, 0x623EDC1F, 0xEBB5DADC, 0xF995911B, 0xDAD591A7, 0x6BCE5333, 0x017000F5, 0xE8EED87F, 0xCEF10AC0, 0xD3B6EB63, 0xA5CCEF78,
            0xA4BC5DAA, 0xD2F2AB96, 0x9AEAFF61, 0xA2ED6AA8, 0x61ED3EBD, 0x9282C139, 0xB1233616, 0xE524A0B0, 0xAAA79B05, 0x339B120D, 0xDA209283, 0xFCECB025,
            0x2338D024, 0x74F295FC, 0x19E57380, 0x447D5097, 0xDB449345, 0x691DADA2, 0xE7EE1444, 0xFF877F2C, 0xF1329E38, 0xDA29BC4D, 0xFE262742, 0xA92BD2C1,
            0x0E7A42F6, 0xD17CE8CB, 0x56EC5B0F, 0x3161B769, 0x25F96DB4, 0x6D793440, 0x0BA753FA, 0xCE82A4FA, 0x614945C3, 0x8F2C450D, 0xF7604928, 0x1EC97DF3,
            0xC189D00F, 0xD3F85226, 0x14358F4D, 0x0B5F9DBA, 0x004AA907, 0x2F2622F7, 0x1FFB673E, 0xC6119CA1, 0x665D4F69, 0x90153458, 0x4654E56C, 0xD6635FAF,
            0xDF950C8A, 0xAFE40DBD, 0x4C4040BF, 0x7151F6A3, 0xF826ED29, 0xD5222885, 0xFACFBEBF, 0x517FC528, 0x076306B8, 0x298FBDEC, 0x717E55FA, 0x6632401A,
            0x9DDED4E8, 0x93FC5ED4, 0x3BD53D7A, 0x802E75CD, 0x87744F0A, 0xEA8FCC1B, 0x7CDBA99A, 0xEFE55316, 0x6EC178AB, 0x5A8972A4, 0x50702C98, 0x1FDFA1FB,
            0x44D9B76B, 0x56828007, 0x83C0BFFD, 0x5BD0490E, 0x0E6A681E, 0x2F0BC29A, 0xE1A0438E, 0xB2F60C99, 0x5E1C7AE0, 0x45A0C82C, 0x88E90B3C, 0xC696B9AC,
            0x2A83AE74, 0x65FA13BB, 0xA61F4FEB, 0xE18A8AB0, 0xB9B8E981, 0x4E1555D5, 0x9BADF245, 0x7E35C23E, 0x722E925F, 0x23685BB6, 0x0E45C66E, 0xD4873BE9,
            0xE3C041F4, 0xBE4405A8, 0x138A0FE4, 0xF437C41A, 0xEF55405A, 0x4B1D799D, 0x9C3A794A, 0xCC378576, 0xB60F3D82, 0x7E93A660, 0xC4C25CBD, 0x907FC772,
            0x10961B4D, 0x68680513, 0xFF7BC035, 0x2A438546
        },
        {
            Chk::ValidationOpCodes::Add_Shifted_Sections, Chk::ValidationOpCodes::XOR_Sections_04       , Chk::ValidationOpCodes::XOR_Sections_05,
            Chk::ValidationOpCodes::ORs_And_Shifts      , Chk::ValidationOpCodes::Sub_Shifted_Sections  , Chk::ValidationOpCodes::Add_Shifted_Sections,
            Chk::ValidationOpCodes::XOR_Sections_05     , Chk::ValidationOpCodes::Sub_Shifted_Sections  , Chk::ValidationOpCodes::XOR_Shifted_Sections,
            Chk::ValidationOpCodes::XOR_Sections        , Chk::ValidationOpCodes::ORs_And_Reverse_Shifts, Chk::ValidationOpCodes::ORs_And_Reverse_Shifts,
            Chk::ValidationOpCodes::XOR_Sections_05     , Chk::ValidationOpCodes::XOR_Sections_04       , Chk::ValidationOpCodes::ORs_And_Shifts,
            Chk::ValidationOpCodes::XOR_Sections
        }
    };
    return Section((ChkSection*)new (std::nothrow) VcodSection(data));
}

VcodSection::VcodSection(Chk::VCOD & data) : StructSection<Chk::VCOD, false>(SectionName::VCOD, data)
{

}

bool VcodSection::isDefault()
{
    Section defaultVcodSection = VcodSection::GetDefault();
    VcodSection & defaultVcod = (VcodSection &)(*defaultVcodSection);
    return memcmp(data, defaultVcod.data, sizeof(Chk::VCOD)) == 0;
}

void VcodSection::setToDefault()
{
    Section defaultVcodSection = VcodSection::GetDefault();
    VcodSection & defaultVcod = (VcodSection &)(*defaultVcodSection);
    memcpy(data, defaultVcod.data, sizeof(Chk::VCOD));
}

Section IownSection::GetDefault()
{
    Chk::IOWN data = {
        Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen,
        Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen,
        Sc::Player::SlotOwner::Inactive, Sc::Player::SlotOwner::Inactive, Sc::Player::SlotOwner::Inactive, Sc::Player::SlotOwner::Inactive
    };
    return Section((ChkSection*)new (std::nothrow) IownSection(data));
}

IownSection::IownSection(Chk::IOWN & data) : StructSection<Chk::IOWN, true>(SectionName::IOWN, data)
{

}

Sc::Player::SlotOwner IownSection::getSlotOwner(size_t slotIndex)
{
    if ( slotIndex < Sc::Player::Total )
        return data->owner[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the IOWN section!");
}

void IownSection::setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner)
{
    if ( slotIndex < Sc::Player::Total )
        data->owner[slotIndex] = owner;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the IOWN section!");
}

Section OwnrSection::GetDefault()
{
    Chk::OWNR data = {
        Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen,
        Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen, Sc::Player::SlotOwner::GameOpen,
        Sc::Player::SlotOwner::Inactive, Sc::Player::SlotOwner::Inactive, Sc::Player::SlotOwner::Inactive, Sc::Player::SlotOwner::Inactive
    };
    return Section((ChkSection*)new (std::nothrow) OwnrSection(data));
}

OwnrSection::OwnrSection(Chk::OWNR & data) : StructSection<Chk::OWNR, false>(SectionName::OWNR, data)
{

}

Sc::Player::SlotOwner OwnrSection::getSlotOwner(size_t slotIndex)
{
    if ( slotIndex < Sc::Player::Total )
        return data->owner[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR section!");
}

void OwnrSection::setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner)
{
    if ( slotIndex < Sc::Player::Total )
        data->owner[slotIndex] = owner;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR section!");
}

Section EraSection::GetDefault(Sc::Terrain::Tileset tileset)
{
    Chk::ERA data = { tileset };
    return Section((ChkSection*)new (std::nothrow) EraSection(data));
}

EraSection::EraSection(Chk::ERA & data) : StructSection<Chk::ERA, false>(SectionName::ERA, data)
{

}

Sc::Terrain::Tileset EraSection::getTileset()
{
    return data->tileset;
}

void EraSection::setTileset(Sc::Terrain::Tileset tileset)
{
    data->tileset = tileset;
}

Section DimSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    Chk::DIM data = { tileWidth, tileHeight };
    return Section((ChkSection*)new (std::nothrow) DimSection(data));
}

DimSection::DimSection(Chk::DIM & data) : StructSection<Chk::DIM, false>(SectionName::DIM, data)
{

}

size_t DimSection::getTileWidth()
{
    return data->tileWidth;
}

size_t DimSection::getTileHeight()
{
    return data->tileHeight;
}

size_t DimSection::getPixelWidth()
{
    return data->tileWidth * Sc::Terrain::PixelsPerTile;
}

size_t DimSection::getPixelHeight()
{
    return data->tileHeight * Sc::Terrain::PixelsPerTile;
}

void DimSection::setTileWidth(u16 tileWidth)
{
    data->tileWidth = tileWidth;
}

void DimSection::setTileHeight(u16 tileHeight)
{
    data->tileHeight = tileHeight;
}

void DimSection::setDimensions(u16 tileWidth, u16 tileHeight)
{
    data->tileWidth = tileWidth;
    data->tileHeight = tileHeight;
}


Section SideSection::GetDefault()
{
    Chk::SIDE data = {
        Chk::Race::Terran  , Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran,
        Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran  , Chk::Race::Zerg,
        Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Neutral
    };
    return Section((ChkSection*)new (std::nothrow) SideSection(data));
}

SideSection::SideSection(Chk::SIDE & data) : StructSection<Chk::SIDE, false>(SectionName::SIDE, data)
{

}

Chk::Race SideSection::getPlayerRace(size_t playerIndex)
{
    if ( playerIndex < Sc::Player::Total )
        return data->playerRaces[playerIndex];
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}

void SideSection::setPlayerRace(size_t playerIndex, Chk::Race race)
{
    if ( playerIndex < Sc::Player::Total )
        data->playerRaces[playerIndex] = race;
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}


Section MtxmSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    Section newSection((ChkSection*)new (std::nothrow) MtxmSection());
    s64 numTiles = (s64)tileWidth*(s64)tileHeight;
    ((MtxmSection &)(*newSection)).rawData->add<u16>(0, numTiles);
    return newSection;
}

MtxmSection::MtxmSection() : DynamicSection<false>(SectionName::MTXM)
{

}

u16 MtxmSection::getTile(size_t tileIndex)
{
    if ( tileIndex < rawData->size() / 2 )
        return rawData->get<u16>(tileIndex * 2);
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
}

void MtxmSection::setTile(size_t tileIndex, u16 tileValue)
{
    if ( tileIndex < rawData->size() / 2 )
        rawData->replace(tileIndex * 2, tileValue);
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
}

void setMtxmOrTileDimensions(BufferPtr rawData, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    s64 oldLeft = 0, oldTop = 0, oldRight = oldTileWidth, oldBottom = oldTileHeight,
        newLeft = leftEdge, newTop = topEdge, newRight = (s64)leftEdge+(s64)newTileWidth, newBottom = (s64)topEdge+(s64)newTileHeight,
        currTileWidth = oldTileWidth, currTileHeight = oldTileHeight;

    // Remove tiles as neccessary in order: bottom, top, right, left
    if ( newBottom < oldBottom && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the bottom
    {
        s64 numRowsRemoved = oldBottom - newBottom;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        s64 numBytesRemoved = 2*numTilesRemoved;
        if ( numBytesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(rawData->size() - numBytesRemoved, numBytesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newTop > oldTop && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the top
    {
        s64 numRowsRemoved = newTop - oldTop;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        s64 numBytesRemoved = 2*numTilesRemoved;
        if ( numBytesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(0, numBytesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newRight < oldRight && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the right
    {
        s64 numColumnsRemoved = oldRight - newRight;
        if ( numColumnsRemoved >= currTileWidth )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            s64 numBytesPerRow = 2*currTileWidth;
            s64 numBytesRemovedPerRow = 2*numColumnsRemoved;
            s64 rowDataRemovalOffset = numBytesPerRow - numBytesRemovedPerRow;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesPerRow;
                s64 dataRemovalOffset = rowOffset+rowDataRemovalOffset;
                rawData->del(dataRemovalOffset, numBytesRemovedPerRow);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( newLeft > oldLeft && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the left
    {
        s64 numColumnsRemoved = newLeft - oldLeft;
        if ( numColumnsRemoved >= currTileHeight )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            s64 numBytesPerRow = 2*currTileWidth;
            s64 numBytesRemovedPerRow = 2*numColumnsRemoved;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesPerRow;
                rawData->del(rowOffset, numBytesRemovedPerRow);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( currTileWidth == 0 || currTileHeight == 0 )
    {
        s64 numTilesToAdd = currTileWidth*currTileHeight;
        s64 bytesToAdd = 2*numTilesToAdd;
        rawData->trimTo(bytesToAdd);
    }
    else // currTileWidth > 0 && currTileHeight > 0
    {
        // Add tiles as necessary in order: right, left, top, bottom

        if ( newRight > oldRight ) // Columns need to be added to the right
        {
            s64 numBytesPerRow = 2*currTileWidth;
            s64 numColumnsAdded = newRight - oldRight;
            s64 numBytesAddedPerRow = 2*numColumnsAdded;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesPerRow;
                s64 dataAdditionOffset = rowOffset + numBytesPerRow;
                rawData->insert<u8>(dataAdditionOffset, 0, numBytesAddedPerRow);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newLeft < oldLeft ) // Columns need to be added to the left
        {
            s64 numColumnsAdded = oldLeft - newLeft;
            s64 numBytesAddedPerRow = 2*numColumnsAdded;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numBytesAddedPerRow;
                rawData->insert<u8>(rowOffset, 0, numBytesAddedPerRow);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newTop < oldTop ) // Rows need to be added to the top
        {
            s64 numRowsAdded = oldTop - newTop;
            s64 numBytesAdded = 2*numRowsAdded;
            rawData->insert<u8>(0, numBytesAdded);
            currTileHeight += numRowsAdded;
        }

        if ( newBottom > oldBottom ) // Rows need to be added to the bottom
        {
            s64 numRowsAdded = newBottom - oldBottom;
            s64 numBytesAdded = 2*numRowsAdded;
            rawData->insert<u8>(0, numBytesAdded);
            currTileHeight += numRowsAdded;
        }
    }
}

void MtxmSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    setMtxmOrTileDimensions(rawData, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

u32 MtxmSection::getSize()
{
    return ChkSection::getSize();
}

std::ostream & MtxmSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    rawData->write(s, false);
    return s;
}


Section PuniSection::GetDefault()
{
    Section newSection((ChkSection*)new (std::nothrow) PuniSection());
    ((PuniSection &)(*newSection)).rawData->add<u8>(1, sizeof(Chk::PUNI));
    return newSection;
}

PuniSection::PuniSection() : StructSection<Chk::PUNI, false>(SectionName::PUNI)
{

}

bool PuniSection::isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUnitBuildable[(size_t)unitType][playerIndex] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the PUNI section!");
}

bool PuniSection::isUnitDefaultBuildable(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->defaultUnitBuildable[(size_t)unitType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the PUNI section!");
}

bool PuniSection::playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUnitUsesDefault[(size_t)unitType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable)
{
    Chk::Available unitBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUnitBuildable[(size_t)unitType][playerIndex] = unitBuildable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable)
{
    Chk::Available unitDefaultBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->defaultUnitBuildable[(size_t)unitType] = unitDefaultBuildable;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the PUNI section!");
}

void PuniSection::setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUnitUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUnitUsesDefault[(size_t)unitType][playerIndex] = playerUnitUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the PUNI section!");
}


Section UpgrSection::GetDefault()
{
    Chk::UPGR data = {
        {}, // playerMaxUpgradeLevel (all 0)
        {}, // playerStartUpgradeLevel (all 0)
        { // defaultMaxLevel
            3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,
            1, 1, 0, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,
            1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 0
        },
        {}, // defaultStartLevel (all 0)
        {}, // playerUpgradeUsesDefault (all Yes/1, set below)
    };
    memset(&data.playerUpgradeUsesDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalOriginalTypes*Sc::Player::Total);
    return Section((ChkSection*)new (std::nothrow) UpgrSection(data));
}

UpgrSection::UpgrSection(Chk::UPGR & data) : StructSection<Chk::UPGR, false>(SectionName::UPGR, data)
{

}

size_t UpgrSection::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return (size_t)data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->defaultMaxLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

size_t UpgrSection::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->defaultStartLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

bool UpgrSection::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, u8 maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex] = maxUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, u8 startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex] = startUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, u8 maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->defaultMaxLevel[(size_t)upgradeType] = maxUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, u8 startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->defaultStartLevel[(size_t)upgradeType] = startUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}

void UpgrSection::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUpgradeUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] = playerUpgradeUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the UPGR section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGR section!");
}


Section PtecSection::GetDefault()
{
    Chk::PTEC data = {
        {}, // techAvailableForPlayer (all No/0)
        {}, // techResearchedForPlayer (all No/0)
        {}, // techAvailableByDefault (all Yes/1, set below)
        { // techResearchedByDefault
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes
        },
        {} // playerUsesDefaultsForTech (all Yes/1, set below)
    };
    memset(&data.techAvailableByDefault, (int)Chk::Available::Yes, Sc::Tech::TotalOriginalTypes);
    memset(&data.playerUsesDefaultsForTech, (int)Chk::UseDefault::Yes, Sc::Tech::TotalOriginalTypes*Sc::Player::Total);
    return Section((ChkSection*)new (std::nothrow) PtecSection(data));
}

PtecSection::PtecSection(Chk::PTEC & data) : StructSection<Chk::PTEC, false>(SectionName::PTEC, data)
{

}

bool PtecSection::techAvailable(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techAvailableForPlayer[(size_t)techType][playerIndex] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::techResearched(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techResearchedForPlayer[(size_t)techType][playerIndex] != Chk::Researched::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::techDefaultAvailable(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->techAvailableByDefault[(size_t)techType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::techDefaultResearched(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->techResearchedByDefault[(size_t)techType] != Chk::Researched::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

bool PtecSection::playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techAvailableForPlayer[(size_t)techType][playerIndex] = techAvailable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techResearchedForPlayer[(size_t)techType][playerIndex] = techResearched;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEC section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setDefaultTechAvailable(Sc::Tech::Type techType, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->techAvailableByDefault[(size_t)techType] = techAvailable;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setDefaultTechResearched(Sc::Tech::Type techType, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->techResearchedByDefault[(size_t)techType] = techResearched;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}

void PtecSection::setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] = playerUsesDefault;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEC section!");
}


Section UnitSection::GetDefault()
{
    return Section((ChkSection*)new (std::nothrow) UnitSection());
}

UnitSection::UnitSection() : DynamicSection<false>(SectionName::UNIT)
{

}

size_t UnitSection::numUnits()
{
    return units.size();
}

std::shared_ptr<Chk::Unit> UnitSection::getUnit(size_t unitIndex)
{
    return units[unitIndex];
}

size_t UnitSection::addUnit(std::shared_ptr<Chk::Unit> unit)
{
    units.push_back(unit);
    return units.size()-1;
}

void UnitSection::insertUnit(size_t unitIndex, std::shared_ptr<Chk::Unit> unit)
{
    if ( unitIndex < units.size() )
    {
        auto position = std::next(units.begin(), unitIndex);
        units.insert(position, unit);
    }
    else if ( unitIndex == units.size() )
        units.push_back(unit);
}

void UnitSection::deleteUnit(size_t unitIndex)
{
    if ( unitIndex < units.size() )
    {
        auto unit = std::next(units.begin(), unitIndex);
        units.erase(unit);
    }
}

void UnitSection::moveUnit(size_t unitIndexFrom, size_t unitIndexTo)
{
    size_t unitIndexMin = std::min(unitIndexFrom, unitIndexTo);
    size_t unitIndexMax = std::max(unitIndexFrom, unitIndexTo);
    if ( unitIndexMax < units.size() && unitIndexFrom != unitIndexTo )
    {
        if ( unitIndexMax-unitIndexMin == 1 && unitIndexMax < units.size() ) // Move up or down by 1 using swap
            units[unitIndexMin].swap(units[unitIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto unit = units[unitIndexFrom];
            auto toErase = std::next(units.begin(), unitIndexFrom);
            units.erase(toErase);
            auto insertPosition = std::next(units.begin(), unitIndexTo-1);
            units.insert(insertPosition, unit);
        }
    }
}

u32 UnitSection::getSize()
{
    return units.size() * sizeof(Chk::Unit);
}

std::ostream & UnitSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    for ( auto unit : units )
        s << (*unit);

    return s;
}

Section IsomSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    Section newSection((ChkSection*)new (std::nothrow) IsomSection());
    s64 numIsomEntries = (s64(tileWidth) / s64(2) + s64(1)) * (s64(tileHeight) + s64(1)) * s64(4);
    ((IsomSection &)(*newSection)).rawData->add<u16>(0, numIsomEntries);
    return newSection;
}

IsomSection::IsomSection() : DynamicSection<true>(SectionName::ISOM)
{

}

std::shared_ptr<Chk::IsomEntry> IsomSection::getIsomEntry(size_t isomIndex)
{
    size_t dataLocation = isomIndex * sizeof(Chk::IsomEntry);
    if ( dataLocation + sizeof(Chk::IsomEntry) <= rawData->size() )
        return isomEntries[isomIndex];
    else
        return nullptr;
}

void IsomSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    size_t oldNumIndices = isomEntries.size();
    size_t newNumIndices = (size_t)newTileWidth * (size_t)newTileHeight;
    if ( oldNumIndices < newNumIndices )
    {
        for ( size_t i=oldNumIndices; i<newNumIndices; i++ )
            isomEntries.push_back(std::shared_ptr<Chk::IsomEntry>(new Chk::IsomEntry()));
    }
    else if ( oldNumIndices > newNumIndices )
    {
        auto eraseStart = isomEntries.begin();
        std::advance(eraseStart, newNumIndices);
        isomEntries.erase(eraseStart, isomEntries.end());
    }
}

u32 IsomSection::getSize()
{
    return isVirtual() ? ChkSection::getSize() : isomEntries.size() * sizeof(Chk::IsomEntry);
}

std::ostream & IsomSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    if ( isVirtual() )
        rawData->write(s, false);
    else
    {
        for ( auto isomEntry : isomEntries )
            s << (*isomEntry);
    }
    return s;
}
Section TileSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    Section newSection((ChkSection*)new (std::nothrow) TileSection());
    s64 numTiles = (s64)tileWidth*(s64)tileHeight;
    ((TileSection &)(*newSection)).rawData->add<u16>(0, numTiles);
    return newSection;
}

TileSection::TileSection() : DynamicSection<true>(SectionName::TILE)
{

}

u16 TileSection::getTile(size_t tileIndex)
{
    return rawData->get<u16>((s64)tileIndex*(s64)2);
}

void TileSection::setTile(size_t tileIndex, u16 tileValue)
{
    rawData->replace<u16>((s64)tileIndex*(s64)2, tileValue);
}

void TileSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    setMtxmOrTileDimensions(rawData, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

u32 TileSection::getSize()
{
    return ChkSection::getSize();
}

std::ostream & TileSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return rawData->write(s, false);
}

Section Dd2Section::GetDefault()
{
    return Section((ChkSection*)new (std::nothrow) Dd2Section());
}

Dd2Section::Dd2Section() : DynamicSection<true>(SectionName::DD2)
{

}

size_t Dd2Section::numDoodads()
{
    return doodads.size();
}

std::shared_ptr<Chk::Doodad> Dd2Section::getDoodad(size_t doodadIndex)
{
    return doodads[doodadIndex];
}

size_t Dd2Section::addDoodad(std::shared_ptr<Chk::Doodad> doodad)
{
    doodads.push_back(doodad);
    return doodads.size()-1;
}

void Dd2Section::insertDoodad(size_t doodadIndex, std::shared_ptr<Chk::Doodad> doodad)
{
    if ( doodadIndex < doodads.size() )
    {
        auto position = std::next(doodads.begin(), doodadIndex);
        doodads.insert(position, doodad);
    }
    else if ( doodadIndex == doodads.size() )
        doodads.push_back(doodad);
}

void Dd2Section::deleteDoodad(size_t doodadIndex)
{
    if ( doodadIndex < doodads.size() )
    {
        auto doodad = std::next(doodads.begin(), doodadIndex);
        doodads.erase(doodad);
    }
}

void Dd2Section::moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo)
{
    size_t doodadIndexMin = std::min(doodadIndexFrom, doodadIndexTo);
    size_t doodadIndexMax = std::max(doodadIndexFrom, doodadIndexTo);
    if ( doodadIndexMax < doodads.size() && doodadIndexFrom != doodadIndexTo )
    {
        if ( doodadIndexMax-doodadIndexMin == 1 && doodadIndexMax < doodads.size() ) // Move up or down by 1 using swap
            doodads[doodadIndexMin].swap(doodads[doodadIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto doodad = doodads[doodadIndexFrom];
            auto toErase = std::next(doodads.begin(), doodadIndexFrom);
            doodads.erase(toErase);
            auto insertPosition = std::next(doodads.begin(), doodadIndexTo-1);
            doodads.insert(insertPosition, doodad);
        }
    }
}

u32 Dd2Section::getSize()
{
    return doodads.size() * sizeof(Chk::Doodad);
}

std::ostream & Dd2Section::writeData(std::ostream &s, u32 sizeInBytes)
{
    if ( isVirtual() )
        rawData->write(s, false);
    else
    {
        for ( auto doodad : doodads )
            s << (*doodad);
    }
    return s;
}

Section Thg2Section::GetDefault()
{
    return Section((ChkSection*)new (std::nothrow) Thg2Section());
}

Thg2Section::Thg2Section() : DynamicSection<false>(SectionName::THG2)
{

}

size_t Thg2Section::numSprites()
{
    return sprites.size();
}

std::shared_ptr<Chk::Sprite> Thg2Section::getSprite(size_t spriteIndex)
{
    return sprites[spriteIndex];
}

size_t Thg2Section::addSprite(std::shared_ptr<Chk::Sprite> sprite)
{
    sprites.push_back(sprite);
    return sprites.size()-1;
}

void Thg2Section::insertSprite(size_t spriteIndex, std::shared_ptr<Chk::Sprite> sprite)
{
    if ( spriteIndex < sprites.size() )
    {
        auto position = std::next(sprites.begin(), spriteIndex);
        sprites.insert(position, sprite);
    }
    else if ( spriteIndex == sprites.size() )
        sprites.push_back(sprite);
}

void Thg2Section::deleteSprite(size_t spriteIndex)
{
    if ( spriteIndex < sprites.size() )
    {
        auto sprite = std::next(sprites.begin(), spriteIndex);
        sprites.erase(sprite);
    }
}

void Thg2Section::moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo)
{
    size_t spriteIndexMin = std::min(spriteIndexFrom, spriteIndexTo);
    size_t spriteIndexMax = std::max(spriteIndexFrom, spriteIndexTo);
    if ( spriteIndexMax < sprites.size() && spriteIndexFrom != spriteIndexTo )
    {
        if ( spriteIndexMax-spriteIndexMin == 1 && spriteIndexMax < sprites.size() ) // Move up or down by 1 using swap
            sprites[spriteIndexMin].swap(sprites[spriteIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto sprite = sprites[spriteIndexFrom];
            auto toErase = std::next(sprites.begin(), spriteIndexFrom);
            sprites.erase(toErase);
            auto insertPosition = std::next(sprites.begin(), spriteIndexTo-1);
            sprites.insert(insertPosition, sprite);
        }
    }
}

u32 Thg2Section::getSize()
{
    return sprites.size() * sizeof(Chk::Sprite);
}

std::ostream & Thg2Section::writeData(std::ostream &s, u32 sizeInBytes)
{
    for ( auto sprite : sprites )
        s << (*sprite);

    return s;
}

Section MaskSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    Section newSection((ChkSection*)new (std::nothrow) MaskSection());
    s64 numTiles = (s64)tileWidth*(s64)tileHeight;
    ((MaskSection &)(*newSection)).rawData->add<u8>(255, numTiles);
    return newSection;
}

MaskSection::MaskSection() : DynamicSection<true>(SectionName::MASK)
{

}

u8 MaskSection::getFog(size_t tileIndex)
{
    return rawData->get<u8>((s64)tileIndex);
}

void MaskSection::setFog(size_t tileIndex, u8 fogOfWarPlayers)
{
    rawData->replace<u16>((s64)tileIndex, fogOfWarPlayers);
}

void MaskSection::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    s64 oldLeft = 0, oldTop = 0, oldRight = oldTileWidth, oldBottom = oldTileHeight,
        newLeft = leftEdge, newTop = topEdge, newRight = (s64)leftEdge+(s64)newTileWidth, newBottom = (s64)topEdge+(s64)newTileHeight,
        currTileWidth = oldTileWidth, currTileHeight = oldTileHeight;

    // Remove tiles as neccessary in order: bottom, top, right, left
    if ( newBottom < oldBottom && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the bottom
    {
        s64 numRowsRemoved = oldBottom - newBottom;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(rawData->size() - numTilesRemoved, numTilesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newTop > oldTop && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the top
    {
        s64 numRowsRemoved = newTop - oldTop;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= rawData->size() )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            rawData->del(0, numTilesRemoved);
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newRight < oldRight && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the right
    {
        s64 numColumnsRemoved = oldRight - newRight;
        if ( numColumnsRemoved >= currTileWidth )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            s64 rowDataRemovalOffset = currTileWidth - numColumnsRemoved;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                s64 dataRemovalOffset = rowOffset+rowDataRemovalOffset;
                rawData->del(dataRemovalOffset, numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( newLeft > oldLeft && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the left
    {
        s64 numColumnsRemoved = newLeft - oldLeft;
        if ( numColumnsRemoved >= currTileHeight )
        {
            rawData->trimTo(0);
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                rawData->del(rowOffset, numColumnsRemoved);
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( currTileWidth == 0 || currTileHeight == 0 )
    {
        s64 numTilesToAdd = currTileWidth*currTileHeight;
        rawData->trimTo(numTilesToAdd);
    }
    else // currTileWidth > 0 && currTileHeight > 0
    {
        // Add tiles as necessary in order: right, left, top, bottom

        if ( newRight > oldRight ) // Columns need to be added to the right
        {
            s64 numColumnsAdded = newRight - oldRight;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                s64 dataAdditionOffset = rowOffset + currTileWidth;
                rawData->insert<u8>(dataAdditionOffset, 0, numColumnsAdded);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newLeft < oldLeft ) // Columns need to be added to the left
        {
            s64 numColumnsAdded = oldLeft - newLeft;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numColumnsAdded;
                rawData->insert<u8>(rowOffset, 0, numColumnsAdded);
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newTop < oldTop ) // Rows need to be added to the top
        {
            s64 numRowsAdded = oldTop - newTop;
            rawData->insert<u8>(0, numRowsAdded);
            currTileHeight += numRowsAdded;
        }

        if ( newBottom > oldBottom ) // Rows need to be added to the bottom
        {
            s64 numRowsAdded = newBottom - oldBottom;
            rawData->insert<u8>(0, numRowsAdded);
            currTileHeight += numRowsAdded;
        }
    }
}

u32 MaskSection::getSize()
{
    return ChkSection::getSize();
}

std::ostream & MaskSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return rawData->write(s, false);
}

Section StrSection::GetDefault()
{
    Section newSection((ChkSection*)new (std::nothrow) StrSection());
    BufferPtr rawData = ((StrSection *)newSection.get())->rawData;

    const std::vector<std::string> defaultStrings = {
        /* StringId: 1 */ "Untitled Scenario",
        /* StringId: 2 */ "Destroy all enemy buildings.",
        /* StringId: 3 */ "Anywhere",
        /* StringId: 4 */ "Force 1",
        /* StringId: 5 */ "Force 2",
        /* StringId: 6 */ "Force 3",
        /* StringId: 7 */ "Force 4"
    };

    u16 stringCapacity = 1024;
    u16 characterDataStart = 2 + 2*stringCapacity; // Calculate where character data begins (2 bytes for stringCapacity, then 2*stringCapacity for offsets)
    u16 stringStart = characterDataStart + 1; // Move past initial NUL terminator

    for ( const std::string &string : defaultStrings )
    {
        rawData->add<u16>(stringStart); // Add offset for this string
        stringStart += string.size() + 1; // Advance the length of the string plus 1 for the NUL terminator
    }
    u16 numBlankStrings = stringCapacity - defaultStrings.size(); // Find the amount of strings that will have unused offsets
    rawData->add<u16>(characterDataStart, numBlankStrings); // Add all the offsets for the unused strings, pointing to the initial NUL terminator

    rawData->add<char>('\0'); // Add the initial NUL terminator
    for ( const std::string &string : defaultStrings )
        rawData->addStr(string.c_str(), string.size()+1); // Add the string plus the NUL terminator

    return newSection;
}

StrSection::StrSection() : DynamicSection<false>(SectionName::STR)
{

}

bool StrSection::isSynced()
{
    return synced;
}

void StrSection::flagUnsynced()
{
    synced = false;
}

void StrSection::syncFromBuffer(StrSynchronizerPtr strSynchronizer)
{
    synced = false;
    strSynchronizer->synchronzieFromStrBuffer(*rawData);
}

void StrSection::syncToBuffer(StrSynchronizerPtr strSynchronizer)
{
    strSynchronizer->synchronizeToStrBuffer(*rawData);
    synced = true;
}

u32 StrSection::getSize()
{
    return synced ? this->rawData->size() : 0;
}

std::ostream & StrSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return synced ? rawData->write(s, false) : s;
}

Section UprpSection::GetDefault()
{
    Chk::UPRP data = {};
    return Section((ChkSection*)new (std::nothrow) UprpSection(data));
}

UprpSection::UprpSection(Chk::UPRP & data) : StructSection<Chk::UPRP, false>(SectionName::UPRP, data)
{

}

Chk::Cuwp UprpSection::getCuwp(size_t cuwpIndex)
{
    return data->createUnitProperties[cuwpIndex];
}

void UprpSection::setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        memcpy(&data->createUnitProperties[cuwpIndex], &cuwp, sizeof(Chk::Cuwp));
}

size_t UprpSection::findCuwp(const Chk::Cuwp &cuwp)
{
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( memcmp(&cuwp, &data->createUnitProperties[i], sizeof(Chk::Cuwp)) == 0 )
            return i;
    }
    return Sc::Unit::MaxCuwps;
}

Section UpusSection::GetDefault()
{
    Chk::UPUS data = {};
    return Section((ChkSection*)new (std::nothrow) UpusSection(data));
}

UpusSection::UpusSection(Chk::UPUS & data) : StructSection<Chk::UPUS, true>(SectionName::UPUS, data)
{

}

bool UpusSection::cuwpUsed(size_t cuwpIndex)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        return data->cuwpUsed[cuwpIndex] == Chk::CuwpUsed::No ? false : true;
    else
        return false;
}

void UpusSection::setCuwpUsed(size_t cuwpIndex, bool cuwpUsed)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        data->cuwpUsed[cuwpIndex] = cuwpUsed ? Chk::CuwpUsed::Yes : Chk::CuwpUsed::No;
}

size_t UpusSection::getNextUnusedCuwpIndex()
{
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( data->cuwpUsed[i] == Chk::CuwpUsed::No )
            return i;
    }
    return Sc::Unit::MaxCuwps;
}

Section MrgnSection::GetDefault(u16 tileWidth, u16 tileHeight)
{
    Section newSection((ChkSection*)new (std::nothrow) MrgnSection());
    MrgnSection* mrgnSection = (MrgnSection *)newSection.get();
    Chk::Location unusedLocation;
    Chk::Location anywhereLocation;
    anywhereLocation.right = (s64)tileWidth*(s64)32;
    anywhereLocation.bottom = (s64)tileHeight*(s64)32;
    anywhereLocation.stringId = 3;
    mrgnSection->rawData->add(unusedLocation, 63);
    mrgnSection->rawData->add(anywhereLocation, 1);
    return newSection;
}

MrgnSection::MrgnSection() : DynamicSection<false>(SectionName::MRGN)
{

}

size_t MrgnSection::numLocations()
{
    return locations.size();
}

void MrgnSection::sizeToScOriginal()
{
    size_t numLocations = locations.size();
    if ( numLocations > Chk::TotalOriginalLocations )
    {
        auto firstErased = locations.begin();
        std::advance(firstErased, Chk::TotalOriginalLocations);
        locations.erase(firstErased, locations.end());
    }
    else if ( numLocations < Chk::TotalOriginalLocations )
    {
        for ( size_t i=numLocations; i<Chk::TotalOriginalLocations; i++ )
            locations.push_back(std::shared_ptr<Chk::Location>(new Chk::Location()));
    }
}

void MrgnSection::sizeToScHybridOrExpansion()
{
    size_t numLocations = locations.size();
    if ( numLocations > Chk::TotalLocations )
    {
        auto firstErased = locations.begin();
        std::advance(firstErased, Chk::TotalLocations);
        locations.erase(firstErased, locations.end());
    }
    else if ( numLocations < Chk::TotalLocations )
    {
        for ( size_t i=numLocations; i<Chk::TotalLocations; i++ )
            locations.push_back(std::shared_ptr<Chk::Location>(new Chk::Location()));
    }
}

std::shared_ptr<Chk::Location> MrgnSection::getLocation(size_t locationIndex)
{
    return locations[locationIndex];
}

size_t MrgnSection::addLocation(std::shared_ptr<Chk::Location> location)
{
    locations.push_back(location);
    return locations.size()-1;
}

void MrgnSection::insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location)
{
    if ( locationIndex < locations.size() )
    {
        auto position = std::next(locations.begin(), locationIndex);
        locations.insert(position, location);
    }
    else if ( locationIndex == locations.size() )
        locations.push_back(location);
}

void MrgnSection::deleteLocation(size_t locationIndex)
{
    if ( locationIndex < locations.size() )
    {
        auto location = std::next(locations.begin(), locationIndex);
        locations.erase(location);
    }
}

void MrgnSection::moveLocation(size_t locationIndexFrom, size_t locationIndexTo)
{
    size_t locationIndexMin = std::min(locationIndexFrom, locationIndexTo);
    size_t locationIndexMax = std::max(locationIndexFrom, locationIndexTo);
    if ( locationIndexMax < locations.size() && locationIndexFrom != locationIndexTo )
    {
        if ( locationIndexMax-locationIndexMin == 1 && locationIndexMax < locations.size() ) // Move up or down by 1 using swap
            locations[locationIndexMin].swap(locations[locationIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto location = locations[locationIndexFrom];
            auto toErase = std::next(locations.begin(), locationIndexFrom);
            locations.erase(toErase);
            auto insertPosition = std::next(locations.begin(), locationIndexTo-1);
            locations.insert(insertPosition, location);
        }
    }
}

bool MrgnSection::stringUsed(size_t stringId)
{
    u16 u16StringId = (u16)stringId;
    for ( auto location : locations )
    {
        if ( location->stringId == u16StringId )
            return true;
    }
    return false;
}

void MrgnSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto location : locations )
    {
        if ( location->stringId > 0 )
            stringIdUsed[location->stringId] = true;
    }
}

void MrgnSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( auto location : locations )
    {
        auto found = stringIdRemappings.find(location->stringId);
        if ( found != stringIdRemappings.end() )
            location->stringId = found->second;
    }
}

void MrgnSection::deleteString(size_t stringId)
{
    for ( auto location : locations )
    {
        if ( location->stringId == stringId )
            location->stringId = 0;
    }
}

u32 MrgnSection::getSize()
{
    return sizeof(Chk::Location) * locations.size();
}

std::ostream & MrgnSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    size_t actualDataSize = sizeof(Chk::Location) * locations.size();
    if ( actualDataSize != (size_t)sizeInBytes )
        throw new SectionSerializationSizeMismatch(SectionName::MRGN, sizeInBytes, actualDataSize);

    for ( auto location : locations )
        s << (*location);

    return s;
}

Section TrigSection::GetDefault()
{
    return Section((ChkSection*)new (std::nothrow) TrigSection());
}

TrigSection::TrigSection() : DynamicSection<false>(SectionName::TRIG)
{

}

void TrigSection::ReplaceData(buffer & data)
{
    rawData->takeAllData(data);
}

bool TrigSection::AddTrigger(Chk::Trigger* trigger)
{
    return rawData->add<Chk::Trigger>(*trigger);
}

std::shared_ptr<Chk::Trigger> TrigSection::getTrigger(u32 trigNum)
{
    std::shared_ptr<Chk::Trigger> trigger(new (std::nothrow) Chk::Trigger());
    if ( trigger != nullptr && rawData->get(*trigger, trigNum*sizeof(Chk::Trigger)) )
        return trigger;
    else
        return nullptr;
}

size_t TrigSection::numTriggers()
{
    return triggers.size();
}

std::shared_ptr<Chk::Trigger> TrigSection::getTrigger(size_t triggerIndex)
{
    return triggers[triggerIndex];
}

size_t TrigSection::addTrigger(std::shared_ptr<Chk::Trigger> trigger)
{
    triggers.push_back(trigger);
    return triggers.size()-1;
}

void TrigSection::insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger)
{
    if ( triggerIndex < triggers.size() )
    {
        auto position = std::next(triggers.begin(), triggerIndex);
        triggers.insert(position, trigger);
    }
    else if ( triggerIndex == triggers.size() )
        triggers.push_back(trigger);
}

void TrigSection::deleteTrigger(size_t triggerIndex)
{
    if ( triggerIndex < triggers.size() )
    {
        auto trigger = std::next(triggers.begin(), triggerIndex);
        triggers.erase(trigger);
    }
}

void TrigSection::moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo)
{
    size_t triggerIndexMin = std::min(triggerIndexFrom, triggerIndexTo);
    size_t triggerIndexMax = std::max(triggerIndexFrom, triggerIndexTo);
    if ( triggerIndexMax < triggers.size() && triggerIndexFrom != triggerIndexTo )
    {
        if ( triggerIndexMax-triggerIndexMin == 1 && triggerIndexMax < triggers.size() ) // Move up or down by 1 using swap
            triggers[triggerIndexMin].swap(triggers[triggerIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto trigger = triggers[triggerIndexFrom];
            auto toErase = std::next(triggers.begin(), triggerIndexFrom);
            triggers.erase(toErase);
            auto insertPosition = std::next(triggers.begin(), triggerIndexTo-1);
            triggers.insert(insertPosition, trigger);
        }
    }
}

bool TrigSection::stringUsed(size_t stringId)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->stringUsed(stringId) )
            return true;
    }
    return false;
}

bool TrigSection::gameStringUsed(size_t stringId)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->gameStringUsed(stringId) )
            return true;
    }
    return false;
}

bool TrigSection::commentStringUsed(size_t stringId)
{
    for ( auto trigger : triggers )
    {
        if ( trigger->commentStringUsed(stringId) )
            return true;
    }
    return false;
}

void TrigSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedStrings(stringIdUsed);
}

void TrigSection::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedGameStrings(stringIdUsed);
}

void TrigSection::markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto trigger : triggers )
        trigger->markUsedCommentStrings(stringIdUsed);
}

void TrigSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( auto trigger : triggers )
        trigger->remapStringIds(stringIdRemappings);
}

void TrigSection::deleteString(size_t stringId)
{
    for ( auto trigger : triggers )
        trigger->deleteString(stringId);
}

u32 TrigSection::getSize()
{
    return sizeof(Chk::Trigger) * triggers.size();
}

std::ostream & TrigSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    size_t actualDataSize = sizeof(Chk::Location) * triggers.size();
    if ( actualDataSize != (size_t)sizeInBytes )
        throw new SectionSerializationSizeMismatch(SectionName::TRIG, sizeInBytes, actualDataSize);

    for ( auto trigger : triggers )
        s << (*trigger);
}

Section MbrfSection::GetDefault()
{
    return Section((ChkSection*)new (std::nothrow) MbrfSection());
}

MbrfSection::MbrfSection() : DynamicSection<false>(SectionName::MBRF)
{

}

size_t MbrfSection::numBriefingTriggers()
{
    return briefingTriggers.size();
}

std::shared_ptr<Chk::Trigger> MbrfSection::getBriefingTrigger(size_t briefingTriggerIndex)
{
    return briefingTriggers[briefingTriggerIndex];
}

size_t MbrfSection::addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    briefingTriggers.push_back(briefingTrigger);
    return briefingTriggers.size()-1;
}

void MbrfSection::insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    if ( briefingTriggerIndex < briefingTriggers.size() )
    {
        auto position = std::next(briefingTriggers.begin(), briefingTriggerIndex);
        briefingTriggers.insert(position, briefingTrigger);
    }
    else if ( briefingTriggerIndex == briefingTriggers.size() )
        briefingTriggers.push_back(briefingTrigger);
}

void MbrfSection::deleteBriefingTrigger(size_t briefingTriggerIndex)
{
    if ( briefingTriggerIndex < briefingTriggers.size() )
    {
        auto briefingTrigger = std::next(briefingTriggers.begin(), briefingTriggerIndex);
        briefingTriggers.erase(briefingTrigger);
    }
}

void MbrfSection::moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo)
{
    size_t briefingTriggerIndexMin = std::min(briefingTriggerIndexFrom, briefingTriggerIndexTo);
    size_t briefingTriggerIndexMax = std::max(briefingTriggerIndexFrom, briefingTriggerIndexTo);
    if ( briefingTriggerIndexMax < briefingTriggers.size() && briefingTriggerIndexFrom != briefingTriggerIndexTo )
    {
        if ( briefingTriggerIndexMax-briefingTriggerIndexMin == 1 && briefingTriggerIndexMax < briefingTriggers.size() ) // Move up or down by 1 using swap
            briefingTriggers[briefingTriggerIndexMin].swap(briefingTriggers[briefingTriggerIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto briefingTrigger = briefingTriggers[briefingTriggerIndexFrom];
            auto toErase = std::next(briefingTriggers.begin(), briefingTriggerIndexFrom);
            briefingTriggers.erase(toErase);
            auto insertPosition = std::next(briefingTriggers.begin(), briefingTriggerIndexTo-1);
            briefingTriggers.insert(insertPosition, briefingTrigger);
        }
    }
}

bool MbrfSection::stringUsed(size_t stringId)
{
    for ( auto briefingTrigger : briefingTriggers )
    {
        if ( briefingTrigger->briefingStringUsed(stringId) )
            return true;
    }
    return false;
}

void MbrfSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->markUsedStrings(stringIdUsed);
}

void MbrfSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->remapBriefingStringIds(stringIdRemappings);
}

void MbrfSection::deleteString(size_t stringId)
{
    for ( auto briefingTrigger : briefingTriggers )
        briefingTrigger->deleteString(stringId);
}

u32 MbrfSection::getSize()
{
    return sizeof(Chk::Trigger) * briefingTriggers.size();
}

std::ostream & MbrfSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    for ( auto briefingTrigger : briefingTriggers )
        s << (*briefingTrigger);
}

Section SprpSection::GetDefault()
{
    constexpr u16 scenarioNameString = 1;
    constexpr u16 scenarioDescriptionString = 2;
    Chk::SPRP data = { scenarioNameString, scenarioDescriptionString };
    return Section((ChkSection*)new (std::nothrow) SprpSection(data));
}

SprpSection::SprpSection(Chk::SPRP & data) : StructSection<Chk::SPRP, false>(SectionName::SPRP, data)
{

}

size_t SprpSection::getScenarioNameStringId()
{
    return data->scenarioNameStringId;
}

size_t SprpSection::getScenarioDescriptionStringId()
{
    return data->scenarioDescriptionStringId;
}

void SprpSection::setScenarioNameStringId(u16 scenarioNameStringId)
{
    data->scenarioNameStringId = scenarioNameStringId;
}

void SprpSection::setScenarioDescriptionStringId(u16 scenarioDescriptionStringId)
{
    data->scenarioDescriptionStringId = scenarioDescriptionStringId;
}

bool SprpSection::stringUsed(size_t stringId)
{
    return data->scenarioNameStringId == (u16)stringId || data->scenarioDescriptionStringId == (u16)stringId;
}

void SprpSection::deleteString(size_t stringId)
{
    if ( data->scenarioNameStringId == stringId )
        data->scenarioNameStringId = 0;

    if ( data->scenarioDescriptionStringId == stringId )
        data->scenarioDescriptionStringId = 0;
}

void SprpSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    auto scenarioNameRemapping = stringIdRemappings.find(data->scenarioNameStringId);
    auto scenarioDescriptionRemapping = stringIdRemappings.find(data->scenarioDescriptionStringId);

    if ( scenarioNameRemapping != stringIdRemappings.end() )
        data->scenarioNameStringId = scenarioNameRemapping->second;

    if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
        data->scenarioDescriptionStringId = scenarioDescriptionRemapping->second;
}

void SprpSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( data->scenarioNameStringId > 0 )
        stringIdUsed[data->scenarioNameStringId] = true;

    if ( data->scenarioDescriptionStringId > 0 )
        stringIdUsed[data->scenarioDescriptionStringId] = true;
}


Section ForcSection::GetDefault()
{
    constexpr u16 Force1String = 4;
    constexpr u16 Force2String = 5;
    constexpr u16 Force3String = 6;
    constexpr u16 Force4String = 7;

    Chk::FORC data = {
        { // playerForce
            Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1,
            Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1
        },
        { Force1String, Force2String, Force3String, Force4String }, // forceString
        { (u8)Chk::ForceFlags::All, (u8)Chk::ForceFlags::All, (u8)Chk::ForceFlags::All, (u8)Chk::ForceFlags::All } // flags
    };
    return Section((ChkSection*)new (std::nothrow) ForcSection(data));
}

ForcSection::ForcSection(Chk::FORC & data) : StructSection<Chk::FORC, false>(SectionName::FORC, data)
{

}

Chk::Force ForcSection::getPlayerForce(Sc::Player::SlotOwner slotOwner)
{
    if ( (size_t)slotOwner < Sc::Player::TotalSlots )
        return data->playerForce[(size_t)slotOwner];
}

size_t ForcSection::getForceStringId(Chk::Force force)
{
    if ( (size_t)force < Chk::TotalForces )
        return data->forceString[(size_t)force];
}

u8 ForcSection::getForceFlags(Chk::Force force)
{
    if ( (size_t)force < Chk::TotalForces )
        return data->flags[(size_t)force];
}

void ForcSection::setPlayerForce(Sc::Player::SlotOwner slotOwner, Chk::Force force)
{
    if ( (size_t)slotOwner < Sc::Player::TotalSlots )
        data->playerForce[(size_t)slotOwner] = force;
}

void ForcSection::setForceStringId(Chk::Force force, u16 forceStringId)
{
    if ( (size_t)force < Chk::TotalForces )
        data->forceString[(size_t)force] = forceStringId;
}

void ForcSection::setForceFlags(Chk::Force force, u8 forceFlags)
{
    if ( (size_t)force < Chk::TotalForces )
        data->flags[(size_t)force] = forceFlags;
}

bool ForcSection::stringUsed(size_t stringId)
{
    return data->forceString[0] == (u16)stringId || data->forceString[1] == (u16)stringId || data->forceString[2] == (u16)stringId || data->forceString[3] == (u16)stringId;
}

void ForcSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceString[i] > 0 )
            stringIdUsed[data->forceString[i]] = true;
    }
}

void ForcSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    auto forceOneRemapping = stringIdRemappings.find(data->forceString[0]);
    auto forceTwoRemapping = stringIdRemappings.find(data->forceString[1]);
    auto forceThreeRemapping = stringIdRemappings.find(data->forceString[2]);
    auto forceFourRemapping = stringIdRemappings.find(data->forceString[3]);

    if ( forceOneRemapping != stringIdRemappings.end() )
        data->forceString[0] = forceOneRemapping->second;

    if ( forceTwoRemapping != stringIdRemappings.end() )
        data->forceString[1] = forceTwoRemapping->second;

    if ( forceThreeRemapping != stringIdRemappings.end() )
        data->forceString[2] = forceThreeRemapping->second;

    if ( forceFourRemapping != stringIdRemappings.end() )
        data->forceString[3] = forceFourRemapping->second;
}

void ForcSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceString[i] == stringId )
            data->forceString[i] = 0;
    }
}

Section WavSection::GetDefault()
{
    Chk::WAV data = {};
    return Section((ChkSection*)new (std::nothrow) WavSection(data));
}

WavSection::WavSection(Chk::WAV & data) : StructSection<Chk::WAV, true>(SectionName::WAV, data)
{

}

bool WavSection::stringIsSound(size_t stringId)
{
    u32 u32StringId = stringId;
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] == u32StringId )
            return true;
    }
    return false;
}

size_t WavSection::getSoundStringId(size_t soundIndex)
{
    if ( soundIndex < Chk::TotalSounds )
        return data->soundPathStringId[soundIndex];
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the WAV section!");
}

void WavSection::setSoundStringId(size_t soundIndex, size_t soundStringId)
{
    if ( soundIndex < Chk::TotalSounds )
        data->soundPathStringId[soundIndex] = soundStringId;
}

bool WavSection::stringUsed(size_t stringId)
{
    return stringIsSound(stringId);
}

void WavSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] > 0 && data->soundPathStringId[i] < Chk::MaxStrings )
            stringIdUsed[data->soundPathStringId[i]] = true;
    }
}

void WavSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        auto found = stringIdRemappings.find(data->soundPathStringId[i]);
        if ( found != stringIdRemappings.end() )
            data->soundPathStringId[i] = found->second;
    }
}

void WavSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPathStringId[i] == stringId )
            data->soundPathStringId[i] = 0;
    }
}


Section UnisSection::GetDefault()
{
    Chk::UNIS data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // hitpoints (all 0)
        {}, // shieldPoints (all 0)
        {}, // armorPoints (all 0)
        {}, // buildTime (all 0)
        {}, // mineralCost (all 0)
        {}, // gasCost (all 0)
        {}, // nameString (all 0)
        {}, // baseDamage (all 0)
        {}, // upgradeDamage (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Unit::TotalTypes);
    return Section((ChkSection*)new (std::nothrow) UnisSection(data));
}

UnisSection::UnisSection(Chk::UNIS & data) : StructSection<Chk::UNIS, false>(SectionName::UNIS, data)
{

}

bool UnisSection::unitUsesDefaultSettings(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->useDefault[(size_t)unitType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u32 UnisSection::getUnitHitpoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->hitpoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitShieldPoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->shieldPoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u8 UnisSection::getUnitArmorLevel(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->armorLevel[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitBuildTime(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->buildTime[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitMineralCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->mineralCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getUnitGasCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->gasCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

size_t UnisSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->nameStringId[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getWeaponBaseDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        return data->baseDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
}

u16 UnisSection::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        return data->upgradeDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->useDefault[(size_t)unitType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->hitpoints[(size_t)unitType] = hitpoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->shieldPoints[(size_t)unitType] = shieldPoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->armorLevel[(size_t)unitType] = armorLevel;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->buildTime[(size_t)unitType] = buildTime;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->mineralCost[(size_t)unitType] = mineralCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->gasCost[(size_t)unitType] = gasCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->nameStringId[(size_t)unitType] = nameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

void UnisSection::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        data->baseDamage[(size_t)weaponType] = baseDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
}

void UnisSection::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::TotalOriginal )
        data->upgradeDamage[(size_t)weaponType] = upgradeDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIS section!");
}

bool UnisSection::stringUsed(size_t stringId)
{
    u16 u16StringId = (u16)stringId;
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == u16StringId )
            return true;
    }
    return false;
}

void UnisSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] > 0 )
            stringIdUsed[data->nameStringId[i]] = true;
    }
}

void UnisSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            data->nameStringId[i] = found->second;
    }
}

void UnisSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == stringId )
            data->nameStringId[i] = 0;
    }
}


Section UpgsSection::GetDefault()
{
    Chk::UPGS data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // baseMineralCost (all 0)
        {}, // mineralCostFactor (all 0)
        {}, // baseGasCost (all 0)
        {}, // gasCostFactor (all 0)
        {}, // baseResearchTime (all 0)
        {} // researchTimeFactor (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalOriginalTypes);
    return Section((ChkSection*)new (std::nothrow) UpgsSection(data));
}

UpgsSection::UpgsSection(Chk::UPGS & data) : StructSection<Chk::UPGS, false>(SectionName::UPGS, data)
{

}

bool UpgsSection::upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->useDefault[(size_t)upgradeType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseMineralCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseMineralCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getMineralCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->mineralCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseGasCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseGasCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getGasCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->gasCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getBaseResearchTime(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->baseResearchTime[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

u16 UpgsSection::getResearchTimeFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        return data->researchTimeFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}


void UpgsSection::setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->useDefault[(size_t)upgradeType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseMineralCost[(size_t)upgradeType] = baseMineralCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->mineralCostFactor[(size_t)upgradeType] = mineralCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseGasCost[(size_t)upgradeType] = baseGasCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->gasCostFactor[(size_t)upgradeType] = gasCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->baseResearchTime[(size_t)upgradeType] = baseResearchTime;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}

void UpgsSection::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalOriginalTypes )
        data->researchTimeFactor[(size_t)upgradeType] = researchTimeFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGS section!");
}


Section TecsSection::GetDefault()
{
    Chk::TECS data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // mineralCost (all 0)
        {}, // gasCost (all 0)
        {}, // researchTime (all 0)
        {} // energyCost (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Tech::TotalOriginalTypes);
    return Section((ChkSection*)new (std::nothrow) TecsSection(data));
}

TecsSection::TecsSection(Chk::TECS & data) : StructSection<Chk::TECS, false>(SectionName::TECS, data)
{

}

bool TecsSection::techUsesDefaultSettings(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->useDefault[(size_t)techType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechMineralCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->mineralCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechGasCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->gasCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechResearchTime(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->researchTime[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

u16 TecsSection::getTechEnergyCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        return data->energyCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}


void TecsSection::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->useDefault[(size_t)techType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->mineralCost[(size_t)techType] = mineralCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechGasCost(Sc::Tech::Type techType, u16 gasCost)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->gasCost[(size_t)techType] = gasCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->researchTime[(size_t)techType] = researchTime;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}

void TecsSection::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost)
{
    if ( (size_t)techType < Sc::Tech::TotalOriginalTypes )
        data->energyCost[(size_t)techType] = energyCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECS section!");
}


Section SwnmSection::GetDefault()
{
    Chk::SWNM data = {};
    return Section((ChkSection*)new (std::nothrow) SwnmSection(data));
}

SwnmSection::SwnmSection(Chk::SWNM & data) : StructSection<Chk::SWNM, true>(SectionName::SWNM, data)
{

}

size_t SwnmSection::getSwitchNameStringId(size_t switchIndex)
{
    if ( switchIndex < Chk::TotalSwitches )
        return data->switchName[switchIndex];
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the SWNM section!");
}

void SwnmSection::setSwitchNameStringId(size_t switchIndex, size_t stringId)
{
    if ( switchIndex < Chk::TotalSwitches )
        data->switchName[switchIndex] = stringId;
}

bool SwnmSection::stringUsed(size_t stringId)
{
    u32 u32StringId = (u32)stringId;
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == u32StringId )
            return true;
    }
    return false;
}

void SwnmSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] > 0 && data->switchName[i] < Chk::MaxStrings )
            stringIdUsed[data->switchName[i]] = true;
    }
}

void SwnmSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        auto found = stringIdRemappings.find(data->switchName[i]);
        if ( found != stringIdRemappings.end() )
            data->switchName[i] = found->second;
    }
}

void SwnmSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == stringId )
            data->switchName[i] = 0;
    }
}


Section ColrSection::GetDefault()
{
    Chk::COLR data = {
        Chk::PlayerColor::Red   , Chk::PlayerColor::Blue , Chk::PlayerColor::Teal , Chk::PlayerColor::Purple,
        Chk::PlayerColor::Orange, Chk::PlayerColor::Brown, Chk::PlayerColor::White, Chk::PlayerColor::Yellow
    };
    return Section((ChkSection*)new (std::nothrow) ColrSection(data));
}

ColrSection::ColrSection(Chk::COLR & data) : StructSection<Chk::COLR, false>(SectionName::COLR, data)
{

}

Chk::PlayerColor ColrSection::getPlayerColor(Sc::Player::SlotOwner slotOwner)
{
    if ( (size_t)slotOwner < Sc::Player::TotalSlots )
        return data->playerColor[(size_t)slotOwner];
    else
        throw std::out_of_range(std::string("slotOwner: ") + std::to_string((u32)slotOwner) + " is out of range for the COLR section!");
}

void ColrSection::setPlayerColor(Sc::Player::SlotOwner slotOwner, Chk::PlayerColor color)
{
    if ( (size_t)slotOwner < Sc::Player::TotalSlots )
        data->playerColor[(size_t)slotOwner] = color;
}

Section PupxSection::GetDefault()
{
    Chk::PUPx data = {
        {}, // playerMaxUpgradeLevel (all 0)
        {}, // playerStartUpgradeLevel (all 0)
        {
            3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,
            1, 1, 0, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,
            1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 0, 0, 1,
            0, 1, 0, 1,    1, 1, 1, 0,    0, 0, 0, 0,    0
        }, // defaultMaxLevel
        {}, // defaultStartLevel (all 0)
        {} // playerUpgradeUsesDefault (all Yes/1, set below)
    };
    memset(&data.playerUpgradeUsesDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalTypes*Sc::Player::Total);
    return Section((ChkSection*)new (std::nothrow) PupxSection(data));
}

PupxSection::PupxSection(Chk::PUPx & data) : StructSection<Chk::PUPx, false>(SectionName::PUPx, data)
{

}

size_t PupxSection::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex];
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->defaultMaxLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

size_t PupxSection::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->defaultStartLevel[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

bool PupxSection::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerMaxUpgradeLevel[(size_t)upgradeType][playerIndex] = maxUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerStartUpgradeLevel[(size_t)upgradeType][playerIndex] != startUpgradeLevel;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->defaultMaxLevel[(size_t)upgradeType] = maxUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->defaultStartLevel[(size_t)upgradeType] = startUpgradeLevel;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}

void PupxSection::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->playerUpgradeUsesDefault[(size_t)upgradeType][playerIndex] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUPx section!");
    }
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the PUPx section!");
}


Section PtexSection::GetDefault()
{
    Chk::PTEx data = {
        {}, // techAvailableForPlayer (all No/0)
        {}, // techResearchedForPlayer (all No/0)
        {}, // techAvailableByDefault (all Yes/1, set below)
        { // techResearchedByDefault
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::Yes, Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No
        },
        {} // playerUsesDefaultsForTech (all Yes/1, set below)
    };
    memset(&data.techAvailableByDefault, (int)Chk::Available::Yes, Sc::Tech::TotalTypes);
    memset(&data.playerUsesDefaultsForTech, (int)Chk::UseDefault::Yes, Sc::Tech::TotalTypes*Sc::Player::Total);
    return Section((ChkSection*)new (std::nothrow) PtexSection(data));
}

PtexSection::PtexSection(Chk::PTEx & data) : StructSection<Chk::PTEx, false>(SectionName::PTEx, data)
{

}

bool PtexSection::techAvailable(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techAvailableForPlayer[(size_t)techType][playerIndex] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::techResearched(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->techResearchedForPlayer[(size_t)techType][playerIndex] != Chk::Researched::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::techDefaultAvailable(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->techAvailableByDefault[(size_t)techType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::techDefaultResearched(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->techResearchedByDefault[(size_t)techType] != Chk::Researched::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

bool PtexSection::playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techAvailableForPlayer[(size_t)techType][playerIndex] = techAvailable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            data->techResearchedForPlayer[(size_t)techType][playerIndex] = techResearched;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PTEx section!");
    }
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setDefaultTechAvailable(Sc::Tech::Type techType, bool available)
{
    Chk::Available techAvailable = available ? Chk::Available::Yes : Chk::Available::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->techAvailableByDefault[(size_t)techType] = techAvailable;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setDefaultTechResearched(Sc::Tech::Type techType, bool researched)
{
    Chk::Researched techResearched = researched ? Chk::Researched::Yes : Chk::Researched::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->techResearchedByDefault[(size_t)techType] = techResearched;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

void PtexSection::setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->playerUsesDefaultsForTech[(size_t)techType][playerIndex] = playerUsesDefault;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the PTEx section!");
}

Section UnixSection::GetDefault()
{
    Chk::UNIx data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // hitpoints (all 0)
        {}, // shieldPoints (all 0)
        {}, // armorPoints (all 0)
        {}, // buildTime (all 0)
        {}, // mineralCost (all 0)
        {}, // gasCost (all 0)
        {}, // nameString (all 0)
        {}, // baseDamage (all 0)
        {}, // upgradeDamage (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Unit::TotalTypes);
    return Section((ChkSection*)new (std::nothrow) UnixSection(data));
}

UnixSection::UnixSection(Chk::UNIx & data) : StructSection<Chk::UNIx, false>(SectionName::UNIx, data)
{

}

bool UnixSection::unitUsesDefaultSettings(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->useDefault[(size_t)unitType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIS section!");
}

u32 UnixSection::getUnitHitpoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->hitpoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitShieldPoints(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->shieldPoints[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

u8 UnixSection::getUnitArmorLevel(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->armorLevel[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitBuildTime(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->buildTime[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitMineralCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->mineralCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getUnitGasCost(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->gasCost[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

size_t UnixSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->nameStringId[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getWeaponBaseDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        return data->baseDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
}

u16 UnixSection::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        return data->upgradeDamage[(size_t)weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->useDefault[(size_t)unitType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->hitpoints[(size_t)unitType] = hitpoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->shieldPoints[(size_t)unitType] = shieldPoints;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->armorLevel[(size_t)unitType] = armorLevel;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->buildTime[(size_t)unitType] = buildTime;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->mineralCost[(size_t)unitType] = mineralCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->gasCost[(size_t)unitType] = gasCost;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setUnitNameStringId(Sc::Unit::Type unitType, u16 nameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->nameStringId[(size_t)unitType] = nameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u32)unitType) + " is out of range for the UNIx section!");
}

void UnixSection::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        data->baseDamage[(size_t)weaponType] = baseDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
}

void UnixSection::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage)
{
    if ( (size_t)weaponType < Sc::Weapon::Total )
        data->upgradeDamage[(size_t)weaponType] = upgradeDamage;
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((u32)weaponType) + " is out of range for the UNIx section!");
}

bool UnixSection::stringUsed(size_t stringId)
{
    u16 u16StringId = (u16)stringId;
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == u16StringId )
            return true;
    }
    return false;
}

void UnixSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] > 0 )
            stringIdUsed[data->nameStringId[i]] = true;
    }
}

void UnixSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            data->nameStringId[i] = found->second;
    }
}

void UnixSection::deleteString(size_t stringId)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->nameStringId[i] == stringId )
            data->nameStringId[i] = 0;
    }
}


Section UpgxSection::GetDefault() // 794
{
    Chk::UPGx data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // baseMineralCost (all 0)
        {}, // mineralCostFactor (all 0)
        {}, // baseGasCost (all 0)
        {}, // gasCostFactor (all 0)
        {}, // baseResearchTime (all 0)
        {} // researchTimeFactor (all 0)
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalTypes);
    return Section((ChkSection*)new (std::nothrow) UpgxSection(data));
}

UpgxSection::UpgxSection(Chk::UPGx & data) : StructSection<Chk::UPGx, false>(SectionName::UPGx, data)
{

}

bool UpgxSection::upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->useDefault[(size_t)upgradeType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseMineralCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseMineralCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getMineralCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->mineralCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseGasCost(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseGasCost[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getGasCostFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->gasCostFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getBaseResearchTime(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->baseResearchTime[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

u16 UpgxSection::getResearchTimeFactor(Sc::Upgrade::Type upgradeType)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        return data->researchTimeFactor[(size_t)upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}


void UpgxSection::setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->useDefault[(size_t)upgradeType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->baseMineralCost[(size_t)upgradeType] = baseMineralCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->mineralCostFactor[(size_t)upgradeType] = mineralCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->baseGasCost[(size_t)upgradeType] = baseGasCost;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->gasCostFactor[(size_t)upgradeType] = gasCostFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->baseResearchTime[(size_t)upgradeType] = baseResearchTime;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

void UpgxSection::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor)
{
    if ( (size_t)upgradeType < Sc::Upgrade::TotalTypes )
        data->researchTimeFactor[(size_t)upgradeType] = researchTimeFactor;
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((u32)upgradeType) + " is out of range for the UPGx section!");
}

Section TecxSection::GetDefault()
{
    Chk::TECx data = {
        {}, // useDefault (all Yes/1, set below)
        {}, // mineralCost
        {}, // gasCost
        {}, // researchTime
        {} // energyCost
    };
    memset(&data.useDefault, (int)Chk::UseDefault::Yes, Sc::Tech::TotalTypes);
    return Section((ChkSection*)new (std::nothrow) TecxSection(data));
}

TecxSection::TecxSection(Chk::TECx & data) : StructSection<Chk::TECx, false>(SectionName::TECx, data)
{

}

bool TecxSection::techUsesDefaultSettings(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->useDefault[(size_t)techType] != Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechMineralCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->mineralCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechGasCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->gasCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechResearchTime(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->researchTime[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

u16 TecxSection::getTechEnergyCost(Sc::Tech::Type techType)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        return data->energyCost[(size_t)techType];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}


void TecxSection::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->useDefault[(size_t)techType] = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->mineralCost[(size_t)techType] = mineralCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechGasCost(Sc::Tech::Type techType, u16 gasCost)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->gasCost[(size_t)techType] = gasCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->researchTime[(size_t)techType] = researchTime;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

void TecxSection::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost)
{
    if ( (size_t)techType < Sc::Tech::TotalTypes )
        data->energyCost[(size_t)techType] = energyCost;
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string((u32)techType) + " is out of range for the TECx section!");
}

Section OstrSection::GetDefault()
{
    Chk::OSTR data = {
        1, // Version
        0, // scenarioName
        0, // scenarioDescription
        {}, // forceName (all 0)
        {}, // unitName (all 0)
        {}, // expUnitName (all 0)
        {}, // soundPath (all 0)
        {}, // switchName (all 0)
        {} // locationName (all 0)
    };
    return Section((ChkSection*)new (std::nothrow) OstrSection(data));
}

OstrSection::OstrSection(Chk::OSTR & data) : StructSection<Chk::OSTR, true>(SectionName::OSTR, data)
{

}

u32 OstrSection::getVersion()
{
    return data->version;
}

u32 OstrSection::getScenarioNameStringId()
{
    return data->scenarioName;
}

u32 OstrSection::getScenarioDescriptionStringId()
{
    return data->scenarioDescription;
}

u32 OstrSection::getForceNameStringId(Chk::Force force)
{
    if ( (size_t)force < Chk::TotalForces )
        return data->forceName[(size_t)force];
    else
        throw std::out_of_range(std::string("ForceIndex: ") + std::to_string((u32)force) + " is out of range for the OSTR section!");
}

u32 OstrSection::getUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->unitName[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the OSTR section!");
}

u32 OstrSection::getExpUnitNameStringId(Sc::Unit::Type unitType)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        return data->expUnitName[(size_t)unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the OSTR section!");
}

u32 OstrSection::getSoundPathStringId(size_t soundIndex)
{
    if ( soundIndex < Chk::TotalSounds )
        return data->soundPath[soundIndex];
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the OSTR section!");
}

u32 OstrSection::getSwitchNameStringId(size_t switchIndex)
{
    if ( switchIndex < Chk::TotalSwitches )
        return data->switchName[switchIndex];
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the OSTR section!");
}

u32 OstrSection::getLocationNameStringId(size_t locationIndex)
{
    if ( locationIndex < Chk::TotalLocations )
        return data->locationName[locationIndex];
    else
        throw std::out_of_range(std::string("locationIndex: ") + std::to_string((u32)locationIndex) + " is out of range for the OSTR section!");
}

void OstrSection::setScenarioNameStringId(u32 scenarioNameStringId)
{
    data->scenarioName = scenarioNameStringId;
}

void OstrSection::setScenarioDescriptionStringId(u32 scenarioDescriptionStringId)
{
    data->scenarioDescription = scenarioDescriptionStringId;
}

void OstrSection::setForceNameStringId(Chk::Force force, u32 forceNameStringId)
{
    if ( (size_t)force < Chk::TotalForces )
        data->forceName[(size_t)force] = forceNameStringId;
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string((u32)force) + " is out of range for the OSTR section!");
}

void OstrSection::setUnitNameStringId(Sc::Unit::Type unitType, u32 unitNameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->unitName[(size_t)unitType] = unitNameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the OSTR section!");
}

void OstrSection::setExpUnitNameStringId(Sc::Unit::Type unitType, u32 expUnitNameStringId)
{
    if ( (size_t)unitType < Sc::Unit::TotalTypes )
        data->expUnitName[(size_t)unitType] = expUnitNameStringId;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string((u16)unitType) + " is out of range for the OSTR section!");
}

void OstrSection::setSoundPathStringId(size_t soundIndex, u32 soundPathStringId)
{
    if ( soundIndex < Chk::TotalSounds )
        data->soundPath[soundIndex] = soundPathStringId;
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the OSTR section!");
}

void OstrSection::setSwitchNameStringId(size_t switchIndex, u32 switchNameStringId)
{
    if ( switchIndex < Chk::TotalSwitches )
        data->switchName[switchIndex] = switchNameStringId;
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the OSTR section!");
}

void OstrSection::setLocationNameStringId(size_t locationIndex, u32 locationNameStringId)
{
    if ( locationIndex < Chk::TotalLocations )
        data->locationName[locationIndex] = locationNameStringId;
    else
        throw std::out_of_range(std::string("locationIndex: ") + std::to_string((u32)locationIndex) + " is out of range for the OSTR section!");
}

bool OstrSection::stringUsed(size_t stringId)
{
    if ( data->scenarioName == stringId || data->scenarioDescription == stringId )
        return true;

    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->unitName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->expUnitName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPath[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == stringId )
            return true;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        if ( data->locationName[i] == stringId )
            return true;
    }
}

void OstrSection::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( data->scenarioName != 0 )
        stringIdUsed[data->scenarioName] = true;

    if ( data->scenarioDescription != 0 )
        stringIdUsed[data->scenarioDescription] = true;

    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceName[i] != 0 )
            stringIdUsed[data->forceName[i]] = true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->unitName[i] != 0 )
            stringIdUsed[data->unitName[i]] = true;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->expUnitName[i] != 0 )
            stringIdUsed[data->expUnitName[i]] = true;
    }

    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPath[i] != 0 )
            stringIdUsed[data->soundPath[i]] = true;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] != 0 )
            stringIdUsed[data->switchName[i]] = true;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        if ( data->locationName[i] != 0 )
            stringIdUsed[data->locationName[i]] = true;
    }
}

void OstrSection::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    auto scenarioNameRemapping = stringIdRemappings.find(data->scenarioName);
    auto scenarioDescriptionRemapping= stringIdRemappings.find(data->scenarioDescription);
    auto forceOneRemapping = stringIdRemappings.find(data->forceName[0]);
    auto forceTwoRemapping = stringIdRemappings.find(data->forceName[1]);
    auto forceThreeRemapping = stringIdRemappings.find(data->forceName[2]);
    auto forceFourRemapping = stringIdRemappings.find(data->forceName[3]);

    if ( scenarioNameRemapping != stringIdRemappings.end() )
        data->scenarioName = scenarioNameRemapping->second;

    if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
        data->scenarioDescription = scenarioDescriptionRemapping->second;
    
    if ( forceOneRemapping != stringIdRemappings.end() )
        data->forceName[0] = forceOneRemapping->second;

    if ( forceTwoRemapping != stringIdRemappings.end() )
        data->forceName[1] = forceTwoRemapping->second;

    if ( forceThreeRemapping != stringIdRemappings.end() )
        data->forceName[2] = forceThreeRemapping->second;

    if ( forceFourRemapping != stringIdRemappings.end() )
        data->forceName[3] = forceFourRemapping->second;
    
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->unitName[i]);
        if ( found != stringIdRemappings.end() )
            data->unitName[i] = found->second;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(data->expUnitName[i]);
        if ( found != stringIdRemappings.end() )
            data->expUnitName[i] = found->second;
    }
    
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        auto found = stringIdRemappings.find(data->soundPath[i]);
        if ( found != stringIdRemappings.end() )
            data->soundPath[i] = found->second;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        auto found = stringIdRemappings.find(data->switchName[i]);
        if ( found != stringIdRemappings.end() )
            data->switchName[i] = found->second;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        auto found = stringIdRemappings.find(data->locationName[i]);
        if ( found != stringIdRemappings.end() )
            data->locationName[i] = found->second;
    }
}

void OstrSection::deleteString(size_t stringId)
{
    if ( data->scenarioName == stringId )
        data->scenarioName = 0;
    
    if ( data->scenarioDescription == stringId )
        data->scenarioDescription = 0;

    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( data->forceName[i] == stringId )
            data->forceName[i] = 0;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->unitName[i] == stringId )
            data->unitName[i] = 0;
    }

    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( data->expUnitName[i] == stringId )
            data->expUnitName[i] = 0;
    }

    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( data->soundPath[i] == stringId )
            data->soundPath[i] = 0;
    }

    for ( size_t i=0; i<Chk::TotalSwitches; i++ )
    {
        if ( data->switchName[i] == stringId )
            data->switchName[i] = 0;
    }

    for ( size_t i=0; i<Chk::TotalLocations; i++ )
    {
        if ( data->locationName[i] == stringId )
            data->locationName[i] = 0;
    }
}


Section KstrSection::GetDefault()
{
    Section newSection((ChkSection*)new (std::nothrow) KstrSection());
    KstrSection* strSection = (KstrSection *)newSection.get();
    strSection->synced = false;
    return newSection;
}

KstrSection::KstrSection() : DynamicSection<true>(SectionName::KSTR)
{

}

bool KstrSection::isSynced()
{
    return synced;
}

void KstrSection::flagUnsynced()
{
    synced = false;
}

void KstrSection::syncFromBuffer(StrSynchronizerPtr strSynchronizer)
{
    synced = false;
    strSynchronizer->synchronzieFromStrBuffer(*rawData);
}

void KstrSection::syncToBuffer(StrSynchronizerPtr strSynchronizer)
{
    strSynchronizer->synchronizeToStrBuffer(*rawData);
    synced = true;
}

u32 KstrSection::getSize()
{
    return synced ? this->rawData->size() : 0;
}

std::ostream & KstrSection::writeData(std::ostream &s, u32 sizeInBytes)
{
    return synced ? rawData->write(s, false) : s;
}


bool Versions::is(Chk::Version version)
{
    return ver->getVersion() == version;
}

bool Versions::isOriginal()
{
    return ver->isOriginal();
}

bool Versions::isHybrid()
{
    return ver->isHybrid();
}

bool Versions::isExpansion()
{
    return ver->isExpansion();
}

bool Versions::isHybridOrAbove()
{
    return ver->isHybridOrAbove();
}

void Versions::changeTo(Chk::Version version)
{
    Chk::Version oldVersion = ver->getVersion();
    ver->setVersion(version);
    if ( version < Chk::Version::StarCraft_Hybrid )
    {
        type->setType(Chk::Type::RAWS);
        iver->setVersion(Chk::IVersion::Current);
        layers->sizeLocationsToScOriginal();
    }
    else if ( version < Chk::Version::StarCraft_BroodWar )
    {
        type->setType(Chk::Type::RAWS);
        iver->setVersion(Chk::IVersion::Current);
        ive2->setVersion(Chk::I2Version::StarCraft_1_04);
        layers->sizeLocationsToScHybridOrExpansion();
    }
    else // version >= Chk::Version::StarCraft_Broodwar
    {
        type->setType(Chk::Type::RAWB);
        iver->setVersion(Chk::IVersion::Current);
        ive2->setVersion(Chk::I2Version::StarCraft_1_04);
        layers->sizeLocationsToScHybridOrExpansion();
    }
}

bool Versions::hasDefaultValidation()
{
    return vcod->isDefault();
}

void Versions::setToDefaultValidation()
{
    vcod->setToDefault();
}


Strings::Strings() :
    StrSynchronizer((u32)StrCompressFlag::DuplicateStringRecycling, (u32)StrCompressFlag::AllNonInterlacing)
{

}

size_t Strings::getCapacity(Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        return strings.size();
    else if ( storageScope == Chk::Scope::Editor )
        return kStrings.size();
    else
        return 0;
}

bool Strings::stringUsed(size_t stringId, Chk::Scope usageScope, Chk::Scope storageScope, bool ensureStored)
{
    if ( storageScope == Chk::Scope::Game && (stringId < strings.size() && strings[stringId] != nullptr || !ensureStored) )
    {
        if ( stringId < Chk::MaxStrings ) // 16 or 32-bit stringId
        {
            if ( usageScope == Chk::Scope::Either )
                return sprp->stringUsed(stringId) || players->stringUsed(stringId) || properties->stringUsed(stringId) || layers->stringUsed(stringId) || triggers->stringUsed(stringId);
            else if ( usageScope == Chk::Scope::Game )
                return sprp->stringUsed(stringId) || players->stringUsed(stringId) || properties->stringUsed(stringId) || triggers->gameStringUsed(stringId);
            else if ( usageScope == Chk::Scope::Editor )
                return layers->stringUsed(stringId) || triggers->editorStringUsed(stringId);
        }
        else // stringId >= Chk::MaxStrings // 32-bit stringId
        {
            return usageScope == Chk::Scope::Either && triggers->stringUsed(stringId) ||
                usageScope == Chk::Scope::Game && triggers->gameStringUsed(stringId) ||
                usageScope == Chk::Scope::Editor && triggers->editorStringUsed(stringId);
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (stringId < kStrings.size() && kStrings[stringId] != nullptr || !ensureStored) )
        return ostr->stringUsed(stringId);
}

bool Strings::stringStored(size_t stringId, Chk::Scope storageScope)
{
    return ((u32)storageScope & (u32)Chk::Scope::Game == (u32)Chk::Scope::Game) && stringId < strings.size() && strings[stringId] != nullptr ||
        ((u32)storageScope & (u32)Chk::Scope::Editor == (u32)Chk::Scope::Editor) && stringId < kStrings.size() && kStrings[stringId] != nullptr; 
}

void Strings::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        bool markGameStrings = (u32)usageScope & (u32)Chk::Scope::Game == (u32)Chk::Scope::Game;
        bool markEditorStrings = (u32)usageScope & (u32)Chk::Scope::Editor == (u32)Chk::Scope::Editor;

        if ( markGameStrings )
        {
            sprp->markUsedStrings(stringIdUsed); // {SPRP, Game, u16}: Scenario Name and Scenario Description
            players->markUsedStrings(stringIdUsed); // {FORC, Game, u16}: Force Names
            properties->markUsedStrings(stringIdUsed); // {UNIS, Game, u16}: Unit Names (original); {UNIx, Game, u16}: Unit names (expansion)
            if ( markEditorStrings ) // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, ...
                triggers->markUsedStrings(stringIdUsed); // ... transmission text, next scenario, sound path, comment; {MBRF, Game, u32}: mission objectives, sound, text message
            else
                triggers->markUsedGameStrings(stringIdUsed); // {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, transmission text, next scenario, sound path
        }

        if ( markEditorStrings )
        {
            layers->markUsedStrings(stringIdUsed); // {MRGN, Editor, u16}: location name
            if ( !markGameStrings )
                triggers->markUsedEditorStrings(stringIdUsed); // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: comment
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        ostr->markUsedStrings(stringIdUsed);
    }
}

void Strings::markValidUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope)
{
    markUsedStrings(stringIdUsed, usageScope);
    switch ( storageScope )
    {
        case Chk::Scope::Game:
            {
                size_t limit = std::min((size_t)Chk::MaxStrings, strings.size());
                size_t stringId = 1;
                for ( ; stringId < limit; stringId++ )
                {
                    if ( stringIdUsed[stringId] && strings[stringId] == nullptr )
                        stringIdUsed[stringId] = false;
                }
                for ( ; stringId < Chk::MaxStrings; stringId++ )
                {
                    if ( stringIdUsed[stringId] )
                        stringIdUsed[stringId] = false;
                }
            }
            break;
        case Chk::Scope::Editor:
            {
                size_t limit = std::min((size_t)Chk::MaxStrings, kStrings.size());
                size_t stringId = 1;
                for ( ; stringId < limit; stringId++ )
                {
                    if ( stringIdUsed[stringId] && kStrings[stringId] == nullptr )
                        stringIdUsed[stringId] = false;
                }
                for ( ; stringId < Chk::MaxStrings; stringId++ )
                {
                    if ( stringIdUsed[stringId] )
                        stringIdUsed[stringId] = false;
                }
            }
            break;
        case Chk::Scope::Either:
            {
                size_t limit = std::min(std::min((size_t)Chk::MaxStrings, strings.size()), kStrings.size());
                size_t stringId = 1;
                for ( ; stringId < limit; stringId++ )
                {
                    if ( stringIdUsed[stringId] && strings[stringId] == nullptr && kStrings[stringId] == nullptr )
                        stringIdUsed[stringId] = false;
                }

                if ( strings.size() > kStrings.size() )
                {
                    for ( ; stringId < strings.size(); stringId++ )
                    {
                        if ( stringIdUsed[stringId] && strings[stringId] == nullptr )
                            stringIdUsed[stringId] = false;
                    }
                }
                else if ( kStrings.size() > strings.size() )
                {
                    for ( ; stringId < kStrings.size(); stringId++ )
                    {
                        if ( stringIdUsed[stringId] && kStrings[stringId] == nullptr )
                            stringIdUsed[stringId] = false;
                    }
                }

                for ( ; stringId < Chk::MaxStrings; stringId++ )
                {
                    if ( stringIdUsed[stringId] )
                        stringIdUsed[stringId] = false;
                }
        
            }
            break;
    }
}

template <typename StringType>
std::shared_ptr<StringType> Strings::getString(size_t stringId, Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame: return stringId < kStrings.size() && kStrings[stringId] != nullptr ? convert<StringType>(kStrings[stringId]) : (stringId < strings.size() ? convert<StringType>(strings[stringId]) : nullptr);
        case Chk::Scope::Game: return stringId < strings.size() ? convert<StringType>(strings[stringId]) : nullptr;
        case Chk::Scope::Editor: return stringId < kStrings.size() ? convert<StringType>(kStrings[stringId]) : nullptr;
        case Chk::Scope::GameOverEditor: return stringId < strings.size() && strings[stringId] != nullptr ? convert<StringType>(strings[stringId]) : (stringId < kStrings.size() ? convert<StringType>(kStrings[stringId]) : nullptr);
        default: return nullptr;
    }
}
template std::shared_ptr<RawString> Strings::getString<RawString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getString<EscString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getString<ChkdString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getString<SingleLineChkdString>(size_t stringId, Chk::Scope storageScope);

template <typename StringType>
size_t Strings::findString(const StringType &str, Chk::Scope storageScope)
{
    RawString rawStr;
    ConvertStr<StringType, RawString>(str, rawStr);
    switch ( storageScope )
    {
        case Chk::Scope::Game:
            {
                for ( size_t stringId=1; stringId<strings.size(); stringId++ )
                {
                    if ( strings[stringId] != nullptr && strings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
        case Chk::Scope::Editor:
            {
                for ( size_t stringId=1; stringId<kStrings.size(); stringId++ )
                {
                    if ( kStrings[stringId] != nullptr && kStrings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
        case Chk::Scope::GameOverEditor:
        case Chk::Scope::Either:
            {
                for ( size_t stringId=1; stringId<strings.size(); stringId++ )
                {
                    if ( strings[stringId] != nullptr && strings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
                for ( size_t stringId=1; stringId<kStrings.size(); stringId++ )
                {
                    if ( kStrings[stringId] != nullptr && kStrings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
        case Chk::Scope::EditorOverGame:
            {
                for ( size_t stringId=1; stringId<kStrings.size(); stringId++ )
                {
                    if ( kStrings[stringId] != nullptr && kStrings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
                for ( size_t stringId=1; stringId<strings.size(); stringId++ )
                {
                    if ( strings[stringId] != nullptr && strings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
    }
    return (size_t)Chk::StringId::NoString;
}

void Strings::setCapacity(size_t stringCapacity, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( stringCapacity > Chk::MaxStrings )
            throw MaximumStringsExceeded();

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t numValidUsedStrings = 0;
        size_t highestValidUsedStringId = 0;
        for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
        {
            if ( stringIdUsed[stringId] )
            {
                numValidUsedStrings ++;
                highestValidUsedStringId = stringId;
            }
        }

        if ( numValidUsedStrings > stringCapacity )
            throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragmentStr(false);
            else
                throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        }
        
        while ( strings.size() < stringCapacity )
            strings.push_back(nullptr);

        while ( strings.size() > stringCapacity )
            strings.pop_back();
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        if ( stringCapacity > Chk::MaxKStrings )
            throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::KSTR), stringCapacity, Chk::MaxKStrings);

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        size_t numValidUsedStrings = 0;
        size_t highestValidUsedStringId = 0;
        for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
        {
            if ( stringIdUsed[stringId] )
            {
                numValidUsedStrings ++;
                highestValidUsedStringId = stringId;
            }
        }

        if ( numValidUsedStrings > stringCapacity )
            throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::KSTR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragmentKstr(false);
            else
                throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::KSTR), numValidUsedStrings, stringCapacity, autoDefragment);
        }

        while ( kStrings.size() < stringCapacity )
            kStrings.push_back(nullptr);

        while ( kStrings.size() > stringCapacity )
            kStrings.pop_back();
    }
}

template <typename StringType>
size_t Strings::addString(const StringType &str, Chk::Scope storageScope, bool autoDefragment)
{
    RawString rawString;
    ConvertStr<StringType, RawString>(str, rawString);

    size_t stringId = findString<StringType>(str, storageScope);
    if ( stringId != (size_t)Chk::StringId::NoString )
        return stringId; // String already exists, return the id

    if ( storageScope == Chk::Scope::Game )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t nextUnusedStringId = getNextUnusedStringId(stringIdUsed, Chk::Scope::Game, true);
        
        if ( nextUnusedStringId >= strings.size() )
            setCapacity(nextUnusedStringId, Chk::Scope::Game, autoDefragment);
        else if ( nextUnusedStringId == 0 && storageScope == Chk::Scope::Game )
            throw MaximumStringsExceeded();

        strings[nextUnusedStringId] = ScStrPtr(new ScStr(rawString));
        return nextUnusedStringId;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        size_t nextUnusedStringId = getNextUnusedStringId(stringIdUsed, Chk::Scope::Editor, true);

        if ( nextUnusedStringId >= kStrings.size() )
            setCapacity(nextUnusedStringId, Chk::Scope::Editor, autoDefragment);
        else if ( nextUnusedStringId == 0 )
            throw MaximumStringsExceeded();

        kStrings[nextUnusedStringId] = ScStrPtr(new ScStr(rawString));
        return nextUnusedStringId;
    }
    return (size_t)Chk::StringId::NoString;
}
template size_t Strings::addString<RawString>(const RawString &str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<EscString>(const EscString &str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<ChkdString>(const ChkdString &str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<SingleLineChkdString>(const SingleLineChkdString &str, Chk::Scope storageScope, bool autoDefragment);
        
template <typename StringType>
void Strings::replaceString(size_t stringId, const StringType &str, Chk::Scope storageScope)
{
    RawString rawString;
    ConvertStr<StringType, RawString>(str, rawString);

    if ( storageScope == Chk::Scope::Game && stringId < strings.size() )
        strings[stringId] = ScStrPtr(new ScStr(rawString, StrProp()));
    else if ( storageScope == Chk::Scope::Editor && stringId < kStrings.size() )
        kStrings[stringId] = ScStrPtr(new ScStr(rawString, StrProp()));
}
template void Strings::replaceString<RawString>(size_t stringId, const RawString &str, Chk::Scope storageScope);
template void Strings::replaceString<EscString>(size_t stringId, const EscString &str, Chk::Scope storageScope);
template void Strings::replaceString<ChkdString>(size_t stringId, const ChkdString &str, Chk::Scope storageScope);
template void Strings::replaceString<SingleLineChkdString>(size_t stringId, const SingleLineChkdString &str, Chk::Scope storageScope);

void Strings::deleteString(size_t stringId, Chk::Scope storageScope, bool deleteOnlyIfUnused)
{
    if ( (u32)storageScope & (u32)Chk::Scope::Game == (u32)Chk::Scope::Game )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Game) )
        {
            if ( stringId < strings.size() )
                strings[stringId] = nullptr;
            
            sprp->deleteString(stringId);
            players->deleteString(stringId);
            properties->deleteString(stringId);
            layers->deleteString(stringId);
            triggers->deleteString(stringId);
        }
    }
    
    if ( (u32)storageScope & (u32)Chk::Scope::Editor == (u32)Chk::Scope::Editor )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, true) )
        {
            if ( stringId < kStrings.size() )
                kStrings[stringId] = nullptr;

            ostr->deleteString(stringId);
        }
    }
}

void Strings::moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
        size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
        if ( stringIdMin > 0 && stringIdMax <= strings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Game);
            ScStrPtr selected = strings[stringIdFrom];
            stringIdUsed[stringIdFrom] = false;
            std::map<u32, u32> stringIdRemappings;
            if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                        {
                            ScStrPtr highestString = strings[stringId-1];
                            strings[stringId-1] = nullptr;
                            stringIdUsed[stringId-1] = false;
                            strings[stringId] = highestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(stringId-1, stringId));
                            break;
                        }
                    }
                }
                stringIdRemappings.insert(std::pair<u32, u32>(stringIdFrom, stringIdTo));
            }
            else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                        {
                            ScStrPtr lowestString = strings[stringId+1];
                            strings[stringId+1] = nullptr;
                            stringIdUsed[stringId+1] = false;
                            strings[stringId] = lowestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(stringId+1, stringId));
                            break;
                        }
                    }
                }
            }
            strings[stringIdTo] = selected;
            stringIdRemappings.insert(std::pair<u32, u32>(stringIdFrom, stringIdTo));
            remapStringIds(stringIdRemappings, Chk::Scope::Game);
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
        size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
        if ( stringIdMin > 0 && stringIdMax <= kStrings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Editor);
            ScStrPtr selected = kStrings[stringIdFrom];
            stringIdUsed[stringIdFrom] = false;
            std::map<u32, u32> stringIdRemappings;
            if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                        {
                            ScStrPtr highestString = kStrings[stringId-1];
                            kStrings[stringId-1] = nullptr;
                            stringIdUsed[stringId-1] = false;
                            kStrings[stringId] = highestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(stringId-1, stringId));
                            break;
                        }
                    }
                }
                stringIdRemappings.insert(std::pair<u32, u32>(stringIdFrom, stringIdTo));
            }
            else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                        {
                            ScStrPtr lowestString = kStrings[stringId+1];
                            kStrings[stringId+1] = nullptr;
                            stringIdUsed[stringId+1] = false;
                            kStrings[stringId] = lowestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(stringId+1, stringId));
                            break;
                        }
                    }
                }
            }
            kStrings[stringIdTo] = selected;
            stringIdRemappings.insert(std::pair<u32, u32>(stringIdFrom, stringIdTo));
            remapStringIds(stringIdRemappings, Chk::Scope::Editor);
        }
    }
}

size_t Strings::rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo, bool autoDefragment)
{
    if ( changeStorageScopeTo == Chk::Scope::Editor && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Game, true) )
    {
        RawStringPtr toRescope = getString<RawString>(stringId, Chk::Scope::Game);
        size_t newStringId = addString<RawString>(*toRescope, Chk::Scope::Editor, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == sprp->getScenarioNameStringId() )
            {
                stringIdsReplaced.insert(ostr->getScenarioNameStringId());
                ostr->setScenarioNameStringId(newStringId);
            }
            if ( stringId == sprp->getScenarioDescriptionStringId() )
            {
                stringIdsReplaced.insert(ostr->getScenarioDescriptionStringId());
                ostr->setScenarioDescriptionStringId(newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == players->getForceStringId((Chk::Force)i) )
                {
                    stringIdsReplaced.insert(ostr->getForceNameStringId((Chk::Force)i));
                    ostr->setForceNameStringId((Chk::Force)i, newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::No) )
                {
                    stringIdsReplaced.insert(ostr->getUnitNameStringId((Sc::Unit::Type)i));
                    ostr->setUnitNameStringId((Sc::Unit::Type)i, newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::Yes) )
                {
                    stringIdsReplaced.insert(ostr->getExpUnitNameStringId((Sc::Unit::Type)i));
                    ostr->setExpUnitNameStringId((Sc::Unit::Type)i, newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == triggers->getSoundStringId(i) )
                {
                    stringIdsReplaced.insert(ostr->getSoundPathStringId(i));
                    ostr->setSoundPathStringId(i, newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == triggers->getSwitchNameStringId(i) )
                {
                    stringIdsReplaced.insert(ostr->getSwitchNameStringId(i));
                    ostr->setSwitchNameStringId(i, newStringId);
                }
            }
            size_t numLocations = layers->numLocations();
            for ( size_t i=0; i<numLocations; i++ )
            {
                if ( stringId == layers->getLocation(i)->stringId )
                {
                    stringIdsReplaced.insert(ostr->getLocationNameStringId(i));
                    ostr->setLocationNameStringId(i, newStringId);
                }
            }
            // TODO: When implementing TRIG and MBRF extended strings you'll have to implement string rescoping for triggers

            deleteString(stringId, Chk::Scope::Game, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Editor, true);
        }
    }
    else if ( changeStorageScopeTo == Chk::Scope::Game && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, true) )
    {
        RawStringPtr toRescope = getString<RawString>(stringId, Chk::Scope::Editor);
        size_t newStringId = addString<RawString>(*toRescope, Chk::Scope::Game, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == ostr->getScenarioNameStringId() )
            {
                stringIdsReplaced.insert(sprp->getScenarioNameStringId());
                sprp->setScenarioNameStringId(newStringId);
            }
            if ( stringId == ostr->getScenarioDescriptionStringId() )
            {
                stringIdsReplaced.insert(sprp->getScenarioDescriptionStringId());
                sprp->setScenarioDescriptionStringId(newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == ostr->getForceNameStringId((Chk::Force)i) )
                {
                    stringIdsReplaced.insert(players->getForceStringId((Chk::Force)i));
                    players->setForceStringId((Chk::Force)i, newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == ostr->getUnitNameStringId((Sc::Unit::Type)i) )
                {
                    stringIdsReplaced.insert(properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::No));
                    properties->setUnitNameStringId((Sc::Unit::Type)i, newStringId, Chk::UseExpSection::No);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == ostr->getExpUnitNameStringId((Sc::Unit::Type)i) )
                {
                    stringIdsReplaced.insert(properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::Yes));
                    properties->setUnitNameStringId((Sc::Unit::Type)i, newStringId, Chk::UseExpSection::Yes);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == ostr->getSoundPathStringId(i) )
                {
                    stringIdsReplaced.insert(triggers->getSoundStringId(i));
                    triggers->setSoundStringId(i, newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == ostr->getSwitchNameStringId(i) )
                {
                    stringIdsReplaced.insert(triggers->getSwitchNameStringId(i));
                    triggers->setSwitchNameStringId(i, newStringId);
                }
            }
            size_t numLocations = layers->numLocations();
            for ( size_t i=0; i<numLocations; i++ )
            {
                if ( stringId == ostr->getLocationNameStringId(i) )
                {
                    Chk::LocationPtr location = layers->getLocation(i);
                    stringIdsReplaced.insert(location->stringId);
                    location->stringId = newStringId;
                }
            }
            // TODO: When implementing TRIG and MBRF extended strings you'll have to implement string rescoping for triggers

            deleteString(stringId, Chk::Scope::Editor, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Game, true);
        }
    }
    return 0;
}

size_t Strings::getScenarioNameStringId(Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getScenarioNameStringId() : sprp->getScenarioNameStringId();
}

size_t Strings::getScenarioDescriptionStringId(Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getScenarioDescriptionStringId() : sprp->getScenarioDescriptionStringId();
}

size_t Strings::getForceNameStringId(Chk::Force force, Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getForceNameStringId(force) : players->getForceStringId(force);
}

size_t Strings::getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        return properties->getUnitNameStringId(unitType, useExp);
    else if ( storageScope == Chk::Scope::Editor )
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType);
            case Chk::UseExpSection::Yes: return ostr->getExpUnitNameStringId(unitType);
            case Chk::UseExpSection::No: return ostr->getUnitNameStringId(unitType);
            case Chk::UseExpSection::YesIfAvailable: return ostr->getExpUnitNameStringId(unitType) != 0 ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType);
            case Chk::UseExpSection::NoIfOrigAvailable: return ostr->getUnitNameStringId(unitType) != 0 ? ostr->getUnitNameStringId(unitType) : ostr->getExpUnitNameStringId(unitType);
        }
    }
    return 0;
}

size_t Strings::getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getSoundPathStringId(soundIndex) : triggers->getSoundStringId(soundIndex);
}

size_t Strings::getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getSwitchNameStringId(switchIndex) : triggers->getSwitchNameStringId(switchIndex);
}

size_t Strings::getLocationNameStringId(size_t locationIndex, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        return ostr->getLocationNameStringId(locationIndex);
    else
    {
        Chk::LocationPtr location = layers->getLocation(locationIndex);
        return location != nullptr ? location->stringId : 0;
    }
}

void Strings::setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setScenarioNameStringId(scenarioNameStringId);
    else
        sprp->setScenarioNameStringId(scenarioNameStringId);
}

void Strings::setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setScenarioDescriptionStringId(scenarioDescriptionStringId);
    else
        sprp->setScenarioDescriptionStringId(scenarioDescriptionStringId);
}

void Strings::setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setForceNameStringId(force, forceNameStringId);
    else
        players->setForceStringId(force, forceNameStringId);
}

void Strings::setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        properties->setUnitNameStringId(unitType, unitNameStringId, useExp);
    else
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto:
            case Chk::UseExpSection::Both: ostr->setUnitNameStringId(unitType, unitNameStringId); ostr->setExpUnitNameStringId(unitType, unitNameStringId); break;
            case Chk::UseExpSection::YesIfAvailable:
            case Chk::UseExpSection::Yes: ostr->setExpUnitNameStringId(unitType, unitNameStringId); break;
            case Chk::UseExpSection::NoIfOrigAvailable:
            case Chk::UseExpSection::No: ostr->setUnitNameStringId(unitType, unitNameStringId); break;
        }
    }
}

void Strings::setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setSoundPathStringId(soundIndex, soundPathStringId);
    else
        triggers->setSoundStringId(soundIndex, soundPathStringId);
}

void Strings::setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setSwitchNameStringId(switchIndex, switchNameStringId);
    else
        triggers->setSwitchNameStringId(switchIndex, switchNameStringId);
}

void Strings::setLocationNameStringId(size_t locationIndex, size_t locationNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setLocationNameStringId(locationIndex, locationNameStringId);
    else
    {
        auto location = layers->getLocation(locationIndex);
        if ( location != nullptr )
            location->stringId = locationNameStringId;
    }
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
std::shared_ptr<StringType> Strings::getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Game: return getString<StringType>(gameStringId, Chk::Scope::Game);
        case Chk::Scope::Editor: return getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::GameOverEditor: return gameStringId != 0 ? getString<StringType>(gameStringId, Chk::Scope::Game) : getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame: return editorStringId != 0 ? getString<StringType>(editorStringId, Chk::Scope::Editor) : getString<StringType>(gameStringId, Chk::Scope::Game);
    }
    return nullptr;
}
template std::shared_ptr<RawString> Strings::getString<RawString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getString<EscString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getString<ChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getString<SingleLineChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getScenarioName(Chk::Scope storageScope)
{
    return getString<StringType>(sprp->getScenarioNameStringId(), ostr->getScenarioNameStringId(), storageScope);
}
template std::shared_ptr<RawString> Strings::getScenarioName<RawString>(Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getScenarioName<EscString>(Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getScenarioName<ChkdString>(Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getScenarioName<SingleLineChkdString>(Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getScenarioDescription(Chk::Scope storageScope)
{
    return getString<StringType>(sprp->getScenarioDescriptionStringId(), ostr->getScenarioDescriptionStringId(), storageScope);
}
template std::shared_ptr<RawString> Strings::getScenarioDescription<RawString>(Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getScenarioDescription<EscString>(Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getScenarioDescription<ChkdString>(Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getScenarioDescription<SingleLineChkdString>(Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getForceName(Chk::Force force, Chk::Scope storageScope)
{
    return getString<StringType>(players->getForceStringId(force), ostr->getForceNameStringId(force), storageScope);
}
template std::shared_ptr<RawString> Strings::getForceName<RawString>(Chk::Force force, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getForceName<EscString>(Chk::Force force, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getForceName<ChkdString>(Chk::Force force, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getForceName<SingleLineChkdString>(Chk::Force force, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getUnitName(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    return getString<StringType>(
        properties->getUnitNameStringId(unitType, useExp),
        properties->useExpansionUnitSettings(useExp) ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType),
        storageScope);
}
template std::shared_ptr<RawString> Strings::getUnitName<RawString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getUnitName<EscString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getUnitName<ChkdString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getSoundPath(size_t soundIndex, Chk::Scope storageScope)
{
    return getString<StringType>(triggers->getSoundStringId(soundIndex), ostr->getSoundPathStringId(soundIndex), storageScope);
}
template std::shared_ptr<RawString> Strings::getSoundPath<RawString>(size_t soundIndex, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getSoundPath<EscString>(size_t soundIndex, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getSoundPath<ChkdString>(size_t soundIndex, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getSoundPath<SingleLineChkdString>(size_t soundIndex, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getSwitchName(size_t switchIndex, Chk::Scope storageScope)
{
    return getString<StringType>(triggers->getSwitchNameStringId(switchIndex), ostr->getSwitchNameStringId(switchIndex), storageScope);
}
template std::shared_ptr<RawString> Strings::getSwitchName<RawString>(size_t switchIndex, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getSwitchName<EscString>(size_t switchIndex, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getSwitchName<ChkdString>(size_t switchIndex, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getSwitchName<SingleLineChkdString>(size_t switchIndex, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getLocationName(size_t locationIndex, Chk::Scope storageScope)
{
    return getString<StringType>((locationIndex < layers->numLocations() ? layers->getLocation(locationIndex)->stringId : 0), ostr->getLocationNameStringId(locationIndex), storageScope);
}
template std::shared_ptr<RawString> Strings::getLocationName<RawString>(size_t locationIndex, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getLocationName<EscString>(size_t locationIndex, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getLocationName<ChkdString>(size_t locationIndex, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getLocationName<SingleLineChkdString>(size_t locationIndex, Chk::Scope storageScope);

template <typename StringType>
void Strings::setScenarioName(const StringType &scenarioNameString, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioNameString, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                sprp->setScenarioNameStringId(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setScenarioNameStringId(newStringId);
        }
    }
}
template void Strings::setScenarioName<RawString>(const RawString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<EscString>(const EscString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<ChkdString>(const ChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<SingleLineChkdString>(const SingleLineChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setScenarioDescription(const StringType &scenarioDescription, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioDescription, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                sprp->setScenarioDescriptionStringId(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setScenarioDescriptionStringId(newStringId);
        }
    }
}
template void Strings::setScenarioDescription<RawString>(const RawString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<EscString>(const EscString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<ChkdString>(const ChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<SingleLineChkdString>(const SingleLineChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setForceName(Chk::Force force, const StringType &forceName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && (u32)force < Chk::TotalForces )
    {
        size_t newStringId = addString<StringType>(forceName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                players->setForceStringId(force, newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setForceNameStringId(force, newStringId);
        }
    }
}
template void Strings::setForceName<RawString>(Chk::Force force, const RawString &forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<EscString>(Chk::Force force, const EscString &forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<ChkdString>(Chk::Force force, const ChkdString &forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<SingleLineChkdString>(Chk::Force force, const SingleLineChkdString &forceName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setUnitName(Sc::Unit::Type unitType, const StringType &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && (size_t)unitType < Sc::Unit::TotalTypes )
    {
        size_t newStringId = addString<StringType>(unitName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                properties->setUnitNameStringId(unitType, newStringId, useExp);
            else if ( storageScope == Chk::Scope::Editor )
            {
                switch ( useExp )
                {
                    case Chk::UseExpSection::Auto: versions.isHybridOrAbove() ? ostr->setExpUnitNameStringId(unitType, newStringId) : ostr->setUnitNameStringId(unitType, newStringId); break;
                    case Chk::UseExpSection::Yes: ostr->setExpUnitNameStringId(unitType, newStringId); break;
                    case Chk::UseExpSection::No: ostr->setUnitNameStringId(unitType, newStringId); break;
                    default: ostr->setUnitNameStringId(unitType, newStringId); ostr->setExpUnitNameStringId(unitType, newStringId); break;
                }
            }
        }
    }
}
template void Strings::setUnitName<RawString>(Sc::Unit::Type unitType, const RawString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<EscString>(Sc::Unit::Type unitType, const EscString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<ChkdString>(Sc::Unit::Type unitType, const ChkdString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, const SingleLineChkdString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setSoundPath(size_t soundIndex, const StringType &soundPath, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && soundIndex < Chk::TotalSounds )
    {
        size_t newStringId = addString<StringType>(soundPath, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                triggers->setSoundStringId(soundIndex, newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setSoundPathStringId(soundIndex, newStringId);
        }
    }
}
template void Strings::setSoundPath<RawString>(size_t soundIndex, const RawString &soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<EscString>(size_t soundIndex, const EscString &soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<ChkdString>(size_t soundIndex, const ChkdString &soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<SingleLineChkdString>(size_t soundIndex, const SingleLineChkdString &soundPath, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setSwitchName(size_t switchIndex, const StringType &switchName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && switchIndex < Chk::TotalSwitches )
    {
        size_t newStringId = addString<StringType>(switchName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                triggers->setSwitchNameStringId(switchIndex, newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setSwitchNameStringId(switchIndex, newStringId);
        }
    }
}
template void Strings::setSwitchName<RawString>(size_t switchIndex, const RawString &switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<EscString>(size_t switchIndex, const EscString &switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<ChkdString>(size_t switchIndex, const ChkdString &switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<SingleLineChkdString>(size_t switchIndex, const SingleLineChkdString &switchName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setLocationName(size_t locationIndex, const StringType &locationName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && locationIndex < layers->numLocations() )
    {
        size_t newStringId = addString<StringType>(locationName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                layers->getLocation(locationIndex)->stringId = newStringId;
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setLocationNameStringId(locationIndex, newStringId);
        }
    }
}
template void Strings::setLocationName<RawString>(size_t locationIndex, const RawString &locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<EscString>(size_t locationIndex, const EscString &locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<ChkdString>(size_t locationIndex, const ChkdString &locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<SingleLineChkdString>(size_t locationIndex, const SingleLineChkdString &locationName, Chk::Scope storageScope, bool autoDefragment);

bool Strings::checkFit(StrCompressionElevatorPtr compressionElevator)
{
    size_t numStrings = strings.size();
    if ( numStrings > Chk::MaxStrings )
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, Chk::MaxStrings);

    size_t totalCharacterSpace = 0;
    for ( auto string : strings )
    {
        if ( string != nullptr )
            totalCharacterSpace += string->length() + 1;
    }

    constexpr size_t totalBytesAvailable = u16_max - 3;
    size_t totalOffsetSpace = 2*numStrings;
    
    return totalOffsetSpace + totalCharacterSpace <= totalBytesAvailable;
}

void Strings::synchronizeToStrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the basic, staredit standard, STR section format, and not allowing section sizes over 65536

        u16 numStrings;
        u16 stringOffsets[numStrings]; // Offset of the start of the string within the section
        void[] stringData; // Character data, first comes initial NUL character... then all strings, in order, each NUL terminated
    */

    size_t numStrings = strings.size();
    if ( numStrings > Chk::MaxStrings )
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, Chk::MaxStrings);

    size_t totalCharacterSpace = 0;
    for ( auto string : strings )
    {
        if ( string != nullptr )
            totalCharacterSpace += string->length() + 1;
    }

    constexpr size_t totalBytesAvailable = u16_max - 3; // u16_max for the maximum size of string offsets minus three (two bytes for numStrings, one for NUL char)

    if ( totalCharacterSpace > totalBytesAvailable )
        throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::STR), totalCharacterSpace, totalBytesAvailable);

    size_t totalOffsetSpace = 2 * numStrings;
    if ( totalOffsetSpace + totalCharacterSpace > totalBytesAvailable )
        throw MaximumOffsetAndCharsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, totalCharacterSpace, totalBytesAvailable);

    buffer newStringSection;
    buffer characterData;
    u16 characterDataStart = 2 + (u16)totalOffsetSpace;
    newStringSection.add<u16>((u16)numStrings); // Add numStrings
    characterData.add<s8>(0); // Add initial NUL character=
    for ( auto string : strings )
    {
        size_t stringLength = string != nullptr ? string->length() : 0;
        if ( stringLength == 0 )
            newStringSection.add<u16>(characterDataStart); // Point this string to initial NUL character
        else // stringLength > 0
        {
            newStringSection.add<u16>(characterDataStart + characterData.size()); // Set the offset this string will be at
            characterData.addStr(string->str, stringLength+1); // Add the string plus its NUL terminator
        }
    }
    newStringSection.addStr((char*)characterData.getPtr(0), characterData.size());
    characterData.flush();
    rawData.takeAllData(newStringSection);
}

void Strings::synchronzieFromStrBuffer(const buffer &rawData)
{
    size_t sectionSize = rawData.size();
    size_t numStrings = sectionSize > 2 ? (size_t)rawData.get<u16>(0) : (sectionSize == 1 ? (u16)rawData.get<u8>(0) : 0);
    size_t highestStringWithValidOffset = std::min(numStrings, sectionSize/2);
    strings.clear();

    size_t stringNum = 1;
    for ( ; stringNum <= highestStringWithValidOffset; ++stringNum ) // Process all strings with valid offsets
    {
        size_t offsetPos = 2*stringNum;
        u16 stringOffset = rawData.get<u16>(offsetPos);
        loadString(strings, rawData, stringOffset, sectionSize);
    }
    if ( highestStringWithValidOffset < numStrings ) // Some offsets aren't within bounds
    {
        if ( sectionSize % 2 > 0 ) // Can read one byte of an offset
        {
            stringNum ++;
            u16 stringOffset = (u16)rawData.get<u8>(sectionSize-1);
            loadString(strings, rawData, stringOffset, sectionSize);
        }
        for ( ; stringNum <= numStrings; ++stringNum ) // Any remaining strings are fully out of bounds
            strings.push_back(nullptr);
    }
}

void Strings::synchronizeToKstrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the standard KSTR format
        
        u32 version; // Current version: 2
        u32 numStrings; // Number of strings in the section
        u32 stringOffsets[0]; // u32 stringOffsets[numStrings]; // Offsets to each string within the string section (not within stringData, but within the whole section)
        StringProperties[numStrings] stringProperties; // String properties
        void[] stringData; // List of strings, each null terminated
    */

    size_t numStrings = kStrings.size();
    if ( numStrings > Chk::MaxStrings )
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::KSTR), kStrings.size(), Chk::MaxStrings);

    size_t totalCharacterSpace = 0;
    for ( auto string : kStrings )
    {
        if ( string != nullptr )
            totalCharacterSpace += string->length() + 1;
    }

    constexpr size_t totalBytesAvailable = u32_max - 9; // u32_max for the maximum size of string offsets minus 9 (8 for versions and numStrings, one for NUL char)

    if ( totalCharacterSpace > totalBytesAvailable )
        throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::KSTR), totalCharacterSpace, totalBytesAvailable);

    size_t totalOffsetSpace = 2 * numStrings;
    if ( totalOffsetSpace + totalCharacterSpace > totalBytesAvailable )
        throw MaximumOffsetAndCharsExceeded(ChkSection::getNameString(SectionName::KSTR), numStrings, totalCharacterSpace, totalBytesAvailable);

    buffer newStringSection;
    buffer characterData;
    u32 characterDataStart = 2 + (u32)totalOffsetSpace;
    newStringSection.add<u32>((u32)numStrings); // Add numStrings
    characterData.add<s8>(0); // Add initial NUL character
    for ( auto string : kStrings )
    {
        size_t stringLength = string != nullptr ? string->length() : 0;
        if ( stringLength == 0 )
            newStringSection.add<u32>(characterDataStart); // Point this string to initial NUL character
        else // stringLength > 0
        {
            newStringSection.add<u32>(characterDataStart + characterData.size()); // Set the offset this string will be at
            characterData.addStr(string->str, stringLength+1); // Add the string plus its NUL terminator
        }
    }
    newStringSection.addStr((char*)characterData.getPtr(0), characterData.size());
    characterData.flush();
    rawData.takeAllData(newStringSection);
}

void Strings::synchronzieFromKstrBuffer(const buffer &rawData)
{
    size_t sectionSize = rawData.size();
    size_t numStrings = sectionSize > 4 ? (size_t)rawData.get<u32>(0) : 0;
    size_t highestStringWithValidOffset = std::min(numStrings, (sectionSize-4)/4);
    kStrings.clear();

    size_t stringNum = 1;
    for ( ; stringNum <= highestStringWithValidOffset; ++stringNum ) // Process all strings with valid offsets
    {
        size_t offsetPos = 4+4*stringNum;
        u32 stringOffset = rawData.get<u32>(offsetPos);
        loadString(kStrings, rawData, stringOffset, sectionSize);
    }
    if ( highestStringWithValidOffset < numStrings ) // Some offsets aren't within bounds
    {
        if ( sectionSize % 2 > 0 ) // Can read one byte of an offset
        {
            stringNum ++;
            u16 stringOffset = (u16)rawData.get<u8>(sectionSize-1);
            loadString(kStrings, rawData, stringOffset, sectionSize);
        }
        for ( ; stringNum <= numStrings; ++stringNum ) // Any remaining strings are fully out of bounds
            kStrings.push_back(nullptr);
    }
}

const std::vector<u32> Strings::compressionFlagsProgression = {
    (u32)StrCompressFlag::None,
    (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SubStringRecycling,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::OffsetInterlacing,
    (u32)StrCompressFlag::OffsetInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::OffsetInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::OffsetInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::OrderShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SpareShuffledInterlacing,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing
        | (u32)StrCompressFlag::SizeBytesRecycling
};

void Strings::loadString(std::deque<ScStrPtr> &stringContainer, const buffer &rawData, const u16 &stringOffset, const size_t &sectionSize)
{
    if ( (size_t)stringOffset < sectionSize )
    {
        s64 nextNulPos = 0;
        if ( rawData.getNext('\0', (s64)stringOffset, nextNulPos) ) // Has NUL terminator
        {
            if ( nextNulPos > stringOffset ) // Regular string
                stringContainer.push_back(ScStrPtr(new ScStr(rawData.getString((s64)stringOffset, nextNulPos-stringOffset))));
            else // Zero length string
                stringContainer.push_back(nullptr);
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
            stringContainer.push_back(ScStrPtr(new ScStr(rawData.getString((s64)stringOffset, sectionSize-stringOffset))));
        else // Character data would be out of bounds
            stringContainer.push_back(nullptr);
    }
}

size_t Strings::getNextUnusedStringId(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope storageScope, bool checkBeyondScopedCapacity)
{
    if ( (u32)storageScope & (u32)Chk::Scope::Game == (u32)Chk::Scope::Game )
    {
        size_t limit = checkBeyondScopedCapacity ? Chk::MaxStrings : getCapacity(Chk::Scope::Game);
        for ( size_t i=1; i<limit; i++ )
        {
            if ( !stringIdUsed[i] )
                return i;
        }
    }
    if ( (u32)storageScope & (u32)Chk::Scope::Editor == (u32)Chk::Scope::Editor )
    {
        size_t limit = checkBeyondScopedCapacity ? 1 : getCapacity(Chk::Scope::Editor);
        for ( size_t i=Chk::MaxStrings-1; i>=limit; i-- )
        {
            if ( !stringIdUsed[i] )
                return i;
        }
    }
    return 0;
}

void Strings::resolveParantage()
{
    for ( auto string = strings.begin(); string != strings.end(); ++string )
    {
        auto otherString = string;
        ++otherString;
        for ( ; otherString != strings.end(); ++otherString )
            ScStr::adopt(*string, *otherString);
    }
}

void Strings::resolveParantage(ScStrPtr string)
{
    if ( string != nullptr )
    {
        string->disown();
        for ( auto it = strings.begin(); it != strings.end(); ++it )
        {
            ScStrPtr otherString = *it;
            if ( otherString != nullptr && otherString != string )
                ScStr::adopt(string, otherString);
        }
    }
}

bool Strings::defragmentStr(bool matchCapacityToUsage)
{
    size_t nextCandidateStringId = 0;
    size_t numStrings = strings.size();
    std::map<u32, u32> stringIdRemappings;
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( strings[i] == nullptr )
        {
            for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
            {
                if ( strings[j] != nullptr )
                {
                    strings[i] = strings[j];
                    stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                    break;
                }
            }
        }
    }

    if ( !stringIdRemappings.empty() )
    {
        remapStringIds(stringIdRemappings, Chk::Scope::Game);
        return true;
    }
    return false;
}

bool Strings::defragmentKstr(bool matchCapacityToUsage)
{
    size_t nextCandidateStringId = 0;
    size_t numStrings = kStrings.size();
    std::map<u32, u32> stringIdRemappings;
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( strings[i] == nullptr )
        {
            for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
            {
                if ( strings[j] != nullptr )
                {
                    strings[i] = strings[j];
                    stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                    break;
                }
            }
        }
    }

    if ( !stringIdRemappings.empty() )
    {
        remapStringIds(stringIdRemappings, Chk::Scope::Editor);
        return true;
    }
    return false;
}

void Strings::replaceStringId(size_t oldStringId, size_t newStringId)
{
    Chk::SPRP & sprp = this->sprp->asStruct();
    Chk::FORC & forc = players->forc->asStruct();
    Chk::UNIS & unis = properties->unis->asStruct();
    Chk::UNIx & unix = properties->unix->asStruct();
    Chk::WAV & wav = triggers->wav->asStruct();
    Chk::SWNM & swnm = triggers->swnm->asStruct();

    if ( oldStringId != newStringId )
    {
        #define ReplaceStringId(val) { if ( val == oldStringId ) val = newStringId; }
        ReplaceStringId(sprp.scenarioNameStringId);
        ReplaceStringId(sprp.scenarioDescriptionStringId);

        for ( size_t i=0; i<Chk::TotalForces; i++ )
            ReplaceStringId(forc.forceString[i]);

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            ReplaceStringId(unis.nameStringId[i]);

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            ReplaceStringId(unix.nameStringId[i]);

        for ( size_t i=0; i<Chk::TotalSounds; i++ )
            ReplaceStringId(wav.soundPathStringId[i]);

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            ReplaceStringId(swnm.switchName[i]);

        for ( size_t i=0; i<layers->numLocations(); i++ )
            ReplaceStringId(layers->getLocation(i)->stringId);

        for ( size_t triggerIndex=0; triggerIndex<triggers->numTriggers(); triggerIndex++ )
        {
            std::shared_ptr<Chk::Trigger> trigger = triggers->getTrigger(triggerIndex);
            for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
            {
                Chk::Action & action = trigger->actions[actionIndex];
                if ( (size_t)action.actionType < Chk::Action::NumActionTypes )
                {
                    if ( Chk::Action::actionUsesStringArg[(size_t)action.actionType] )
                        ReplaceStringId(action.stringId);
                    if ( Chk::Action::actionUsesSoundArg[(size_t)action.actionType] )
                        ReplaceStringId(action.soundStringId);
                }
            }
        }

        for ( size_t briefingTriggerIndex=0; briefingTriggerIndex<triggers->numBriefingTriggers(); briefingTriggerIndex++ )
        {
            std::shared_ptr<Chk::Trigger> briefingTrigger = triggers->getBriefingTrigger(briefingTriggerIndex);
            for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
            {
                Chk::Action & action = briefingTrigger->actions[actionIndex];
                if ( (size_t)action.actionType < Chk::Action::NumActionTypes )
                {
                    if ( Chk::Action::briefingActionUsesStringArg[(size_t)action.actionType] )
                        ReplaceStringId(action.stringId);
                    if ( Chk::Action::briefingActionUsesSoundArg[(size_t)action.actionType] )
                        ReplaceStringId(action.soundStringId);
                }
            }
        }
    }
}

void Strings::remapStringIds(std::map<u32, u32> stringIdRemappings, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        sprp->remapStringIds(stringIdRemappings);
        players->remapStringIds(stringIdRemappings);
        properties->remapStringIds(stringIdRemappings);
        layers->remapStringIds(stringIdRemappings);
        triggers->remapStringIds(stringIdRemappings);
    }
    else if ( storageScope == Chk::Scope::Editor )
        ostr->remapStringIds(stringIdRemappings);
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
std::shared_ptr<StringType> Strings::convert(ScStrPtr string)
{
    if ( string == nullptr )
        return nullptr;
    else
    {
        RawString rawString(string->str);
        std::shared_ptr<StringType> outString = std::shared_ptr<StringType>(new StringType());
        ConvertStr<RawString, StringType>(rawString, *outString);
        return outString;
    }
}
template std::shared_ptr<RawString> Strings::convert<RawString>(ScStrPtr string);
template std::shared_ptr<EscString> Strings::convert<EscString>(ScStrPtr string);
template std::shared_ptr<ChkdString> Strings::convert<ChkdString>(ScStrPtr string);
template std::shared_ptr<SingleLineChkdString> Strings::convert<SingleLineChkdString>(ScStrPtr string);

Sc::Player::SlotOwner Players::getSlotOwner(size_t slotIndex, Chk::Scope scope)
{
    switch ( scope )
    {
        case Chk::Scope::Game: return ownr->getSlotOwner(slotIndex);
        case Chk::Scope::Editor: return iown->getSlotOwner(slotIndex);
        case Chk::Scope::EditorOverGame: return iown != nullptr ? iown->getSlotOwner(slotIndex) : ownr->getSlotOwner(slotIndex);
        default: return ownr != nullptr ? ownr->getSlotOwner(slotIndex) : iown->getSlotOwner(slotIndex);
    }
    return Sc::Player::SlotOwner::Inactive;
}

void Players::setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner, Chk::Scope scope)
{
    switch ( scope )
    {
        case Chk::Scope::Game: ownr->setSlotOwner(slotIndex, owner); break;
        case Chk::Scope::Editor: iown->setSlotOwner(slotIndex, owner); break;
        default: ownr->setSlotOwner(slotIndex, owner); iown->setSlotOwner(slotIndex, owner); break;
    }
}

Chk::Race Players::getPlayerRace(size_t playerIndex)
{
    return side->getPlayerRace(playerIndex);
}

void Players::setPlayerRace(size_t playerIndex, Chk::Race race)
{
    side->setPlayerRace(playerIndex, race);
}

Chk::PlayerColor Players::getPlayerColor(Sc::Player::SlotOwner slotOwner)
{
    return colr->getPlayerColor(slotOwner);
}

void Players::setPlayerColor(Sc::Player::SlotOwner slotOwner, Chk::PlayerColor color)
{
    colr->setPlayerColor(slotOwner, color);
}

Chk::Force Players::getPlayerForce(Sc::Player::SlotOwner slotOwner)
{
    return forc->getPlayerForce(slotOwner);
}

size_t Players::getForceStringId(Chk::Force force)
{
    return forc->getForceStringId(force);
}

u8 Players::getForceFlags(Chk::Force force)
{
    return forc->getForceFlags(force);
}

void Players::setPlayerForce(Sc::Player::SlotOwner slotOwner, Chk::Force force)
{
    forc->setPlayerForce(slotOwner, force);
}

void Players::setForceStringId(Chk::Force force, u16 forceStringId)
{
    forc->setForceStringId(force, forceStringId);
}

void Players::setForceFlags(Chk::Force force, u8 forceFlags)
{
    forc->setForceFlags(force, forceFlags);
}

bool Players::stringUsed(size_t stringId)
{
    return forc->stringUsed(stringId);
}

void Players::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    forc->markUsedStrings(stringIdUsed);
}

void Players::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    forc->remapStringIds(stringIdRemappings);
}

void Players::deleteString(size_t stringId)
{
    forc->deleteString(stringId);
}

Sc::Terrain::Tileset Terrain::getTileset()
{
    return era->getTileset();
}

void Terrain::setTileset(Sc::Terrain::Tileset tileset)
{
    era->setTileset(tileset);
}

size_t Terrain::getTileWidth()
{
    return dim->getTileWidth();
}

size_t Terrain::getTileHeight()
{
    return dim->getTileHeight();
}

size_t Terrain::getPixelWidth()
{
    return dim->getPixelWidth();
}

size_t Terrain::getPixelHeight()
{
    return dim->getPixelHeight();
}

void Terrain::setTileWidth(u16 newTileWidth, s32 leftEdge)
{
    u16 tileWidth = dim->getTileWidth();
    u16 tileHeight = dim->getTileHeight();
    isom->setDimensions(newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    tile->setDimensions(newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    mtxm->setDimensions(newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    dim->setTileWidth(tileWidth);
}

void Terrain::setTileHeight(u16 newTileHeight, s32 topEdge)
{
    u16 tileWidth = dim->getTileWidth();
    u16 tileHeight = dim->getTileHeight();
    isom->setDimensions(tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    tile->setDimensions(tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    mtxm->setDimensions(tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    dim->setTileHeight(tileHeight);
}

void Terrain::setDimensions(u16 newTileWidth, u16 newTileHeight, s32 leftEdge, s32 topEdge)
{
    u16 tileWidth = dim->getTileWidth();
    u16 tileHeight = dim->getTileHeight();
    isom->setDimensions(newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    tile->setDimensions(newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    mtxm->setDimensions(newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    dim->setDimensions(newTileWidth, newTileHeight);
}

u16 Terrain::getTile(size_t tileXc, size_t tileYc, Chk::Scope scope)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileYc*tileWidth + tileXc;
    switch ( scope )
    {
        case Chk::Scope::Game: return mtxm->getTile(tileIndex);
        case Chk::Scope::Editor: return tile->getTile(tileIndex);
        case Chk::Scope::EditorOverGame: return tile != nullptr ? tile->getTile(tileIndex) : mtxm->getTile(tileIndex);
        default: return mtxm != nullptr ? mtxm->getTile(tileIndex) : tile->getTile(tileIndex);
    }
    return 0;
}

inline u16 Terrain::getTilePx(size_t pixelXc, size_t pixelYc, Chk::Scope scope)
{
    return getTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, scope);
}

void Terrain::setTile(size_t tileXc, size_t tileYc, u16 tileValue, Chk::Scope scope)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileYc*tileWidth + tileXc;
    switch ( scope )
    {
        case Chk::Scope::Game: mtxm->setTile(tileIndex, tileValue); break;
        case Chk::Scope::Editor: tile->setTile(tileIndex, tileValue); break;
        default: mtxm->setTile(tileIndex, tileValue); tile->setTile(tileIndex, tileValue); break;
    }
}

inline void Terrain::setTilePx(size_t pixelXc, size_t pixelYc, u16 tileValue, Chk::Scope scope)
{
    setTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, tileValue, scope);
}

std::shared_ptr<Chk::IsomEntry> Terrain::getIsomEntry(size_t isomIndex)
{
    return isom->getIsomEntry(isomIndex);
}

void Layers::setTileWidth(u16 tileWidth, u16 sizeValidationFlags, s32 leftEdge)
{
    Terrain::setTileWidth(tileWidth, leftEdge);
    validateSizes(sizeValidationFlags);
}

void Layers::setTileHeight(u16 tileHeight, u16 sizeValidationFlags, s32 topEdge)
{
    Terrain::setTileHeight(tileHeight, topEdge);
    validateSizes(sizeValidationFlags);
}

void Layers::setDimensions(u16 tileWidth, u16 tileHeight, u16 sizeValidationFlags, s32 leftEdge, s32 topEdge)
{
    Terrain::setDimensions(tileWidth, tileHeight, leftEdge, topEdge);
    validateSizes(sizeValidationFlags);
}

void Layers::validateSizes(u16 sizeValidationFlags)
{
    bool updateAnywhereIfAlreadyStandard = sizeValidationFlags & SizeValidationFlag::UpdateAnywhereIfAlreadyStandard == SizeValidationFlag::UpdateAnywhereIfAlreadyStandard;
    bool updateAnywhere = sizeValidationFlags & SizeValidationFlag::UpdateAnywhere == SizeValidationFlag::UpdateAnywhere;
    if ( (!updateAnywhereIfAlreadyStandard && updateAnywhere) || (updateAnywhereIfAlreadyStandard && anywhereIsStandardDimensions()) )
        matchAnywhereToDimensions();

    if ( sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsLocations == SizeValidationFlag::UpdateOutOfBoundsLocations )
        downsizeOutOfBoundsLocations();

    if ( sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsDoodads == SizeValidationFlag::RemoveOutOfBoundsDoodads )
        removeOutOfBoundsDoodads();

    if ( sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsUnits == SizeValidationFlag::UpdateOutOfBoundsUnits )
        updateOutOfBoundsUnits();
    else if ( sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsUnits == SizeValidationFlag::RemoveOutOfBoundsUnits )
        removeOutOfBoundsUnits();

    if ( sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsSprites == SizeValidationFlag::UpdateOutOfBoundsSprites )
        updateOutOfBoundsSprites();
    else if ( sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsSprites == SizeValidationFlag::RemoveOutOfBoundsSprites )
        removeOutOfBoundsSprites();
}

u8 Layers::getFog(size_t tileXc, size_t tileYc)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileWidth*tileYc + tileXc;
    return mask->getFog(tileIndex);
}

u8 Layers::getFogPx(size_t pixelXc, size_t pixelYc)
{
    return getFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile);
}

void Layers::setFog(size_t tileXc, size_t tileYc, u8 fogOfWarPlayers)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileWidth*tileYc + tileXc;
    mask->setFog(tileIndex, fogOfWarPlayers);
}

void Layers::setFogPx(size_t pixelXc, size_t pixelYc, u8 fogOfWarPlayers)
{
    setFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, fogOfWarPlayers);
}

size_t Layers::numSprites()
{
    return thg2->numSprites();
}

std::shared_ptr<Chk::Sprite> Layers::getSprite(size_t spriteIndex)
{
    return thg2->getSprite(spriteIndex);
}

size_t Layers::addSprite(std::shared_ptr<Chk::Sprite> sprite)
{
    return thg2->addSprite(sprite);
}

void Layers::insertSprite(size_t spriteIndex, std::shared_ptr<Chk::Sprite> sprite)
{
    thg2->insertSprite(spriteIndex, sprite);
}

void Layers::deleteSprite(size_t spriteIndex)
{
    thg2->deleteSprite(spriteIndex);
}

void Layers::moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo)
{
    thg2->moveSprite(spriteIndexFrom, spriteIndexTo);
}

void Layers::updateOutOfBoundsSprites()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numSprites = thg2->numSprites();
    for ( size_t i=0; i<numSprites; i++ )
    {
        std::shared_ptr<Chk::Sprite> sprite = thg2->getSprite(i);

        if ( sprite->xc >= pixelWidth )
            sprite->xc = pixelWidth-1;

        if ( sprite->yc >= pixelHeight )
            sprite->yc = pixelHeight-1;
    }
}

void Layers::removeOutOfBoundsSprites()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numSprites = thg2->numSprites();
    for ( size_t i = numSprites-1; i < numSprites; i-- )
    {
        std::shared_ptr<Chk::Sprite> sprite = thg2->getSprite(i);
        if ( sprite->xc >= pixelWidth || sprite->yc >= pixelHeight )
            thg2->deleteSprite(i);
    }
}

size_t Layers::numDoodads()
{
    return dd2->numDoodads();
}

std::shared_ptr<Chk::Doodad> Layers::getDoodad(size_t doodadIndex)
{
    return dd2->getDoodad(doodadIndex);
}

size_t Layers::addDoodad(std::shared_ptr<Chk::Doodad> doodad)
{
    return dd2->addDoodad(doodad);
}

void Layers::insertDoodad(size_t doodadIndex, std::shared_ptr<Chk::Doodad> doodad)
{
    return dd2->insertDoodad(doodadIndex, doodad);
}

void Layers::deleteDoodad(size_t doodadIndex)
{
    return dd2->deleteDoodad(doodadIndex);
}

void Layers::moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo)
{
    return dd2->moveDoodad(doodadIndexFrom, doodadIndexTo);
}

void Layers::removeOutOfBoundsDoodads()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numDoodads = dd2->numDoodads();
    for ( size_t i = numDoodads-1; i < numDoodads; i-- )
    {
        std::shared_ptr<Chk::Doodad> doodad = dd2->getDoodad(i);
        if ( doodad->xc >= pixelWidth || doodad->yc >= pixelHeight )
            dd2->deleteDoodad(i);
    }
}

size_t Layers::numUnits()
{
    return unit->numUnits();
}

std::shared_ptr<Chk::Unit> Layers::getUnit(size_t unitIndex)
{
    return unit->getUnit(unitIndex);
}

size_t Layers::addUnit(std::shared_ptr<Chk::Unit> unit)
{
    return this->unit->addUnit(unit);
}

void Layers::insertUnit(size_t unitIndex, std::shared_ptr<Chk::Unit> unit)
{
    return this->unit->insertUnit(unitIndex, unit);
}

void Layers::deleteUnit(size_t unitIndex)
{
    return unit->deleteUnit(unitIndex);
}

void Layers::moveUnit(size_t unitIndexFrom, size_t unitIndexTo)
{
    return unit->moveUnit(unitIndexFrom, unitIndexTo);
}

void Layers::updateOutOfBoundsUnits()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numUnits = unit->numUnits();
    for ( size_t i=0; i<numUnits; i++ )
    {
        std::shared_ptr<Chk::Unit> currUnit = unit->getUnit(i);

        if ( currUnit->xc >= pixelWidth )
            currUnit->xc = pixelWidth-1;

        if ( currUnit->yc >= pixelHeight )
            currUnit->yc = pixelHeight-1;
    }
}

void Layers::removeOutOfBoundsUnits()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numUnits = unit->numUnits();
    for ( size_t i = numUnits-1; i < numUnits; i-- )
    {
        std::shared_ptr<Chk::Unit> currUnit = unit->getUnit(i);
        if ( currUnit->xc >= pixelWidth || currUnit->yc >= pixelHeight )
            unit->deleteUnit(i);
    }
}

size_t Layers::numLocations()
{
    return mrgn->numLocations();
}

void Layers::sizeLocationsToScOriginal()
{
    mrgn->sizeToScOriginal();
}

void Layers::sizeLocationsToScHybridOrExpansion()
{
    mrgn->sizeToScHybridOrExpansion();
}

std::shared_ptr<Chk::Location> Layers::getLocation(size_t locationIndex)
{
    return mrgn->getLocation(locationIndex);
}

size_t Layers::addLocation(std::shared_ptr<Chk::Location> location)
{
    return mrgn->addLocation(location);
}

void Layers::insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location)
{
    return mrgn->insertLocation(locationIndex, location);
}

void Layers::deleteLocation(size_t locationIndex)
{
    mrgn->deleteLocation(locationIndex);
}

void Layers::moveLocation(size_t locationIndexFrom, size_t locationIndexTo)
{
    mrgn->moveLocation(locationIndexFrom, locationIndexTo);
}

void Layers::downsizeOutOfBoundsLocations()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numLocations = mrgn->numLocations();
    for ( size_t i=0; i<numLocations; i++ )
    {
        std::shared_ptr<Chk::Location> location = mrgn->getLocation(i);

        if ( location->left >= pixelWidth )
            location->left = pixelWidth-1;

        if ( location->right >= pixelWidth )
            location->right = pixelWidth-1;

        if ( location->top >= pixelHeight )
            location->top = pixelHeight-1;

        if ( location->bottom >= pixelHeight )
            location->bottom = pixelHeight-1;
    }
}

bool Layers::anywhereIsStandardDimensions()
{
    std::shared_ptr<Chk::Location> anywhere = mrgn->getLocation((size_t)Chk::LocationId::Anywhere);
    return anywhere != nullptr && anywhere->left == 0 && anywhere->top == 0 && anywhere->right == dim->getPixelWidth() && anywhere->bottom == dim->getPixelHeight();
}

void Layers::matchAnywhereToDimensions()
{
    std::shared_ptr<Chk::Location> anywhere = mrgn->getLocation((size_t)Chk::LocationId::Anywhere);
    if ( anywhere != nullptr )
    {
        anywhere->left = 0;
        anywhere->top = 0;
        anywhere->right = dim->getPixelWidth();
        anywhere->bottom = dim->getPixelHeight();
    }
}

bool Layers::stringUsed(size_t stringId)
{
    return mrgn->stringUsed(stringId);
}

void Layers::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    mrgn->markUsedStrings(stringIdUsed);
}

void Layers::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    mrgn->remapStringIds(stringIdRemappings);
}

void Layers::deleteString(size_t stringId)
{
    mrgn->deleteString(stringId);
}

bool Properties::useExpansionUnitSettings(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::YesIfAvailable: return unix != nullptr ? true : false;
        case Chk::UseExpSection::NoIfOrigAvailable: return unis != nullptr ? false : true;
    }
}

bool Properties::unitUsesDefaultSettings(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->unitUsesDefaultSettings(unitType) : unis->unitUsesDefaultSettings(unitType);
}

u32 Properties::getUnitHitpoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitHitpoints(unitType) : unis->getUnitHitpoints(unitType);
}

u16 Properties::getUnitShieldPoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitShieldPoints(unitType) : unis->getUnitShieldPoints(unitType);
}

u8 Properties::getUnitArmorLevel(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitArmorLevel(unitType) : unis->getUnitArmorLevel(unitType);
}

u16 Properties::getUnitBuildTime(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitBuildTime(unitType) : unis->getUnitBuildTime(unitType);
}

u16 Properties::getUnitMineralCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitMineralCost(unitType) : unis->getUnitMineralCost(unitType);
}

u16 Properties::getUnitGasCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitGasCost(unitType) : unis->getUnitGasCost(unitType);
}

size_t Properties::getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitNameStringId(unitType) : unis->getUnitNameStringId(unitType);
}

u16 Properties::getWeaponBaseDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getWeaponBaseDamage(weaponType) : unis->getWeaponBaseDamage(weaponType);
}

u16 Properties::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getWeaponUpgradeDamage(weaponType) : unis->getWeaponUpgradeDamage(weaponType);
}

void Properties::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitUsesDefaultSettings(unitType, useDefault); unix->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::Yes: unix->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::No: unis->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitUsesDefaultSettings(unitType, useDefault) : unis->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitUsesDefaultSettings(unitType, useDefault) : unix->setUnitUsesDefaultSettings(unitType, useDefault); break;
    }
}

void Properties::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitHitpoints(unitType, hitpoints); unix->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::Yes: unix->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::No: unis->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitHitpoints(unitType, hitpoints) : unis->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitHitpoints(unitType, hitpoints) : unix->setUnitHitpoints(unitType, hitpoints); break;
    }
}

void Properties::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitShieldPoints(unitType, shieldPoints); unix->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::Yes: unix->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::No: unis->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitShieldPoints(unitType, shieldPoints) : unis->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitShieldPoints(unitType, shieldPoints) : unix->setUnitShieldPoints(unitType, shieldPoints); break;
    }
}

void Properties::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitArmorLevel(unitType, armorLevel); unix->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::Yes: unix->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::No: unis->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitArmorLevel(unitType, armorLevel) : unis->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitArmorLevel(unitType, armorLevel) : unix->setUnitArmorLevel(unitType, armorLevel); break;
    }
}

void Properties::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitBuildTime(unitType, buildTime); unix->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::Yes: unix->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::No: unis->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitBuildTime(unitType, buildTime) : unis->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitBuildTime(unitType, buildTime) : unix->setUnitBuildTime(unitType, buildTime); break;
    }
}

void Properties::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitMineralCost(unitType, mineralCost); unix->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::Yes: unix->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::No: unis->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitMineralCost(unitType, mineralCost) : unis->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitMineralCost(unitType, mineralCost) : unix->setUnitMineralCost(unitType, mineralCost); break;
    }
}

void Properties::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitGasCost(unitType, gasCost); unix->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::Yes: unix->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::No: unis->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitGasCost(unitType, gasCost) : unis->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitGasCost(unitType, gasCost) : unix->setUnitGasCost(unitType, gasCost); break;
    }
}

void Properties::setUnitNameStringId(Sc::Unit::Type unitType, size_t nameStringId, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitNameStringId(unitType, nameStringId); unix->setUnitNameStringId(unitType, nameStringId); break;
        case Chk::UseExpSection::Yes: unix->setUnitNameStringId(unitType, nameStringId); break;
        case Chk::UseExpSection::No: unis->setUnitNameStringId(unitType, nameStringId); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitNameStringId(unitType, nameStringId) : unis->setUnitNameStringId(unitType, nameStringId); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitNameStringId(unitType, nameStringId) : unix->setUnitNameStringId(unitType, nameStringId); break;
    }
}

void Properties::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setWeaponBaseDamage(weaponType, baseDamage); unix->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::Yes: unix->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::No: unis->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setWeaponBaseDamage(weaponType, baseDamage) : unis->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setWeaponBaseDamage(weaponType, baseDamage) : unix->setWeaponBaseDamage(weaponType, baseDamage); break;
    }
}

void Properties::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setWeaponUpgradeDamage(weaponType, upgradeDamage); unix->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::Yes: unix->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::No: unis->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setWeaponUpgradeDamage(weaponType, upgradeDamage) : unis->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setWeaponUpgradeDamage(weaponType, upgradeDamage) : unix->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
    }
}

bool Properties::isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex)
{
    return puni->isUnitBuildable(unitType, playerIndex);
}

bool Properties::isUnitDefaultBuildable(Sc::Unit::Type unitType)
{
    return puni->isUnitDefaultBuildable(unitType);
}

bool Properties::playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex)
{
    return puni->playerUsesDefault(unitType, playerIndex);
}

void Properties::setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable)
{
    puni->setUnitBuildable(unitType, playerIndex, buildable);
}

void Properties::setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable)
{
    puni->setUnitDefaultBuildable(unitType, buildable);
}

void Properties::setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    puni->setPlayerUsesDefault(unitType, playerIndex, useDefault);
}

bool Properties::useExpansionUpgradeCosts(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::YesIfAvailable: return upgx != nullptr ? true : false;
        case Chk::UseExpSection::NoIfOrigAvailable: return upgs != nullptr ? false : true;
    }
}

bool Properties::upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->upgradeUsesDefaultSettings(upgradeType) : upgs->upgradeUsesDefaultSettings(upgradeType);
}

u16 Properties::getBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseMineralCost(upgradeType) : upgs->getBaseMineralCost(upgradeType);
}

u16 Properties::getMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getMineralCostFactor(upgradeType) : upgs->getMineralCostFactor(upgradeType);
}

u16 Properties::getBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseGasCost(upgradeType) : upgs->getBaseGasCost(upgradeType);
}

u16 Properties::getGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getGasCostFactor(upgradeType) : upgs->getGasCostFactor(upgradeType);
}

u16 Properties::getBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseResearchTime(upgradeType) : upgs->getBaseResearchTime(upgradeType);
}

u16 Properties::getResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getResearchTimeFactor(upgradeType) : upgs->getResearchTimeFactor(upgradeType);
}

void Properties::setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault); upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::Yes: upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::No: upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault) : upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault) : upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
    }
}

void Properties::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setBaseMineralCost(upgradeType, baseMineralCost); upgx->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::Yes: upgx->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::No: upgs->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setBaseMineralCost(upgradeType, baseMineralCost) : upgs->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setBaseMineralCost(upgradeType, baseMineralCost) : upgx->setBaseMineralCost(upgradeType, baseMineralCost); break;
    }
}

void Properties::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setMineralCostFactor(upgradeType, mineralCostFactor); upgx->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::Yes: upgx->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::No: upgs->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setMineralCostFactor(upgradeType, mineralCostFactor) : upgs->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setMineralCostFactor(upgradeType, mineralCostFactor) : upgx->setMineralCostFactor(upgradeType, mineralCostFactor); break;
    }
}

void Properties::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setBaseGasCost(upgradeType, baseGasCost); upgx->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::Yes: upgx->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::No: upgs->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setBaseGasCost(upgradeType, baseGasCost) : upgs->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setBaseGasCost(upgradeType, baseGasCost) : upgx->setBaseGasCost(upgradeType, baseGasCost); break;
    }
}

void Properties::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setGasCostFactor(upgradeType, gasCostFactor); upgx->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::Yes: upgx->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::No: upgs->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setGasCostFactor(upgradeType, gasCostFactor) : upgs->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setGasCostFactor(upgradeType, gasCostFactor) : upgx->setGasCostFactor(upgradeType, gasCostFactor); break;
    }
}

void Properties::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setBaseResearchTime(upgradeType, baseResearchTime); upgx->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::Yes: upgx->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::No: upgs->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setBaseResearchTime(upgradeType, baseResearchTime) : upgs->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setBaseResearchTime(upgradeType, baseResearchTime) : upgx->setBaseResearchTime(upgradeType, baseResearchTime); break;
    }
}

void Properties::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setResearchTimeFactor(upgradeType, researchTimeFactor); upgx->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::Yes: upgx->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::No: upgs->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setResearchTimeFactor(upgradeType, researchTimeFactor) : upgs->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setResearchTimeFactor(upgradeType, researchTimeFactor) : upgx->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
    }
}

bool Properties::useExpansionUpgradeLeveling(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::YesIfAvailable: return pupx != nullptr ? true : false;
        case Chk::UseExpSection::NoIfOrigAvailable: return upgr != nullptr ? false : true;
    }
}

size_t Properties::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getMaxUpgradeLevel(upgradeType, playerIndex) : upgr->getMaxUpgradeLevel(upgradeType, playerIndex);
}

size_t Properties::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getStartUpgradeLevel(upgradeType, playerIndex) : upgr->getStartUpgradeLevel(upgradeType, playerIndex);
}

size_t Properties::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getDefaultMaxUpgradeLevel(upgradeType) : upgr->getDefaultMaxUpgradeLevel(upgradeType);
}

size_t Properties::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getDefaultStartUpgradeLevel(upgradeType) : upgr->getDefaultStartUpgradeLevel(upgradeType);
}

bool Properties::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->playerUsesDefault(upgradeType, playerIndex) : upgr->playerUsesDefault(upgradeType, playerIndex);
}

void Properties::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel) : upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel) : pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
    }
}

void Properties::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel) : upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel) : pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
    }
}

void Properties::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel) : upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel) : pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
    }
}

void Properties::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel) : upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel) : pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
    }
}

void Properties::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::Yes: pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::No: upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault) : upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault) : pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
    }
}

bool Properties::useExpansionTechCosts(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::YesIfAvailable: return tecx != nullptr ? true : false;
        case Chk::UseExpSection::NoIfOrigAvailable: return tecs != nullptr ? false : true;
    }
}

bool Properties::techUsesDefaultSettings(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->techUsesDefaultSettings(techType) : tecs->techUsesDefaultSettings(techType);
}

u16 Properties::getTechMineralCost(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechMineralCost(techType) : tecs->getTechMineralCost(techType);
}

u16 Properties::getTechGasCost(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechGasCost(techType) : tecs->getTechGasCost(techType);
}

u16 Properties::getTechResearchTime(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechResearchTime(techType) : tecs->getTechResearchTime(techType);
}

u16 Properties::getTechEnergyCost(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechEnergyCost(techType) : tecs->getTechEnergyCost(techType);
}

void Properties::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: tecs->setTechUsesDefaultSettings(techType, useDefault); tecx->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::Yes: tecx->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::No: tecs->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechUsesDefaultSettings(techType, useDefault) : tecs->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechUsesDefaultSettings(techType, useDefault) : tecx->setTechUsesDefaultSettings(techType, useDefault); break;
    }
}

void Properties::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: tecs->setTechMineralCost(techType, mineralCost); tecx->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::Yes: tecx->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::No: tecs->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechMineralCost(techType, mineralCost) : tecs->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechMineralCost(techType, mineralCost) : tecx->setTechMineralCost(techType, mineralCost); break;
    }
}

void Properties::setTechGasCost(Sc::Tech::Type techType, u16 gasCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: tecs->setTechGasCost(techType, gasCost); tecx->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::Yes: tecx->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::No: tecs->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechGasCost(techType, gasCost) : tecs->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechGasCost(techType, gasCost) : tecx->setTechGasCost(techType, gasCost); break;
    }
}

void Properties::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: tecs->setTechResearchTime(techType, researchTime); tecx->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::Yes: tecx->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::No: tecs->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechResearchTime(techType, researchTime) : tecs->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechResearchTime(techType, researchTime) : tecx->setTechResearchTime(techType, researchTime); break;
    }
}

void Properties::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: tecs->setTechEnergyCost(techType, energyCost); tecx->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::Yes: tecx->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::No: tecs->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechEnergyCost(techType, energyCost) : tecs->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechEnergyCost(techType, energyCost) : tecx->setTechEnergyCost(techType, energyCost); break;
    }
}

bool Properties::useExpansionTechAvailability(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::YesIfAvailable: return ptex != nullptr ? true : false;
        case Chk::UseExpSection::NoIfOrigAvailable: return ptec != nullptr ? false : true;
    }
}

bool Properties::techAvailable(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techAvailable(techType, playerIndex) : ptec->techAvailable(techType, playerIndex);
}

bool Properties::techResearched(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techResearched(techType, playerIndex) : ptec->techResearched(techType, playerIndex);
}

bool Properties::techDefaultAvailable(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techDefaultAvailable(techType) : ptec->techDefaultAvailable(techType);
}

bool Properties::techDefaultResearched(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techDefaultResearched(techType) : ptec->techDefaultResearched(techType);
}

bool Properties::playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->playerUsesDefault(techType, playerIndex) : ptec->playerUsesDefault(techType, playerIndex);
}

void Properties::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: ptec->setTechAvailable(techType, playerIndex, available); ptex->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::Yes: ptex->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::No: ptec->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setTechAvailable(techType, playerIndex, available) : ptec->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setTechAvailable(techType, playerIndex, available) : ptex->setTechAvailable(techType, playerIndex, available); break;
    }
}

void Properties::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: ptec->setTechResearched(techType, playerIndex, researched); ptex->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::Yes: ptex->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::No: ptec->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setTechResearched(techType, playerIndex, researched) : ptec->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setTechResearched(techType, playerIndex, researched) : ptex->setTechResearched(techType, playerIndex, researched); break;
    }
}

void Properties::setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: ptec->setDefaultTechAvailable(techType, available); ptex->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::Yes: ptex->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::No: ptec->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setDefaultTechAvailable(techType, available) : ptec->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setDefaultTechAvailable(techType, available) : ptex->setDefaultTechAvailable(techType, available); break;
    }
}

void Properties::setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: ptec->setDefaultTechResearched(techType, researched); ptex->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::Yes: ptex->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::No: ptec->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setDefaultTechResearched(techType, researched) : ptec->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setDefaultTechResearched(techType, researched) : ptex->setDefaultTechResearched(techType, researched); break;
    }
}

void Properties::setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: ptec->setPlayerUsesDefault(techType, playerIndex, useDefault); ptex->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::Yes: ptex->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::No: ptec->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setPlayerUsesDefault(techType, playerIndex, useDefault) : ptec->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setPlayerUsesDefault(techType, playerIndex, useDefault) : ptex->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
    }
}

bool Properties::stringUsed(size_t stringId)
{
    return unis->stringUsed(stringId) || unix->stringUsed(stringId);
}

void Properties::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    unis->markUsedStrings(stringIdUsed);
    unix->markUsedStrings(stringIdUsed);
}

void Properties::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    unis->remapStringIds(stringIdRemappings);
    unix->remapStringIds(stringIdRemappings);
}

void Properties::deleteString(size_t stringId)
{
    unis->deleteString(stringId);
    unix->deleteString(stringId);
}

Chk::Cuwp Triggers::getCuwp(size_t cuwpIndex)
{
    return uprp->getCuwp(cuwpIndex);
}

void Triggers::setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp)
{
    return uprp->setCuwp(cuwpIndex, cuwp);
}

size_t Triggers::addCuwp(const Chk::Cuwp &cuwp, bool fixUsageBeforeAdding)
{
    size_t found = uprp->findCuwp(cuwp);
    if ( found < Sc::Unit::MaxCuwps )
        return found;
    else
    {
        if ( fixUsageBeforeAdding )
            fixCuwpUsage();

        size_t nextUnused = upus->getNextUnusedCuwpIndex();
        if ( nextUnused < Sc::Unit::MaxCuwps )
            uprp->setCuwp(nextUnused, cuwp);
    }
}

void Triggers::fixCuwpUsage()
{
    for ( size_t i=0; i<Sc::Unit::MaxCuwps; i++ )
        upus->setCuwpUsed(i, false);

    size_t numTriggers = trig->numTriggers();
    for ( size_t triggerIndex=0; triggerIndex<numTriggers; triggerIndex++ )
    {
        Chk::TriggerPtr trigger = trig->getTrigger(triggerIndex);
        for ( size_t actionIndex=0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            Chk::Action & action = trigger->action(actionIndex);
            if ( action.actionType == Chk::Action::Type::CreateUnitWithProperties && action.number < Sc::Unit::MaxCuwps )
                upus->setCuwpUsed(action.number, true);
        }
    }
}

bool Triggers::cuwpUsed(size_t cuwpIndex)
{
    return upus->cuwpUsed(cuwpIndex);
}

void Triggers::setCuwpUsed(size_t cuwpIndex, bool cuwpUsed)
{
    upus->setCuwpUsed(cuwpIndex, cuwpUsed);
}

void Triggers::ReplaceData(buffer & data) // TODO: Remove me
{
    throw std::exception("TODO: Remove me");
}

bool Triggers::AddTrigger(Chk::Trigger* trigger) // TODO: Remove me
{
    throw std::exception("TODO: Remove me");
}

std::shared_ptr<Chk::Trigger> Triggers::getTrigger(u32 trigNum) // TODO: Remove me
{
    throw std::exception("TODO: Remove me");
}

size_t Triggers::numTriggers()
{
    return trig->numTriggers();
}

std::shared_ptr<Chk::Trigger> Triggers::getTrigger(size_t triggerIndex)
{
    return trig->getTrigger(triggerIndex);
}

size_t Triggers::addTrigger(std::shared_ptr<Chk::Trigger> trigger)
{
    return trig->addTrigger(trigger);
}

void Triggers::insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger)
{
    trig->insertTrigger(triggerIndex, trigger);
}

void Triggers::deleteTrigger(size_t triggerIndex)
{
    trig->deleteTrigger(triggerIndex);
}

void Triggers::moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo)
{
    trig->moveTrigger(triggerIndexFrom, triggerIndexTo);
}

size_t Triggers::numBriefingTriggers()
{
    return mbrf->numBriefingTriggers();
}

std::shared_ptr<Chk::Trigger> Triggers::getBriefingTrigger(size_t briefingTriggerIndex)
{
    return mbrf->getBriefingTrigger(briefingTriggerIndex);
}

size_t Triggers::addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    return mbrf->addBriefingTrigger(briefingTrigger);
}

void Triggers::insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    mbrf->insertBriefingTrigger(briefingTriggerIndex, briefingTrigger);
}

void Triggers::deleteBriefingTrigger(size_t briefingTriggerIndex)
{
    mbrf->deleteBriefingTrigger(briefingTriggerIndex);
}

void Triggers::moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo)
{
    mbrf->moveBriefingTrigger(briefingTriggerIndexFrom, briefingTriggerIndexTo);
}

size_t Triggers::getSwitchNameStringId(size_t switchIndex)
{
    swnm->getSwitchNameStringId(switchIndex);
}

void Triggers::setSwitchNameStringId(size_t switchIndex, size_t stringId)
{
    swnm->setSwitchNameStringId(switchIndex, stringId);
}

bool Triggers::stringIsSound(size_t stringId)
{
    return wav->stringIsSound(stringId);
}

size_t Triggers::getSoundStringId(size_t soundIndex)
{
    return wav->getSoundStringId(soundIndex);
}

void Triggers::setSoundStringId(size_t soundIndex, size_t soundStringId)
{
    wav->setSoundStringId(soundIndex, soundStringId);
}

bool Triggers::stringUsed(size_t stringId)
{
    return wav->stringUsed(stringId) || swnm->stringUsed(stringId) || trig->stringUsed(stringId) || mbrf->stringUsed(stringId);
}

bool Triggers::gameStringUsed(size_t stringId)
{
    return trig->gameStringUsed(stringId) || mbrf->stringUsed(stringId);
}

bool Triggers::editorStringUsed(size_t stringId)
{
    return wav->stringUsed(stringId) || swnm->stringUsed(stringId) || trig->commentStringUsed(stringId);
}

void Triggers::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    wav->markUsedStrings(stringIdUsed);
    swnm->markUsedStrings(stringIdUsed);
    trig->markUsedStrings(stringIdUsed);
    mbrf->markUsedStrings(stringIdUsed);
}

void Triggers::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    trig->markUsedGameStrings(stringIdUsed);
    mbrf->markUsedStrings(stringIdUsed);
}

void Triggers::markUsedEditorStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    wav->markUsedStrings(stringIdUsed);
    swnm->markUsedStrings(stringIdUsed);
    trig->markUsedCommentStrings(stringIdUsed);
}

void Triggers::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    wav->remapStringIds(stringIdRemappings);
    swnm->remapStringIds(stringIdRemappings);
    trig->remapStringIds(stringIdRemappings);
    mbrf->remapStringIds(stringIdRemappings);
}

void Triggers::deleteString(size_t stringId)
{
    wav->deleteString(stringId);
    swnm->deleteString(stringId);
    trig->deleteString(stringId);
    mbrf->deleteString(stringId);
}
