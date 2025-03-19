#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <limits>
#include <chrono>
#include <ctime>
#include <queue>
#include <unordered_map>
#include <memory>
#include "date.h"
#include "data_structures.h"
using namespace std;

// Add category enum for expense categorization
enum class Category {
    INCOME,
    FOOD,
    HOUSING,
    TRANSPORTATION,
    ENTERTAINMENT,
    UTILITIES,
    HEALTHCARE,
    EDUCATION,
    OTHER
};

// Function to convert Category to string
string categoryToString(Category cat) {
    switch(cat) {
        case Category::INCOME: return "Income";
        case Category::FOOD: return "Food";
        case Category::HOUSING: return "Housing";
        case Category::TRANSPORTATION: return "Transportation";
        case Category::ENTERTAINMENT: return "Entertainment";
        case Category::UTILITIES: return "Utilities";
        case Category::HEALTHCARE: return "Healthcare";
        case Category::EDUCATION: return "Education";
        case Category::OTHER: return "Other";
        default: return "Unknown";
    }
}

// Function to convert string to Category
Category stringToCategory(const string& str) {
    if (str == "Income") return Category::INCOME;
    if (str == "Food") return Category::FOOD;
    if (str == "Housing") return Category::HOUSING;
    if (str == "Transportation") return Category::TRANSPORTATION;
    if (str == "Entertainment") return Category::ENTERTAINMENT;
    if (str == "Utilities") return Category::UTILITIES;
    if (str == "Healthcare") return Category::HEALTHCARE;
    if (str == "Education") return Category::EDUCATION;
    return Category::OTHER;
}

class Transaction {
protected:
    double amount;
    string description;
    Date date;
    Category category;

public:
    Transaction(double amt, const string &des, Category cat = Category::OTHER) {
        amount = amt;
        description = des;
        date = Date(); // Current date
        category = cat;
    }
    
    Transaction(double amt, const string &des, const Date& dt, Category cat = Category::OTHER) {
        amount = amt;
        description = des;
        date = dt;
        category = cat;
    }

    virtual void display() {
        cout << setw(12) << date << setw(15) << amount << setw(15) << categoryToString(category) << setw(20) << description;
    }
    
    virtual double getAmount() const {
        return amount;
    }
    
    Category getCategory() const {
        return category;
    }
    
    Date getDate() const {
        return date;
    }
    
    string getDescription() const {
        return description;
    }
    
    // For file I/O
    virtual void saveToFile(ofstream& file) const {
        file << "T " << amount << " " << description << " " << date << " " << categoryToString(category) << endl;
    }
    
    virtual string getType() const {
        return "Transaction";
    }
};

class Income : public Transaction {
public:
    Income(double amt, const string& des, Category cat = Category::INCOME) 
        : Transaction(amt, des, cat) {}
    
    Income(double amt, const string& des, const Date& dt, Category cat = Category::INCOME) 
        : Transaction(amt, des, dt, cat) {}

    void display() override {
        cout << setw(15) << "Income";
        Transaction::display();
        cout << endl;
    }
    
    void saveToFile(ofstream& file) const override {
        file << "I " << amount << " " << description << " " << date << " " << categoryToString(category) << endl;
    }
    
    string getType() const override {
        return "Income";
    }
};

class Expenditure : public Transaction {
public:
    Expenditure(double amt, const string &des, Category cat = Category::OTHER) 
        : Transaction(amt, des, cat) {}
    
    Expenditure(double amt, const string &des, const Date& dt, Category cat = Category::OTHER) 
        : Transaction(amt, des, dt, cat) {}

    void display() override {
        cout << setw(15) << "Expenditure";
        Transaction::display();
        cout << endl;
    }
    
    void saveToFile(ofstream& file) const override {
        file << "E " << amount << " " << description << " " << date << " " << categoryToString(category) << endl;
    }
    
    string getType() const override {
        return "Expenditure";
    }
};

class Investment {
protected:
    double amount;
    int duration;
    Date startDate;

public:
    Investment(double amt, int dur) {
        amount = amt;
        duration = dur;
        startDate = Date(); // Current date
    }
    
    Investment(double amt, int dur, const Date& dt) {
        amount = amt;
        duration = dur;
        startDate = dt;
    }

    virtual void display() {
        cout << setw(15) << amount << setw(15) << duration << setw(15) << startDate;
    }

