#include <FS.h>   
#include "SPIFFS.h"
#define ID   "WiFi SSID:"
#define PASS "WiFi PASS:"
#define RELAY   "RELAY Time:"
#define SERVER_IP "Server IP:"
#define ORG_ID "Org ID:"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path,"r");
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, "w");
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, String message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, "a+");
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    } 
    Serial.println("- read from file:");
    while(file.available()){
        Serial.print(file.read());
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}
/***************************************************************************************************/
uint8_t Exportfile(fs::FS &fs, const char * path, String data){
   char Buf;
   String GetID;
   //String CheckID = "0004877016";    //Hardcoded check ID
   uint8_t Flag=0;
   
   Serial.printf("Checking file: %s\r\n", path);

    File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return 1;
    }
    //Serial.println("- Check from file:");
    while(file.available()){
        Buf=file.read();
        if(Buf=='\n')   
          GetID="";    
        else
          GetID+=Buf; 
        
        if(GetID == data){
             GetID=""; 
             while(file.available()){
                Buf=file.read();
                if(Buf=='\n') {
                  Serial.print("Data : ");
                  Serial.println(GetID); 
                  GetID=""; 
                  return 1; 
                }   
                else
                   GetID+=Buf; 
             }
        }
    }
    file.close();
    Serial.println("data Not Matched : "); 
    return 0; 
}
/***************************************************************************************************/
uint8_t Importfile(fs::FS &fs, const char * path, String data, String msg){
   char Buf;
   String GetID;
   //String CheckID = "0004877016";    //Hardcoded check ID
   uint8_t Flag=0;
   
   Serial.printf("Checking file: %s\r\n", path);

    File file = fs.open(path, "r");
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return 1;
    }
    //Serial.println("- Check from file:");
    while(file.available()){
        Buf=file.read();
        if(Buf=='\n')   
          GetID="";    
        else
          GetID+=Buf; 
        
        Serial.print("file Address :");
        Serial.println(file);
        if(GetID == data){
             Serial.print("String Matched : ");
             Serial.println(GetID);
             GetID=""; 
             
             Serial.println("- Open file for appending");
             File fileappend = fs.open(path, FILE_APPEND);
             fileappend = file;     
             //file.close();
             if(!fileappend){
                  Serial.println("- failed to open file for appending");
                  return 1;
             } 
             Serial.print("file Address :");
             Serial.println(file);
             Serial.print("fileappend Address :");
             Serial.println(fileappend);
             
             if(fileappend.print(msg)){
                  Serial.println("- message appended");
             } else {
                  Serial.println("- append failed");
             }
             
             fileappend.close();
        }
    }
    Serial.println("data Not Matched : "); 
    return 0; 
}
/*******************************************************************************/
void setup(){
    Serial.begin(115200);
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    
    writeFile(SPIFFS, "/Config.txt", "");
    listDir(SPIFFS, "/", 0);
}

void loop(){
  if(Serial.available()) {
    String Inc_Cmd = Serial.readStringUntil('\n'); 
      Serial.println(Inc_Cmd);
      if(Inc_Cmd == "CLEAR-C")
          writeFile(SPIFFS, "/Config.txt", "");   //Clear File
      else if(Inc_Cmd == "READ-C")
          readFile(SPIFFS, "/Config.txt");
      else if(Inc_Cmd == "APPEND-ID")
          appendFile(SPIFFS, "/Config.txt", "WiFi SSID:");  
      else if(Inc_Cmd == "APPEND-PASS")       
          appendFile(SPIFFS, "/Config.txt", "WiFi PASS:");
      else if(Inc_Cmd == "APPEND-IP")
          appendFile(SPIFFS, "/Config.txt", "Server IP:");
      else if(Inc_Cmd == "APPEND-RELAY")
          appendFile(SPIFFS, "/Config.txt", "RELAY Time:");
      else if(Inc_Cmd == "APPEND-ORGID")
          appendFile(SPIFFS, "/Config.txt", "Org ID:");
      else if(Inc_Cmd == "CHECK"){
          Exportfile(SPIFFS, "/Config.txt",ID);
          Exportfile(SPIFFS, "/Config.txt",PASS); 
          Exportfile(SPIFFS, "/Config.txt",SERVER_IP);
          Exportfile(SPIFFS, "/Config.txt",RELAY);  
          Exportfile(SPIFFS, "/Config.txt",ORG_ID);  
      }
      else if(Inc_Cmd == "ADD") {      
          Importfile(SPIFFS, "/Config.txt",ID,"Sensedge technologies");  
      }
      else
          appendFile(SPIFFS, "/Config.txt", Inc_Cmd+"\r\n");
    }
}
