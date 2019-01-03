# Wvs extension

This project includes a small framework to extend funcionality into Brazil MapleStory server files using C++.

It is composed by the following components: 
- WVSCommon - shared code among the other parts.
- WvsClient - client code that runs on top of maplestory.exe and includes bypass code for BMS and GMS V53. 
- WvsGame - extends the main gameplay binaries in server architecture architecture. 
- WvsShop - includes fixes for the cash shop server.
- WvsCenter - includes fixes for the the core part that connecteds all the server components.
- WvsLogin - includes fixes for the login server. 

After compilation the final result will be a DLL one dll for the client component and one for each server component.

# Configuring version:

Each compilation supports either GMS Version 53 client or Brazil Maple Story V8 and the version can be set in Config.cpp in WvsCommon project, along with other debug flags. 

# Stability:

Crashs may happen in WvsGame.exe / WvsGame.dll and is not ready for production use yet. 

# Global Maple Story V53 support:

The BMS V8 binaries have a packet structures similar to GMS V53 which allows a GMS client  to connect  to BMS backend. Although higher versions may be supported until character selection. There are a lot of changes during version 54 that breaks some packet structures, for example, V54 is prepared to support multiple pets, while v53 not. 

Know issues:
- After a login-out there is no way to loggin again without restarting the client.

The solo purpose of this implementation was to study reversing enginnering. 
The owner of this repository is not responsable for any misusage relating to this.

