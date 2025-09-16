#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace std;

const string FILENAME = "Clientss.txt";
const string Separator = "#//#";

void ShowMainMenue();
void AddNewClient();
void ShowAddNewClientsScreen();
void ShowAllClientsScreen();
void ShowDeleteClient();
void ShowUpdateClientScreen();
void ShowFindClientScreen();
void ShowEndScreen();
void ShowTransactionsMenue();
void GoBackToMainMenue();
void GoBackToTransactionsMenue();

enum enMainMenueOptions {
    ShowClientList = 1,
    AddNewClieent = 2,
    DeleteClient = 3,
    UpdateClientInformation = 4,
    FindClient = 5,
    Transactions = 6,
    Exit = 7
};

enum enTransactionsMenueOptions { Deposit = 1, Withdraw = 2, TotalBalance = 3, BackToMainMenue = 4 };

struct sClient {
    string AccountNumber;
    string PinCode;
    string Name;
    string Phone;
    double Balance = 0.0;
    bool MarkForDelete = false;
};

vector<string> SplitString(string Line) {
    vector<string> vString;
    size_t Pos;
    string Word;
    while ((Pos = Line.find(Separator)) != string::npos) {
        Word = Line.substr(0, Pos);
        if (!Word.empty()) vString.push_back(Word);
        Line.erase(0, Pos + Separator.length());
    }
    if (!Line.empty()) vString.push_back(Line);
    return vString;
}

sClient ConvertLineToRecord(const string& Line) {
    sClient Client;
    vector<string> vClient = SplitString(Line);
    if (vClient.size() >= 5) {
        Client.AccountNumber = vClient[0];
        Client.PinCode       = vClient[1];
        Client.Name          = vClient[2];
        Client.Phone         = vClient[3];
        Client.Balance       = stod(vClient[4]);
    }
    return Client;
}

vector<sClient> LoadClientsDataFromFile() {
    vector<sClient> vClient;
    fstream MyFile(FILENAME, ios::in);
    if (MyFile.is_open()) {
        string Line;
        while (getline(MyFile, Line)) vClient.push_back(ConvertLineToRecord(Line));
        MyFile.close();
    }
    return vClient;
}

string ReadClientAccountNumber() {
    cout << "Please Enter Account Number? ";
    string AccountNumber;
    getline(cin >> ws, AccountNumber);
    return AccountNumber;
}

bool ClientExistsByAccountNumber(const string& AccountNumber) {
    fstream MyFile(FILENAME, ios::in);
    if (MyFile.is_open()) {
        string Line;
        while (getline(MyFile, Line)) {
            sClient Client = ConvertLineToRecord(Line);
            if (Client.AccountNumber == AccountNumber) {
                MyFile.close();
                return true;
            }
        }
        MyFile.close();
    }
    return false;
}

sClient ReadNewClient() {
    sClient Client;
    cout << "Enter Account Number: ";
    getline(cin >> ws, Client.AccountNumber);
    while (ClientExistsByAccountNumber(Client.AccountNumber)) {
        cout << "Account Number " << Client.AccountNumber << " Already Exists, Enter Another Account Number: ";
        getline(cin >> ws, Client.AccountNumber);
    }

    cout << "Enter Pin Code: ";
    getline(cin, Client.PinCode);

    cout << "Enter Name: ";
    getline(cin, Client.Name);

    cout << "Enter Phone: ";
    getline(cin, Client.Phone);

    cout << "Enter Balance: ";
    cin >> Client.Balance;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return Client;
}

string ConvertRecordToLine(const sClient& Client) {
    string Line;
    Line += Client.AccountNumber + Separator;
    Line += Client.PinCode + Separator;
    Line += Client.Name + Separator;
    Line += Client.Phone + Separator;
    Line += to_string(Client.Balance);
    return Line;
}

void AddDataLineToFile(const sClient& Client) {
    fstream MyFile(FILENAME, ios::out | ios::app);
    if (MyFile.is_open()) {
        MyFile << ConvertRecordToLine(Client) << endl;
        MyFile.close();
    }
}

void AddNewClient() {
    sClient Client = ReadNewClient();
    AddDataLineToFile(Client);
}

