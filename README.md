# ENGR301Demo

Demonstration of the updated Pack Transit System for ENGR 301, Engineering Communications, at UNR. This programs simulates the Pack Transit Bus going through its route. It will only stop when a bus stop is flagged as having a person waiting.

# How to Compile and Run

The demonstration requires SFML version 3 or higher to compile. sfml-graphics, sfml-window, and sfml-system must be linked with main.cpp.

An example compilation instruction using g++ is given below:  
g++ -std=c++17 -Wall main.cpp -o main.exe -lsfml-graphics -lsfml-window -lsfml-system  
Which can then be run using:  
./main.exe

# Controls

The number keys 1-5 are used to flag that stops 1-5 are waiting for the bus to arrive. This will cause the bus to briefly pause at those stops before continuing on its route.
