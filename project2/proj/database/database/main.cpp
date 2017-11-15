//  EECS 495 Database Project 3.
//
// Team Memebers:
// Lizhi Liang (lll0891)
// Tianju Ma (tml5872)

#include <mysql.h>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

void login();
void transcript();
void course_detail();
void withdraw1();
void personal_details();
void student_menu();
void Enroll();
bool canenroll(string year, string quarter,string courseID,string q1,string y1,string q2,string y2);
bool cannotenroll(string year, string quarter,string courseID,string q1,string y1,string q2,string y2);
bool enrolled(string courseID);


string student_id;
string student_password;

MYSQL *conn = mysql_init(NULL);
MYSQL *connection_result;

int main (int argc, const char * argv[]) {
    mysql_real_connect(conn, "localhost", "root", "Ma930729", "project3-nudb", 0, NULL, CLIENT_MULTI_RESULTS);
    login();
}

void login() {
    while (true) {
        cout << "please enter your student id:\n";
        cin >> student_id;
        
        cout << "please enter your pasword:\n";
        cin >> student_password;
        
        string statement ="SELECT * FROM student WHERE Id = '"+student_id+"' and Password = '"+student_password+"';";
        
        mysql_query(conn, statement.c_str());
        string error = mysql_error(conn);
        
        MYSQL_RES *res_set;
        
        res_set = mysql_store_result(conn);
        int numrows = (int)mysql_num_rows(res_set);
        
        if (numrows == 0) {
            cout << "Invalid information, please check the student id and password and try again.\n";
            cout << "\n";
        } else {
            string error = mysql_error(conn);
            student_menu();
            break;
        }
    }
}

void student_menu() {
    cout << "student menu:\n";
    cout << "\n";
    time_t nowtime;
    nowtime = time(NULL);
    struct tm *local;
    local=localtime(&nowtime);
    string year = to_string(local->tm_year+1900);
    int month = local->tm_mon+1;
    string quarter;
    if(month >= 9 && month <= 12)
    {
        quarter = "Q1";
    }
    else if(month >= 1 && month <= 3)
    {
        quarter = "Q2";
    }
    else if(month >= 4 && month <= 6)
    {
        quarter = "Q3";
    }
    else
    {
        quarter = "Q4";
    }
    cout << "Student Id:"<<student_id<<"   Quarter:"<<quarter<<"   Year:"<< year<<endl;
    
    mysql_close(conn);
    conn = mysql_init ( NULL );
    mysql_real_connect(conn, "localhost", "root", "Ma930729", "project3-nudb", 0, NULL, CLIENT_MULTI_RESULTS);
    
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    
    string statement = "SELECT * FROM transcript WHERE StudId = '" + student_id + "' and Year = '"+ year + "' and Semester= '"+ quarter + "' and Grade is NULL;";
    
    mysql_query(conn, statement.c_str());
    string error = mysql_error(conn);
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    cout << "current courses:" << endl;
    
    for (int i = 0; i < numrows; i++)
    {
        row = mysql_fetch_row(res_set);
        if(row != NULL)
        {
            cout << "Course:" << row[1]<<endl;
        }
    }
    mysql_free_result( res_set );
    
    while (true) {
        cout << "\n";
        int options;
        cout <<"please make your choice:\n";
        cout << "1. Transcript\n";
        cout << "2. Enroll\n";
        cout << "3. Withdraw\n";
        cout << "4. Personal Details\n";
        cout << "5. Logout\n";
        cin >> options;
        
        switch (options) {
            case 1: {
                transcript();
                break;
            }
            case 2: {
                Enroll();
                break;
            }
            case 3: {
                withdraw1();
                break;
            }
            case 4: {
                personal_details();
                break;
            }
            case 5: {
                login();
                break;
            }
            default:{
                cout << "invalid option, please try again\n" << endl;
                student_menu();
            }
        }
    }
}

void transcript() {
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    
    string statement ="SELECT UoSCode, Grade FROM transcript WHERE StudId = '"+student_id+"';";
    
    mysql_query(conn, statement.c_str());
    
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    if (numrows == 0) {
        cout << "This student hasn't taken any course yet!!!!!!!!!!!!!!!!!!!\n";
    } else {
        cout << "\n";
        for (int i = 0; i < numrows; i++) {
            row = mysql_fetch_row( res_set );
            if( row != NULL ) {
                cout << "UoSCode : " << row[0] << endl;
                if (row[1] == NULL) {
                    cout << "Grade: " << "NULL" << endl;
                } else {
                    cout << "Grade: " << row[1] << endl;
                }
                cout << "\n";
            }
        }
        // free resources
        mysql_free_result( res_set );
    }
    
    while (true) {
        string ope_num;
        cout << "Please enter the next operation you want to take:\n";
        cout << "1: Go to Course Details.\n";
        cout << "2: Go to Stduent Menu.\n";
        cin >> ope_num;
        
        if (ope_num == "1") {
            course_detail();
            return;
        } else if (ope_num == "2") {
            student_menu();
            return;
        } else {
            cout << "invalid input, please try again.\n";
        }
    }
    
}

