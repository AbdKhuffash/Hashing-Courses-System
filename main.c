#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define LOADFACTOR .7
///Khuffash


typedef struct
{
    char name[25];
    int credit_hours;
    char code[25];
    char department[25];
    char topics[250];


} course;


struct node
{
    course course;
    int condition ; //0->empty  1->used  2->deleted
};
/// GLOBALLY DEFINED VARIABLES
FILE *courses_file;

struct node **Hash_Table_Double;
int Hash_Table_Double_Size;
int Number_Collisions_Double=0;

struct node **Hash_Table_Linear;
int Hash_Table_Linear_Size;
int Number_Collisions_Linear=0;
///------------------------------

//--------------------------------general functions--------------------------//
void Read_file()//reads the data from the file
{
    char course_info[500];
    courses_file=fopen("offered_courses.txt","r");

    while(!feof(courses_file))
    {
        course temp_course;

        fgets(course_info,500,courses_file);

        int *cutter=strtok(course_info,":#/\n"); //reading the course name
        strcpy(temp_course.name,cutter);

        cutter=strtok(NULL,":#/\n");//reading the course hours;
        temp_course.credit_hours=atoi(cutter);

        cutter=strtok(NULL,":#/\n"); //reading the course code
        strcpy(temp_course.code,cutter);

        cutter=strtok(NULL,":#/\n");//reading the course department
        strcpy(temp_course.department,cutter);

        cutter=strtok(NULL,":#/\n"); //reading the course topics

        strcpy(temp_course.topics,cutter);

        Insert_linear(temp_course);
        Insert_double(temp_course);

    }
}

int Count_Nodes(char filename[]) //counts the lines in the file
{

    FILE *fp;
    int count = 0;
    char c;
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Could not open file %s", filename);
        return 0;
    }
    for (c = getc(fp); c != EOF; c = getc(fp))
        if (c == '\n')
            count = count + 1;
    return count+1;
}

int iSPrime(int x) //checks if the number is prime -> return 1 ,or not prime -> return 0
{

    for (int i = 2; i * i <= x; i++)
    {
        if (x % i == 0)
            return 0;
    }
    return 1;
}

int Find_size(int x) //finds the size of the table , 2 * number of nodes ( number of lines in text file) and increaments till it find the first prrime number
{
    for (int i = 2 * x;; i++)
    {
        if (iSPrime(i))
            return i;
    }
}

int String_Hashing(char *x,int Table_size) //return the sum of asci codes  shifted to base 2^5;
{
    long value=0;
    while(*x !='\0')
    {
        value=(value << 5) + *(x++);
    }

    // return abs(value % Hash_Table_Linear_Size); ///it will not matter if we take the mod of the hash value.
    return abs(value);
}

int find_Less_Prime(int size) ///double hashing *hash2 (R);
{
    for(int i=size-1; i>1; i--)
    {
        if(iSPrime(i))
            return i;
    }
}

int Spots_aquaried(struct node **Hash_Table,int size)///both
{
    int counter=0;
    for(int i=0; i<size; i++)

        if(Hash_Table[i]->condition==1)
            counter++;

    return counter;
}
//---------------------------Linear table Functions-----------------------------------------//
void create_Hash_Table_Linear() // creating linear function and malloc'ing in memory
{
    if(Count_Nodes("offered_courses.txt")>0)
    {
        Hash_Table_Linear_Size=Find_size(Count_Nodes("offered_courses.txt"));
        Hash_Table_Linear=(struct node **)calloc((Hash_Table_Linear_Size),sizeof(struct node*)); /* To allocate the whole block  in memory   */

        for(int i=0; i < Hash_Table_Linear_Size ; i++)
        {
            Hash_Table_Linear[i]=malloc(sizeof(struct node));  // to allocate the specific size of every segment in the array
            Hash_Table_Linear[i]->condition=0;  /*Making the condition for every node equals 0 ,hence empty */
        }
    }
}

