/***********************************************************************
 *nurseProgram.c
 *A program that allows nurses to enter patient's details
 *Candidate number: 181355
 *12/2017
 **********************************************************************/
 
#include <stdio.h>
#include <string.h>


#define HEADERLEN 48  /*must be less than or equal to PADDING length*/
#define PADDING "======================================================"
#define OR ||
#define AND &&
#define KEY 7
#define PASSWORDLEN 9
#define NURSES 5
#define LIMIT 1000
#define NAMELIM 26
#define FILELIM 51

struct DOB
{
    int day;
    int month;
    int year;
};

struct patientInfo
{
    char firstName[NAMELIM];
    char lastName[NAMELIM];
    struct DOB dt;
    float height;
    float waist;
    float weight;
    char comment[LIMIT];  
};

void displayTitle(char[]);
void usernamePassword(char[], char[], char[]);
_Bool verifyPassword(char[], char[]);
void encrypt(char[], char[]);
char offset(char, char, char);
_Bool getStoredPassword(char[], char[]);
void patientDataChoice();
struct patientInfo createNewPatient();
_Bool writePatientFile(struct patientInfo);
void pressAnyToContinue();
_Bool confirmYN(char *);
void equalsLine();
void errorMessage();
void emptyBuffer();
void writeEncryptedString(FILE *, char[]);
void writeEncryptedName(FILE *, char[], char[]);
void writeEncryptedFloat(FILE *, float);
void writeEncryptedDate(FILE *, int, int, int);
void encryptWrite(FILE *, char[]);

/*main entry point*/
int main(void)
{
    char userAttempt[PASSWORDLEN] = {'\0'};
    char passAttempt[PASSWORDLEN] = {'\0'};
    char passEncrypted[PASSWORDLEN] = {'\0'};
    
    displayTitle("Patient Data Entry");
    /*get username and password from user*/
    usernamePassword(userAttempt, passAttempt, passEncrypted);
    
    return 0;
}

/*get username and password attempt from user*/
void usernamePassword(char userAttempt[], char passAttempt[] ,
    char passEncrypted[])
{
    char passChar = '\0';
    int ii = 0;
    
    printf("\nPlease enter your username and password below\n");
    printf("\nUsername: ");
    while(scanf("%25s", userAttempt) != 1)
    {
        errorMessage();
    }
    printf("\nPassword: ");
        
    /*loop until valid password*/
    while(1)
    {
        passChar = getch();
        
        /*if password entered reaches over 8 chars or user hits*/
        /*return end loop*/
        if(passChar == '\r' OR ii == PASSWORDLEN)
        {
            passAttempt[ii] = '\0';
            break;
        }
        /*populate password attempt with entered character*/
        passAttempt[ii] = passChar;
        /*echo typed character as an **/
        printf("*");
        ii++;
    }
    
    /*if password entered is correct allow access*/
    if(verifyPassword(userAttempt, passAttempt) == 1)
    {
        /*inform user of success*/
        printf("\n\nPassword is correct\n");
        pressAnyToContinue();
        system("cls");
        /*asks if they want to create a new patient file*/
        patientDataChoice();
    }
    else
    {
        /*inform user of failure and fall through to program exit*/
        printf("\nPassword/Username incorrect\n");        
    }
}

/*verifies if password matches stored password*/
_Bool verifyPassword(char userAttempt[], char passAttempt[])
{
    char attemptHash[PASSWORDLEN] = {'\0'};
    char storedHash[PASSWORDLEN] = {'\0'};
    _Bool result = 0;
    
    /*get entered password hash*/
    encrypt(passAttempt, attemptHash);
 
    /*now check this hash against stored passwords*/
    getStoredPassword(userAttempt, storedHash);
     
    /*do they match?*/
    if(strcmp(attemptHash, storedHash) == 0)
    {
        result = 1;
    }
    return result;    
}

/*encrypts a string*/
void encrypt(char unEncrypted[], char encrypted[])
{
    int ii=0;
    char ch = '\0';
    
    /*loops through unencrypted string*/
    for(ii=0; unEncrypted[ii] != '\0'; ii++)
    {
        ch = unEncrypted[ii];
        
        if(ch >= 'a' && ch <= 'z')
        {
            /*apply offset to lower case alpha*/
            encrypted[ii] = offset('z', 'a', ch);
        }
        else
        if(ch >= 'A' && ch <= 'Z')
        {
            /*apply offset to upper case alpha*/
            encrypted[ii] = offset('Z', 'A', ch);
        }
        else
        if(ch >= '0' && ch <= '9')
        {
            /*apply offset to digit*/
            encrypted[ii] = offset('9', '0', ch);
        }
        else
        {
            /*no encryption applied to char outside specified ranges*/
            encrypted[ii] = ch;
        }
    }
    /*terminate encrypted string*/
    encrypted[ii] = '\0';
}

