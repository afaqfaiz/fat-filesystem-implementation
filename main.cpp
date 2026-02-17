#include <iostream>
#include <vector>
#include "string.h"
#include <fstream>
#include <utility>
#include <cmath>
#include <queue>
#include<cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

#define Directory_Size 128
#define Max_File_Name_Size 64 //bytes
#define Max_File_Size 128 // in blocks
#define Block_Size 1024
#define Total_Disk_Size (64 * 1024 * 1024)
#define FAT_EOF -1
#define FAT_FREE -2
#define FAT_size  Total_Disk_Size/Block_Size
#define start_i_dir 0
#define end_i_dir 17920
#define file_data_byte 1592320
#define  start_block_data 1554
int start_i_data= 0;
std:: string  Directory_Name="root";
const int Total_Blocks = Total_Disk_Size / Block_Size;


struct Directory{
    char fileName[Max_File_Name_Size];
    int fileSize;
    int firstBlock;
    char parentdir[Max_File_Name_Size];
    bool isdirectory;
    bool inuse;
};
Directory dir[Directory_Size];

//first store next fat block, .second store disk block to store data
vector<int> FAT(Total_Disk_Size/Block_Size,{FAT_FREE});
queue<int>freefat;
queue<int>freedir;






void formatediskfile() {
    std::ofstream file("disk.bin", std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open disk.bin for formatting!" << std::endl;
        return;
    }

    // Create a 1 KB block filled with binary zeros
    char block[Block_Size];
    std::memset(block, 0, Block_Size); // Fill the block with binary zeroes

    // Write 64,000 blocks minus 3,000 to the file (or adjust as needed)
    for (int i = 0; i < Total_Blocks - 3036; ++i) {
        file.write(block, Block_Size);
    }

    file.close();
    if (file) {
        std::cout << "disk.bin formatted successfully as a 64 MB file with binary zeroes.\n";
    } else {
        std::cerr << "Error occurred while formatting disk.bin!\n";
    }
}

void saveAllDataToBinaryFile(const char* filename) {
    cout<<"Save all data to Binary  file";
}

void displayfat()
{
    for(int i=0; i<100; i++)
    {
        cout << "FAT[" << i << "] = " << FAT[i]<<endl;
    }
}

void displaydir()
{
    for(int i=0; i<Directory_Size; i++)
    {
        if(dir[i].inuse)
        {
            cout << "Name: " << dir[i].fileName << ", Size: " << dir[i].fileSize << ", First Block: " << dir[i].firstBlock << ", Parent Dir: " << dir[i].parentdir << ", Is Directory: " << dir[i].isdirectory << ", Inuse: " << dir[i].inuse << endl;
        }
    }
}
void formate()
{
    for(int i=0; i<Directory_Size; i++)
    {
        dir[i].inuse = false;
        if(dir[i].inuse == false)
        {
            freedir.push(i);
        }
    }
    for(int i=0; i<Total_Disk_Size/Block_Size; i++)
    {
        FAT[i] = FAT_FREE;
        if(FAT[i] == FAT_FREE)
        {
            freefat.push(i);
        }
    }

}
void storefreefat_dir()
{
    //the queue ccontain the freefat
    for(int i=0; i<Total_Disk_Size/Block_Size;i++)
    {
         if(FAT[i] == FAT_FREE)
        {
            freefat.push(i);
        }
    }
    //free dir
    for(int i=0; i<Directory_Size; i++)
    {
        if(dir[i].inuse == false)
        {
            freedir.push(i);
        }
    }
}
void displayfreefat()
{
    while(!freefat.empty())
    {
        cout << freefat.front() << " ";
        freefat.pop();
    }
    cout << endl;
}
void displayfreedir()
{
    while(!freedir.empty())
    {
        cout << freedir.front() << " ";
        freedir.pop();
    }
    cout << endl;
}