void Insert_linear(course Course_info)///linear
{
    int LinearIndex;

    int key=String_Hashing(Course_info.name,Hash_Table_Linear_Size);
    int ind=(key % Hash_Table_Linear_Size);//if no collosion in the index found then it will be the index المعتمد
    if(Hash_Table_Linear[ind]->condition==0) //else collission
    {
        LinearIndex=ind;
    }
    else
    {
        int i=1;
        int next=(key + i) % Hash_Table_Linear_Size; //linear function

        while(Hash_Table_Linear[next]->condition!=0) // if its not empty, then collossion will occur , hence loop the while
        {
            i++;
            next=(key +i) % Hash_Table_Linear_Size;
        }
        Number_Collisions_Linear=Number_Collisions_Linear+i;

        LinearIndex=next;

    }
    //after getting the first empty spot -> then  change the info of it and its condition

    Hash_Table_Linear[LinearIndex]->course=Course_info;

    Hash_Table_Linear[LinearIndex]->condition=1;

    if(Spots_aquaried(Hash_Table_Linear,Hash_Table_Linear_Size) >(LOADFACTOR * Hash_Table_Linear_Size)) ///spots > LF ? rehash:
    {
        ReHash_Linear();
    }
}
//------------------------------------REHASH--------------------------//
void ReHash_Linear()
{ /* copy the data in another COURSE ARRAY *not node* while keeping the track of the old size,
    then free the old table ,
    after that re insert the data by traversing the COURSE array and inserting every index;
    */
    Number_Collisions_Linear=0; // new table -> new collisions
    int tempsize=Hash_Table_Linear_Size;
    int indexesNEW=Spots_aquaried(Hash_Table_Linear,Hash_Table_Linear_Size);
    course newTable[indexesNEW];
    int j=0;


///000
    Hash_Table_Linear_Size=Find_size(tempsize);
    Hash_Table_Linear=realloc(Hash_Table_Linear,(Hash_Table_Linear_Size) * sizeof(struct node*));
    for(int i=0 ; i<Hash_Table_Linear;i++)
    {
        Hash_Table_Linear[i]->condition=0;
    }
///000

   /* for(int i=0; i<tempsize; i++)
    {
        if(Hash_Table_Linear[i]->condition==1)
        {
            newTable[j]=Hash_Table_Linear[i]->course;
            j++;
        }
        Hash_Table_Linear[i]=NULL;
        free(Hash_Table_Linear[i]);
    }
    Hash_Table_Linear_Size=Find_size(tempsize);
    Hash_Table_Linear=malloc((Hash_Table_Linear_Size) * sizeof(struct node*));

    for(int i=0; i<Hash_Table_Linear_Size; i++)
    {
        Hash_Table_Linear[i]=malloc(sizeof(struct node));
        Hash_Table_Linear[i]->condition=0;
    }*/

    for(int i=0; i<indexesNEW; i++) //for loop to traverse the copied table * has old size*
    {
        Insert_linear(newTable[i]);
    }
}

int Find_index_linear(char name[])
{
    int key=String_Hashing(name,Hash_Table_Linear_Size);
    int i=0; //will be comparing from the first occurance of collission

    int newIndex=(key+i)% Hash_Table_Linear_Size;

    while(strcasecmp(name,Hash_Table_Linear[newIndex]->course.name)!=0) //not case sensetive in string.h header
    {
        i++;
        newIndex=(key+i)% Hash_Table_Linear_Size;
        if(Hash_Table_Linear[newIndex]->condition==0) //the next is  empty then no such name.// not included in table
            return -1;
    }
    if(Hash_Table_Linear[newIndex]->condition==1)
        return newIndex;
    else
        return -1;
}

//---------------------------------------------------Double table Functions-----------------------//
void create_Hash_Table_Double()
{
    if(Count_Nodes("offered_courses.txt")>0)
    {
        Hash_Table_Double_Size=Find_size(Count_Nodes("offered_courses.txt"));
        Hash_Table_Double=(struct node **)calloc((Hash_Table_Double_Size),sizeof(struct node*));

        for(int i=0; i<Hash_Table_Double_Size; i++)
        {
            Hash_Table_Double[i]=malloc(sizeof(struct node));
            Hash_Table_Double[i]->condition=0;
        }

    }
}


void Insert_double(course Course_info)
{
    int doubleIndex;

    int key=String_Hashing(Course_info.name,Hash_Table_Double_Size);
    int ind=(key % Hash_Table_Double_Size); // no need to use the equation at first , cuz it will be kinda the same since i=0 -> hash2 =0;

    if(Hash_Table_Double[ind]->condition==0)
        doubleIndex=ind;
    else
    {
        int hash1,hash2,i=1;
        int r=find_Less_Prime(Hash_Table_Double_Size);
        hash1=(key % Hash_Table_Double_Size);
        hash2=r-(key % r);

        int next=(hash1 + (hash2 * i)) % Hash_Table_Double_Size;

        while(Hash_Table_Double[next]->condition!=0)
        {
            i++;
            next=(hash1 + (i * hash2)) % Hash_Table_Double_Size;
        }
        Number_Collisions_Double=Number_Collisions_Double+i;
        doubleIndex=next;
    }

    Hash_Table_Double[doubleIndex]->course=Course_info;

    Hash_Table_Double[doubleIndex]->condition=1;


    if(Spots_aquaried(Hash_Table_Double,Hash_Table_Double_Size) > (LOADFACTOR *Hash_Table_Double_Size))
    {
        ReHash_Double();
    }

}
//------------------------------------REHASH--------------------------//

