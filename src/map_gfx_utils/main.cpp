#include <cross_cut/logger.h>
#include <mapping_core/mapping_core.h>
#include <iostream>

Logger logger {};

int main()
{
    auto scData = std::make_unique<Sc::Data>();
    if ( scData->load() )
        std::cout << "Badlands has " << scData->terrain.get(Sc::Terrain::Tileset::Badlands).tileGroups.size() << " tile groups\n";
    else
        std::cout << "Failed to load scData!\n";
    
    auto mapFile = std::make_unique<MapFile>(MapFile::getDefaultOpenMapBrowser());
    std::cout << mapFile->getFilePath() << ": has " << mapFile->read.units.size() << " units.\n";
    
    return 0;
}