    virtual double maturityAmount() {
        return amount;
    }
    
    double getAmount() const {
        return amount;
    }
    
    int getDuration() const {
        return duration;
    }
    
    Date getStartDate() const {
        return startDate;
    }
    
    // For file I/O
    virtual void saveToFile(ofstream& file) const {
        file << "INV " << amount << " " << duration << " " << startDate << endl;
    }
    
    virtual string getType() const {
        return "Investment";
    }
};

class SIP : public Investment {
private:
    double monthly;

public:
    SIP(double amt, int dur, double monAmt) : Investment(amt, dur) {
        monthly = monAmt;
    }
    
    SIP(double amt, int dur, double monAmt, const Date& dt) : Investment(amt, dur, dt) {
        monthly = monAmt;
    }

    void display() override {
        cout << setw(15) << "SIP";
        Investment::display();
        cout << setw(20) << monthly << endl;
    }

    double maturityAmount() override {
        double final = amount * pow(1 + (0.096/12), duration*12);
        return final + (monthly * 12 * duration);
    }
    
    double getMonthly() const {
        return monthly;
    }
    
    void saveToFile(ofstream& file) const override {
        file << "SIP " << amount << " " << duration << " " << startDate << " " << monthly << endl;
    }
    
    string getType() const override {
        return "SIP";
    }
};

class FD : public Investment {
public:
    FD(double amt, int dur) : Investment(amt, dur) {}
    
    FD(double amt, int dur, const Date& dt) : Investment(amt, dur, dt) {}

    void display() override {
        cout << setw(15) << "FD";
        Investment::display();
        cout << endl;
    }

    double maturityAmount() override {
        return amount * pow((1 + 0.071), duration);
    }
    
    void saveToFile(ofstream& file) const override {
        file << "FD " << amount << " " << duration << " " << startDate << endl;
    }
    
    string getType() const override {
        return "FD";
    }
};

class FinanceManager {
private:
    // Add new member variables
    std::priority_queue<UpcomingPayment, std::vector<UpcomingPayment>, PaymentCompare> upcomingPayments;
    Trie descriptionTrie;
    TransactionIndex transactionIndex;

public:
    vector<Transaction*> transactions;
    vector<Investment*> investments;

    FinanceManager() {}
    
    ~FinanceManager() {
        // Clean up memory
        for (auto t : transactions) {
            delete t;
        }
        for (auto i : investments) {
            delete i;
        }
    }

    void addTransaction(Transaction* t) {
        transactions.push_back(t);
        descriptionTrie.insert(t->getDescription());
        std::string id = transactionIndex.addTransaction(t);
        // You might want to store the ID somewhere in the Transaction class
    }

    void addInvestment(Investment* i) {
        investments.push_back(i);
    }

    void displayRecord(double balance) {
        cout << "-----------------------------------\n";
        cout << "|        Personal Finance        |\n";
        cout << "-----------------------------------\n";

        cout << "\n||--BALANCE--: " << fixed << setprecision(2) << balance << "||" << endl;

        cout << "\n--SAVINGS--: \n";
        cout << setw(15) << "Type" << setw(12) << "Date" << setw(15) << "Amount" << setw(15) << "Category" << setw(20) << "Description" << endl;
        cout << string(77, '-') << endl;
        for (auto t : transactions) {
            t->display();
        }

        cout << "\n--INVESTMENTS--\n";
        cout << setw(15) << "Type" << setw(15) << "Amount" << setw(15) << "Duration" << setw(15) << "Start Date" << setw(20) << "Monthly amount" << endl;
        cout << string(80, '-') << endl;
        for (auto i : investments) {
            i->display();
        }
    }
    
