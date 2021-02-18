#include <stdio.h>
#include <string.h> //contains strcmp(),strcpy(),strlen(),etc
#include <ctype.h>  //contains toupper(), tolower(),etc
#include <stdlib.h> //contains rand(),etc
#include <time.h>

#define MAX_YR 9999
#define MIN_YR 1900
#define MAX_SIZE_USER_NAME 30
#define MAX_SIZE_PASSWORD 20
#define BOOK_DATABASE "Books.bin"
#define MEMBER_DATABASE "Members.bin"
#define ISSUED_DATABASE "Issued.bin"

// Macro related to the books info
#define MAX_BOOK_NAME 50
#define MAX_AUTHOR_NAME 50
#define MAX_STUDENT_NAME 50
#define MAX_CATEGORY_NAME 20
#define RETURNTIME 15
#define FINE_AMOUNT_PER_DAY 2.5

//list of global variables that can be acccessed form anywhere in program
FILE *fp, *fs;
char catagories[][15] = {"Computer", "Electronics", "Electrical", "Civil", "Mechanical", "Economics", "Physics", "Chemistry", "Maths", "Biology", "Others"};

//struct to store the date
typedef struct
{
    int yyyy; //Year
    int mm;   //Month
    int dd;   //Day
} Date;

// Struct to store various information related to the book
typedef struct
{
    int id;                        //Book ID
    char stname[MAX_STUDENT_NAME]; //Student Name
    char name[MAX_BOOK_NAME];      //Name of Book
    char Author[MAX_AUTHOR_NAME];  //Author of the book
    int quantity;                  //Quantity of books
    double Price;                  //Price of each book
    int count;                     //Count
    int rackno;                    // Rack number of the book
    char cat[MAX_CATEGORY_NAME];   // Category
    Date issued;                   // Issue date
    Date duedate;                  //Due date
} s_BooksInfo;

s_BooksInfo global_book_data;

// Struct to store the user's access credentials
typedef struct
{
    int user_id;                       //user ID
    char username[MAX_SIZE_USER_NAME]; //username
    char password[MAX_SIZE_PASSWORD];  //password
    // int admin_access;                  //Set 1 for admin access and set 0 for normal users
} user_login;

// **********************************************************************************************************
// *******************************************Few Helper Functions*******************************************
// **********************************************************************************************************

//Generate Random Rack  DONE
int rack_number()
{
    int lower = 1, upper = 10;
    srand(time(0));
    int rand_num = (rand() % (upper - lower + 1)) + 1;
    return rand_num;
}

//Print The welcome message  DONE
void welcomeMessage()
{
    system("clear");
    printf("\t\t\t###########################################################################");
    printf("\n\t\t\t############                                                   ############");
    printf("\n\t\t\t############      Library management System Project in C       ############");
    printf("\n\t\t\t############                                                   ############");
    printf("\n\t\t\t###########################################################################");
    printf("\n\t\t\t---------------------------------------------------------------------------\n");
    printf("\n\t\t\t----------------------------------------------------------------------------");
    printf("\n\n\n\n\n");
    printf("\n\t\t\t  **-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**\n");
    printf("\n\t\t\t  **-  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=   -**");
    printf("\n\t\t\t  **-  =                 WELCOME                   =   -**");
    printf("\n\t\t\t  **-  =                   TO                      =   -**");
    printf("\n\t\t\t  **-  =                 LIBRARY                   =   -**");
    printf("\n\t\t\t  **-  =               MANAGEMENT                  =   -**");
    printf("\n\t\t\t  **-  =                 SYSTEM                    =   -**");
    printf("\n\t\t\t  **-  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=   -**");
    printf("\n\t\t\t  **-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**-**\n");
    printf("\n\n\n\t\t\t Enter any key to continue.....");
    getchar();
}

//Check whether the name is valid or not
// A Name is considered invalid if it contains any number DONE
int isNameValid(const char *name)
{
    int validName = 1;
    int len = 0;
    int index = 0;
    len = strlen(name);
    for (index = 0; index < len; ++index)
    {
        if (!(isalpha(name[index])) && (name[index] != '\n') && (name[index] != ' '))
        {
            validName = 0;
            break;
        }
    }
    return validName;
}

