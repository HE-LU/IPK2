#include <iostream>
#include <unistd.h>
#include <string>
#include <cctype>
#include <sstream>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <cstring>
#include <fstream>

#include <vector>

#include "socket.h"

/*
 * 0 0 0  0 0 0 0 tuxik pepa pavel
 * L U G N H S ? DATA
 */

using namespace std;

typedef struct struct_params
{
  int p;
} struct_params;

struct_params get_params(int, char**);

int main(int argc, char** argv)
{
  struct_params params = get_params(argc, argv);
    
  
  
  class_socket socket;
  if(socket.s_listen(params.p) != 0)
  {
    cout << "Cannost listen!" << endl;
    exit(-1);
  }
  
  
  
  while(1) 
  {
    pid_t child_pid;
    cout << "Server listening on port: " << htons(params.p) << endl;
   
    
    sockaddr_in clientAddr;
    socklen_t sin_size=sizeof(struct sockaddr_in);
    class_socket client_sock;
    client_sock.sock = accept(socket.sock, (struct sockaddr*)&clientAddr, &sin_size);
    
    cout << "Client connected! ID: " << client_sock.sock << endl;
    
    
    
    child_pid = fork();
    
    if (child_pid == 0) 
    {
      cout << "FORKED!" << endl;
      
      string received_string = client_sock.s_read();

      cout << "Server received:  " << received_string << endl;
      
      string response_params;
      bool uid = 0;
      string response;
      
  
      response_params = received_string.substr(0,6);
      string data = received_string.substr(7,received_string.size()-7);
      
      vector<string> find;
      unsigned r_start = 0;
      unsigned r_end = data.size();
      unsigned cmp = -1;
      
      cout << "START: " << r_start << " END: " << r_end << endl;
      
      data.append(" ");
      while(r_start != cmp)
      {
        unsigned r_mezera = data.find(" ",r_start);
        if (r_mezera != cmp)
          find.push_back(data.substr(r_start, r_mezera - r_start));
        else
          break;
        r_start = r_mezera+1;
      }
               
      ifstream file;
      file.open("/etc/passwd");
      if(!file.good())
      {
        return 1; // exit if file not found
      }    
      
      while (!file.eof())
      {
        bool parse = false;
        string radek;
        getline(file,radek);
        
        if(uid)
        {
          string s_uid;
          unsigned uid_start = radek.find(":");
          uid_start = radek.find(":",uid_start+1);
          unsigned uid_end = radek.find(":",uid_start+1);
          
          for(int i=0;i<find.size();i++)
          {
            cout << "Comparing : " << s_uid << " with: " << find.at(i) << endl;
            if(find.at(i) == s_uid)
              parse = true;
          }
        }
        else
        {
          string s_login;
          unsigned login_start = 0;
          unsigned login_end = radek.find(":",login_start);
          
          s_login = radek.substr(login_start,login_end-login_start); 
          
          for(int i=0;i<find.size();i++)
          {
            cout << "Comparing : " << s_login << " with: " << find.at(i) << endl;
            if(find.at(i) == s_login)
              parse = true;
          }
        }
        
        if(parse)
        {
          cout << "Parse: " << radek << endl;
          // login_name:passwd:UID:GID:user_name:directory:shell
          //     L             U   G    N         H      S    
          //     1              2  3     4         5      6
          
          // LOGIN_NAME
          unsigned parser_start = 0;
          unsigned parser_end = radek.find(":");
          unsigned data_end = radek.size();
          vector<string> parsed; 
          parsed.push_back(""); // 0
          parsed.push_back(radek.substr(parser_start,parser_end-parser_start)); // 1
          
          // PASSWORD
          parser_start = parser_end;
          parser_end = radek.find(":",parser_start+1);
          
          // UID
          parser_start = parser_end;
          parser_end = radek.find(":",parser_start+1);
          parsed.push_back(radek.substr(parser_start+1,parser_end-parser_start-1)); // 2
          
          // GID
          parser_start = parser_end;
          parser_end = radek.find(":",parser_start+1);
          parsed.push_back(radek.substr(parser_start+1,parser_end-parser_start-1)); // 3
          
          // USER_NAME
          parser_start = parser_end;
          parser_end = radek.find(":",parser_start+1);
          parsed.push_back(radek.substr(parser_start+1,parser_end-parser_start-1)); // 4
          
          // DIRECTORY
          parser_start = parser_end;
          parser_end = radek.find(":",parser_start+1);
          parsed.push_back(radek.substr(parser_start+1,parser_end-parser_start-1)); // 5
          
          // SHELL
          parser_start = parser_end;
          parsed.push_back(radek.substr(parser_start+1,data_end-2)); // 6
          
          for(int i=0;i<6;i++)
          {
            int num = response_params.at(i);
            num = num - '0';
            response.append(parsed.at(num));
            response.append(" ");
          }
        }        
      }
      
      cout << "Sending!" << endl << response << endl;
      client_sock.s_write(response);
      
      cout << "DONE" << endl;
      
      cout << endl << uid << endl;
      
      client_sock.s_disconnect();
    }
  }
  
  return 0;
}

struct_params get_params(int argc, char** argv)
{
  struct_params par;
  par.p = -1;
  
  int c;
  
  while( (c = getopt(argc, (char**)argv, "p:?")) != -1)
  {
    if(c == 'p' && optarg)
    {
      istringstream stream(optarg);
      stream >> par.p;
      
      ostringstream o_stream;
      o_stream << par.p;
      
      if(o_stream.str().compare(optarg))
      {
        cerr << "Chyba: zadany port neni cislo!" << endl;
        exit (-1);
      }
    }
    else
    {
      cerr << "Chyba: neznamy parametr!" << endl;
      exit(-1);
    }
  }
  
  if(par.p < 0)
  {
    cout << "server -p port" << endl;
    exit(1);
  }
  
  return par;
}