    // Generate monthly report
    void generateMonthlyReport(int month, int year) {
        cout << "\n----- Monthly Report for " << month << "/" << year << " -----\n";
        
        double totalIncome = 0.0;
        double totalExpense = 0.0;
        map<Category, double> categoryExpenses;
        
        for (auto t : transactions) {
            Date date = t->getDate();
            if (date.toString().find("/" + to_string(month) + "/" + to_string(year)) != string::npos) {
                if (t->getType() == "Income") {
                    totalIncome += t->getAmount();
                } else if (t->getType() == "Expenditure") {
                    totalExpense += t->getAmount();
                    categoryExpenses[t->getCategory()] += t->getAmount();
                }
            }
        }
        
        cout << "Total Income: " << fixed << setprecision(2) << totalIncome << endl;
        cout << "Total Expenses: " << fixed << setprecision(2) << totalExpense << endl;
        cout << "Net Savings: " << fixed << setprecision(2) << (totalIncome - totalExpense) << endl;
        
        cout << "\nExpense Breakdown by Category:\n";
        for (const auto& pair : categoryExpenses) {
            cout << setw(20) << categoryToString(pair.first) << ": " << fixed << setprecision(2) << pair.second;
            // Show percentage of total expenses
            if (totalExpense > 0) {
                cout << " (" << fixed << setprecision(1) << (pair.second / totalExpense * 100) << "%)";
            }
            cout << endl;
        }
    }
    
    // Save data to file
    bool saveToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Save transactions
        file << transactions.size() << endl;
        for (auto t : transactions) {
            t->saveToFile(file);
        }
        
        // Save investments
        file << investments.size() << endl;
        for (auto i : investments) {
            i->saveToFile(file);
        }
        
        file.close();
        return true;
    }
    
    // Load data from file
    bool loadFromFile(const string& filename, double& balance) {
        ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        // Clear existing data
        for (auto t : transactions) delete t;
        for (auto i : investments) delete i;
        transactions.clear();
        investments.clear();
        
        // Load transactions
        int transactionCount;
        file >> transactionCount;
        file.ignore(); // Skip newline
        
        for (int i = 0; i < transactionCount; i++) {
            char type;
            double amount;
            string description, categoryStr;
            int day, month, year;
            
            file >> type >> amount;
            file.ignore(); // Skip space
            
            // Read description (may contain spaces)
            getline(file, description, ' ');
            
            // Read date
            file >> day >> month >> year;
            Date date(day, month, year);
            
            // Read category
            file >> categoryStr;
            
            Category category = stringToCategory(categoryStr);
            
            if (type == 'I') {
                transactions.push_back(new Income(amount, description, date, category));
                balance += amount;
            } else if (type == 'E') {
                transactions.push_back(new Expenditure(amount, description, date, category));
                balance -= amount;
            }
        }
        
        // Load investments
        int investmentCount;
        file >> investmentCount;
        file.ignore(); // Skip newline
        
        for (int i = 0; i < investmentCount; i++) {
            string type;
            double amount;
            int duration;
            int day, month, year;
            
            file >> type >> amount >> duration;
            file >> day >> month >> year;
            Date startDate(day, month, year);
            
            if (type == "SIP") {
                double monthly;
                file >> monthly;
                investments.push_back(new SIP(amount, duration, monthly, startDate));
            } else if (type == "FD") {
                investments.push_back(new FD(amount, duration, startDate));
            }
            
            balance -= amount; // Deduct investment amount from balance
        }
        
        file.close();
        return true;
    }

    // Add new methods
    void addUpcomingPayment(const Date& date, const std::string& desc, double amount, bool isInvestment = false) {
        upcomingPayments.push(UpcomingPayment(date, desc, amount, isInvestment));
    }
    
    void displayUpcomingPayments() {
        auto tempQueue = upcomingPayments;
        cout << "\n--UPCOMING PAYMENTS--\n";
        cout << setw(12) << "Date" << setw(20) << "Description" << setw(15) << "Amount" << setw(15) << "Type" << endl;
        cout << string(62, '-') << endl;
        
        while (!tempQueue.empty()) {
            const auto& payment = tempQueue.top();
            cout << setw(12) << payment.dueDate 
                 << setw(20) << payment.description 
                 << setw(15) << fixed << setprecision(2) << payment.amount
                 << setw(15) << (payment.isInvestment ? "Investment" : "Payment") << endl;
            tempQueue.pop();
        }
    }
    
    std::vector<std::string> getDescriptionSuggestions(const std::string& prefix) {
        return descriptionTrie.getSuggestions(prefix);
    }
    
    Transaction* findTransactionById(const std::string& id) {
        return transactionIndex.getTransaction(id);
    }
};

class User {
public:
    FinanceManager manager;
    double balance;
    string username;
    string dataFile;