void course_detail() {
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    
    string course_num;
    cout << "Please enter the course number you want to inspect:\n";
    cin >> course_num;
    
    string statement = "select a.UoSCode, a.UoSName, a.Year, a.Semester, a.Enrollment, a.MaxEnrollment, a.Name, b.Grade from( select UoSCode, UoSName, Year, Semester, Enrollment, MaxEnrollment, Name from (unitofstudy inner join uosoffering using(UoSCode)) inner join faculty on Id = InstructorId where UoSCode = ( select UoSCode from transcript where StudId = '"+student_id+"' and UoSCode = '"+course_num+"') and Semester = (select Semester from transcript where StudId = '"+student_id+"' and UoSCode = '"+course_num+"') and Year = (select Year from transcript where StudId = "+student_id+" and UoSCode = '"+course_num+"')) a, transcript as b where b.UoSCode = '"+course_num+"' and b.StudId = '"+student_id+"';";
    
    mysql_query(conn, statement.c_str());
    
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    if (numrows == 0) {
        cout << "Invalid Course Number !!!!!!!!!!!!!!!!!!!\n";
    } else {
        for (int i = 0; i < numrows; i++) {
            row = mysql_fetch_row( res_set );
            if ( row != NULL ) {
                cout << "UoSCode : " << row[0] << endl;
                cout << "UoSName: " << row[1] << endl;
                cout << "Year : " << row[2] << endl;
                cout << "Semester : " << row[3] << endl;
                cout << "Enrollment : " << row[4] << endl;
                cout << "MaxEnrollment : " << row[5] << endl;
                cout << "Name : " << row[6] << endl;
                if (row[7] == NULL) {
                    cout << "Grade: " << "NULL" << endl;
                } else {
                    cout << "Grade: " << row[7] << endl;
                }
                cout << "\n";
            }
        }
    }
    // free resources
    mysql_free_result( res_set );
}

void withdraw1()
{
    string withdraw_course;
    string option;
    string statement;
    
    cout << "These are the courses that you can withdraw from:\n";
    cout << "\n";
    
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    
    statement = "SELECT transcript.UoSCode, UoSName, Semester, Year FROM transcript join unitofstudy WHERE transcript.UoSCode = unitofstudy.UoSCode AND StudId = '"+student_id+"' AND transcript.Grade is NULL;";
    
    mysql_query(conn, statement.c_str());
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    if ( numrows == 0 ) {
        cout << "There are no course you can withdraw.\n";
        student_menu();
        return;
    } else {
        for (int i = 0; i < numrows; i++) {
            row = mysql_fetch_row( res_set );
            if( row != NULL ) {
                cout << "Course Code: " << row[0] << endl;
                cout << "Course Name: " << row[1] << endl;
                cout << "Semester : " << row[2] << endl;
                cout << "Year: " << row[3] << endl;
                cout << "\n";
            }
        }
    }
    
    while (true) {
        cout << "Enter the course you want to withdraw:\n";
        cin >> withdraw_course;
        
        MYSQL_RES *res_set;
        MYSQL_ROW row;
        statement = "SELECT transcript.UoSCode FROM transcript WHERE StudId = '"+student_id+"' AND transcript.Grade is NULL and UoSCode = '"+withdraw_course+"';";
        
        mysql_query(conn, statement.c_str());
        res_set = mysql_store_result(conn);
        int numrows = (int)mysql_num_rows(res_set);
        
        
        if (numrows == 0) {
            cout << "Can not withdraw from this course.\n";
            cout << "Press 1 to go back to menu. Press other to continue withdrawing.";
            cin >> option;
            if(option == "1") return;
            else continue;
        } else {
            statement = "call withdraw_procedure('"+withdraw_course+"', '"+student_id+"');";
            
            if(mysql_query(conn,statement.c_str()) == 0) {
                do {
                    if(mysql_field_count(conn) > 0) {
                        res_set = mysql_store_result(conn);
                        mysql_free_result(res_set);
                    }
                } while(mysql_next_result(conn) == 0);
            }
            cout << "Withdraw successful.\n";
            cout << "\n";
            statement = "select @message;";
            mysql_query(conn,statement.c_str());
            res_set = mysql_store_result(conn);
            row = mysql_fetch_row(res_set);
            if(strcmp(row[0], "The enrollment is smaller than half.") == 0) {
                cout << "Warning: The enrollment is smaller than half.\n";
            }
        }
        cout << "What do you want to do next?\n";
        cout << "1. continue withdrawing.\n";
        cout << "2. go back to main menu.\n";
        cin >> option;
        
        if(option == "1") {
            continue;
        } else if (option == "2") {
            student_menu();
            return;
        }
    }
}