void createfile(string filename)
{
     if(filename.size()>Max_File_Name_Size)
    {
        cout<<"Filename name is too long"<<endl;
        return;
    }
    for(int i=0; i<Directory_Size;i++)
    {
         if(strcmp(dir[i].fileName,filename.c_str())==0 && dir[i].inuse==true && dir[i].isdirectory==false)
        {
            if(strcmp(dir[i].parentdir,Directory_Name.c_str())==0)
            {
                cout<<"file already exist\n";
                return;
            }
            
        }
    }
    // for(int i=0;i<Directory_Size;i++){
    //      if(strcmp(dir[i].fileName,filename.c_str())==0)
    //     {
    //         cout<<"file with name " <<filename<<" already exist"<<endl;
    //         return;
    //     }
    // }
    int index = -1;
    for(int i=0;i<Directory_Size;i++){
        if(dir[i].inuse == false){
            index = i;
            break;
        }
    }
    if(index == -1){
        cout<<"No free directory found"<<endl;
        return;

    }
    //allocate file 
    string parent_dir=Directory_Name;
    strcpy(dir[index].fileName,filename.c_str());
    dir[index].fileSize = 0;
    dir[index].firstBlock=-1;
    strcpy(dir[index].parentdir,parent_dir.c_str());
    dir[index].isdirectory = false;
    dir[index].inuse = true;


    cout<<"create file "<<filename<<endl;
}
void listfile(){
    for(int i=0;i<Directory_Size;i++)
    {
        if(dir[i].inuse==true)
        {
              cout<<i <<" - fileName: "<< dir[i].fileName<<endl;
        }
    }
}

//--------------------------------------------writing data in files

void allocateblock(int startblock, int blocksneeded)
{
    int block;
    int prevblock= startblock;
    int prev1;
   while(blocksneeded>0)
   {
    if(freefat.empty()){
        cout<<"No free block found, memory full\n";
        return;
    }
     block=freefat.front();
     cout<<"block "<<block<<endl;
    freefat.pop();
    FAT[prevblock]=block;
    blocksneeded--;
    prev1=prevblock;
    prevblock=block;
    
   }
   FAT[prev1]=FAT_EOF;
}

void writedatablock(int blocksneeded,int startblock,string data)
{
    std::fstream file("disk.bin", std::ios::binary | std::ios::in | std::ios::out);
    if(!file)
    {
        cout<<"file not opened, some error occured"<<endl;
        return;
    }
    int data_size=data.size();
    int cur_block=startblock;
    
    int written_bytes=0;
    int offset = 0;
      while (blocksneeded >0 && cur_block!=FAT_EOF) 
      {
        int cur_disk_block=cur_block+start_block_data;

        file.seekp(cur_disk_block*Block_Size,std::ios::beg);
        int remaining_bytes=data_size-written_bytes;
        int bytes_to_write=(remaining_bytes>Block_Size)?Block_Size:remaining_bytes;

        file.write(data.data()+written_bytes,bytes_to_write);
        written_bytes+=bytes_to_write;

        cur_block=FAT[cur_block];
         blocksneeded--;
            
      }

}
void writeDatainFile(string filename,string data)
{
     int startblock=-1;
     int dirindex=-1;
    for(int i=0;i<Directory_Size;i++){
        //cout<<"file name"<<dir[i].fileName<<endl;
         if(dir[i].fileName==filename)
        {
            dirindex=i;
            if(dir[i].firstBlock !=-1)
            {
                cout<<"file alrady have data\n";
                 startblock=dir[i].firstBlock;
                //  formateFileData(startblock);
                break;
            }
            else{
                startblock=freefat.front();
                freefat.pop();
                dir[dirindex].firstBlock=startblock;
                break;
            }
        }
    }
    
    cout<<"startblock: "<<startblock<<","<<dir[dirindex].firstBlock<<endl;
    int blocksneeded=std::ceil((double)data.size() / Block_Size);
    allocateblock(startblock,blocksneeded);
        cout<<"blocksneeded: "<<blocksneeded<<endl;
        int cur_block = startblock;
        

        writedatablock(blocksneeded,startblock,data);
      
        cout<<"data written to the file.."<<filename<<endl;
}
std::string readfile(string filename)
{
    int startblock = -1;
    for (int i = 0; i < Directory_Size; i++) {
        if (dir[i].fileName == filename) {
            startblock = dir[i].firstBlock;
            break;
        }
    }

    if (startblock == -1) {
        std::cout << "Error: File not found in directory\n";
        return "";
    }

    // Open the disk file
    std::fstream file("disk.bin", std::ios::binary | std::ios::in);
    if (!file) {
        std::cout << "Error: Could not open disk.bin\n";
        return "";
    }

    std::string data; // To store the retrieved data
    int cur_block = startblock;

    // Traverse the FAT chain to read data
    while (cur_block != FAT_EOF) {
        int cur_disk_block = cur_block + start_block_data;

        // Seek to the block position in the file
        file.seekg(cur_disk_block * Block_Size, std::ios::beg);

        // Read the block data
        char buffer[Block_Size];
        file.read(buffer, Block_Size);

        // Append the read data to the result string
        data.append(buffer, file.gcount());

        // Move to the next block in the chain
        cur_block = FAT[cur_block];
    }

    file.close();
    return data;

}