void ReHash_Double()
{
    Number_Collisions_Double=0;
    int tempSize=Hash_Table_Double_Size;
    int indexesNEW=Spots_aquaried(Hash_Table_Double,Hash_Table_Double_Size);
    course newTable[indexesNEW];
    int j=0;

    for(int i=0; i<tempSize; i++)
    {
        if(Hash_Table_Double[i]->condition==1)
        {
            newTable[j]=Hash_Table_Double[i]->course;
            j++;
        }
        Hash_Table_Double[i]=NULL;
        free(Hash_Table_Double[i]);
    }
    Hash_Table_Double_Size=Find_size(tempSize);
    Hash_Table_Double=malloc((Hash_Table_Double_Size)* sizeof(struct node *));

    for(int i=0; i<Hash_Table_Double_Size ; i++)
    {
        Hash_Table_Double[i]=malloc(sizeof(struct node));
        Hash_Table_Double[i]->condition=0;
    }

    for(int i=0; i<indexesNEW; i++)
    {
        Insert_double(newTable[i]);
    }
}

int find_index_double(char name[])
{
    int key=String_Hashing(name,Hash_Table_Double_Size);
    int hash1,hash2,i=0;

    hash1=(key %Hash_Table_Double_Size);
    hash2=find_Less_Prime(Hash_Table_Double_Size)-(key % find_Less_Prime(Hash_Table_Double_Size));

    int newIndex=(hash1 + (hash2 * i)) % Hash_Table_Double_Size;

    while(strcasecmp(name,Hash_Table_Double[newIndex]->course.name)!=0)
    {
        i++;
        newIndex=(hash1 +( i * hash2)) % Hash_Table_Double_Size;

        if(Hash_Table_Double[newIndex]->condition==0)
            return -1;
    }
    if(Hash_Table_Double[newIndex]->condition==1) //course found return index
        return newIndex;
    else //con = 2 hence deleted
        return -1;
}



//-----------------------------------Print Functions----------------//
void Print_Table(struct node **Hash_Table, int size)///both
{

    printf("\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n");
    printf("Course Name\t\t\t Credit Hours\t\tCourse Code\t\t                 Department\n");
    printf("------------------------------------------------------------------------------------------------------------------------\n");

    for (int index = 0; index < size; index++)
    {
        if (Hash_Table[index]->condition == 2)
            printf("\t\t****************************************Deleted Spot****************************************\n\n");

        else if (Hash_Table[index]->condition == 0)
            printf("\t\t****************************************Empty Spot******************************************\n\n");

        else
        {
            printf("%-25s\t\t%d\t\t %-25s\t\t%-25s\n\n",Hash_Table[index]->course.name,Hash_Table[index]->course.credit_hours,Hash_Table[index]->course.code,Hash_Table[index]->course.department);

        }

    }
    printf("------------------------------------------------------------------------------------------------------------------------\n");
}

void print_File(struct node **Table,int size,FILE *f)
{
    for (int i=0; i<size; i++)
    {
        if(Table[i]->condition==1)
            fprintf(f,"%s:%d#%s#%s/%s\n",Table[i]->course.name,Table[i]->course.credit_hours,Table[i]->course.code,Table[i]->course.department,Table[i]->course.topics);
    }
}