/*applies encryption key to a character, wrapping appropriately*/
char offset(char top, char bottom, char ch)
{
    /*range is difference between highest and lowest value in range*/
    int range = (top - bottom) + 1;
    /*encryption key*/
    int offset = 7;
    /*wraparound encryption key*/
    int offset1 = range - offset;
    char cc = ch;
    
    /*do we need to wraparound?*/
    if(cc > (top - offset))
    {
        /*yes*/
        cc -= offset1;
    }
    else
    {
        /*no*/
        cc += offset;
    }

    /*return encrypted character*/
    return cc;
}


/*gets stored password hash for entered username*/
_Bool getStoredPassword(char userAttempt[], char storedHash[])
{
    FILE *ptrFile;
    char buf[1000];
    char ss[] = ":\n";
    _Bool result = 0;

    /*open the file*/
    ptrFile = fopen("users.txt", "r");
    if (!ptrFile)
    {
        /*report error here*/
        printf("\nunable to read password file\n");
        return result;
    }

    /*get each line in the file,'\n' terminated (max 1000 char length)*/
    while (fgets(buf, 1000, ptrFile) != NULL)
    {
        /*tokenize each line*/
        char *token;

        /*get the first token, which is stored username*/
        token = strtok(buf, ss);

        /*did we get a token?*/
        if (token != NULL)
        {
            /*does the stored username match entered username?*/
            if(strcmp(token,userAttempt) == 0)
            {
                /*they match, so get next token (stored password)*/
                token = strtok(NULL, ss);
        
                /*did we get the password token?*/
                if(token != NULL)
                {
                    /*put stored password into stored hash*/
                    strcpy(storedHash, token);    
                    result = 1;
                    /*break out of file reading loop*/
                    break;
                }        
            }
        }
    }

    /*close users file*/
    fclose(ptrFile);
    return result;
}

/*asks user if they want to create a new patient file*/
void patientDataChoice()
{
    displayTitle("Patient Data Entry");
    while(confirmYN("\nDo you wish to create a" 
        " new patient file?(Y/N): ") == 1)
    {
        /*get patient details and write to file*/
        if(writePatientFile(createNewPatient()) == 1)
        {
            printf("\nFile written successfully");
        }
        else
        {
            printf("\nFile not written");
        }
    }
}

/*asks user for patient info and stores in a structure*/
struct patientInfo createNewPatient()
{
    struct patientInfo patient;
    
    printf("\nEnter patient's first name: ");
    scanf("%25s", patient.firstName);
    printf("\nEnter patient's last name: ");
    scanf("%25s", patient.lastName);
    
    /*populate date struct nested within patient struct*/
    printf("\nEnter the patient's D.O.B in the form (DD/MM/YYYY): ");
    while(scanf("%d/%d/%d", &patient.dt.day, &patient.dt.month, 
        &patient.dt.year) != 3 OR patient.dt.day < 1 OR 
        patient.dt.day > 31 OR patient.dt.month < 1 OR 
        patient.dt.month > 12 OR patient.dt.year < 1900 OR 
        patient.dt.year >2017)
    {
        errorMessage();
    }
    printf("\nPlease enter the patient's height in cm: ");
    while(scanf("%f", &patient.height) != 1 OR patient.height < 25 OR 
        patient.height > 250)
    {
        errorMessage();
    }
    printf("\nPlease enter the patient's waist measurement in cm: ");
    while(scanf("%f", &patient.waist) != 1 OR patient.waist < 40 OR 
        patient.waist > 300)
    {
        errorMessage();
    }
    printf("\nPlease enter the patient's weight in kg: ");
    while(scanf("%f", &patient.weight) != 1 OR patient.weight < 1 OR 
        patient.weight > 300)
    {
        errorMessage();
    }
    printf("\nPlease enter a comment for the patient: ");
    emptyBuffer();
    while(scanf("%1000[^\n]", patient.comment) != 1)
    {
        errorMessage();
    } 
                       
    /*returns populated patient struct*/
    return patient;
}

