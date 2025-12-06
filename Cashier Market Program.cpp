#include <iostream>// For input/output (cout, cin)
#include <string>// For string handling
#include <vector>// For dynamic array (vector)
#include <thread>// For sleeping (used in typewriter effect)
#include <chrono>// For time durations (used in typewriter)
#include <cstdlib>// For system() function (console clear)
#include <iomanip>// For formatting numbers (like 2 decimal points)
#include <sstream>// For string streams (building formatted strings)
#include <fstream>// For reading/writing files
#include <limits>// For clearing input buffer and max values
#include <algorithm>// For algorithms like find_if

using namespace std;// So we don’t need std:: prefix everywhere

    // Define a structure to represent a product
    struct Product {
    string name;// Name of the product
    int quantity;// Quantity in stock
    double price;// Price per unit
    };

    // Global variables
    
    // 'static' makes this inventory remember all 
    //products even when the function ends
    static vector<Product> inventory;    // List of products currently in inventory
    //I use static because without static Every time you call the function, it’s like opening a 
    //brand new notebook. Whatever you wrote last time is gone.
    //But With static You’re using the same notebook every time. Whatever you wrote last time 
    //is still there, so you can keep counting without losing track.
    
    
    // 'static' makes this soldProducts list remember all 
    //sold items even after the function ends
    static vector<Product> soldProducts; // List of products that have been sold
    
    // 'static' ensures totalEarnings keeps its value across multiple function calls, 
    // so the total money earned accumulates and doesn't reset
    static double totalEarnings = 0.0;   // Total earnings from sold products
    
    
    // 'static' makes FAST_MODE retain its value across function calls
    // It controls whether the typewriter effect is disabled (true = fast mode, false = normal)
    static bool FAST_MODE = false;// If true, typewriter effect is disabled for faster output
    
    //'static const' means LOW_STOCK_THRESHOLD is a fixed value 
    //shared across all functions/files It defines the quantity 
    //at which a product is considered "low stock" (5 units in this case)
    static const int LOW_STOCK_THRESHOLD = 5;  // Minimum stock before showing low-stock warning



    // Function to escape commas and backslashes for CSV file storage
    static string escapeCsv(const string &s) {
    /*So what is CSV? CSV stands for Comma-Separated Value
    It’s a simple file format used to store tabular data 
    (like a spreadsheet or database table) in plain text. 
    Each line represents a row, and each value (or “field”) 
    in the row is separated by a comma.

    Example:

    Name,Quantity,Price
    Paper,10,2.50
    Pen,5,1.20
    Notebook,7,3.00*/
    
    string out;
    //Initialize an empty string to store the escaped output
    
    for(char c : s){
    //Loop through each character in the input string 's'
   
    if(c == ',')
    // Check if the character is a comma
    
    out += "\\,";
    // Escape the comma by adding a backslash before it
 
    else if(c == '\\')
    // Check if the character is a backslash
   
    out += "\\\\";
    // Escape the backslash by adding another backslash
 
    else 
    out += c;
    // For all other characters, add them as they are
    }
    
    return out;
    // Return the fully escaped string
    
    //escapeCsv() → Prepares text for saving in a CSV file 
    //so commas and backslashes don’t break the format.
    }

    //UNESCAPE CSV FUNCTION
    // Function to reverse the escaping of CSV backslashes and commas
    // This converts an escaped string (from escapeCsv) back into normal text
    // Example: "Paper\\, white" becomes "Paper, white"
    static string unescapeCsv(const string &s){
    
    string out;
    //Initialize an empty string to store the unescaped output
    
    out.reserve(s.size());
    //Reserve space in advance to optimize memory allocation

    for(size_t i = 0; i < s.size(); ++i) { 
    //Loop through each character in the input string
    
    if (s[i] == '\\' && i + 1 < s.size()) { 
    //If the current character is a backslash
    
    out += s[i + 1];
    //Add the next character to the output (skip the backslash)
  
    ++i;
    //Skip the next character because it was already added
  
    } else {
  
    out += s[i];
    //Otherwise, just add the current character as is
  
    }
    }

    return out;// Return the unescaped string
    
    //unescapeCsv Reads that text back into normal readable strings 
    //for your program.
    
    }

    // Typewriter effect function: prints text character by character
   
    void typewriter(const string &text, int delay_ms = 30){
    
    if (FAST_MODE) {cout<<(text); return;} 
    // If fast mode, just print text instantly
    
    for (char c : text){
    // Loop through each character
    
    cout << c << flush;
    // Print character immediately
    
    this_thread::sleep_for(chrono::milliseconds(delay_ms)); 
    // Delay for effect
    }
    }

    // Function to clear console screen (works on Windows and POSIX)
    void clearConsole() {
    #ifdef _WIN32
    system("cls");  // Clear console in Windows
    #else
    system("clear"); // Clear console in Linux/Mac
    #endif
    }

    // Function to save inventory, sold products, and earnings to file
    void saveData() {
    ofstream file("data.txt");           // Open file for writing
    if (!file.is_open()) {               // If file failed to open
    cerr << "Warning: Unable to open data.txt for writing. Data not saved.\n";
    return;
    }

    // Save earnings
    file << "[EARNINGS]\n" << fixed << setprecision(2) << totalEarnings << "\n";

    // Save inventory
    file << "[INVENTORY]\n";
    for (const auto &p : inventory) {
    file << escapeCsv(p.name) << "," << p.quantity << "," << fixed << setprecision(2) << p.price << "\n";
    }

    // Save sold products
    file << "[SOLD]\n";
    for (const auto &s : soldProducts) {
    file << escapeCsv(s.name) << "," << s.quantity << "," << fixed << setprecision(2) << s.price << "\n";
    }
    }

    // Function to load data from file into memory
    void loadData() {
    ifstream file("data.txt");           // Open file for reading
    if (!file.is_open()) return;         // If file doesn't exist, do nothing

    string line, section;                // Line content and current section
    while (getline(file, line)) {        // Read file line by line
    if (line.empty()) continue;      // Skip empty lines
    if (line == "[EARNINGS]" || line == "[INVENTORY]" || line == "[SOLD]") {
    section = line;              // Track which section we are in
    continue;
    }

    try { // Try-catch to safely parse lines
    if (section == "[EARNINGS]") {
    string trimmed = line;
    // Trim whitespace from start
    trimmed.erase(trimmed.begin(), find_if(trimmed.begin(), trimmed.end(), [](char ch){return !isspace((unsigned char)ch);}));
    // Trim whitespace from end
    trimmed.erase(find_if(trimmed.rbegin(), trimmed.rend(), [](char ch){return !isspace((unsigned char)ch);}).base(), trimmed.end());
    if (!trimmed.empty()) totalEarnings = stod(trimmed); // Convert string to double
    } else if (section == "[INVENTORY]" || section == "[SOLD]") {
    vector<string> fields;
    string cur;
    bool esc = false;
    // Parse CSV line while handling escaped commas
    for (size_t i = 0; i < line.size(); ++i) {
    char c = line[i];
    if (esc) { cur += c; esc = false; continue; } // Add escaped character
    if (c == '\\') { esc = true; continue; }// Set escape flag
    if (c == ',') { fields.push_back(cur); cur.clear(); } // Split by comma
    else cur += c;
    }
    fields.push_back(cur); // Add last field
    if (fields.size() < 3) continue; // Skip malformed lines

    string name = unescapeCsv(fields[0]); // Unescape product name
    int q = stoi(fields[1]);// Quantity as int
    double pr = stod(fields[2]);// Price as double

    if (section == "[INVENTORY]") inventory.push_back({name, q, pr}); // Add to inventory
    else soldProducts.push_back({name, q, pr});// Add to sold products
    }
    } catch (...) { continue; } // Ignore errors
    }
    }

    // Function to clear leftover input from cin (prevents input bugs)
    static void clearInputLine() {
    cin.clear(); // Clear error flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Remove everything left in input buffer
    }

    // Function to prompt for integer input safely
    int promptInt(const string &prompt, int minVal = numeric_limits<int>::min(), int maxVal = numeric_limits<int>::max()) {
    while (true) {
    cout << prompt;     // Show message
    int value;
    if (cin >> value) { // Try reading integer
    if (value < minVal || value > maxVal) { // Check range
    cout << "Value out of range. Try again.\n";
    clearInputLine();
    continue;
    }
    clearInputLine(); // Clear leftover input
    return value;     // Return valid input
    } else {
    cout << "Invalid input. Please enter a number.\n";
    clearInputLine();
    }
    }
    }

    // Function to prompt for double input safely
    double promptDouble(const string &prompt, double minVal = numeric_limits<double>::lowest(), double maxVal = numeric_limits<double>::max()) {
    while (true) {
    cout << prompt;
    double value;
    if (cin >> value) {
    if (value < minVal || value > maxVal) {
    cout << "Value out of range. Try again.\n";
    clearInputLine();
    continue;
    }
    clearInputLine();
    return value;
    } else {
    cout << "Invalid input. Please enter a number.\n";
    clearInputLine();
    }
    }
    }

    // Function to prompt for a string input
    string promptLine(const string &prompt) {
    cout << prompt;// Show message
    string s;
    getline(cin, s);// Read entire line
    return s;// Return string
    }

    // Function to display a list of products with optional indexing and low-stock alerts
    void displayInventory(const vector<Product> &items, bool showIndex = true, bool lowStockAlert = true) {
    if (items.empty()) {  // If no items, print message
    cout << "No products.\n";
    return;
    }
    for (size_t i = 0; i < items.size(); ++i) {
    stringstream ss;
    ss << fixed << setprecision(2) << items[i].price; // Format price with 2 decimals
    if (showIndex) cout << to_string(i+1) << ". ";     // Show index if requested
    cout << items[i].name << " | Qty: " << items[i].quantity << " | Price: " << ss.str();
    if (lowStockAlert && items[i].quantity > 0 && items[i].quantity < LOW_STOCK_THRESHOLD)
    cout << "  <-- LOW STOCK!";                   // Low stock warning
    else if (lowStockAlert && items[i].quantity == 0)
    cout << "  <-- OUT OF STOCK!";                // Out of stock warning
    cout << "\n---------------------------------\n";    // Separator line
    }
    }

    // Function to add products to inventory
    void addProducts() {
    char more = 'Y';
    while (more == 'Y' || more == 'y') {
    clearConsole();
    typewriter("__________ ADD PRODUCTS __________\n", 5);
    string name = promptLine("Enter product name (commas allowed): "); // Get name
    if (name.empty()) { cout << "Product name cannot be empty.\n"; continue; }
    int qty = promptInt("Enter quantity: ", 0); // Get quantity
    double price = promptDouble("Enter price: ", 0.0); // Get price

    bool found = false;
    for (auto &p : inventory) { // Check if product already exists
    if (p.name == name) {
    p.quantity += qty;   // Add to existing quantity
    p.price = price;    // Update price
    found = true;
    break;
    }
    }
    if (!found) inventory.push_back({name, qty, price}); // Add new product

    cout << "Product added/updated successfully!\n";
    cout << "Add more? (Y/N): ";
    cin >> more;
    clearInputLine();
    }
    saveData(); // Save updated inventory
    }

    // Function to check inventory and show products
    void checkProducts() {
    clearConsole();
    typewriter("__________ INVENTORY __________\n", 5);
    displayInventory(inventory); // Use reusable display function
    cout << "Press ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Function to sell products
    void sellProducts() {
    char more = 'Y';
    while (more == 'Y' || more == 'y') {
    clearConsole();
    if (inventory.empty()) {
    cout << "No products available to sell!\n";
    cout << "Press ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return; // Exit selling function if inventory is empty
    }

    cout << "__________ SELL PRODUCTS __________\n";
    displayInventory(inventory, true, true); // Show inventory with indices and low-stock alerts

    int choice = promptInt("Enter product number to sell: ", 1, static_cast<int>(inventory.size()));
    Product &p = inventory[choice-1]; // Reference to chosen product
    cout << "Selected: " << p.name << " (Stock: " << p.quantity << ")\n";
    int qty = promptInt("Enter quantity to sell: ", 1); // Ask how many units to sell

    if (qty > p.quantity) { // If requested quantity exceeds stock
    cout << "Not enough stock!\n";
    } else {
    p.quantity -= qty;                 // Reduce inventory
    double sale = qty * p.price;       // Calculate earnings from this sale
    totalEarnings += sale;             // Add to total earnings

    bool foundSold = false;
    for (auto &s : soldProducts) {    // Check if product exists in sold list
    if (s.name == p.name) {
    s.quantity += qty;        // Increment quantity sold
    foundSold = true;
    break;
    }
    }
    if (!foundSold) soldProducts.push_back({p.name, qty, p.price}); // Add new sold product

    stringstream ss; ss << fixed << setprecision(2) << sale;
    cout << "Sold successfully! Earned " << ss.str() << "\n";

    // Low-stock and out-of-stock alerts
    if (p.quantity > 0 && p.quantity < LOW_STOCK_THRESHOLD)
    cout << "⚠ WARNING: \"" << p.name << "\" stock is low (" << p.quantity << " remaining)!\n";
    else if (p.quantity == 0)
    cout << "⚠ \"" << p.name << "\" is now out of stock!\n";
    }

    cout << "Sell another? (Y/N): ";
    cin >> more;
    clearInputLine(); // Clear leftover input
    }
    saveData(); // Save changes after selling
    }

    // Function to delete a product from inventory
    void deleteProduct() {
    char more = 'Y';  // Variable to check if user wants to delete more products, initialized to 'Y' (Yes)

    while (more == 'Y' || more == 'y') { // Loop to allow multiple deletions as long as user says yes
    clearConsole(); // Clear the console for a clean display

    if (inventory.empty()) { // Check if inventory is empty
    cout << "No products to delete.\nPress ENTER to continue..."; // Show message if nothing to delete
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Wait for user to press ENTER
    return; // Exit the function early since there is nothing to delete
    }

    cout << "__________ DELETE PRODUCT __________\n"; // Display header for delete section

    displayInventory(inventory); // Call function to display all products in inventory with numbers

    // Prompt the user to enter which product number to delete
    // promptInt ensures valid input between 1 and inventory.size()
    int choice = promptInt("Enter product number to delete: ", 1, static_cast<int>(inventory.size()));

    string deletedName = inventory[choice-1].name; // Store the name of the product being deleted for display

    inventory.erase(inventory.begin() + (choice-1)); // Remove the product from the inventory vector

    cout << deletedName << " has been deleted from inventory.\n"; // Inform user of successful deletion

    saveData(); // Save updated inventory and other data to file to keep it persistent

    cout << "delete another? (Y/N): "; // Ask user if they want to delete another product
    cin >> more; // Read user response to continue or exit loop
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer to prevent leftover characters
    }

    // Function to view sold products
    void viewSoldProducts() {
    clearConsole();
    cout << "__________ SOLD PRODUCTS __________\n";
    displayInventory(soldProducts, false, false); // Show sold products without indices or stock alerts
    cout << "Press ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Function to view total earnings
    void viewEarnings() {
    clearConsole();
    stringstream ss;
    ss << fixed << setprecision(2) << totalEarnings; // Format total earnings
    cout << "__________ EARNINGS __________\n";
    cout << "Total Earnings: " << ss.str() << "\n";
    cout << "Press ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

     
    //Function to search products by name
    void searchProducts(){ 
    
    //Clear the console to start fresh
    clearConsole(); 

    //Check if the inventory is empty
    if (inventory.empty()) { 
    //the system inform the user if the inventory is empty
    cout << "Inventory is empty.\nPress ENTER to continue..."; 
    //Wait for the user to press ENTER before continuing
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    
    return; //Exit the function early because there are no products
    }
    //the system ask the user to enter a product name to search for
    //only letters in CAPSLOCKS allowed because the product encoded
    //in the inventory is Also in CAPSLOCK 
    string query = promptLine("\nEnter product name to search: \n"); //the promptline includes input and output together at the same line
    
    //Check if the user entered an empty string (Didn't put any letter & numbers)
    if(query.empty()){ 
    
    //Inform the user that the search area cannot be empty
    cout << "Search area cannot be empty.\nPress ENTER to continue..."; 
    
    //Wait for user to press ENTER if press enter then back to main page
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    
    return; //Exit the function early because the query is invalid
    }

    //Clear the console again before displaying search results
    clearConsole(); 
    
    //Print a header design for the search results
    cout << "\n__________ SEARCH RESULTS __________\n"; 
    // I use a variable productSearcher para obvious Yung purpose 
    //ng object it's purpose is to search through the inventory 
    //because I have a header that saves data and loads data para
    //kahit di nag rurun Yung code sa compiler ay nandun pa din Yung
    //data na na-collect ko from the previous transactions
    
    //check if any product matches the name that's in the search area
    bool found = false; 
    
    //Loop through all products in the inventory
    //productSearcher → A variable used to loop through 
    //the inventory. Its purpose is to check each product 
    //one by one to see if it matches the user’s search query.
    for (size_t productSearcher = 0; productSearcher < inventory.size(); ++productSearcher){ 
    
    //Check if the current product's name contains the search query
    if (inventory[productSearcher].name.find(query) != string::npos){ //string::npos is a constant that basically means “not found.”
    //inventory[productSearcher].name → Accesses the name of the current 
    //product in the loop.
    
    //Create a stringstream to format numbers
    stringstream ss; 
    
    //Format the price to 2 decimal places
    ss << fixed << setprecision(2) << inventory[productSearcher].price; 
            
    //Display the product number, name, quantity, and formatted price
    cout << productSearcher+1 << ". " << inventory[productSearcher].name << " | Qty: " << inventory[productSearcher].quantity << " | Price: " << ss.str(); 
            
    //If the product quantity is low but not zero, show a low stock warning
    if (inventory[productSearcher].quantity > 0 && inventory[productSearcher].quantity < LOW_STOCK_THRESHOLD) 
    cout << "  <-- LOW STOCK!"; 
    
    //If the product is completely out of stock, show an out-of-stock warning
    else if (inventory[productSearcher].quantity == 0) 
    cout << "  <-- OUT OF STOCK!"; 
            
    //Print a separator for readability
    cout << "\n---------------------------------\n"; 
    //Mark that at least one product was found
    found = true; 
    }
    }

    //If no products matched the search query
    if (!found) 
    //Inform the user that nothing matched
    cout << "No products found matching \"" << query << "\".\n"; 
    
    //Prompt the user to press ENTER before continuing
    cout << "Press ENTER to continue..."; 
    //Wait for user input
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    }

    //Main program loop
    int main() { 
    //Load previous data from the file "data.txt", collected from all previous transactions
    loadData(); 

    //Infinite loop for main menu; will keep running until the user chooses to exit
    while (true) { 
    //Clear the console before displaying the main menu
    clearConsole();
 
 
 
    //menu display and list ng mga functions, each options has its own functions 
 
    //1. add Products para ano? syempre mag add ning products 🫩
 
    //2. Check products para ano? syempre mag check ning products😮‍💨
    // check products will redirect you to the inventory para ano? para mag check malamang
 
    //3.sell products, para ano? syempre mag sell ng products, this will also redirect you
    //to the inventory but this time it will ask kung Anong number kang item ang babakalon
    //kada item ay may sariling number, pakatapos mong ibigay ang number ng product ay 
    //hihingi ng specific amount of item Yung system sayo 
 
    //4. delete products this will also redirect you to the inventory para mag choose ng number
    //ng product para I delete mo, now the system will just allow you to delete manually, meaning 
    //pag nka delete kana ma reredirect kana sa main page na Puno ng functions, pero NVM/ Nilagyan 
    //kuna ng option kung mag dedelete paba or Hindi na WHAHAHA😌
 
    //5. viewSoldProducts ma reredirect ka sa inventory but this time makikita mo lahat ng na benta 
    //mo lng na product and quantity ng products na na-benta mo.
  
    //6. viewEarnings this will display the total money you earned sa lahat ng products na na-benta mo
    //now diko sya Nilagyan ng item na na-sold mo para ma earn mo Yung amount ng money na Yun Kase ngani
    //ugwa ngani ning view sold products ngani, dun nlng tignan😃
 
    //7. Fast mode ang function nito ay to disable the typewriter effect for fast work and to save time kung
    //Maraming bumibili syempre kailangan natin maging efficient but my program has both efficiency and style
    //kumbaga mabilis na pogi pa my typewriter pa for smooth experience, no to boringness😎

    //8. Exit and save all transactions data, yun lng boring🫩, but this function is very important for 
    //supermarkets Kase para ma track nila ang sales history for reporting purposes. COOL😎.

    //9. Search Product the system will ask you to enter the name of the product only on CAPSLOCK/BIG LETTERS!😑😑 
    //if not CAPSLOCK/big letters, the system will not found your products kahit na available pa Yan kung Hindi 
    //naka CAPSLOCK Yung product na sinearch mo di din ma hahanap, this is very useful if you have hundreds of products 
    //so follow rules para ano?? para walang problema 
    
    typewriter("_______________________________________\n" , 3);
    typewriter("|                                     |\n" , 3);
    typewriter("|           WELCOME TO C+COUNT        |\n" , 3);
    typewriter("|_____________________________________|\n" , 3);
    typewriter(" 1 | Add Products\n" , 3);
    typewriter(" 2 | Check Products\n" , 3);
    typewriter(" 3 | Sell Products\n" , 3);
    typewriter(" 4 | Delete Product\n" , 3);
    typewriter(" 5 | View Sold Products\n" , 3);
    typewriter(" 6 | View Earnings\n" , 3);
    typewriter(" 7 | Fast Mode (currently " , 3); cout<< (FAST_MODE ? "ON" : "OFF") << ")\n";
    typewriter(" 8 | Exit (save data)\n" , 3);
    typewriter(" 9 | Search Product by Name\n" , 3);

    int choice = promptInt("Enter choice: ", 1, 9); // Get user menu choice
    
    
    //yang mga case po is like an else if but I use switch
    //para less hassle pag code and specialy para I ca-callout
    //ko nlng sya galing sa void and para less line of code
    //Each of this choice is a container of functions, that's why I name it its functions so less
    //confusion for me or the reader.
    
    switch (choice) { // Execute selected option
    case 1: addProducts(); break;
    case 2: checkProducts(); break;
    case 3: sellProducts(); break;
    case 4: deleteProduct(); break;
    case 5: viewSoldProducts(); break;
    case 6: viewEarnings(); break;
    case 7:
    FAST_MODE = !FAST_MODE; //turn on or off typewriter fast mode
    cout << "Fast mode is now " << (FAST_MODE ? "ON" : "OFF") << ". Press ENTER to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    break;
    case 8:
    saveData(); // Save data before exit
    cout << "Data saved. Exiting...\n";
    return 0;
    case 9: searchProducts(); break;
    }
    }

    return 0; // Program end
}