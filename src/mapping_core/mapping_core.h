#ifndef MAPPINGCORE_H
#define MAPPINGCORE_H

/**
    Mapping core provides a library to work with StarCraft maps (.scx, .scm) and scenario files (.chk), and with StarCraft to the extent needed to load and work with resources from StarCraft's data files

    Mapping core abides by the following rules:
    - All strings in mapping core (with the exception of code near system calls in SystemIO.cpp) are UTF8
    - All code, excepting that in the SystemIO.h/SystemIO.cpp files should only use C++ standard code, nothing system or compiler specific (with some allowances as no compiler perfectly mirrors C++ standards)
    - All code in SystemIO.h/SystemIO.cpp should either have full cross-platform support, or, on unsupported systems be safe to compile, call, and get back appropriate indications of failure
*/

#include "basics.h" // Numerous useful definitions, constants, and utility functions
#include "sha256.h" // Provides the means to compute sha256 hashes for securing sensitive passwords or keys

#include "action_descriptor.h" // Defines the descriptors used for giving summary text to actions
#include "casc_archive.h" // A CASC file is an archive format (like .zip) specialized for StarCraft Remastered
#include "chk.h" // Defines all static structures, constants, and enumerations specific to scenario files (.chk)
#include "escape_strings.h" // Defines several string types that extend basic strings in ways useful for mapping purposes
#include "map_file.h" // A map file is a Scenario wrapped inside of an MpqFile (or rarely a standalone Scenario)
#include "mpq_file.h" // An MPQ file is an archive format (like .zip) specialized for StarCraft
#include "sc.h" // Contains resources to load assets from StarCraft and defines static structures, constants, and enumerations general to StarCraft
#include "scenario.h" // Resources for working with scenarios - scenario are the core piece of a map and describe their versioning, strings, player information, terrain, units, locations, properties, triggers and more

#include "text_trig_compiler.h" // Provides the means to compile text triggers into a scenario file
#include "text_trig_generator.h" // Provides the means to turn triggers into text 

#include "archive_cluster.h" // An ArchiveCluster is a special type of ArchiveFile which reads from and prioritized set of ArchiveFiles (e.g. patch_rt.mpq, BrooDat.mpq, StarDat.mpq)
#include "archive_file.h" // Defines a standard interface for archive files, implemented by MpqFile and CascArchive
#include "file_browser.h" // This file defines a standard, extensible interface for browsing for file paths, be they a location to save files at or open files from, as well as a default implementation
#include "system_io.h" // This file contains any functions that require use of system specific code, and does so in a guarded manner to stay cross-platform safe

#endif