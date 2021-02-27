/***********************************************************************
 *consultantProgram.c
 *A program that allows consultants to see patient's details
 *Candidate number: 181355
 *12/2017
 **********************************************************************/
#include <stdio.h>
#include <string.h>

#define HEADERLEN 48  /* must be less than or equal to PADDING length */
#define PADDING "======================================================"
#define OR ||
#define AND &&
#define NAMELIM 26
#define FILELIM 51
#define LIMIT 1000
#define DATELIM 11
#define FLOATLIM 9

struct DOB
{
    int day;
    int month;
    int year;
};

struct patientInfo
{
    char name[NAMELIM];
    char date[DATELIM];
    char height[FLOATLIM];
    char waist[FLOATLIM];
    char weight[FLOATLIM];
    char comment[LIMIT];  
};

void pressAnyToContinue();
_Bool confirmYN(char[]);
void equalsLine();
void emptyBuffer();
void displayTitle();
void getPatientName(char[]);
void getFileName(char[], struct DOB);
void openFile(char[], char[]);
void editComment(struct patientInfo, char[]);
void writeNewFile(struct patientInfo, char[],char[]);
void decrypt(char[], char[]);
char offset(char, char, char);
void encrypt(char[], char[]);
char offsetEnc(char, char, char);

/*main entry point*/
int main(void)
{
    char patientName[NAMELIM] = {'\0'};
    
    displayTitle("Patient Data Viewing");
    printf("\nWelcome");
    getPatientName(patientName);
    
    return 0;
}

/*gets user to input patient's last name and D.O.B*/
void getPatientName(char patientName[])
{
    struct DOB enteredDOB;
    

    printf("\n\nPlease input a patient's last name: ");
    scanf("%s", patientName);
    printf("\nEnter the patient's D.O.B in the form (DD/MM/YYYY): ");
    while(scanf("%d/%d/%d", &enteredDOB.day, &enteredDOB.month, 
        &enteredDOB.year) != 3 OR enteredDOB.day < 1 OR enteredDOB.day >
        31 OR enteredDOB.month < 1 OR enteredDOB.month > 12 OR 
        enteredDOB.year < 1900 OR enteredDOB.year >2017)
    {
        printf("\nInvalid value");
        printf("\nPlease try again: ");
        emptyBuffer();
    }
    
    /*creates file name*/
    getFileName(patientName, enteredDOB);
    
}

/*creates a file name using information from user*/
void getFileName(char patientName[], struct DOB enteredDOB)
{
    char fileName[FILELIM];
    
    /*uses last name and D.O.B*/
    sprintf(fileName, "%s%04d%02d%02d.aow\0", patientName,
        enteredDOB.year,enteredDOB.month,enteredDOB.day);
    printf("\nFile name:  [%s]\n", fileName);
    
    /*opens file with created file name*/
    openFile(fileName, patientName);
}

void openFile(char fileName[], char patientName[])
{
    FILE *ptrFile;
    
    char tmp[LIMIT]; 

    struct patientInfo patient;
    
    /*attempts to open file with created filename*/
    ptrFile = fopen(fileName, "r");
    
    /*could file be opened?*/
    if (!ptrFile)
    {
        /*file not opened*/
        printf("\nPatient not found\n");
        /*does the user want to search again*/
        if(confirmYN("Do you want to try again?(Y/N): ") == 1)
        {
            getPatientName(patientName);
        }
    }
    else
    {
        /*file opened so information is read from file into temporary*/ 
        /*buffer, then decrypted and stored in the patient info struct*/
         
        fscanf(ptrFile, "%51[^\n]", tmp);
        decrypt(tmp,patient.name); 
        fscanf(ptrFile, "%s", tmp);
        decrypt(tmp, patient.date);
        fscanf(ptrFile, "%s", tmp);
        decrypt(tmp, patient.height);
        fscanf(ptrFile, "%s", tmp);
        decrypt(tmp, patient.waist);  
        fscanf(ptrFile, "%s", tmp);
        decrypt(tmp, patient.weight);
        fscanf(ptrFile, "\n%1000[^\n]", tmp);
        decrypt(tmp, patient.comment);
    
        /*closes patient file*/
        fclose(ptrFile);
        
        /*displays decrypted patient info on screen*/
        printf("\n");
        displayTitle(patient.name);
        printf("\nPatient D.O.B:      %s", patient.date);
        printf("\nPatient Height(cm): %s", patient.height);
        printf("\nPatient Waist(cm):  %s", patient.waist);
        printf("\nPatient Weight(kg): %s", patient.weight);
        printf("\nComment:            %s", patient.comment);
        
        /*allows user to edit comment stored on file*/
        editComment(patient, fileName);
    }
    
}

/*asks user if they want to add to comments on file, then gets the*/ 
/*new comment*/
void editComment(struct patientInfo patient, char fileName[])
{
    char newNote[LIMIT] = {'\0'};


    if(confirmYN("\n\nDo you want to add notes to the comment?(Y/N): ")   
        == 1)
    {
        /*if yes, get new comment*/
        printf("\nEnter notes to be added to comments: ");
        emptyBuffer();
        scanf("%[^\n]", newNote);
        printf("\nNew notes: %s", newNote);
        /*writes file with new comment*/
        writeNewFile(patient, fileName, newNote);   
    }
    /*if no program will close*/
}