// Function to check leap year.  DONE
//Function returns 1 if leap year
int IsLeapYear(int year)
{
    return (((year % 4 == 0) &&
             (year % 100 != 0)) ||
            (year % 400 == 0));
}

// returns 1 if given date is valid.  DONE
int isValidDate(Date *validDate)
{
    //check range of year,month and day
    if (validDate->yyyy > MAX_YR ||
        validDate->yyyy < MIN_YR)
        return 0;
    if (validDate->mm < 1 || validDate->mm > 12)
        return 0;
    if (validDate->dd < 1 || validDate->dd > 31)
        return 0;
    //Handle feb days in leap year
    if (validDate->mm == 2)
    {
        if (IsLeapYear(validDate->yyyy))
            return (validDate->dd <= 29);
        else
            return (validDate->dd <= 28);
    }
    //handle months which has only 30 days
    if (validDate->mm == 4 || validDate->mm == 6 ||
        validDate->mm == 9 || validDate->mm == 11)
        return (validDate->dd <= 30);
    return 1;
}

// Function to check whether a particular file exists or not
int isFileExists(const char *path)
{
    // Try to open file
    FILE *fp = fopen(path, "rb");
    int status = 0;
    // If file does not exists
    if (fp != NULL)
    {
        status = 1;
        // File exists hence close file
        fclose(fp);
    }
    return status;
}

// ******************************************************************************************************
// ******************************************* Functions Used *******************************************
// ******************************************************************************************************