void storedirfat()
{
    //store data in disk.bin in binary format
    std::fstream file("disk.bin", std::ios::binary | std::ios::in | std::ios::out);
    for (int i = 0; i < Directory_Size; ++i) {
        file.write(reinterpret_cast<const char*>(&dir[i]), sizeof(Directory));
    }
    //write FAT file to disk.bin
        int offset=18*1024;
       file.seekp(offset, std::ios::beg);
    if (!file) {
        std::cerr << "Error seeking to the specified position!\n";
        return;
    }

    // Write each Directory struct to the file
    for (int i = 0; i < (Total_Disk_Size/Block_Size); ++i) {
        file.write(reinterpret_cast<const char*>(&FAT[i]), sizeof(FAT));
    }

    // file.seekp(1591297,std::ios::beg);
    // int x=8;
    // file.write(reinterpret_cast<const char*>(&x), sizeof(x));
    file.close();

}
void reloaddirfat()
{
    //load data from the disk.bin(contain ) into dir
    // ifstream file("disk.bin", ios::binary);
     std::ifstream file("disk.bin", std::ios::binary | std::ios::in);
    //file.read((char*)&dir, sizeof(dir));
    //load FAT MEMORY file into FAT from disk.bin
    file.seekg(0, std::ios::beg);
    if(!file)
    {
        std::cerr<<"Error seeking to the specified position for directory!\n";
        return;
    }
    for(int i=0; i<Directory_Size; i++)
    {
        file.read(reinterpret_cast<char *>(&dir[i]),sizeof(Directory));
        if(dir[i].inuse==false)
        {
            freedir.push(i);
        }
    }

    int offset=18*1024;
    file.seekg(offset,std::ios::beg);
    if(!file){
        std::cerr<<"Error seeking to the specified position for FAT!\n";
    }
    for(int i=0; i<(Total_Disk_Size/Block_Size);i++)
    {
        file.read(reinterpret_cast<char *>(&FAT[i]),sizeof(FAT));
        if(FAT[i]==FAT_FREE)
        {
            freefat.push(i);
        }
    }

    //file.read((char*)&FAT, sizeof(FAT));

    file.close();
}



//move back in directory
void moveback() {
    if (Directory_Name == "root") {
        cout << "You are already in the < root > directory\n";
        return;
    }

    for (int i = 0; i < Directory_Size; i++) {
        if (strcmp(dir[i].fileName, Directory_Name.c_str()) == 0 && dir[i].inuse && dir[i].isdirectory) {
            
            Directory_Name = std::string(dir[i].parentdir);
            //Directory_Name = dir[i].parentdir;

            cout << "Moved to directory: " << Directory_Name << endl;
            return;
        }
    }

    // If no matching directory entry is found
    cout << "Error: Parent directory not found\n";
}
//move to give dirname
void cdfunction(string dirname)
{
    for(int i=0; i<Directory_Size;i++)
    {
        if(strcmp(dir[i].fileName,dirname.c_str())==0 && dir[i].inuse && dir[i].isdirectory==true)
        {
            Directory_Name=dirname;
            return;

        }
    }

}
//create new directory
void createDirectory(string dirname)
{
    if(dirname.size()>Max_File_Name_Size)
    {
        cout<<"Directry name is too long";
        return;
    }
    for(int i=0; i<Directory_Size;i++)
    {
         if(strcmp(dir[i].fileName,dirname.c_str())==0 && dir[i].inuse==true && dir[i].isdirectory==true)
        {
            if(strcmp(dir[i].parentdir,Directory_Name.c_str())==0)
            {
                cout<<"Directory already exist\n";
                return;
            }
            
        }
    }
    int x=freedir.front();
    freedir.pop();
    strcpy(dir[x].parentdir,Directory_Name.c_str());
    dir[x].inuse = true;
    dir[x].isdirectory = true;
    dir[x].fileSize=0;
    dir[x].firstBlock=-1;
    strcpy(dir[x].fileName,dirname.c_str());
}

void formateFileData(int startblock) {
    int cur_block = startblock;
    std::fstream file("disk.bin", std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Error opening disk.bin\n";
        return;
    }

    while (cur_block != FAT_EOF) {
        int cur_disk_block = cur_block + start_block_data;
        file.seekp(cur_disk_block * Block_Size, std::ios::beg);
        file.write(std::string(Block_Size, '\0').c_str(), Block_Size);

        int prev = cur_block;
        cur_block = FAT[cur_block];
        FAT[prev] = FAT_FREE;
    }

    file.close();
}

