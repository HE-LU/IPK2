#include <iostream>
#include <fstream>
#include <vector>
#include <sys/wait.h>
 
#include "socket.h"

using namespace std;

// ------------------ STRUCT ----------------
typedef struct struct_params
{
  int p;
} struct_params;
// ------------------ STRUCT ----------------


struct_params get_params(int, char**);

void child_die(int sig)
{
  sig = sig; // get rid of warning!
  while(waitpid(-1,0,WNOHANG)>0);
}

// ------------------ MAIN ----------------
int main(int argc, char** argv)
{
  (void) signal(SIGCHLD, child_die);
  
  struct_params params = get_params(argc, argv);
    
  // ------- Listen on socket -------------
  class_socket socket;
  if(socket.s_listen(params.p) != 0)
  {
    cout << "Cannost listen!" << endl << flush;
    exit(-1);
  }
  // ------- Listen on socket -------------
  
  
  while(1) 
  {
    pid_t child_pid;
    
    //cout << "Server listening on port: " << htons(params.p) << endl << flush;
    
    sockaddr_in clientAddr;
    socklen_t sin_size=sizeof(struct sockaddr_in);
    class_socket client_sock;
    client_sock.sock = accept(socket.sock, (struct sockaddr*)&clientAddr, &sin_size);
    
    //cout << "Client connected! ID: " << client_sock.sock << endl << flush;
    
    child_pid = fork();
    for(int z = 0; child_pid < 0 && z < 6; z++)
    {
      cerr << "FORK FAILED!" << endl;
      usleep(10000);
      child_pid = fork();
    }
    
    if (child_pid < 0)
    {
      cerr << "OU OU Fork Failed 5 times... exiting!" << endl;
      exit(-1);
    }
      
    if (child_pid == 0) 
    {         
      string received_string = client_sock.s_read();

      //cout << "Server received:  " << received_string << endl << flush;
      
      string response_params; 
      string response;
      
      response_params = received_string.substr(0,6);
      string data = received_string.substr(7,received_string.size()-7);
      string uid = received_string.substr(6,1);
      
      vector<string> find;
      unsigned r_start = 0;
      unsigned cmp = -1;
      
      //cout << "START: " << r_start << " END: " << r_end << endl << flush;
      
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
        cerr << "/etc/passwd does not exist... what?" << endl;
        return 1; // exit if file not found
      }    
      
      // Loop for all logins or uids
      for(unsigned int y=0;y<find.size();y++)
      {
        file.clear();
        file.seekg (0, file.beg);
        bool parsed = false;
        
        while (!file.eof())
        {
          bool parse = false;
          string radek;
          getline(file,radek);
                 
          if(uid == "0")
          {
            string s_uid;
            unsigned uid_start = radek.find(":");
            uid_start = radek.find(":",uid_start+1);
            unsigned uid_end = radek.find(":",uid_start+1);
            
            s_uid = radek.substr(uid_start+1,uid_end-uid_start-1); 
            
            if(find.at(y) == s_uid)
            {
              parse = true;
              parsed = true;
            }
          }
          else
          {
            string s_login;
            unsigned login_start = 0;
            unsigned login_end = radek.find(":",login_start);
            
            s_login = radek.substr(login_start,login_end-login_start); 
            
            if(find.at(y) == s_login)
            {
              parse = true;
              parsed = true;
            }
          }
          
          if(parse)
          {
            //cout << "Parse: " << radek << endl << flush;
            
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
                if(((i+1) < 6) && response_params.at(i+1) != '0')
                {
                  //cout << " Next is: \"" << response_params.at(i+1) << "\" Appending space!" << endl;
                  response.append(" ");
                }  
            }
            response.append("\n");
          }        
        }
        if(!parsed)
        {
          if(uid == "0")
            response.append("::ERROR:: UID unknown:"+find.at(y)+"\n");
          else
            response.append("::ERROR:: Login unknown:"+find.at(y)+"\n");
        }
      }
      
      client_sock.s_write(response);
      client_sock.s_write("::END::");
      
      client_sock.s_disconnect();
    }
  }
  
  return 0;
}
// ------------------ MAIN ----------------


// ---------------- GET_PARAMS ----------------
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
// ---------------- GET_PARAMS ----------------