void sys();
int main()
{
    sys();

    ReHash_Linear();


    create_Hash_Table_Double();
    create_Hash_Table_Linear();
    Read_file();

    course tempc;
    char c;

    while(1)
    {
        printf("\n1. Print hashed tables (including empty spots).\n");
        printf("2. Print out table size and the load factor.\n");
        printf("3. Print out the used hash functions.\n");
        printf("4. Insert a new record to hash table (insertion will be done on both hash tables).\n");
        printf("5. Search for a specific word (specify which table to search in).\n");
        printf("6. Delete a specific record (from both tables).\n");
        printf("7. Compare between the two methods in terms of number of collisions occurred.\n");
        printf("8. Save hash table back to a file named saved_courses.txt (of the double hashing)\n");

        int choice;
        printf("Enter Your choice: ");
        scanf("%d",&choice);

        switch(choice)
        {
        case 1:
            system("cls");

            printf("*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*Linear Hash Table*\\*\\*\\*\\*\\*\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*");
            Print_Table(Hash_Table_Linear,Hash_Table_Linear_Size);
            printf("\n===========================================================================================================================\n");
            printf("\n*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*Double Hash Table*\\*\\*\\*\\*\\*\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*\\*");
            Print_Table(Hash_Table_Double,Hash_Table_Double_Size);

            break;

        case 2:
            system("cls");
            printf("**************************\n");
            printf("Table Size: %d           *\n",Hash_Table_Linear_Size);
            printf("Load Factor: %.2lf        *\n",LOADFACTOR);
            printf("**************************\n");
            break;

        case 3:
            system("cls");
            printf("********************************************\n");
            printf("The function used in Linear hash table:\n(Key + i) MOD %d \n\n",Hash_Table_Linear_Size);
            printf("The function used in Double hash table:\nhash1=(Key MOD %d)\nhash2=%d - (Key MOD %d)\nHash(key)=hash1+ (i*hash2)) MOD %d\n",Hash_Table_Double_Size,find_Less_Prime(Hash_Table_Double_Size),find_Less_Prime(Hash_Table_Double_Size),Hash_Table_Double_Size);
            printf("********************************************\n");
            break;

        case 4:
            system("cls");
            printf("Enter A Course Name: ");
            scanf("%c",&c);
            gets(tempc.name);

            printf("Enter The Credit Hours: ");
            scanf("%d",&tempc.credit_hours);

            printf("Enter The Code: ");
            scanf("%c",&c);
            strupr(gets(tempc.code));



            printf("Enter The Department:  ");
            //scanf("%c",&c);
            gets(tempc.department);

            printf("Enter the Topic: ");
            //scanf("%c",&c);
            gets(tempc.topics);


            Insert_linear(tempc);
            Insert_double(tempc);


            break;

        case 5:
            system("cls");
            printf("Which Table you want to search in?\n");
            printf("1. Linear hashing table\n");
            printf("2. Double hashing table\n");
            printf("Enter your Choice: ");

            int choice_table;
            scanf("%d",&choice_table);

            switch (choice_table)
            {
            case 1:

                printf("Enter A Course Name: ");
                scanf("%c",&c);
                char name[25];
                gets(name);

                int index=Find_index_linear(name);
                if(index==-1)
                    printf("Course Not Found\n");
                else
                {
                    system("cls");
                    printf("******************\n");
                    printf("Linear Table\nCourse Found At Index: %d",index);
                    printf("\n");
                    printf("Course Name: %s\nCourse Code: %s\nCredit Hours: %d\nDepartment: %s\nTopics: %s",Hash_Table_Linear[index]->course.name,Hash_Table_Linear[index]->course.code,Hash_Table_Linear[index]->course.credit_hours,Hash_Table_Linear[index]->course.department,Hash_Table_Linear[index]->course.topics);
                    printf("\n******************");
                }

                break;

            case 2:
                printf("Enter A course Name: ");
                scanf("%c",&c);
                char name2[25];
                gets(name2);

                int indexd=find_index_double(name2);

                if(indexd==-1)
                    printf("Course Not Found\n");
                else
                {
                    system("cls");
                    printf("******************\n");
                    printf("Double Table\nCourse Found At Index: %d",indexd);
                    printf("\n");
                    printf("Course Name: %s\nCourse Code: %s\nCredit Hours: %d\nDepartment: %s\nTopics: %s",Hash_Table_Double[indexd]->course.name,Hash_Table_Double[indexd]->course.code,Hash_Table_Double[indexd]->course.credit_hours,Hash_Table_Double[indexd]->course.department,Hash_Table_Double[indexd]->course.topics);
                    printf("\n******************");
                }

                break;

            default:
                printf("wrong choice Re-Enter\n");
            }



            break;

        case 6:
            system("cls");
            printf("Enter A Course Name: ");
            scanf("%c",&c);
            char name[25];
            gets(name);

            int index=Find_index_linear(name);
            ///
            if(index !=-1)
            {
                Hash_Table_Linear[index]->condition=2;
            }

            index=find_index_double(name);

            if(index !=-1)
            {
                Hash_Table_Double[index]->condition=2;
            }



            break;

        case 7:
            system("cls");
            printf("*****************************************\n");
            printf("%d Collissions in Linear Hashing table  *\n",Number_Collisions_Linear);
            printf("%d Collissions in Double Hashing table  *\n",Number_Collisions_Double);
            printf("*****************************************\n");



            break;

        case 8:
            fclose(courses_file);
            courses_file=fopen("offered_courses.txt","w");
            print_File(Hash_Table_Double,Hash_Table_Double_Size,courses_file);
            fclose(courses_file);

            printf("\nThanks for ur time!\nData Saved!\n");

            // or restart the pc :)

            for(int i=0; i<Hash_Table_Double_Size; i++)
            {
                Hash_Table_Double[i]=NULL;
                free(Hash_Table_Double[i]);
            }

             for(int i=0; i<Hash_Table_Linear_Size; i++)
            {
                Hash_Table_Linear[i]=NULL;
                free(Hash_Table_Linear[i]);
            }


            return;
            break;

        default:
            printf("\nWrong Choice Re-Enter!\n");
        }

    }

    return 0;
}
void sys()
{
    system("title Khuffash");
    system("color a");
}
//hi