    User(double initialBalance, const string& name = "default") {
        balance = initialBalance;
        username = name;
        dataFile = username + "_finance_data.txt";
        
        // Try to load existing data
        if (!manager.loadFromFile(dataFile, balance)) {
            cout << "No existing data found. Starting with a fresh account.\n";
        } else {
            cout << "Loaded existing data for " << username << ".\n";
        }
    }
    
    ~User() {
        // Save data when user object is destroyed
        saveData();
    }
    
    bool saveData() {
        return manager.saveToFile(dataFile);
    }

    void operations() {
        int choice = -1;
        while (choice != 0) {
            system("cls");
            cout << "\n--OPTIONS--\n";
            cout << "1. Record INCOME\n";
            cout << "2. Record EXPENDITURE\n";
            cout << "3. Make Investment\n";
            cout << "4. Finance Information\n";
            cout << "5. Investment Information\n";
            cout << "6. Monthly Report\n";
            cout << "7. Save Data\n";
            cout << "8. Add upcoming payment\n";
            cout << "0. Exit\n";
            cout << "Enter choice : ";
            
            // Input validation
            while (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number: ";
            }
            
            system("cls");
            switch (choice) {
                case 1: {
                    double amt;
                    string desc;
                    cout << "Enter amount : ";
                    while (!(cin >> amt) || amt <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid amount. Please enter a positive number: ";
                    }
                    
                    cout << "Enter description : ";
                    cin.ignore();
                    getline(cin, desc);
                    
                    manager.addTransaction(new Income(amt, desc));
                    balance += amt;
                    cout << "\nIncome of " << fixed << setprecision(2) << amt << " recorded successfully!\n";
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }

                case 2: {
                    double amt;
                    string desc;
                    Category category;
                    
                    cout << "Enter amount: ";
                    while (!(cin >> amt) || amt <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid amount. Please enter a positive number: ";
                    }
                    
                    if (balance - amt < 1000) {
                        cout << "Error: Balance cannot go below 1000." << endl;
                        system("pause");
                        break;
                    }
                    
                    cin.ignore();
                    cout << "Enter description: ";
                    getline(cin, desc);
                    
                    // Category selection
                    cout << "\nSelect category:\n";
                    cout << "1. Food\n";
                    cout << "2. Housing\n";
                    cout << "3. Transportation\n";
                    cout << "4. Entertainment\n";
                    cout << "5. Utilities\n";
                    cout << "6. Healthcare\n";
                    cout << "7. Education\n";
                    cout << "8. Other\n";
                    cout << "Enter choice (1-8): ";
                    
                    int catChoice;
                    while (!(cin >> catChoice) || catChoice < 1 || catChoice > 8) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid choice. Please enter a number between 1 and 8: ";
                    }
                    
                    switch (catChoice) {
                        case 1: category = Category::FOOD; break;
                        case 2: category = Category::HOUSING; break;
                        case 3: category = Category::TRANSPORTATION; break;
                        case 4: category = Category::ENTERTAINMENT; break;
                        case 5: category = Category::UTILITIES; break;
                        case 6: category = Category::HEALTHCARE; break;
                        case 7: category = Category::EDUCATION; break;
                        default: category = Category::OTHER; break;
                    }
                    
                    manager.addTransaction(new Expenditure(amt, desc, category));
                    balance -= amt;
                    cout << "\nExpenditure of " << fixed << setprecision(2) << amt << " recorded successfully!\n";
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }

                case 3: {
                    makeInvestment();
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }

                case 4: {
                    manager.displayRecord(balance);
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }

                case 5: {
                    cout << "--MATURITY AMOUNTS--||\n";
                    if (manager.investments.empty()) {
                        cout << "No investments found.\n";
                    } else {
                        for (size_t i = 0; i < manager.investments.size(); i++) {
                            Investment* inv = manager.investments[i];
                            cout << "\nInvestment " << i + 1 << " : " << fixed << setprecision(2) << inv->maturityAmount() << " Rs" << endl;
                            cout << setw(15) << "Type" << setw(15) << "Amount" << setw(15) << "Duration" << setw(15) << "Start Date" << setw(20) << "Monthly amount" << endl;
                            cout << string(80, '-') << endl;
                            inv->display();
                        }
                    }
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }
                
                case 6: {
                    int month, year;
                    cout << "Enter month (1-12): ";
                    while (!(cin >> month) || month < 1 || month > 12) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid month. Please enter a number between 1 and 12: ";
                    }
                    
                    cout << "Enter year: ";
                    while (!(cin >> year) || year < 2000 || year > 2100) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid year. Please enter a year between 2000 and 2100: ";
                    }
                    
                    manager.generateMonthlyReport(month, year);
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }
                
