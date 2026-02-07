#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <chrono>

template <typename T> 
class DyanamicArray{

    //Stores the address of our pointer.
    T* array;
    //The total amount elements that can be added to the vector.
    int capacity;
    //The current amount of elements in our vector.
    int current;

    //Everything from now on in this class can be accessed by anything.
    public:

    //An intial capacity of 1 and telling the OS to reserve ram for 1 element of X datatype.
    DyanamicArray(){
        array = new T[1];
        capacity = 1;
        current = 0;
    }

    //This is called a deconstructor and it makes sure that the program give the OS its memory back when done with.
    ~DyanamicArray(){
        delete[] array;
    }

    //Function to add an element to the end of the array.
    void push_back(T data){
        
        if (current == capacity){
            //Allocates a block big enough for twice the capacity in the array.
            T* temp = new T[2 * capacity];

            //This will copy existing elements in the new block of memory.
            for (int i=0; i < capacity; i++){
                temp[i] = array[i];  //Array is the old internal pointer.
            }
            //Free the old block.
            delete[] array;
            //Update the capacity and point array to the new block.
            capacity *= 2;
            array = temp;
        }
        //Store the new element and increase its size.
        array[current] = data;
        current++;
    }

    void erase(int index){
        //Checks if index is valid
        if (index < 0 || index >= current){
            std::cerr << "Error: Index out of bounds!" << std::endl;
            return;            
        }
        //shifts the elements after the index one position to the left
        for (int i = index; i < current - 1; i++){
            array[i] = array[i + 1];
        }

        current--;
    }

    T* begin(){
        //returns the pointer of the first element
        return array; 
    }

    T* end(){
        //returns the pointer just past the last element
        return array + current;
    }

    //Gets the size of my array
    int size(){
        return current;
    }

    T& operator[](int index){
        //lets me use brackets in the code
        return array[index];
    }
};
int MainMenu(); //This is a global function so i can access it anywhere.

std::string LiveDate(){
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    std::string date =
        std::to_string(t->tm_mday) + "-" +
        std::to_string(t->tm_mon + 1) + "-" +       //This function allows me to access the live date and use it through the "date"
        std::to_string(t->tm_year + 1900);          //variable below.

    return date;
}
std::string date = LiveDate();
struct Note{
    int ID;
    std::string Date;
    std::string Title;
    std::string Body;
    std::string Tags;
};
DyanamicArray<Note>NewNotes; //This vector is connected to my Note struct.
int nextID = 1;

//This function handles the master file so that the user data can be read back into the program on launch 
void loadIndex(){
    std::ifstream indexFile("/home/shez/NoteForge/index.txt");
    if (!indexFile.is_open()){
        std::cerr << "Error finding master file - starting fresh!" << std::endl;
        return;
    }

    std::string line;
    while(std::getline(indexFile,line)){
        std::stringstream ss(line);

        Note N;
        std::string idStr;
        std::string filename;
        
        std::getline (ss, idStr, '|');
        if (idStr.empty() || idStr.find_first_not_of("0123456789") != std::string::npos){
            std::cerr << "Warning: Skipping invalid line in index file" << std::endl;
            continue;
        }                                                                                                     
        N.ID = std::stoi(idStr);
        std::getline(ss, N.Date, '|');
        std::getline(ss, N.Title, '|');
        std::getline(ss, filename);

        std::string fullDir = "/home/shez/NoteForge/Notes/" + filename;
        std::ifstream bodyFile(fullDir);
        if (bodyFile.is_open()){
            std::getline(bodyFile,N.Body , '\0');
            bodyFile.close();
        } 
        else{
            N.Body = "";
            std::cerr << "Warning: Missing note file " << fullDir << std::endl;
        }

        NewNotes.push_back(N);

        if (N.ID >= nextID){
            nextID = N.ID + 1;
        }
    }
    indexFile.close();
}

//This path allows the program to talk to my file directory when needed.
std::string path = "/home/shez/NoteForge/Notes/";