void personal_details() {
    string operation;
    string statement;
    
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    statement = "select * from student where Id = '" + student_id + "'";
    
    mysql_query(conn, statement.c_str());
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    cout << "\n";
    for (int i = 0; i < numrows; i++) {
        row = mysql_fetch_row( res_set );
        if ( row != NULL ) {
            cout << "Id : " << student_id << endl;
            cout << "student name: " << row[1] << endl;
            cout << "password : " << row[2] << endl;
            cout << "address : " << row[3] << endl;
            
        }
    }
    mysql_free_result( res_set );
    
    cout << "Please enter the information type you want to modify: (password/address)\n";
    cin >> operation;
    
    if (operation == "password") {
        string new_password;
        cout << "Please enter the new password:\n";
        cin >> new_password;
        statement = "update student set Password = '" +new_password +"' where Id = '" + student_id + "';";
        if (mysql_query(conn,statement.c_str()) == 0) {
            cout << "modification success !!!!!!!!!\n";
        } else {
            cout << "Some error occurs !!!!!!!!!\n";
        }
    } else if (operation == "address") {
        string new_address;
        cout << "Please enter the new address:\n";
        cin >> new_address;
        statement = "update student set Address = '" + new_address +"' where Id = '" + student_id + "';";
        if (mysql_query(conn,statement.c_str()) == 0) {
            cout << "modification success !!!!!!!!!\n";
        } else {
            cout << "Some error occurs !!!!!!!!!\n";
        }
    } else {
        cout << "invalid information type.";
    }
}

void Enroll() {
    time_t nowtime;
    nowtime = time(NULL);
    struct tm *local;
    local=localtime(&nowtime);
    string year1 = to_string(local->tm_year+1900);
    int month = local->tm_mon+1;
    string quarter1;
    if(month >= 9 && month <= 12)
    {
        quarter1 = "Q1";
    }
    else if(month >= 1 && month <= 3)
    {
        quarter1 = "Q2";
    }
    else if(month >= 4 && month <= 6)
    {
        quarter1 = "Q3";
    }
    else
    {
        quarter1 = "Q4";
    }
    
    string year2;
    string quarter2;
    if(quarter1== "Q1"){
        quarter2 = "Q2";
        year2 = to_string(local->tm_year+1901);
    }
    else if(quarter1== "Q2"){
        quarter2 = "Q3";
        year2 = year1;
    }
    else if (quarter1== "Q3"){
        quarter2 = "Q4";
        year2 = year1;
    }
    else if (quarter1== "Q4"){
        quarter2 = "Q1";
        year2 = year1;
    }
    
    MYSQL_RES *res_set;
    MYSQL_ROW row;
    string temp = "SELECT a.UoSCode,a.Semester,a.Year FROM uosoffering a where ((a.Semester = '"+quarter1+"' and a.year = '"+year1+"') or (a.Semester = '"+quarter2+"'and a.year = '"+year2+"')) and a.UoSCode not in (select c.UoScode from transcript c where c.StudId = '"+student_id+"' );";
    mysql_query(conn, temp.c_str());
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    
    
    cout << "please enroll in the following courses:" << endl;
    for (int i = 0; i < numrows; i++)
    {
        row = mysql_fetch_row(res_set);
        if(row != NULL)
        {
            cout << "CourseID: " << row[0]<< "   Quarter: " << row[1]<< "   Year: " << row[2] << endl;
        }
    }
    mysql_free_result( res_set );
    
    string enrollID;
    string quarter;
    int year;
    cout << "please enter the course ID you want to enroll in:"<<endl;
    cin >> enrollID;
    
    cout << "please enter the quarter you want to enroll in"<<endl;
    cin >> quarter;
    
    cout << "please enter the Year you want to enroll in"<<endl;
    cin >> year;
    
    if(!(to_string(year) == year1 || to_string(year) == year2)){
        cout<<"invalid year"<<endl;
        Enroll();
    }
    if(!(quarter == quarter1 || quarter == quarter2)){
        cout<<"invalid quarter"<<endl;
        Enroll();
    }
    
    if (enrolled(enrollID)){
        cout<<"you already enroll in this course"<<endl;
        cout << "Enter 1 to go to enroll menu" << endl
        << "Enter 2 to go to student menu" << endl;
        int options;
        cin >> options;
        switch (options) {
            case 1: {
                Enroll();
                break;
            }
            case 2: {
                student_menu();
                break;
            }
            default:{
                cout << "invalid option, back to student menu" << endl;
                student_menu();
            }
        }
    }
    else if(canenroll(to_string(year),quarter,enrollID,quarter1,year1,quarter2,year2)){
        
        MYSQL_RES *res_set;
        string temp = "CALL addcourse ("+student_id+",'"+enrollID+"','"+ to_string(year) +"','"+quarter+"');";
        
        mysql_close(conn);
        conn = mysql_init ( NULL );
        mysql_real_connect(conn, "localhost", "root", "Ma930729", "project3-nudb", 0, NULL, CLIENT_MULTI_RESULTS);
        
        mysql_query(conn, temp.c_str());
        string error = mysql_error(conn);
        if (error == "") {
            res_set = mysql_store_result(conn);
            cout<< "enroll successful"<<endl;
            mysql_free_result( res_set );
            cout << "Enter 1 to go to enroll menu" << endl
            << "Enter 2 to go to student menu" << endl;
            int options;
            cin >> options;
            switch (options) {
                case 1: {
                    Enroll();
                    break;
                }
                case 2: {
                    student_menu();
                    break;
                }
                default:{
                    cout << "invalid option, back to student menu" << endl;
                    student_menu();
                }
            }
        } else {
            cout << "Some error happens, please try again." << endl;
        }
    }
    else if((cannotenroll(to_string(year),quarter,enrollID,quarter1,year1,quarter2,year2))){
        cout<<"you cannot enroll in this course, please complete the following pre-requisites first"<<endl;
        
        MYSQL_RES *res_set;
        string preclass="CALL prereq('"+enrollID+"','"+student_id +"');" ;
        
        mysql_close(conn);
        conn = mysql_init ( NULL );
        mysql_real_connect(conn, "localhost", "root", "Ma930729", "project3-nudb", 0, NULL, CLIENT_MULTI_RESULTS);
        
        mysql_query(conn, preclass.c_str());
        res_set = mysql_store_result(conn);
        int numrows3 = (int)mysql_num_rows(res_set);
        
        for (int i = 0; i < numrows3; i++)
        {
            row = mysql_fetch_row(res_set);
            if(row != NULL)
            {
                cout << row[0] << endl;
            }
        }
        mysql_free_result( res_set );
        
        cout << "Enter 1 to go to enroll menu" << endl
        << "Enter 2 to go to student menu" << endl;
        int options;
        cin >> options;
        switch (options) {
            case 1: {
                Enroll();
                break;
            }
            case 2: {
                student_menu();
                break;
            }
            default:{
                cout << "invalid option, back to student menu" << endl;
                student_menu();
            }
        }
    }
    else{
        cout<<"invalid course or year or quarter, please try again"<<endl;
        Enroll();
    }
}