                case 7: {
                    if (saveData()) {
                        cout << "Data saved successfully!\n";
                    } else {
                        cout << "Error saving data.\n";
                    }
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }

                case 8: {
                    cout << "\n1. Add upcoming payment\n";
                    cout << "2. View upcoming payments\n";
                    cout << "3. Search transactions\n";
                    cout << "Enter choice: ";
                    
                    int subChoice;
                    cin >> subChoice;
                    
                    switch (subChoice) {
                        case 1: {
                            double amt;
                            string desc;
                            int day, month, year;
                            
                            cout << "Enter amount: ";
                            cin >> amt;
                            cin.ignore();
                            cout << "Enter description: ";
                            getline(cin, desc);
                            cout << "Enter due date (day month year): ";
                            cin >> day >> month >> year;
                            
                            manager.addUpcomingPayment(Date(day, month, year), desc, amt);
                            cout << "Upcoming payment added successfully!\n";
                            break;
                        }
                        case 2: {
                            manager.displayUpcomingPayments();
                            break;
                        }
                        case 3: {
                            string prefix;
                            cout << "Enter description prefix: ";
                            cin.ignore();
                            getline(cin, prefix);
                            
                            auto suggestions = manager.getDescriptionSuggestions(prefix);
                            cout << "\nMatching transactions:\n";
                            for (const auto& suggestion : suggestions) {
                                cout << "- " << suggestion << endl;
                            }
                            break;
                        }
                    }
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
                }

                case 0:
                    saveData();
                    cout << "Thank you for using the Finance Management System!\n";
                    break;

                default:
                    cout << "\nNo such option:(";
                    cout << "\n\n\n\n";
                    system("pause");
                    break;
            }
        }
    }

    void makeInvestment() {
        int sub = -1;
        while (sub != 0) {
            cout << "\nWhich one:\n";
            cout << "1. SIP\n";
            cout << "2. FD\n";
            cout << "0. Go back\n";
            cout << "Enter your choice : ";
            
            while (!(cin >> sub)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number: ";
            }

            switch (sub) {
                case 1: {
                    double amt, monthly;
                    int dur;
                    cout << "Enter amount : ";
                    while (!(cin >> amt) || amt <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid amount. Please enter a positive number: ";
                    }
                    
                    if (balance - amt < 1000) {
                        cout << "ERROR : Min Balance=1000\n";
                        system("pause");
                        return;
                    }
                    
                    cout << "Enter duration in yrs : ";
                    while (!(cin >> dur) || dur <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid duration. Please enter a positive number: ";
                    }
                    
                    cout << "Enter monthly investment amount : ";
                    while (!(cin >> monthly) || monthly <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid amount. Please enter a positive number: ";
                    }
                    
                    manager.addInvestment(new SIP(amt, dur, monthly));
                    balance -= amt;
                    cout << "\nSIP investment of " << fixed << setprecision(2) << amt << " recorded successfully!\n";
                    break;
                }

                case 2: {
                    double amt;
                    int dur;
                    cout << "Enter amount : ";
                    while (!(cin >> amt) || amt <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid amount. Please enter a positive number: ";
                    }
                    
                    if (balance - amt < 1000) {
                        cout << "ERROR: Min Balance=1000\n";
                        system("pause");
                        return;
                    }
                    
                    cout << "Enter duration in yrs : ";
                    while (!(cin >> dur) || dur <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid duration. Please enter a positive number: ";
                    }
                    
                    manager.addInvestment(new FD(amt, dur));
                    balance -= amt;
                    cout << "\nFD investment of " << fixed << setprecision(2) << amt << " recorded successfully!\n";
                    break;
                }

                case 0:
                    break;

                default:
                    system("cls");
                    cout << "Invalid choice.\n\n";
                    break;
            }
        }
    }
};

int main() {
    cout << "---Welcome to Finance Management System!!---\n\n";
    
    string username;
    cout << "Enter your username: ";
    getline(cin, username);
    
    if (username.empty()) {
        username = "default";
    }
    
    User user(2000, username); // Create user with initial balance 2000
    user.operations();

    return 0;
}