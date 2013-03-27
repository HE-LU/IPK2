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

#include "socket.h"

using namespace std;

// -----------------------------------------
// ------------------ STRUCT ----------------
typedef struct params
{
  string h;
  string p;
  string l;
  string u;
  int L;
  int U;
  int G;
  int N;
  int H;
  int S;
  int counter;
} struct_params;
// ------------------ STRUCT ----------------
// -----------------------------------------

struct_params get_params(int, char**);

// -----------------------------------------
// ------------------- MAIN -----------------
// -----------------------------------------
int main(int argc, char** argv)
{
  struct_params params = get_params(argc, argv);
    
  if(params.counter == 0)
  {
    exit(0);
  }
  
  // ------ CREATE SOCKET ------
  class_socket socket;
  
  cout << "Trying open socket on: " << params.h << " port: " << params.p << endl;
  int tmp = socket.s_connect(params.h, params.p);
  if ( tmp == 1)
  {
    fprintf(stderr, "Socket could not be opened\n");
    exit(-1);
  }
  else if ( tmp == 2)
  {
    fprintf(stderr, "Failed to recover host name\n");
    exit(-1);
  }
  

  // ------ Prepare request query ------
  string request;
  
  for(int i=1;i<8;i++)
  {
    if(params.L == i)
      request.append("1");
    if(params.U == i)
      request.append("2");
    if(params.G == i)
      request.append("3");
    if(params.N == i)
      request.append("4");
    if(params.H == i)
      request.append("5");
    if(params.S == i)
      request.append("6");
  }
  
  for(int i=0;i<6-params.counter;i++)
  {
    request.append("0");
  }
  
  if(params.u == "")
    request.append("1"+params.l);
  else
    request.append("0"+params.u);
  
  // ------ Send query to server ------
  cout << "Sending: " << request << endl;
  socket.s_write(request);
  
  // ------ Receive response ------
  cout << "Receiving!" << endl;
  
  string text = socket.s_read();;
  istringstream stream(text);
  string radek;
  
  while(getline(stream,radek))
  {
    if(!radek.compare("::END::"))
      break;
    if(!radek.compare(0,9,"::ERROR::"))
      cerr << radek.substr(10,radek.size()-10) << endl;
    else
      cout << radek << endl;
  }

  // ------ clean and exit ------
  socket.s_disconnect();
  return 0;
}
// -----------------------------------------
// ------------------- MAIN -----------------
// -----------------------------------------

// -----------------------------------------
// ---------------- GET_PARAMS ----------------
// -----------------------------------------
struct_params get_params(int argc, char** argv)
{
  struct_params par;
  par.p = "";
  par.u = "";
  par.h = "";
  par.l = "";
  par.L = 0;
  par.U = 0;
  par.G = 0;
  par.N = 0;
  par.H = 0;
  par.S = 0;
  par.counter = 0;
  
  int c;
  
  while( (c = getopt(argc, (char**)argv, "p:u:h:l:LUGNHS")) != -1)
  {
    if(c == 'p' && optarg)
    {
      par.p = optarg;
      
      int pom;
      istringstream stream(optarg);
      stream >> pom;
      
      ostringstream o_stream;
      o_stream << pom;
      
      if(o_stream.str().compare(par.p))
      {
        cerr << "Chyba: zadany port neni cislo!" << endl;
        exit (-1);
      }
    }
    else if(c == 'u' && optarg)
    {
      string s_pom;
      s_pom = optarg;
      
      int pom;
      istringstream stream(optarg);
      stream >> pom;
      
      ostringstream o_stream;
      o_stream << s_pom;
      
      if(o_stream.str().compare(s_pom))
      {
        cerr << "Chyba: zadane UID neni cislo!" << endl;
        exit (-1);
      }
      
      par.u.append(optarg);
      
      for( ;optind < argc && *argv[optind] != '-'; optind++)
      {
        par.u.append(" ");
        
        istringstream stream2(argv[optind]);
        stream2 >> pom;
        
        ostringstream o_stream2;
        o_stream2 << s_pom;
        
        if(o_stream2.str().compare(s_pom))
        {
          cerr << "Chyba: zadane UID neni cislo!" << endl;
          exit (-1);
        }
        
        par.u.append(argv[optind]);
      }
      
    }
    else if(c == 'h' && optarg)
    {
      par.h = optarg;
    }
    else if(c == 'l' && optarg)
    {
      par.l.append(optarg);
      for( ;optind < argc && *argv[optind] != '-'; optind++)
      {
        par.l.append(" ");
        par.l.append(argv[optind]);
      }
    }
    else if(c == 'L')
    {
      par.counter++;
      par.L = par.counter;
    }
    else if(c == 'U')
    {
      par.counter++;
      par.U = par.counter;
    }
    else if(c == 'G')
    {
      par.counter++;
      par.G = par.counter;
    }
    else if(c == 'N')
    {
      par.counter++;
      par.N = par.counter;
    }
    else if(c == 'H')
    {
      par.counter++;
      par.H = par.counter;
    }
    else if(c == 'S')
    {
      par.counter++;
      par.S = par.counter;
    }
    else
    {
      cerr << "Chyba: neznamy parametr!" << endl;
      exit(-1);
    }
  }
  
  if(par.l != "" && par.u != "")
  {
    cout << "UID and Login set!" << endl;
    exit(1);
  }
  
  if(par.p == "" || par.h == "" || (par.l == "" && par.u == ""))
  {
    cout << "client –h hostname –p port –l login ... –u uid ... –L –U –G –N –H –S" << endl;
    exit(1);
  }
  
  return par;
}
// -----------------------------------------
// ---------------- GET_PARAMS ----------------
// -----------------------------------------