void AddNewClients() {
    char AddAnotherClient = 'N';
    do {
        cout << "\nAdding New Client:\n";
        AddNewClient();
        cout << "\nDo you want to add another client? [Y/N]: ";
        cin >> AddAnotherClient;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while (tolower(AddAnotherClient) == 'y');
}

void ShowClient(const sClient& Client) {
    cout << "Account Number: " << Client.AccountNumber << endl;
    cout << "Pin Code: " << Client.PinCode << endl;
    cout << "Name: " << Client.Name << endl;
    cout << "Phone: " << Client.Phone << endl;
    cout << "Balance: " << fixed << setprecision(2) << Client.Balance << endl;
}

bool FindClientByAccountNumber(const string& AccountNumber, const vector<sClient>& vClient, sClient& Client) {
    for (const sClient& C : vClient) {
        if (C.AccountNumber == AccountNumber) {
            Client = C;
            return true;
        }
    }
    return false;
}

void MarkClientForDeleteByAccountNumber(const string& AccountNumber, vector<sClient>& vClient) {
    for (sClient& C : vClient) {
        if (C.AccountNumber == AccountNumber) {
            C.MarkForDelete = true;
            break;
        }
    }
}

void SaveClientsDataToFile(const vector<sClient>& vClient) {
    fstream MyFile(FILENAME, ios::out);
    if (MyFile.is_open()) {
        for (const sClient& C : vClient) {
            if (!C.MarkForDelete) MyFile << ConvertRecordToLine(C) << endl;
        }
        MyFile.close();
    }
}

bool DeleteClientByAccountNumber(const string& AccountNumber, vector<sClient>& vClient) {
    sClient Client;
    if (FindClientByAccountNumber(AccountNumber, vClient, Client)) {
        ShowClient(Client);
        cout << "\nAre you sure you want to delete this client? [Y/N]: ";
        char Answer = 'N';
        cin >> Answer;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (tolower(Answer) == 'y') {
            MarkClientForDeleteByAccountNumber(AccountNumber, vClient);
            SaveClientsDataToFile(vClient);
            vClient = LoadClientsDataFromFile();
            cout << "\nClient Deleted Successfully.\n";
            return true;
        } else {
            cout << "\nClient Deletion Canceled.\n";
            return false;
        }
    }
    cout << "\nClient Not Found.\n";
    return false;
}

bool UpdateClientByAccountNumber(const string& AccountNumber, vector<sClient>& vClient) {
    sClient Client;
    if (FindClientByAccountNumber(AccountNumber, vClient, Client)) {
        ShowClient(Client);
        cout << "\nEnter New Data:\n";
        sClient NewClient = ReadNewClient();
        for (sClient& C : vClient) {
            if (C.AccountNumber == AccountNumber) {
                C = NewClient;
                break;
            }
        }
        SaveClientsDataToFile(vClient);
        vClient = LoadClientsDataFromFile();
        cout << "\nClient Updated Successfully.\n";
        return true;
    } else {
        cout << "\nClient Not Found.\n";
        return false;
    }
}

void DepositAmount(sClient& Client, double Amount) {
    Client.Balance += Amount;
}

double ReadDepositAmount() {
    double Amount;
    cout << "Enter Amount to Deposit: ";
    cin >> Amount;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return Amount;
}

bool DepositAmountByAccountNumber(const string& AccountNumber, vector<sClient>& vClient, double Amount) {
    cout << "\n\nAre you sure you want to deposit " << fixed << setprecision(2) << Amount
         << " to Account Number " << AccountNumber << "? [Y/N]: ";
    char Answer = 'N';
    cin >> Answer;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (tolower(Answer) == 'y') {
        for (sClient& C : vClient) {
            if (C.AccountNumber == AccountNumber) {
                DepositAmount(C, Amount);
                SaveClientsDataToFile(vClient);
                cout << "\nNew Balance: " << fixed << setprecision(2) << C.Balance << endl;
                return true;
            }
        }
        cout << "\nClient Not Found.\n";
        return false;
    } else {
        cout << "\nDeposit Canceled.\n";
        return false;
    }
}

double ReadWithdrawAmount() {
    double Amount;
    cout << "Enter Amount to Withdraw: ";
    cin >> Amount;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return Amount;
}

void WithdrawAmount(sClient& Client, double Amount) {
    Client.Balance -= Amount;
}

bool WithdrawAmountByAccountNumber(const string& AccountNumber, vector<sClient>& vClient, double Amount) {
    cout << "\n\nAre you sure you want to withdraw " << fixed << setprecision(2) << Amount
         << " from Account Number " << AccountNumber << "? [Y/N]: ";
    char Answer = 'N';
    cin >> Answer;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (tolower(Answer) == 'y') {
        for (sClient& C : vClient) {
            if (C.AccountNumber == AccountNumber) {
                if (C.Balance >= Amount) {
                    WithdrawAmount(C, Amount);
                    SaveClientsDataToFile(vClient);
                    cout << "\nNew Balance: " << fixed << setprecision(2) << C.Balance << endl;
                    return true;
                } else {
                    cout << "\nInsufficient Balance.\n";
                    return false;
                }
            }
        }
        cout << "\nClient Not Found.\n";
        return false;
    } else {
        cout << "\nWithdraw Canceled.\n";
        return false;
    }
}

void ShowWithdrawScreen() {
    cout << "\n-------------------------------------------------\n";
    cout << " Withdraw: \n";
    cout << "-------------------------------------------------\n";
    vector<sClient> vClient = LoadClientsDataFromFile();
    sClient Client;
    string AccountNumber = ReadClientAccountNumber();
    while (!FindClientByAccountNumber(AccountNumber, vClient, Client)) {
        cout << "\nClient with Account Number " << AccountNumber << " Not Found.\n";
        AccountNumber = ReadClientAccountNumber();
    }
    ShowClient(Client);
    double amountToWithdraw = ReadWithdrawAmount();
    WithdrawAmountByAccountNumber(AccountNumber, vClient, amountToWithdraw);
}

void ShowTotalBalanceScreen() {
    cout << "\n-------------------------------------------------\n";
    cout << " Total Balance: \n";
    cout << "-------------------------------------------------\n";
    vector<sClient> vClient = LoadClientsDataFromFile();
    double TotalBalance = 0.0;
    for (const sClient& C : vClient) TotalBalance += C.Balance;
    cout << "Total Balance of All Clients: " << fixed << setprecision(2) << TotalBalance << endl;
}

void ShowAllClientsScreen() {
    vector<sClient> vClient = LoadClientsDataFromFile();
    cout << "Client List (" << vClient.size() << ") Client(s)." << endl;
    cout << "---------------------------------------------------------------------------\n";
    cout << setw(15) << left << "Account Number" << "|"
         << setw(10) << left << "Pin Code" << "|"
         << setw(20) << left << "Client Name" << "|"
         << setw(15) << left << "Phone" << "|"
         << setw(10) << left << "Balance" << endl;
    cout << "---------------------------------------------------------------------------\n";
    for (const sClient& C : vClient) {
        cout << setw(15) << left << C.AccountNumber << "|"
             << setw(10) << left << C.PinCode << "|"
             << setw(20) << left << C.Name << "|"
             << setw(15) << left << C.Phone << "|"
             << setw(10) << left << fixed << setprecision(2) << C.Balance << endl;
    }
    cout << "---------------------------------------------------------------------------\n";
}

void ShowAddNewClientsScreen() {
    cout << "\n-------------------------------------------------\n";
    cout << " Add New Client: \n";
    cout << "-------------------------------------------------\n";
    AddNewClients();
}

void ShowDeleteClient() {
    cout << "\n-------------------------------------------------\n";
    cout << " Delete Client: \n";
    cout << "-------------------------------------------------\n";
    vector<sClient> vClient = LoadClientsDataFromFile();
    string AccountNumber = ReadClientAccountNumber();
    DeleteClientByAccountNumber(AccountNumber, vClient);
}

void ShowUpdateClientScreen() {
    cout << "\n-------------------------------------------------\n";
    cout << " Update Client Information: \n";
    cout << "-------------------------------------------------\n";
    vector<sClient> vClient = LoadClientsDataFromFile();
    string AccountNumber = ReadClientAccountNumber();
    UpdateClientByAccountNumber(AccountNumber, vClient);
}

void ShowFindClientScreen() {
    cout << "\n-------------------------------------------------\n";
    cout << " Find Client: \n";
    cout << "-------------------------------------------------\n";
    vector<sClient> vClient = LoadClientsDataFromFile();
    string AccountNumber = ReadClientAccountNumber();
    sClient Client;
    if (FindClientByAccountNumber(AccountNumber, vClient, Client)) ShowClient(Client);
    else cout << "\nClient Not Found.\n";
}

void ShowDepositScreen() {
    cout << "\n-------------------------------------------------\n";
    cout << " Deposit: \n";
    cout << "-------------------------------------------------\n";
    vector<sClient> vClient = LoadClientsDataFromFile();
    sClient Client;
    string AccountNumber = ReadClientAccountNumber();
    while (!FindClientByAccountNumber(AccountNumber, vClient, Client)) {
        cout << "\nClient with Account Number " << AccountNumber << " Not Found.\n";
        AccountNumber = ReadClientAccountNumber();
    }
    ShowClient(Client);
    double Amount = ReadDepositAmount();
    DepositAmountByAccountNumber(AccountNumber, vClient, Amount);
}

void ShowEndScreen() {
    cout << "\n-------------------------------------------------\n";
    cout << " Thanks For Using Our System. \n";
    cout << "-------------------------------------------------\n";
}

void GoBackToMainMenue() {
    cout << "\n\nPress any Key to go Back to Main Menue...";
    system("pause>0");
    ShowMainMenue();
}

short ReadMainMenueOption() {
    short ChoiceMenue;
    cout << "Choice What You Want To Do? [1-7]: ";
    cin >> ChoiceMenue;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return ChoiceMenue;
}

short ReadTransactionsMenueOption() {
    short ChoiceMenue;
    cout << "Choice What You Want To Do? [1-4]: ";
    cin >> ChoiceMenue;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return ChoiceMenue;
}

void PerFormTransactionsMenueOption() {
    enTransactionsMenueOptions TransactionsMenueOption =
        (enTransactionsMenueOptions)ReadTransactionsMenueOption();

    switch (TransactionsMenueOption) {
        case enTransactionsMenueOptions::Deposit:
            system("cls");
            ShowDepositScreen();
            GoBackToTransactionsMenue();
            break;
        case enTransactionsMenueOptions::Withdraw:
            system("cls");
            ShowWithdrawScreen();
            GoBackToTransactionsMenue();
            break;
        case enTransactionsMenueOptions::TotalBalance:
            system("cls");
            ShowTotalBalanceScreen();
            GoBackToTransactionsMenue();
            break;
        case enTransactionsMenueOptions::BackToMainMenue:
            system("cls");
            GoBackToMainMenue();
            break;
    }
}

void GoBackToTransactionsMenue() {
    cout << "\n\nPress any Key to go Back to Transactions Menue...";
    system("pause>0");
    ShowTransactionsMenue();
}

void ShowTransactionsMenue() {
    system("cls");
    cout << "\n-------------------------------------------------\n";
    cout << " Transactions Menue: \n";
    cout << "-------------------------------------------------\n";
    cout << "[1] Deposit\n";
    cout << "[2] Withdraw\n";
    cout << "[3] Total Balance\n";
    cout << "[4] Back To Main Menue\n";
    cout << "-------------------------------------------------\n";
    PerFormTransactionsMenueOption();
}

void PerFormMainMenueOption(enMainMenueOptions MainMenueOption) {
    switch (MainMenueOption) {
        case enMainMenueOptions::ShowClientList:
            system("cls");
            ShowAllClientsScreen();
            GoBackToMainMenue();
            break;
        case enMainMenueOptions::AddNewClieent:
            system("cls");
            ShowAddNewClientsScreen();
            GoBackToMainMenue();
            break;
        case enMainMenueOptions::DeleteClient:
            system("cls");
            ShowDeleteClient();
            GoBackToMainMenue();
            break;
        case enMainMenueOptions::UpdateClientInformation:
            system("cls");
            ShowUpdateClientScreen();
            GoBackToMainMenue();
            break;
        case enMainMenueOptions::FindClient:
            system("cls");
            ShowFindClientScreen();
            GoBackToMainMenue();
            break;
        case enMainMenueOptions::Transactions:
            system("cls");
            ShowTransactionsMenue();
            break;
        case enMainMenueOptions::Exit:
            system("cls");
            ShowEndScreen();
            break;
    }
}

void ShowMainMenue() {
    system("cls");
    cout << "====================================================\n";
    cout << setw(30) << "Main Menue Screen" << endl;
    cout << "====================================================\n";
    cout << "[1] Show Client List\n";
    cout << "[2] Add New Client\n";
    cout << "[3] Delete Client\n";
    cout << "[4] Update Client Information\n";
    cout << "[5] Find Client\n";
    cout << "[6] Transactions\n";
    cout << "[7] Exit\n";
    cout << "====================================================\n";
    PerFormMainMenueOption((enMainMenueOptions)ReadMainMenueOption());
}

int main() {
    ShowMainMenue();
    return 0;
}