# Castle Defender C++ Project
Castle Defender C++ Project TiU 2025

===========================

Name: Maria-Diana Ene  
Student Number: 2125252

How to Compile:
---------------
This project uses FLTK for the graphical user interface.
If FLTK is installed system-wide (for example in WSL or Linux), the project
can be compiled using the following command: 

g++ -std=c++17 main.cpp Grid.cpp Block.cpp GameWindow.cpp Tower.cpp Enemy.cpp \
-o CastleDefender `fltk-config --cxxflags --ldflags`

For Windows users, the recommended method is to run the project through CLion,
which automatically manages the FLTK configuration.  

How to Run:
-----------
./CastleDefender

Files Included:
---------------
- main.cpp
- Block.cpp and Block.h
- Grid.cpp and Grid.h
- Enemy.cpp and Enemy.h
- Tower.cpp and Tower.h
- GameWindow.cpp and GameWindow.h
- Castle_Defender_Report_Template_F2025.pdf
- README.txt  

Notes:
------
Tested on Windows 11 using CLion (MinGW + FLTK configuration handled by the IDE),
and tested on WSL Ubuntu using the compilation command above.
Both environments ran the project successfully.  