/*decrypts a string*/
void decrypt(char encrypted[], char unEncrypted[])
{
    int ii=0;
    char ch = '\0';
    
    /*loops through encrypted string*/
    for(ii=0; encrypted[ii] != '\0'; ii++)
    {
        ch = encrypted[ii];
       
        if(ch >= 'A' && ch <= 'Z')
        {
            /*apply offset to upper case alpha*/
            unEncrypted[ii] = offset('Z','A', ch);
        }
        else
        if(ch >= 'a' && ch <= 'z')
        {
            /*apply offset to lower case alpha*/
            unEncrypted[ii] = offset('z','a', ch);
        }
        else
        if(ch >= '0' && ch <= '9')
        {
            /*apply offset to digit*/
            unEncrypted[ii] = offset('9','0', ch);
        }
        else
        {
            /*no decryption applied to char outside of specified range*/
            unEncrypted[ii] = ch;
        }
    }
    /*terminate unencrypted string*/
    unEncrypted[ii] = '\0';
}

/*applies decryption key to a character, wrapping around when needed*/
char offset(char top, char bottom, char src)
{
    /*range is difference between height and lowest value in range*/
    int range = (top - bottom) + 1;
    /*decryption/encryption key*/
    int offset = 7;
    /*wraparound encryption/decryption key*/
    int offset1 = range - offset;
    char ch = src;
    
    /*do we need to wraparound?*/
    if(ch < (bottom + offset))
    {
        /*yes*/
        ch += offset1;
    }
    else
    {
        /*no*/
        ch -= offset;
    }
    
    /*return decrypted character*/
    return ch;
}

/*writes a file with encrypted data using previosuly created filename*/
void writeNewFile(struct patientInfo patient, char fileName[],
    char newNote[])
{
    FILE *filePtr;
    char tmp[LIMIT];
    char tmp1[LIMIT];
    
    filePtr = fopen(fileName, "w");
    if (!filePtr)
    {
        /* error message if file cannot be written*/
        printf("\nFile not created\n");
    }
    else
    {
        /*encrypts data then prints to file*/
        encrypt(patient.name, tmp);
        fprintf(filePtr, "%s\n", tmp);
        encrypt(patient.date, tmp);
        fprintf(filePtr, "%s\n", tmp);
        encrypt(patient.height, tmp);
        fprintf(filePtr, "%s\n", tmp);
        encrypt(patient.waist, tmp);
        fprintf(filePtr, "%s\n", tmp);
        encrypt(patient.weight, tmp);
        fprintf(filePtr, "%s\n", tmp);
        sprintf(tmp1, "%s %s", patient.comment, newNote);
        encrypt(tmp1, tmp);
        fprintf(filePtr, "%s\n", tmp);
        
        fclose(filePtr);
        
        printf("\nFile written succesfully");
        
    }
    
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
            /*apply encryption offset to lower case alpha*/
            encrypted[ii] = offsetEnc('z', 'a', ch);
        }
        else
        if(ch >= 'A' && ch <= 'Z')
        {
            /*apply encryption offset to upper case alpha*/
            encrypted[ii] = offsetEnc('Z', 'A', ch);
        }
        else
        if(ch >= '0' && ch <= '9')
        {
            /*apply encryption offset to digit*/
            encrypted[ii] = offsetEnc('9', '0', ch);
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
char offsetEnc(char top, char bottom, char ch)
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

/*waits for user input*/
void pressAnyToContinue()
{
    printf("\nPress any key to continue");
    _getch();
}

/*this function is used for every (Y/N) question */
_Bool confirmYN(char *message)
{
    _Bool result = 0;

    /*show the question prompt*/
    printf(message);

    /* get their answer */
    char answer = '\0';
    emptyBuffer();
    
    while((scanf("%c", &answer) != 1) OR (answer != 'Y' AND 
            answer != 'y' AND answer != 'N' AND answer != 'n'))
	{	
    	printf("Invalid answer\n");   
        printf("Please try again: ");
    	emptyBuffer();                
	}

    if (answer == 'y' OR answer == 'Y')
    {
        /* confirmed */
        result = 1;
    }

    return result;
}

/*displays title between two lines of '=' */
void displayTitle(char *title)
{
    equalsLine();

    /* calculate available space on line / 2*/
    int len = (HEADERLEN-strlen(title))/2;
    
    /*prints the title padded with appropriate number of spaces*/
    printf("%*.*s%s\n", len, len, " ", title);
    
    equalsLine();
}

/*prints out a line of equals of length constant PADDING */
void equalsLine()
{
    printf("%*.*s\n", HEADERLEN, HEADERLEN, PADDING);
}

/*clears the buffer */
void emptyBuffer()
{
    while(getchar() != '\n')
    {
        ;
    }
}
