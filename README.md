# Wvs V8 extension

This repository contains a small framework to introduce new code in both client and server of the leaked version of Brazil Maple Story server files.
The ultimate goal is to preserve the identity of a game that is almost dead and allow developers to include new code and fix bugs in the old executable.

# Architecture
- WVSCommon - Offer classes for hooking code into the compiled binaries and some utility classes reimplementations such as ZMap, ZList and ZString
- WvsGame - Contains reimplementation of C++ classes that maps to the data structures in memory of the executable. 
- WvsLogin - Contains fixes for the login server.
- WvsShop - Contains fixes for the cash shop server.
- WvsClient - Contains bypass and a set of extensions for the BMS V8 client.

# Compiling 

 - Use visual studio 2017.