// Function to delete the book record    DONE
void deleteBooks()
{
    system("clear");
    int found = 0;
    int bookDelete = 0;
    s_BooksInfo Book_Info = {0};
    FILE *tmpFp = NULL;
    printf("\n\t\t\t********************************* BOOK DELETE SECTION *********************************");
    fp = fopen(BOOK_DATABASE, "rb");
    if (fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    // Creating a temporary file to hold the data
    tmpFp = fopen("tmp.bin", "wb");
    if (tmpFp == NULL)
    {
        fclose(fp);
        printf("File is not opened\n");
        exit(1);
    }
    printf("\n\t\t\tEnter the ID of the Book to be deleted : ");
    scanf("%d", &bookDelete);
    while (fread(&Book_Info, sizeof(Book_Info), 1, fp))
    {
        if (Book_Info.id != bookDelete)
        {
            // If the ID of the book in the BOOK DATABASE does not match the
            // ID of the entered book then the book's info is written into
            // the temporary file and it is skipped if the IDs match
            fwrite(&Book_Info, sizeof(Book_Info), 1, tmpFp);
        }
        else
        {
            found = 1;
        }
    }
    if (found)
    {
        printf("\n\t\t\tRecord deleted successfully.....");
        getchar();
        getchar();
    }
    else
    {
        printf("\n\t\t\tRecord not found");
        getchar();
        getchar();
    }
    fclose(fp);
    fclose(tmpFp);
    // Removing the original BOOK DATABASE and renaming the temporary file created with the
    // name of the BOOK DATABASE
    remove(BOOK_DATABASE);
    rename("tmp.bin", BOOK_DATABASE);
}

// Function to delete the book in issued database    DONE
// It takes the book ID as input
void deleteIssuedBooks(int bookID)
{
    system("clear");
    int found = 0;
    s_BooksInfo Book_Info = {0};
    // Creating a temporary file to hold the data
    FILE *tmpFp = NULL;
    fp = fopen(ISSUED_DATABASE, "rb");
    if (fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    tmpFp = fopen("tmp.bin", "wb");
    if (tmpFp == NULL)
    {
        fclose(fp);
        printf("File is not opened\n");
        exit(1);
    }

    // Reading Data one chunk at a time
    // Chunk size is the same as the size of the s_BooksInfo struct

    while (fread(&Book_Info, sizeof(Book_Info), 1, fp))
    {
        if (Book_Info.id != bookID)
        {
            fwrite(&Book_Info, sizeof(Book_Info), 1, tmpFp);
        }
        else
        {
            found = 1;
        }
    }
    if (found)
    {
        // printf("\n\t\t\tRecord deleted successfully.....");
        getchar();
        getchar();
    }
    else
    {
        printf("\n\t\t\tRecord not found");
    }
    fclose(fp);
    fclose(tmpFp);
    // Removing the original ISSUED BOOK DATABASE and renaming the temporary file created with the
    // name of the ISSUED BOOK DATABASE
    remove(ISSUED_DATABASE);
    rename("tmp.bin", ISSUED_DATABASE);
}

// Function to Add new Members  DONE
void add_new_Members(void)
{
    int status = 0;
    system("clear");
    user_login addUserInfoInDataBase = {0};
    printf("\n\t\t\tUpdate Credential");
    fp = fopen(MEMBER_DATABASE, "ab+"); //The member database contains the list of available members
    if (fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    printf("\n\t\t\tADD NEW USER");
    printf("\n\n\t\t\tENTER THE DETAILS BELOW:");
    printf("\n\t\t\t---------------------------------------------------------------------------\n");
    printf("\n\n\t\t\tEnter the User ID:");
    fflush(stdin);
    scanf("%d", &addUserInfoInDataBase.user_id);
    getchar();
    do
    {
        printf("\n\t\t\tUser Name  = ");
        fgets(addUserInfoInDataBase.username, MAX_SIZE_USER_NAME, stdin);
        status = isNameValid(addUserInfoInDataBase.username);
        if (!status)
        {
            printf("\n\t\t\tName contain invalid character. Please enter again.");
        }
    } while (!status);
    status = 0;
    do
    {
        char confirm_password[MAX_SIZE_PASSWORD];
        printf("\n\t\t\tEnter Password  = ");
        fflush(stdin);
        fgets(addUserInfoInDataBase.password, MAX_SIZE_PASSWORD, stdin);
        printf("\n\t\t\tReEnter Password  = ");
        fflush(stdin);
        fgets(confirm_password, MAX_SIZE_PASSWORD, stdin);
        status = strcmp(addUserInfoInDataBase.password, confirm_password);
        if (status)
            printf("\n\t\t\tPASSWORD DOES NOT MATCH \n\t\t\t TRY AGAIN!!");
    } while (status);

    fwrite(&addUserInfoInDataBase, sizeof(addUserInfoInDataBase), 1, fp);
    fclose(fp);
    printf("\n\t\t\tNew Member has been added successfully");
    printf("\n\t\t\tLogin Again:");
    fflush(stdin);
    getchar();
}

// Function to view the member Database  DONE
void viewMemberDatabase()
{
    system("clear");
    int found = 0;
    user_login userData = {0};
    unsigned int countBook = 1;
    printf("\n\t\t\tVIEW MEMBER DETAILS");
    fp = fopen(MEMBER_DATABASE, "rb");
    if (fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    while (fread(&userData, sizeof(userData), 1, fp))
    {
        printf("\n\t\t\t***********************************************************");
        printf("\n\t\t\tMember number = %d\n\n", countBook);
        printf("\n\t\t\tMember ID = %d\n\n", userData.user_id);
        printf("\t\t\tUsername = %s", userData.username);
        printf("\n\t\t\tPassword = %s", userData.password);
        found = 1;
        ++countBook;
    }
    fclose(fp);
    if (!found)
    {
        printf("\n\t\t\tNo Record");
    }
    printf("\n\n\t\t\tPress any key to go to main menu.....");
    getchar();
    getchar();
}

// Function to view issued books DONE
void viewIssuedBooks()
{
    system("clear");
    int found = 0;
    s_BooksInfo addBookInfoInDataBase = {0};
    int countBook = 1;
    printf("\n\t\t\tVIEW ISSUED BOOKS DETAILS");
    fp = fopen(ISSUED_DATABASE, "rb");
    if (fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    while (fread(&addBookInfoInDataBase, sizeof(addBookInfoInDataBase), 1, fp))
    {
        printf("\n\t\t\t***********************************************************");
        printf("\n\t\t\tBook Count = %d\n\n", countBook);
        printf("\t\t\tBook id = %u", addBookInfoInDataBase.id);
        printf("\n\t\t\tBook name = %s", addBookInfoInDataBase.name);
        printf("\n\t\t\tStudent Name = %s", addBookInfoInDataBase.stname);
        printf("\n\t\t\tBook issue date(day/month/year) =  (%d/%d/%d)\n\n", addBookInfoInDataBase.issued.dd,
               addBookInfoInDataBase.issued.mm, addBookInfoInDataBase.issued.yyyy);
        printf("\n\t\t\tBook return date(day/month/year) =  (%d/%d/%d)\n\n", addBookInfoInDataBase.duedate.dd,
               addBookInfoInDataBase.duedate.mm, addBookInfoInDataBase.duedate.yyyy);
        found = 1;

        ++countBook;
    }
    fclose(fp);
    if (!found)
    {
        printf("\n\t\t\tNo Record");
    }
    printf("\n\n\t\t\tPress any key to go to main menu.....");
    fflush(stdin);
    getchar();
    getchar();
}

// Function to view all the books in the library  DONE
void viewBooks()
{
    system("clear");
    int found = 0;
    s_BooksInfo Book_Data = {0};
    // FILE *fp = NULL;
    unsigned int countBook = 1;
    printf("\n\t\t\tVIEW BOOKS DETAILS");
    fp = fopen(BOOK_DATABASE, "rb");
    if (fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    while (fread(&Book_Data, sizeof(Book_Data), 1, fp))
    {
        printf("\n\t\t\t***********************************************************");
        printf("\n\t\t\tBook Count = %d\n\n", countBook);
        printf("\t\t\tBook id = %u", Book_Data.id);
        printf("\n\t\t\tBook name = %s", Book_Data.name);
        printf("\n\t\t\tBook's Author Name = %s", Book_Data.Author);
        printf("\n\t\t\tCategory = %s", Book_Data.cat);
        printf("\n\t\t\tRack Number = %d", Book_Data.rackno);
        found = 1;
        ++countBook;
    }
    fclose(fp);
    if (!found)
    {
        printf("\n\t\t\tNo Record");
    }
    printf("\n\n\t\t\tPress any key to go to main menu.....");
    getchar();
    getchar();
}

// Function to search books in library  DONE
int searchBooksInLibrary()
{
    system("clear");
    int found = 0;
    char bookName[MAX_BOOK_NAME] = {0};
    s_BooksInfo Book_Data = {0};
    fp = fopen(BOOK_DATABASE, "rb");
    if (fp == NULL)
    {
        printf("\n\t\t\tFile is not opened\n");
        exit(1);
    }
    printf("\n\t\t\tSEARCH BOOKS ");
    printf("\n\n\t\t\tEnter Book Name to search:");
    getchar();
    fgets(bookName, MAX_BOOK_NAME, stdin);
    while (fread(&Book_Data, sizeof(Book_Data), 1, fp))
    {
        if (!strcmp(Book_Data.name, bookName))
        {
            found = 1;
            break;
        }
    }
    if (found)
    {
        printf("\n\t\t\tBook id = %u\n", Book_Data.id);
        printf("\t\t\tBook name = %s", Book_Data.name);
        printf("\t\t\tBook's Author's Name = %s", Book_Data.Author);
    }
    else
    {
        printf("\n\t\t\tNo Record in Library");
        getchar();
    }
    fclose(fp);
    printf("\n\n\n\t\t\tPress any key to go to main menu.....");
    getchar();
    return found;
}

//Function to search books in the issued list DONE
int searchBooksInIssued()
{
    system("clear");
    int found = 0;
    char bookName[MAX_BOOK_NAME] = {0};
    s_BooksInfo searchBookInfo = {0};
    fp = fopen(ISSUED_DATABASE, "rb");
    if (fp == NULL)
    {
        printf("\n\t\t\tFile is not opened\n");
        exit(1);
    }
    printf("\n\n\t\t\tEnter Book Name ");
    fflush(stdin);
    getchar();
    fgets(bookName, MAX_BOOK_NAME, stdin);
    while (fread(&searchBookInfo, sizeof(searchBookInfo), 1, fp))
    {
        if (!strcmp(searchBookInfo.name, bookName))
        {
            found = 1;
            break;
        }
    }
    if (found)
    {
        printf("\n\t\t\tBook id = %u", searchBookInfo.id);
        printf("\n\t\t\tBook name = %s", searchBookInfo.name);
        printf("\n\t\t\tBook issue date(day/month/year) =  (%d/%d/%d)", searchBookInfo.issued.dd,
               searchBookInfo.issued.mm, searchBookInfo.issued.yyyy);
        printf("\n\t\t\tBook has been issued to = %s", searchBookInfo.stname);
        printf("\n\t\t\tBook due date(day/month/year) =  (%d/%d/%d)", searchBookInfo.duedate.dd,
               searchBookInfo.duedate.mm, searchBookInfo.duedate.yyyy);
    }
    else
    {
        printf("\n\t\t\tNo Record in Issued Database");
    }
    fclose(fp);
    printf("\n\n\n\t\t\tPress any key to go to main menu.....");
    getchar();
    return found;
}

// Add books in DataBase  DONE
void addBookInDataBase()
{
    system("clear");
    s_BooksInfo addBookInfoInDataBase = {0};
    int status = 0;
    fp = fopen(BOOK_DATABASE, "ab+");
    if (fp == NULL)
    {
        printf("File is not opened\n");
        exit(1);
    }
    printf("\n\t\t\tADD NEW BOOKS");
    printf("\n\n\t\t\tENTER YOUR DETAILS BELOW:");
    printf("\n\t\t\t---------------------------------------------------------------------------\n");
    printf("\n\t\t\tBook ID NO  = ");
    fflush(stdin);
    scanf("%u", &addBookInfoInDataBase.id);
    getchar();
    do
    {
        printf("\n\t\t\tBook Name  = ");
        fflush(stdin);
        fgets(addBookInfoInDataBase.name, MAX_BOOK_NAME, stdin);
        status = isNameValid(addBookInfoInDataBase.name);
        if (!status)
        {
            printf("\n\t\t\tName contain invalid character. Please enter again.");
        }
    } while (!status);
    do
    {
        printf("\n\t\t\tAuthor Name  = ");
        fflush(stdin);
        fgets(addBookInfoInDataBase.Author, MAX_AUTHOR_NAME, stdin);
        status = isNameValid(addBookInfoInDataBase.Author);
        if (!status)
        {
            printf("\n\t\t\tName contain invalid character. Please enter again.");
        }
    } while (!status);
    int i = 0;
    printf("\n\t\t\tAvailable Categories are");
    for (i = 0; i < 11; i++)
    {
        printf("\n\t\t\t%d. %s", (i + 1), catagories[i]);
    }
    do
    {
        int category;
        char term;
        printf("\n\t\t\tCategory  = ");
        fflush(stdin);
        status = scanf("%d%c", &category, &term);
        strcpy(addBookInfoInDataBase.cat, catagories[category - 1]);
        if (status != 2)
        {
            printf("\n\t\t\tCategory contain invalid character. Please enter again.");
        }
    } while (status != 2);

    printf("\n\t\t\tEnter the price: ");
    fflush(stdin);
    scanf("%lf", &addBookInfoInDataBase.Price);
    printf("\n\t\t\tGenerating Rack Number...........");
    addBookInfoInDataBase.rackno = rack_number();
    getchar();
    getchar();
    fwrite(&addBookInfoInDataBase, sizeof(addBookInfoInDataBase), 1, fp);
    fclose(fp);
}

// Function to issue books  DONE
void issueBooks()
{

    system("clear");
    printf("\n\t\t\tIssue Book Section");
    fp = fopen(BOOK_DATABASE, "rb+");
    fs = fopen(ISSUED_DATABASE, "ab+");
    int available_in_library = 0, available_in_issued = 0;
    available_in_library = searchBooksInLibrary();
    if (available_in_library)
    {
        available_in_issued = searchBooksInIssued();
        if (available_in_issued)
        {
            printf("\n\t\t\tSORRY THE BOOK HAS BEEN ISSUED");
            getchar();
            getchar();
        }
        else
        {

            system("clear");
            printf("\n\t\t\t The Book is available");
            printf("\n\t\t\t Please Enter the following information");
            s_BooksInfo addBookInfoInDataBase = {0};
            int status = 0;
            if (fs == NULL)
            {
                printf("File is not opened\n");
                exit(1);
            }
            printf("\n\n\t\t\tENTER YOUR DETAILS BELOW:");
            printf("\n\t\t\t---------------------------------------------------------------------------\n");
            printf("\n\t\t\tBook ID NO  = ");
            fflush(stdin);
            scanf("%u", &addBookInfoInDataBase.id);
            getchar();
            do
            {
                printf("\n\t\t\tBook Name  = ");
                fflush(stdin);
                fgets(addBookInfoInDataBase.name, MAX_BOOK_NAME, stdin);
                status = isNameValid(addBookInfoInDataBase.name);
                if (!status)
                {
                    printf("\n\t\t\tName contain invalid character. Please enter again.");
                }
            } while (!status);
            do
            {
                printf("\n\t\t\tStudent Name  = ");
                fflush(stdin);
                fgets(addBookInfoInDataBase.stname, MAX_STUDENT_NAME, stdin);
                status = isNameValid(addBookInfoInDataBase.stname);
                if (!status)
                {
                    printf("\n\t\t\tName contain invalid character. Please enter again.");
                }
            } while (!status);
            do
            {
                //get date year,month and day from user
                printf("\n\t\t\tEnter date in format (day/month/year): ");
                scanf("%d/%d/%d", &addBookInfoInDataBase.issued.dd, &addBookInfoInDataBase.issued.mm, &addBookInfoInDataBase.issued.yyyy);
                //check date validity
                status = isValidDate(&addBookInfoInDataBase.issued);
                if (!status)
                {
                    printf("\n\t\t\tPlease enter a valid date.\n");
                }
            } while (!status);
            // Calculating the return time
            Date return_date;
            return_date.dd = addBookInfoInDataBase.issued.dd;
            return_date.mm = addBookInfoInDataBase.issued.mm;
            return_date.yyyy = addBookInfoInDataBase.issued.yyyy;
            return_date.dd += RETURNTIME;
            if (!isValidDate(&return_date))
            {
                if (return_date.mm == 2)
                {
                    if (IsLeapYear(return_date.yyyy))
                        return_date.dd -= 29;
                    else
                        return_date.dd -= 28;
                }
                else if (return_date.mm == 4 || return_date.mm == 6 || return_date.mm == 9 || return_date.mm == 11)
                    return_date.dd -= 30;
                else
                    return_date.dd -= 31;
                // Calculating the return month
                return_date.mm += 1;
                if (return_date.mm > 12)
                {
                    return_date.mm -= 1;
                    return_date.yyyy += 1;
                }
            }
            addBookInfoInDataBase.duedate = return_date;

            fwrite(&addBookInfoInDataBase, sizeof(addBookInfoInDataBase), 1, fs);
            fclose(fs);
        }
    }
    else
    {
        printf("\n\t\t\tSORRY THIS BOOK IS NOT AVAILABLE");
        getchar();
    }
}

// Issue Books Menu   DONE
void issueBooksMenu()
{
    system("clear");
    char choice;
    printf("\n\t\t********************************ISSUE SECTION**************************");
    printf("\n\t\t\t1. Issue a Book");
    printf("\n\t\t\t2. View Issued Books");
    printf("\n\t\t\t3. Search Issued Book");
    printf("\n\t\t\t0. To Exit");
    printf("\n\t\t\tEnter a Choice:");
    fflush(stdin);
    getchar();
    choice = getchar();
    switch (choice)
    {
    case '1':
        system("clear");
        issueBooks();
        break;
    case '2':
        system("clear");
        viewIssuedBooks();
        break;
    case '3':
        system("clear");
        int no_use;
        no_use = searchBooksInIssued();
        break;

    default:
        break;
    }
}

// Return Books  DONE   MAYBE
void returnBooks()
{
    int found = 0, status = 0, ID;
    double fine_amount = 0.0;
    char bookName[MAX_BOOK_NAME] = {0};
    Date returnDate;
    printf("\n\t\t\tENTER THE NAME OF THE BOOK TO BE RETURNED  ");
    fflush(stdin);
    getchar();
    fgets(bookName, MAX_BOOK_NAME, stdin);
    fp = fopen(ISSUED_DATABASE, "rb");
    while (fread(&global_book_data, sizeof(global_book_data), 1, fp))
    {
        if (!strcmp(global_book_data.name, bookName))
        {
            found = 1;
            break;
        }
    }
    if (found)
    {
        do
        {
            //get date year,month and day from user
            printf("\n\t\t\tEnter date in format (day/month/year): ");
            scanf("%d/%d/%d", &returnDate.dd, &returnDate.mm, &returnDate.yyyy);
            //check date validity
            status = isValidDate(&returnDate);
            if (!status)
            {
                printf("\n\t\t\tPlease enter a valid date.\n");
            }
        } while (!status);
        fine_amount = ((returnDate.dd - global_book_data.duedate.dd) + (returnDate.mm - global_book_data.duedate.mm) * 30) * FINE_AMOUNT_PER_DAY;
        // Deleting the returned book
        ID = global_book_data.id;
        fclose(fp);
        deleteIssuedBooks(ID);
    }
    else
    {
        printf("\n\t\t\tNo Record in Issued Database");
    }
    if (fine_amount < 0.0)
        fine_amount = 0.0;

    printf("\n\t\t\tFINE AMOUNT = Rs %.2lf", fine_amount);
    getchar();
    printf("\n\n\n\t\t\tPress any key to go to main menu.....");
    getchar();
}

//Main Menu
void menu()
{
    int choice = 0;
    do
    {
        system("clear");
        printf("\n\t\t\tMAIN MENU");
        printf("\n\n\n\t\t\t1.Add Books");
        printf("\n\t\t\t2.Search Books");
        printf("\n\t\t\t3.View Books");
        printf("\n\t\t\t4.Delete Book");
        printf("\n\t\t\t5.View Existing Members");
        printf("\n\t\t\t6.Add New Members");
        printf("\n\t\t\t7.Issue Section");
        printf("\n\t\t\t8.Return Books");
        printf("\n\t\t\t0.Exit");
        printf("\n\n\n\t\t\tEnter choice => ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            addBookInDataBase();
            break;
        case 2:
            searchBooksInLibrary();
            break;
        case 3:
            viewBooks();
            break;
        case 4:
            deleteBooks();
            break;
        case 5:
            viewMemberDatabase();
            break;
        case 6:
            add_new_Members();
            break;
        case 7:
            issueBooksMenu();
            break;
        case 8:
            returnBooks();
            break;
        case 0:
            printf("\n\n\n\t\t\t\tThank you!!!\n\n\n\n\n");
            exit(1);
            break;
        default:
            printf("\n\n\n\t\t\tWrong Entry!!Please re-entered correct option");
        }                  //Switch Ended
    } while (choice != 0); //Loop Ended
}

// Login Function  DONE
void login()
{
    char userName[MAX_SIZE_USER_NAME] = {0};
    char passWord[MAX_SIZE_PASSWORD] = {0};
    int found = 0;
    int L = 0;
    user_login fileHeaderInfo = {0};
    // FILE *fp = NULL;
    printf("\n\t\t\tLogin");
    do
    {
        fp = fopen(MEMBER_DATABASE, "rb");
        if (fp == NULL)
        {
            printf("File is not opened\n");
            exit(1);
        }
        printf("\n\n\n\t\t\t\tUsername:");
        fgets(userName, MAX_SIZE_USER_NAME, stdin);

        printf("\n\t\t\t\tPassword:");
        fgets(passWord, MAX_SIZE_PASSWORD, stdin);

        while (fread(&fileHeaderInfo, sizeof(fileHeaderInfo), 1, fp))
        {
            if ((!strcmp(fileHeaderInfo.username, userName)) && (!strcmp(fileHeaderInfo.password, passWord)))
            {
                found = 1;
                break;
            }
        }
        fclose(fp);
        if (found)
        {
            menu();
        }
        else
        {
            printf("\t\t\t\tLogin Failed Enter Again Username & Password\n\n");
            L++;
        }
    } while (L <= 3);
    if (L > 3)
    {
        printf("\n\t\t\tLogin Failed");
        printf("\t\t\t\tSorry,Unknown User.");
        getchar();
        system("clear");
    }
}

// Function to initialize the member database
// and add a default username and password
void init()
{
    int status = 0;
    const char defaultUsername[] = "bits_pilani\n";
    const char defaultPassword[] = "bits_pilani\n";
    user_login User_Info = {0};
    status = isFileExists(MEMBER_DATABASE);
    if (!status)
    {
        //create the binary file
        fp = fopen(MEMBER_DATABASE, "wb");
        if (fp != NULL)
        {
            //Copy default password
            strncpy(User_Info.password, defaultPassword, sizeof(defaultPassword));
            strncpy(User_Info.username, defaultUsername, sizeof(defaultUsername));
            fwrite(&User_Info, sizeof(user_login), 1, fp);
            fclose(fp);
        }
    }
}
int main()
{
    init();
    welcomeMessage();
    login();
    return 0;
}