/*display entered details on screen then writes to a file*/
_Bool writePatientFile(struct patientInfo patient)
{
    FILE *ptrFile;
    char fileName[FILELIM]; 
    char tmp[LIMIT];
    
    printf("\nFirst name: %s\n", patient.firstName);
    printf("Last name:    %s\n", patient.lastName);
    printf("D.O.B:        %02d/%02d/%04d\n", patient.dt.day, 
     patient.dt.month, patient.dt.year);
    printf("Height:       %5.02f\n", patient.height);
    printf("Waist:        %5.02f\n", patient.waist);
    printf("Weight:       %5.02f\n", patient.weight);
    printf("Comment:      %s\n", patient.comment);

    /*generates patient specific file name*/
    sprintf(fileName, "%s%04d%02d%02d.aow\0", patient.lastName,
        patient.dt.year,patient.dt.month,patient.dt.day);
    
    /*opens file, overwrites file if already exists*/
    ptrFile = fopen(fileName, "w");

    if (!ptrFile)
    {
        /*failed to open file*/
        return 0;
    }

    /*encrypts data and writes to file*/
    writeEncryptedName(ptrFile, patient.firstName, patient.lastName);
    writeEncryptedDate(ptrFile, patient.dt.day, patient.dt.month, 
        patient.dt.year);
    writeEncryptedFloat(ptrFile, patient.height);
    writeEncryptedFloat(ptrFile, patient.waist);
    writeEncryptedFloat(ptrFile, patient.weight);
    writeEncryptedString(ptrFile, patient.comment);

    /*closes file*/
    fclose(ptrFile);

    return 1;
}

/*encrypt and write a string to file*/
void writeEncryptedString(FILE *ptrfile, char unEncrypted[])
{
    char encrypted[LIMIT];
    
    encrypt(unEncrypted, encrypted);
    fprintf(ptrfile, "%s\n", encrypted);
}

/*format name into singular string and encrypt then write to file*/
void writeEncryptedName(FILE *ptrfile, char unEncryptedA[],
     char unEncryptedB[])
{
    char tmp[LIMIT];
    char encrypted[LIMIT];
    
    /*formats full name into temporary string*/
    sprintf(tmp, "%s %s", unEncryptedA, unEncryptedB);
    
    /*encrypt then write string to file*/
    encryptWrite(ptrfile, tmp);
}

/*format float into string then encrypt and write to file*/
void writeEncryptedFloat(FILE *ptrfile, float unEncrypted)
{
    char tmp[LIMIT];
    char encrypted[LIMIT];
    
    sprintf(tmp, "%5.02f", unEncrypted);
    
    encryptWrite(ptrfile, tmp);
}

/*format date into string then encrypt and write to file*/
void writeEncryptedDate(FILE *ptrfile, int unEncryptedDD, 
    int unEncryptedMM, int unEncryptedYYYY)
{
    char tmp[LIMIT];
    char encrypted[LIMIT];
    
    sprintf(tmp, "%02d/%02d/%04d", unEncryptedDD, unEncryptedMM, 
        unEncryptedYYYY);
    
    encryptWrite(ptrfile, tmp);
}

/*encrypt string and write to file*/
void encryptWrite(FILE *ptrfile, char unEncrypted[])
{
    char encrypted[LIMIT];
    
    encrypt(unEncrypted, encrypted);
 
    fprintf(ptrfile,"%s\n", encrypted);
}

/*wait for user input*/
void pressAnyToContinue()
{
    printf("\nPress any key to continue");
    _getch();
}

/*displays title between two lines of '='*/
void displayTitle(char *title)
{
    
    equalsLine();

    /*calculate available space on line / 2*/
    int len = (HEADERLEN-strlen(title))/2;
    
    /*prints the title padded with appropriate number of spaces*/
    printf("%*.*s%s\n", len, len, " ", title);
    
    equalsLine();
}

/*prints out a line of equals of length constant PADDING*/
void equalsLine()
{
    printf("%*.*s\n", HEADERLEN, HEADERLEN, PADDING);
}

/*this function is used for every (Y/N) question*/
_Bool confirmYN(char *message)
{
    _Bool result = 0;

    /*show the question prompt*/
    printf(message);

    /*get their answer*/
    char answer = '\0';
    emptyBuffer();
    
    while((scanf("%c", &answer) != 1) OR (answer != 'Y' AND 
            answer != 'y' AND answer != 'N' AND answer != 'n'))
	{	
        errorMessage();        
	}

    if (answer == 'y' OR answer == 'Y')
    {
        /*confirmed*/
        result = 1;
    }

    return result;
}

/*prints error message*/
void errorMessage()
{
    printf("\nInvalid value\n");
    printf("\nPlease try again: ");
    emptyBuffer();
}

/*clears the buffer*/
void emptyBuffer()
{
    while(getchar() != '\n')
    {
        ;
    }
}
