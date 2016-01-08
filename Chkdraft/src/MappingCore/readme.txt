Mapping Core files are grouped for quick copying to new projects.

Simply copy this directory into your new project, include MappingCore.h in files that use it, and add SFmpq_static.lib to additional dependencies.

Runtime Library should be set to Multi-threaded (/MT), if you must compile with Runtime library set to Multi-threaded DLL (/MD), you must recompile SFMPQ to that end.

Everything in mapping core except SFMPQ, FileIO, and MapFile are C++ standard (with minimal reservations). If you're not programming for a windows based operating system or something with a windows port you will need to alter these files.