bool canenroll(string year, string quarter,string courseID,string q1,string y1,string q2,string y2){
    MYSQL_RES *res_set;
    string statement = "CALL canenroll('"+student_id+"','"+year+"','"+quarter+"','"+courseID+"','"+q1+"','"+y1+"','"+q2+"','"+y2+"');" ;
    
    mysql_close(conn);
    conn = mysql_init ( NULL );
    mysql_real_connect(conn, "localhost", "root", "Ma930729", "project3-nudb", 0, NULL, CLIENT_MULTI_RESULTS);
    
    mysql_query(conn, statement.c_str());
    string error = mysql_error(conn);
    
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    mysql_free_result( res_set );
    
    return numrows;
}

bool cannotenroll(string year, string quarter,string courseID,string q1,string y1,string q2,string y2){
    MYSQL_RES *res_set;
    string statement = "CALL cannotenroll('"+student_id+"','"+year+"','"+quarter+"','"+courseID+"','"+q1+"','"+y1+"','"+q2+"','"+y2+"');" ;
    
    mysql_close(conn);
    conn = mysql_init ( NULL );
    mysql_real_connect(conn, "localhost", "root", "Ma930729", "project3-nudb", 0, NULL, CLIENT_MULTI_RESULTS);
    
    mysql_query(conn, statement.c_str());
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    mysql_free_result( res_set );
    
    return numrows;
}

bool enrolled(string courseID){
    MYSQL_RES *res_set;
    string statement = "CALL enrolled('"+student_id+"','"+courseID+"');" ;
    
    mysql_close(conn);
    conn = mysql_init ( NULL );
    mysql_real_connect(conn, "localhost", "root", "Ma930729", "project3-nudb", 0, NULL, CLIENT_MULTI_RESULTS);
    
    mysql_query(conn, statement.c_str());
    res_set = mysql_store_result(conn);
    int numrows = (int)mysql_num_rows(res_set);
    mysql_free_result( res_set );
    
    return numrows;
}