void saveIndex(){
    std::ofstream indexFile("/home/shez/NoteForge/index.txt");
        if (!indexFile.is_open()){
        std::cerr << "FATAL ERROR could not save Index file!" << std::endl;
        return;
    }

    for (const Note& n : NewNotes){
        indexFile << n.ID << "|" << n.Date << "|" << n.Title << "|" << n.Title << "_" <<  n.Date << ".txt" << "\n";
    }
    indexFile.close();
    std::cout << "Index saved successfully (" << NewNotes.size() << " notes).\n";
}
//THis searches for notes
int SearchNotes(){
    system("clear");
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (true){
        int counter = 0;
        bool FoundAnything = false;
        std::string UserEnquiry;
        std::cout << "(0 to go back) What would you like to search for: ";
        std::getline(std::cin, UserEnquiry);  
                if (UserEnquiry == "0"){
            MainMenu();
            break;
        }
        system("clear");
        std::cout << "====SEARCH RESULTS====" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        for (const Note& n : NewNotes){

            bool foundInThisNote = false;  // Track if THIS note matches
            
            // Check all fields, but don't print yet
            if (n.Tags.find(UserEnquiry) != std::string::npos){
                foundInThisNote = true;
            }
            else if (n.Title.find(UserEnquiry) != std::string::npos){
                foundInThisNote = true;
            }
            else if (n.Body.find(UserEnquiry) != std::string::npos){
                foundInThisNote = true;
            }
            
            // If note matches ANYWHERE, print it ONCE
            if (foundInThisNote){
                FoundAnything = true;
                std::cout << "----------------------" << std::endl;
                std::cout << "ID: " << n.ID << "\n";
                std::cout << "Date: " << n.Date << "\n";
                std::cout << "Title: " << n.Title << "\n";
                std::cout << "Found '" << UserEnquiry << "' in this note" << std::endl;
                std::cout << "----------------------" << std::endl;
                counter++;   
            }

        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Found " << counter << " results" << std::endl;
        std::cout << "Search took " << duration.count() << " milliseconds" << std::endl;

        if (!FoundAnything){
            std::cout << "NOT Found!" << std::endl;
        }
        continue;
    }
return 0;
}

int DeleteNotes(){
    std::string Bfile;
    system("clear");
    int NoteNum;
    while (true){
        std::cout << "====CURRENT NOTES====" << std::endl;
            for (const Note& n : NewNotes) {
            std::cout << "ID: " << n.ID<< "\n";
            std::cout << "Date: " << n.Date<< "\n";
            std::cout << "Title: " << n.Title << "\n\n";
            std::cout << "------------------------" << std::endl;
        }
   
        std::cout << "(0 = Go back) Which note would you like to delete: ";

        while (!(std::cin >> NoteNum)){ //Keeps asking until the user enters the correct data type.
            std::system("clear");
            std::cout << "Invalid input!\nPlease enter a number!\n";
            std::cin.clear();//Resets input errors
            std::cin.ignore(1000, '\n'); //Removes bad input
            break;
        }

        if (NoteNum == 0){
            MainMenu();
            break;
        
        }
        
        for (int i = 0; i < NewNotes.size(); i++) {
            if (NewNotes[i].ID == NoteNum) {
            std::string choice;
            std::cout << "(Y/n) Are you sure you want to delete " << NewNotes[i].Title << ": ";
            std::cin >> choice;
            if (choice == "Y"){
                std::cout << "Note " << NewNotes[i].Title << " deleted." << std::endl;
                std::string Bfile = NewNotes[i].Title + "_" + NewNotes[i].Date + ".txt";
                std::string cmd = "rm \"/home/shez/NoteForge/Notes/" + Bfile + "\"";
            
                std::system(cmd.c_str());

                NewNotes.erase(i);

                // Update IDs
                for (int j = 0; j < NewNotes.size(); j++) {
                    NewNotes[j].ID = j + 1;
                }
                nextID = NewNotes.size() + 1;
                 
                saveIndex();
                system("clear");
                break; 
            }
            else if (choice == "n" || choice == "N") {
                system("clear");
                std::cout << "Deletion Aborted!" << std::endl;
                break;            
            }
            else {
                system("clear");
                std::cout << "Please enter Y or n." << std::endl;

                continue;
            }

        }
    }
    }
return 0;
}

int EditNotes(){
    system("clear");
    int NoteNum;
    while (true){
        std::cout << "====CURRENT NOTES====" << std::endl;
            for (const Note& n : NewNotes) {
            std::cout << "ID: " << n.ID<< "\n";
            std::cout << "Date: " << n.Date<< "\n";
            std::cout << "Title: " << n.Title << "\n\n";
            std::cout << "------------------------" << std::endl;
        }

        std::cout << "(0 = Go back) Which note would you like to edit: ";
        while (!(std::cin >> NoteNum)){ //Keeps asking until the user enters the correct data type.
            std::system("clear");
            std::cout << "Invalid input!\nPlease enter a number!\n";
            std::cin.clear();//Resets input errors
            std::cin.ignore(1000, '\n'); //Removes bad input
            break;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string Mfile;
        for (auto& n : NewNotes){
            if (NoteNum == n.ID){
                Mfile = n.Title + "_" + n.Date + ".txt";
                std::string cmd = "nano " "/home/shez/NoteForge/Notes/" + Mfile;
                std::system(cmd.c_str());
                std::ifstream in("/home/shez/NoteForge/Notes/" + Mfile);
                if (!in.is_open()) {
                    std::cout << "FILE FAILED TO OPEN\n";
                }
                std::string newBody;
                std::string newTags;
                std::string line;
                bool inTagsSection = false;
                while (std::getline(in , line)){
                    if (line == "---TAGS---"){
                        inTagsSection = true;
                        continue;
                    }

                    if (inTagsSection){
                        newTags+=line+"\n";
                    }
                    else{
                        newBody += line + "\n";
                    }
                }

                n.Body = newBody;
                n.Tags = newTags;
                saveIndex();
            } 
        }

        if (NoteNum == 0){
            MainMenu();
            break;
        }

    }   
return 0;
}

int ShowNotes(){
    bool Loop = true;
    system("clear");
    int NoteNum;
    while (Loop){
        std::cout << "====CURRENT NOTES====" << std::endl;
            for (const Note& n : NewNotes) {
            std::cout << "ID: " << n.ID<< "\n";
            std::cout << "Date: " << n.Date<< "\n";
            std::cout << "Title: " << n.Title << "\n\n";
            std::cout << "------------------------" << std::endl;
        }

        std::cout << "Which note would you like to open: ";

        while (!(std::cin >> NoteNum)){ //Keeps asking until the user enters the correct data type.
            std::system("clear");
            std::cout << "Invalid input!\nPlease enter a number!\n";
            std::cin.clear();//Resets input errors
            std::cin.ignore(1000, '\n'); //Removes bad input
        }
        if (NoteNum == 0){
            MainMenu();
            Loop = false;
            break;
        }
        for (const Note& n : NewNotes){
            if (NoteNum == n.ID){
                system("clear");
                std::cout << "ID: " << n.ID<< "\n";
                std::cout << "Date: " << n.Date<< "\n";
                std::cout << "Title: " << n.Title << "\n";
                std::cout << "Body: " << n.Body << "\n\n";
                std::cout << "------------------------" << std::endl;
            }
        }
    }
return 0;
}

int NewNote(){
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string NoteTitle;
    std::string NoteBody;
    std::string NoteTags;
    
    system("clear");
    while(true){
        bool titleExists = false;
        std::cout << "What would you want to name this note: ";
        std::getline(std::cin, NoteTitle); 
        for (const Note& n : NewNotes){
            if (NoteTitle == n.Title){
                titleExists = true;
                break;
            }
        }

        if (titleExists){
        system("clear");
        std::cout << "Note name already exists!" << std::endl;
        }
        else{
            break;
        }
    }

    std::cout << "Enter Notes: ";
    std::getline(std::cin, NoteBody);
    std::cout << "(Begin with #)(Optional)Enter Tags: ";
    std::getline(std::cin, NoteTags);

    std::string filename = path + NoteTitle + "_" + date + ".txt";

    std::ofstream NoteFile(filename);
    if (NoteFile.is_open()){
        NoteFile << NoteBody << "\n\n\n\n" << "---TAGS---" << "\n" << NoteTags <<  std::endl;
         NoteFile.close();
    }
    else if (!NoteFile.is_open()){
        std::cerr << "Error opening file for writing!" << std::endl;
    }

    NewNotes.push_back({nextID++, date , NoteTitle , NoteBody ,NoteTags});
    saveIndex();
    MainMenu();
    return 0;
}

int MainMenu(){
    system("clear");
    int MenuOption;
     while(true){
        std::cout << "Welcome to Note Forge." << std::endl;
        std::cout << "1.New Note" << std::endl;
        std::cout << "2.Show Notes" << std::endl;
        std::cout << "3.Edit Notes" << std::endl;
        std::cout << "4.Delete Notes" << std::endl;
        std::cout << "5.Search Notes" << std::endl;
        std::cout << "6.Exit" << std::endl;
        std::cout << "Please enter a option: ";

        while (!(std::cin >> MenuOption)){ //Keeps asking until the user enters the correct data type.
            std::system("clear");
            std::cout << "Invalid input!\nPlease enter a number!\n";
            std::cin.clear();//Resets input errors
            std::cin.ignore(1000, '\n'); //Removes bad input
            break;
        }
        if(MenuOption == 1){NewNote(); break;}
        else if(MenuOption == 2){ShowNotes(); break;}
        else if(MenuOption == 3){EditNotes(); break;}
        else if(MenuOption == 4){DeleteNotes(); break;}
        else if(MenuOption == 5){SearchNotes(); break;}
        else if(MenuOption == 6){exit(0);}
        else if(MenuOption > 6) {
            std::system("clear");
            std::cout << "Please choose a number below or equal to 5."<< std::endl;
        }    
    }
return MenuOption;
}

int main(){
    loadIndex();
    MainMenu();
    return 0;
}