void deleteFile(string filename) {
    int startblock = -1, dirindex = -1;

    for (int i = 0; i < Directory_Size; i++) {
        if (dir[i].fileName == filename && dir[i].inuse) {
            startblock = dir[i].firstBlock;
            dirindex = i;
            break;
        }
    }

    if (startblock == -1) {
        cout << "File not found in directory\n";
        return;
    }

    // Clear file data
    formateFileData(startblock);

    // Reset directory entry
    dir[dirindex].inuse = false;
    dir[dirindex].firstBlock = -1;

    // Update free directory queue
    freedir.push(dirindex);

    cout << "File deleted successfully\n";
}

void deleteDirectory(string dirname) {
    if (dirname == "root" || dirname == Directory_Name) {
        cout << "Cannot delete root or current directory\n";
        return;
    }

    int dirindex = -1;

    for (int i = 0; i < Directory_Size; i++) {
        if (dir[i].fileName == dirname && dir[i].inuse && dir[i].isdirectory) {
            if (strcmp(dir[i].parentdir, Directory_Name.c_str()) == 0) {
                dirindex = i;
                break;
            }
        }
    }

    if (dirindex == -1) {
        cout << "Directory not found or not in the current directory\n";
        return;
    }

    // Delete all contents within the directory
    for (int i = 0; i < Directory_Size; i++) {
        if (strcmp(dir[i].parentdir, dirname.c_str()) == 0 && dir[i].inuse) {
            if (dir[i].isdirectory) {
                deleteDirectory(dir[i].fileName);  // Recursive for subdirectories
            } else {
                deleteFile(dir[i].fileName);  // For files
            }
        }
    }

    // Reset directory entry
    dir[dirindex].inuse = false;
    dir[dirindex].isdirectory = false;

    // Update free directory queue
    freedir.push(dirindex);

    cout << "Directory deleted successfully\n";
}



int main() {
    // formatediskfile();
    //formate();
        //     return 0;
     reloaddirfat();
    string command, input;
    cout << "Welcome to the File System Shell! Type 'help' for a list of commands.\n";

    while (true) {
        cout << "FS (" << Directory_Name << ") $ "; // Display current directory
        getline(cin, input); // Read the entire line of input

        // Parse command and arguments
        istringstream iss(input);
        vector<string> tokens;
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        if (tokens.empty()) continue;

        // Convert command to lowercase
        string cmd = tokens[0];
        transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        // Handle commands
        if (cmd == "exit" || cmd == "quit") {
            cout << "Exiting the File System Shell. Goodbye!\n";
            break;
        } else if (cmd == "ls") {
            listfile();
        } else if (cmd == "mkdir") {
            if (tokens.size() < 2) {
                cout << "Usage: mkdir <directory_name>\n";
            } else {
                createDirectory(tokens[1]);
            }
        } else if (cmd == "cd") {
            if (tokens.size() < 2) {
                cout << "Usage: cd <directory_name>\n";
            } else if (tokens[1] == "..") {
                moveback();
            } else {
                cdfunction(tokens[1]);
            }
        } else if (cmd == "rm") {
            if (tokens.size() < 2) {
                cout << "Usage: rm <file_name>\n";
            } else {
                deleteFile(tokens[1]);
            }
        } else if (cmd == "rmdir") {
            if (tokens.size() < 2) {
                cout << "Usage: rmdir <directory_name>\n";
            } else {
                deleteDirectory(tokens[1]);
            }
        } else if (cmd == "touch") {
            if (tokens.size() < 2) {
                cout << "Usage: touch <file_name>\n";
            } else {
                createfile(tokens[1]);
            }
        } else if (cmd == "write") {
            if (tokens.size() < 3) {
                cout << "Usage: write <file_name> <data>\n";
            } else {
                writeDatainFile(tokens[1], input.substr(input.find(tokens[2])));
            }
        } else if (cmd == "cat") {
            if (tokens.size() < 2) {
                cout << "Usage: cat <file_name>\n";
            } else {
                readfile(tokens[1]);
            }
        } else if (cmd == "help") {
            cout << "Available commands:\n";
            cout << "  ls              - List files in the current directory\n";
            cout << "  mkdir <name>    - Create a new directory\n";
            cout << "  cd <name>       - Change to a directory\n";
            cout << "  cd ..           - Move to the parent directory\n";
            cout << "  rm <name>       - Delete a file\n";
            cout << "  rmdir <name>    - Delete a directory\n";
            cout << "  touch <name>    - Create a new file\n";
            cout << "  write <name> <data> - Write data to a file\n";
            cout << "  cat <name>      - Display file content\n";
            cout << "  exit/quit       - Exit the shell\n";
        } else {
            cout << "Unknown command: " << cmd << ". Type 'help' for a list of commands.\n";
        }
    }

    return 0;
}


